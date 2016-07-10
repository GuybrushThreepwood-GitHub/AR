
/*===================================================================
	File: DemopApp.cpp
=====================================================================*/

#include "Boot/Includes.h"

#include "DemoAppGLSL.h"

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

DemoApp::DemoApp()
{

}

DemoApp::DemoApp(int &argc, char **argv )
	: App(argc, argv)
{
	m_pPrimitivesDraw = 0;
}

DemoApp::~DemoApp()
{

}

int DemoApp::Initialise()
{
	renderer::InititaliseFBOShader();
	
	m_pPrimitivesDraw = new renderer::Primitives;
	m_pPrimitivesDraw->InitialisePrimitives();

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
	m_PPDetector.setDictionary("ARUCO_MIP_36h12", 0.5f);

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

	// create the FBO 
	m_FBOFinal.Release();

	m_FBOFinal.Initialise();
	m_FBOFinal.Create(windowWidth, windowHeight, (renderer::RTTFLAG_CREATE_COLOURBUFFER | renderer::RTTFLAG_CREATE_DEPTHBUFFER), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, false);
	m_FBOFinal.SetClearColour(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

	renderer::OpenGL::GetInstance()->UseProgram(m_FBOFinal.GetProgram());

	// cache the uniforms and attributes
	m_TextureSampler = m_FBOFinal.GetUniformLocation(renderer::Shader::TexUnit0);
	if (m_TextureSampler != -1)
		glUniform1i(m_TextureSampler, 0);

	m_VertexAttr = m_FBOFinal.GetAttributeLocation(renderer::Shader::Vertex);

	renderer::OpenGL::GetInstance()->UseProgram(0);

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

		cv::GaussianBlur(m_BGRImage, m_BGRImage, cv::Size(9, 9), 1.5, 1.5);

		// by deafult, opencv works in BGR, so we must convert to RGB because OpenGL in windows prefer
		cv::cvtColor(m_BGRImage, m_BGRImage, CV_BGR2RGB);

		// remove distorion in image
		cv::undistort(m_BGRImage, m_RGBImage, m_CameraParameters.CameraMatrix, m_CameraParameters.Distorsion);

		//detect markers
		m_PPDetector.detect(m_RGBImage, m_Markers, m_CameraParameters.CameraMatrix, cv::Mat(), TheMarkerSize, false);

		util::GetCameraProjectionMatrix(m_CameraParameters, m_BGRImage.size(), m_BGRImage.size(), m_ArucoProjMat, 0.05f, 10.0f, false);

		cv::flip(m_BGRImage, m_BGRImage, 0);

		timeToRead = 0.0f;
	}
	return 0;
}

int DemoApp::Render()
{
	// start rendering
	//m_FBOFinal.StartRenderToTexture();
	{
		DrawVideoToTexture();
	}
	//m_FBOFinal.EndRenderToTexture();

	// render the FBO to screen
	renderer::OpenGL::GetInstance()->DepthMode(false, GL_ALWAYS);
	renderer::OpenGL::GetInstance()->SetNearFarClip(-100.0f, 100.0f);
	renderer::OpenGL::GetInstance()->SetupOrthographicView(m_WindowDims.x, m_WindowDims.y, true);
	//m_FBOFinal.RenderToScreen();

	glm::mat4 camView = glm::make_mat4(m_ArucoProjMat);
	renderer::OpenGL::GetInstance()->SetProjectionMatrix(camView);
	renderer::OpenGL::GetInstance()->SetViewMatrix(glm::mat4(1.0f));

	for (unsigned int m = 0; m < m_Markers.size(); m++)
	{
		util::GetMarkerModelViewMatrix(m_Markers[m], m_ArucoMdlViewMat);

		glm::mat4 modelView = glm::make_mat4(m_ArucoMdlViewMat);
		renderer::OpenGL::GetInstance()->SetModelMatrix(modelView);

		m_pPrimitivesDraw->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(TheMarkerSize, 0.0f, 0.0f), glm::vec4(1,0,0,1) );
		m_pPrimitivesDraw->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, TheMarkerSize, 0.0f), glm::vec4(0, 1, 0, 1));
		m_pPrimitivesDraw->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, TheMarkerSize), glm::vec4(0, 0, 1, 1));
	}

	return 0;
}

int DemoApp::Shutdown()
{
	if (m_pPrimitivesDraw != 0)
	{
		m_pPrimitivesDraw->ShutdownPrimitives();
		delete m_pPrimitivesDraw;
		m_pPrimitivesDraw = 0;
	}

	m_VideoCapturer.release();

	m_FBOFinal.Release();

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
	renderer::OpenGL::GetInstance()->UseProgram(m_FBOFinal.GetProgram());

	renderer::OpenGL::GetInstance()->DepthMode(false, GL_ALWAYS);
	renderer::OpenGL::GetInstance()->BlendMode(false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderer::OpenGL::GetInstance()->SetNearFarClip(-100.0f, 100.0f);
	renderer::OpenGL::GetInstance()->SetupOrthographicView(m_WindowDims.x, m_WindowDims.y, true);

	renderer::OpenGL::GetInstance()->DisableVBO();

	// bind the video texture
	renderer::OpenGL::GetInstance()->BindTexture(m_VideoTexture);

	// no PBO
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_BGRImage.size().width, m_BGRImage.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, m_BGRImage.ptr(0));
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_RGBImage.size().width, m_RGBImage.size().height, GL_RGB, GL_UNSIGNED_BYTE, m_RGBImage.ptr(0));
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_BGRImage.size().width, m_BGRImage.size().height, GL_RGB, GL_UNSIGNED_BYTE, m_BGRImage.ptr(0));

	// render fullscreen quad with video texture
	if (m_VertexAttr != -1)
	{
		glEnableVertexAttribArray(m_VertexAttr);
		glVertexAttribPointer(m_VertexAttr, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), &vertList[0]);
	}

	GL_CHECK

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (m_VertexAttr != -1)
	{
		glDisableVertexAttribArray(m_VertexAttr);
	}

	GL_CHECK
}

