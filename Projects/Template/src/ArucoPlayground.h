
/*===================================================================
	File: ArucoPlayground.h
=====================================================================*/

#ifndef __ARUCOPLAYGROUND_H__
#define __ARUCOPLAYGROUND_H__

#include "Boot/Includes.h"

class ArucoPlayground : public IState
{
	public:
		ArucoPlayground(StateManager& stateManager);
		virtual ~ArucoPlayground();

		virtual void Enter();
		virtual void Exit();

		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Render();
		virtual int Resize(int windowWidth, int windowHeight);

	private:
		void CaptureFrameThread();

		void DrawVideoToTexture();

	private:
		float m_LastDelta;

		std::mutex m_CaptureMutex;
		std::thread m_CaptureThread;
		bool m_RunCapture;
		bool m_VideoFrameUpdate;

		std::string m_OCLDevice;
		cv::ocl::Context m_OCLContext;

		cv::VideoCapture m_VideoCapturer;
		cv::Mat m_BGRImage, m_RGBImage, m_ImageWrite;

		aruco::CameraParameters m_CameraParameters;

		aruco::MarkerDetector m_PPDetector;
		std::vector<aruco::Marker> m_Markers;

		GLuint m_VideoTexture;

		renderer::Framebuffer m_FBOFinal;
		GLint m_TextureSampler;
		GLint m_VertexAttr;

		float m_ArucoProjMat[16];
		float m_ArucoMdlViewMat[16];
		renderer::Primitives* m_pPrimitivesDraw;
};

#endif // __ARUCOPLAYGROUND_H__

