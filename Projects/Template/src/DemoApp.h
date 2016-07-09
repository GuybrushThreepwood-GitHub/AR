
#ifndef __DEMOAPP_H__
#define __DEMOAPP_H__

#include "Boot/App.h"

#include <opencv2/core/opengl.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "aruco.h"

class DemoApp : public core::app::App
{	
	public:
		DemoApp();
		DemoApp(int &argc, char **argv );
		virtual ~DemoApp();

		// override
		int Initialise();
		int Resize(int windowWidth, int windowHeight);
		int Update();
		int Render();
		int Shutdown();

	private:
		void CreateFramebuffer( int width, int height);
		void DrawVideoToTexture();

	private:
		std::string m_OCLDevice;
		cv::ocl::Context m_OCLContext;

		cv::VideoCapture m_VideoCapturer;
		cv::Mat m_BGRImage, m_RGBImage;

		aruco::CameraParameters m_CameraParameters;

		aruco::MarkerDetector m_PPDetector;
		std::vector<aruco::Marker> m_Markers;

		float m_ArucoProjMat[16];
		float m_ArucoMdlViewMat[16];
		GLuint m_VideoTexture;
};

#endif // __DEMOAPP_H__

