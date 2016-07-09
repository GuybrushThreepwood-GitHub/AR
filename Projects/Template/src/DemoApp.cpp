
/*===================================================================
	File: DemopApp.cpp
=====================================================================*/

#include "Boot/Includes.h"

#include "DemoApp.h"

namespace
{
	float TheMarkerSize = 0.175f;

	glm::vec2 vertList[] =
	{
		glm::vec2(1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f),
		glm::vec2(1.0f, -1.0f),
		glm::vec2(-1.0f, -1.0f),	
	};

	GLuint pboFrame = renderer::INVALID_OBJECT;

	const int CAMERA_CAPTURE_WIDTH = 640;
	const int CAMERA_CAPTURE_HEIGHT = 480;
}

void GetCameraProjectionMatrix(const aruco::CameraParameters& params, const cv::Size& orgImgSize, const cv::Size& size, float proj_matrix[16], float gnear, float gfar, bool invert);
void argConvGLcpara2(float cparam[3][4], int width, int height, float gnear, float gfar, float m[16], bool invert);
int arParamDecompMat(float source[3][4], float cpara[3][4], float trans[3][4]);

void GetMarkerModelViewMatrix(const aruco::Marker& m, float outMat[16]);

DemoApp::DemoApp()
{

}

DemoApp::DemoApp(int &argc, char **argv )
	: App(argc, argv)
{
	m_VideoTexture = renderer::INVALID_OBJECT;
}

DemoApp::~DemoApp()
{

}

int DemoApp::Initialise()
{

	/*if (cv::ocl::haveOpenCL())
	{
		m_OCLContext = cv::ogl::ocl::initializeContextFromGL();
	}

	if (cv::ocl::useOpenCL())
		m_OCLDevice = cv::ocl::Context::getDefault().device(0).name();
	else
		m_OCLDevice = "NoOpenCL";*/

	// read from camera or from  file
	m_VideoCapturer.open(0);

	// check video is open
	if (!m_VideoCapturer.isOpened())
	{
		return 1;
	}

	// CV_CAP_PROP_POS_MSEC Current position of the video file in milliseconds or video capture timestamp.
	// CV_CAP_PROP_POS_FRAMES 0-based index of the frame to be decoded/captured next.
	// CV_CAP_PROP_POS_AVI_RATIO Relative position of the video file: 0 - start of the film, 1 - end of the film.
	// CV_CAP_PROP_FRAME_WIDTH Width of the frames in the video stream.
	// CV_CAP_PROP_FRAME_HEIGHT Height of the frames in the video stream.
	// CV_CAP_PROP_FPS Frame rate.
	// CV_CAP_PROP_FOURCC 4-character code of codec.
	// CV_CAP_PROP_FRAME_COUNT Number of frames in the video file.
	// CV_CAP_PROP_FORMAT Format of the Mat objects returned by retrieve() .
	// CV_CAP_PROP_MODE Backend-specific value indicating the current capture mode.
	// CV_CAP_PROP_BRIGHTNESS Brightness of the image (only for cameras).
	// CV_CAP_PROP_CONTRAST Contrast of the image (only for cameras).
	// CV_CAP_PROP_SATURATION Saturation of the image (only for cameras).
	// CV_CAP_PROP_HUE Hue of the image (only for cameras).
	// CV_CAP_PROP_GAIN Gain of the image (only for cameras).
	// CV_CAP_PROP_EXPOSURE Exposure (only for cameras).
	// CV_CAP_PROP_CONVERT_RGB Boolean flags indicating whether images should be converted to RGB.
	// CV_CAP_PROP_WHITE_BALANCE Currently not supported
	// CV_CAP_PROP_RECTIFICATION
	m_VideoCapturer.set(CV_CAP_PROP_FPS, 30.0);
	m_VideoCapturer.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_CAPTURE_WIDTH);
	m_VideoCapturer.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_CAPTURE_HEIGHT);

	// read first image to get the dimensions
	m_VideoCapturer >> m_BGRImage;

	// load camera data
	m_CameraParameters.readFromXMLFile("camera_aruco.yml");
	m_CameraParameters.resize(m_BGRImage.size());

	// assign which board to look for 
	m_PPDetector.setDictionary("ARUCO_MIP_36h12", 0.9f);

	m_RGBImage.create(m_BGRImage.size(), CV_8UC3);

	// create texture for video 
	glGenTextures(1, &m_VideoTexture);
	renderer::OpenGL::GetInstance()->BindTexture(m_VideoTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_BGRImage.size().width, m_BGRImage.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, m_BGRImage.ptr(0));

	GL_CHECK

	// resize the app view to match the camera image size
	ResizeWindow(m_BGRImage.size().width, m_BGRImage.size().height);

	return 0;
}

int DemoApp::Resize(int windowWidth, int windowHeight)
{
	if (windowWidth * 3 % 4 != 0) {
		windowWidth += windowWidth * 3 % 4;//resize to avoid padding
		Resize(windowWidth, windowHeight);
	}

	renderer::OpenGL::GetInstance()->SetViewport(windowWidth, windowHeight);

	return 0;
}

int DemoApp::Update()
{
	static float timeToRead = 0.0f;

	timeToRead += m_ElapsedTime;
	if (timeToRead > 0.15f)
	{
		//m_VideoCapturer >> m_BGRImage;
		m_VideoCapturer.read(m_BGRImage);

		cv::GaussianBlur( m_BGRImage, m_BGRImage, cv::Size(9,9), 1.5, 1.5 );

		// by deafult, opencv works in BGR, so we must convert to RGB because OpenGL in windows prefer
		cv::cvtColor(m_BGRImage, m_BGRImage, CV_BGR2RGB);

		// remove distorion in image
		cv::undistort(m_BGRImage, m_RGBImage, m_CameraParameters.CameraMatrix, m_CameraParameters.Distorsion);

		//detect markers
		m_PPDetector.detect(m_RGBImage, m_Markers, m_CameraParameters.CameraMatrix, cv::Mat(), TheMarkerSize, false);

		GetCameraProjectionMatrix(m_CameraParameters, m_BGRImage.size(), m_BGRImage.size(), m_ArucoProjMat, 0.05f, 10.0f, false);

		timeToRead = 0.0f;
	}
	return 0;
}

void axis(float size)
{
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f); // origin of the line
	glVertex3f(size, 0.0f, 0.0f); // ending point of the line
	glEnd();

	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f); // origin of the line
	glVertex3f(0.0f, size, 0.0f); // ending point of the line
	glEnd();


	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f); // origin of the line
	glVertex3f(0.0f, 0.0f, size); // ending point of the line
	glEnd();
}

int DemoApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	///draw image in the buffer
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0f, m_RGBImage.size().width, 0.0f, m_RGBImage.size().height, -1.0f, 1.0f);

	DrawVideoToTexture();

	renderer::OpenGL::GetInstance()->DepthMode(true, GL_LESS);
	renderer::OpenGL::GetInstance()->BlendMode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	///Set the appropriate projection matrix so that rendering is done in a enrvironment
	//like the real camera (without distorsion)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(m_ArucoProjMat);

	//now, for each marker,
	for (unsigned int m = 0; m<m_Markers.size(); m++)
	{
		GetMarkerModelViewMatrix(m_Markers[m], m_ArucoMdlViewMat);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(m_ArucoMdlViewMat);

		axis(TheMarkerSize);

		glColor3f(1.0f, 0.4f, 0.4f);
		glTranslatef(0.0f, 0.0f, TheMarkerSize*0.5f);
		glPushMatrix();
			//glutWireCube(TheMarkerSize);
		glPopMatrix();
	}

	GL_CHECK

	return 0;
}

int DemoApp::Shutdown()
{
	m_VideoCapturer.release();

	if (m_VideoTexture != renderer::INVALID_OBJECT)
		glDeleteTextures(1, &m_VideoTexture);

	if (pboFrame != renderer::INVALID_OBJECT)
		glDeleteBuffers(1, &pboFrame);

	return 0;
}

void DemoApp::CreateFramebuffer(int width, int height)
{

}

/////////////////////////////////////////////////////
/// Method: DrawVideoToTexture
/// Params: None
///
/////////////////////////////////////////////////////
void DemoApp::DrawVideoToTexture()
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	renderer::OpenGL::GetInstance()->DepthMode(false, GL_ALWAYS);
	renderer::OpenGL::GetInstance()->DisableVBO();

	glEnable(GL_TEXTURE_2D);
	renderer::OpenGL::GetInstance()->BindTexture(m_VideoTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_BGRImage.size().width, m_BGRImage.size().height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_BGRImage.ptr(0));
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_BGRImage.size().width, m_BGRImage.size().height, GL_RGB, GL_UNSIGNED_BYTE, m_BGRImage.ptr(0));

	// work out correct uv coords
	glm::vec2 vUVCoords[4];
	vUVCoords[0] = glm::vec2(0.0f, 1.0f);
	vUVCoords[1] = glm::vec2(1.0f, 1.0f);
	vUVCoords[2] = glm::vec2(0.0f, 0.0f);
	vUVCoords[3] = glm::vec2(1.0f, 0.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	float vertList[] =
	{
		0.0f, 0.0f,
		static_cast<float>(m_RGBImage.size().width), 0.0f,
		0.0f, static_cast<float>(m_RGBImage.size().height),
		static_cast<float>(m_RGBImage.size().width), static_cast<float>(m_RGBImage.size().height)
	};

	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f);

		glVertexPointer(2, GL_FLOAT, 0, &vertList[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &vUVCoords[0]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	renderer::OpenGL::GetInstance()->BindTexture(0);
	glDisable(GL_TEXTURE_2D);

	GL_CHECK
}

void GetMarkerModelViewMatrix(const aruco::Marker& m, float outMat[16])
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

void GetCameraProjectionMatrix(const aruco::CameraParameters& params, const cv::Size& orgImgSize, const cv::Size& size, float outMat[16], float gnear, float gfar, bool invert)
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

float norm(float a, float b, float c) { return (sqrt(a * a + b * b + c * c)); }

float dot(float a1, float a2, float a3, float b1, float b2, float b3) { return (a1 * b1 + a2 * b2 + a3 * b3); }

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
