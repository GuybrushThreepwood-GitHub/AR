
#ifndef __ARUCOUTILS_H__
#define __ARUCOUTILS_H__

#include <opencv2/core/opengl.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "aruco.h"

namespace util
{
	void GetCameraProjectionMatrix(const aruco::CameraParameters& params, const cv::Size& orgImgSize, const cv::Size& size, float proj_matrix[16], float gnear, float gfar, bool invert);

	void GetMarkerModelViewMatrix(const aruco::Marker& m, float outMat[16]);

} // namespace util

#endif // __ARUCOUTILS_H__
