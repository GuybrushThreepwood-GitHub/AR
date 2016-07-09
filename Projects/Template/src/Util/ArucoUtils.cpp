
/*===================================================================
	File: ArucoUtils.cpp
=====================================================================*/

#include "Boot/Includes.h"

void argConvGLcpara2(float cparam[3][4], int width, int height, float gnear, float gfar, float m[16], bool invert);
int arParamDecompMat(float source[3][4], float cpara[3][4], float trans[3][4]);

float norm(float a, float b, float c) { return (sqrt(a * a + b * b + c * c)); }

float dot(float a1, float a2, float a3, float b1, float b2, float b3) { return (a1 * b1 + a2 * b2 + a3 * b3); }

void util::GetMarkerModelViewMatrix(const aruco::Marker& m, float outMat[16])
{
	// check if paremeters are valid
	bool invalid = false;
	for (int i = 0; i < 3 && !invalid; i++) {
		if (m.Tvec.at< float >(i, 0) != -999999.0f)
			invalid |= false;
		if (m.Rvec.at< float >(i, 0) != -999999.0f)
			invalid |= false;
	}
	if (invalid)
		return;

	cv::Mat Rot(3, 3, CV_32FC1), Jacob;
	cv::Rodrigues(m.Rvec, Rot, Jacob);

	float para[3][4];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			para[i][j] = Rot.at< float >(i, j);

	// now, add the translation
	para[0][3] = m.Tvec.at< float >(0, 0);
	para[1][3] = m.Tvec.at< float >(1, 0);
	para[2][3] = m.Tvec.at< float >(2, 0);
	float scale = 1.0f;

	outMat[0 + 0 * 4] = para[0][0];
	// R1C2
	outMat[0 + 1 * 4] = para[0][1];
	outMat[0 + 2 * 4] = para[0][2];
	outMat[0 + 3 * 4] = para[0][3];
	// R2
	outMat[1 + 0 * 4] = para[1][0];
	outMat[1 + 1 * 4] = para[1][1];
	outMat[1 + 2 * 4] = para[1][2];
	outMat[1 + 3 * 4] = para[1][3];
	// R3
	outMat[2 + 0 * 4] = -para[2][0];
	outMat[2 + 1 * 4] = -para[2][1];
	outMat[2 + 2 * 4] = -para[2][2];
	outMat[2 + 3 * 4] = -para[2][3];
	outMat[3 + 0 * 4] = 0.0;
	outMat[3 + 1 * 4] = 0.0;
	outMat[3 + 2 * 4] = 0.0;
	outMat[3 + 3 * 4] = 1.0;
	if (scale != 0.0f) {
		outMat[12] *= scale;
		outMat[13] *= scale;
		outMat[14] *= scale;
	}
}

void util::GetCameraProjectionMatrix(const aruco::CameraParameters& params, const cv::Size& orgImgSize, const cv::Size& size, float outMat[16], float gnear, float gfar, bool invert)
{
	//if (cv::countNonZero(Distorsion) != 0)
	//	std::cerr << "CameraParameters::glGetProjectionMatrix :: The camera has distortion coefficients " << __FILE__ << " " << __LINE__ << endl;
	if (params.isValid() == false)
	{
		return;
		//throw cv::Exception(9100, "invalid camera parameters", "CameraParameters::glGetProjectionMatrix", __FILE__, __LINE__);
	}

	// Deterime the rsized info
	float Ax = float(size.width) / float(orgImgSize.width);
	float Ay = float(size.height) / float(orgImgSize.height);
	float _fx = params.CameraMatrix.at< float >(0, 0) * Ax;
	float _cx = params.CameraMatrix.at< float >(0, 2) * Ax;
	float _fy = params.CameraMatrix.at< float >(1, 1) * Ay;
	float _cy = params.CameraMatrix.at< float >(1, 2) * Ay;
	float cparam[3][4] = { { _fx, 0.0f, _cx, 0.0f }, { 0.0f, _fy, _cy, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f } };

	argConvGLcpara2(cparam, size.width, size.height, gnear, gfar, outMat, invert);
}

void argConvGLcpara2(float cparam[3][4], int width, int height, float gnear, float gfar, float m[16], bool invert)
{

	float icpara[3][4];
	float trans[3][4];
	float p[3][3], q[4][4];
	int i, j;

	cparam[0][2] *= -1.0f;
	cparam[1][2] *= -1.0f;
	cparam[2][2] *= -1.0f;

	if (arParamDecompMat(cparam, icpara, trans) < 0)
	{
		return;//throw cv::Exception(9002, "parameter error", "MarkerDetector::argConvGLcpara2", __FILE__, __LINE__);
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			p[i][j] = icpara[i][j] / icpara[2][2];
		}
	}
	q[0][0] = (2.0f * p[0][0] / width);
	q[0][1] = (2.0f * p[0][1] / width);
	q[0][2] = ((2.0f * p[0][2] / width) - 1.0f);
	q[0][3] = 0.0f;

	q[1][0] = 0.0f;
	q[1][1] = (2.0f * p[1][1] / height);
	q[1][2] = ((2.0f * p[1][2] / height) - 1.0f);
	q[1][3] = 0.0f;

	q[2][0] = 0.0f;
	q[2][1] = 0.0f;
	q[2][2] = (gfar + gnear) / (gfar - gnear);
	q[2][3] = -2.0f * gfar * gnear / (gfar - gnear);

	q[3][0] = 0.0f;
	q[3][1] = 0.0f;
	q[3][2] = 1.0f;
	q[3][3] = 0.0f;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 3; j++) {
			m[i + j * 4] = q[i][0] * trans[0][j] + q[i][1] * trans[1][j] + q[i][2] * trans[2][j];
		}
		m[i + 3 * 4] = q[i][0] * trans[0][3] + q[i][1] * trans[1][3] + q[i][2] * trans[2][3] + q[i][3];
	}

	if (!invert) {
		m[13] = -m[13];
		m[1] = -m[1];
		m[5] = -m[5];
		m[9] = -m[9];
	}
}

int arParamDecompMat(float source[3][4], float cpara[3][4], float trans[3][4])
{
	int r, c;
	float Cpara[3][4];
	float rem1, rem2, rem3;

	if (source[2][3] >= 0) {
		for (r = 0; r < 3; r++) {
			for (c = 0; c < 4; c++) {
				Cpara[r][c] = source[r][c];
			}
		}
	}
	else {
		for (r = 0; r < 3; r++) {
			for (c = 0; c < 4; c++) {
				Cpara[r][c] = -(source[r][c]);
			}
		}
	}

	for (r = 0; r < 3; r++) {
		for (c = 0; c < 4; c++) {
			cpara[r][c] = 0.0f;
		}
	}

	cpara[2][2] = norm(Cpara[2][0], Cpara[2][1], Cpara[2][2]);
	trans[2][0] = Cpara[2][0] / cpara[2][2];
	trans[2][1] = Cpara[2][1] / cpara[2][2];
	trans[2][2] = Cpara[2][2] / cpara[2][2];
	trans[2][3] = Cpara[2][3] / cpara[2][2];

	cpara[1][2] = dot(trans[2][0], trans[2][1], trans[2][2], Cpara[1][0], Cpara[1][1], Cpara[1][2]);
	rem1 = Cpara[1][0] - cpara[1][2] * trans[2][0];
	rem2 = Cpara[1][1] - cpara[1][2] * trans[2][1];
	rem3 = Cpara[1][2] - cpara[1][2] * trans[2][2];
	cpara[1][1] = norm(rem1, rem2, rem3);
	trans[1][0] = rem1 / cpara[1][1];
	trans[1][1] = rem2 / cpara[1][1];
	trans[1][2] = rem3 / cpara[1][1];

	cpara[0][2] = dot(trans[2][0], trans[2][1], trans[2][2], Cpara[0][0], Cpara[0][1], Cpara[0][2]);
	cpara[0][1] = dot(trans[1][0], trans[1][1], trans[1][2], Cpara[0][0], Cpara[0][1], Cpara[0][2]);
	rem1 = Cpara[0][0] - cpara[0][1] * trans[1][0] - cpara[0][2] * trans[2][0];
	rem2 = Cpara[0][1] - cpara[0][1] * trans[1][1] - cpara[0][2] * trans[2][1];
	rem3 = Cpara[0][2] - cpara[0][1] * trans[1][2] - cpara[0][2] * trans[2][2];
	cpara[0][0] = norm(rem1, rem2, rem3);
	trans[0][0] = rem1 / cpara[0][0];
	trans[0][1] = rem2 / cpara[0][0];
	trans[0][2] = rem3 / cpara[0][0];

	trans[1][3] = (Cpara[1][3] - cpara[1][2] * trans[2][3]) / cpara[1][1];
	trans[0][3] = (Cpara[0][3] - cpara[0][1] * trans[1][3] - cpara[0][2] * trans[2][3]) / cpara[0][0];

	for (r = 0; r < 3; r++) {
		for (c = 0; c < 3; c++) {
			cpara[r][c] /= cpara[2][2];
		}
	}

	return 0;
}
