
/*===================================================================
	File: ProfileSelectState.cpp
=====================================================================*/

#include "ArucoPlayground.h"

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

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ArucoPlayground::ArucoPlayground(StateManager& stateManager)
: IState( stateManager, 0 )
{
	m_LastDelta = 0.0f;
	m_pPrimitivesDraw = nullptr;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ArucoPlayground::~ArucoPlayground()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void ArucoPlayground::Enter()
{
	// vsync on/off
	SDL_GL_SetSwapInterval(0);

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
		return;
	}

	// read first image to get the dimensions
	m_VideoCapturer >> m_BGRImage;

	// load camera data
	m_CameraParameters.readFromXMLFile("camera_aruco.yml");
	m_CameraParameters.resize(m_BGRImage.size());

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
	m_VideoCapturer.set(CV_CAP_PROP_FRAME_WIDTH, m_BGRImage.size().width);
	m_VideoCapturer.set(CV_CAP_PROP_FRAME_HEIGHT, m_BGRImage.size().height);

	// assign which board to look for 
	m_PPDetector.setDictionary("ARUCO_MIP_36h12", 0.5f);

	m_RGBImage.create(m_BGRImage.size(), CV_8UC3);

	// make sure the first frame is the correct colour and orientation
	cv::cvtColor(m_BGRImage, m_BGRImage, CV_BGR2RGB);
	cv::flip(m_BGRImage, m_BGRImage, 0);

	// create texture for video 
	glGenTextures(1, &m_VideoTexture);
	renderer::OpenGL::GetInstance()->BindTexture(m_VideoTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_BGRImage.size().width, m_BGRImage.size().height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_BGRImage.ptr(0));

	m_RunCapture = true;
	m_VideoFrameUpdate = false;
	m_CaptureThread = std::thread(&ArucoPlayground::CaptureFrameThread, this);
	//m_CaptureThread.join();
	m_CaptureThread.detach();

	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void ArucoPlayground::Exit()
{
	// stop the thread
	m_RunCapture = false;

	if (m_pPrimitivesDraw != nullptr)
	{
		m_pPrimitivesDraw->ShutdownPrimitives();
		delete m_pPrimitivesDraw;
		m_pPrimitivesDraw = nullptr;
	}

	m_VideoCapturer.release();

	m_FBOFinal.Release();

	if (m_VideoTexture != renderer::INVALID_OBJECT)
		glDeleteTextures(1, &m_VideoTexture);

	if (pboFrame != renderer::INVALID_OBJECT)
		glDeleteBuffers(1, &pboFrame);
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int ArucoPlayground::TransitionIn()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int ArucoPlayground::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void ArucoPlayground::Update(float deltaTime)
{
	m_LastDelta = deltaTime;

	static float timeToRead = 0.0f;

	timeToRead += m_LastDelta;
	//if (timeToRead > 0.15f)
	{
		//m_VideoCapturer >> m_BGRImage;
		//m_VideoCapturer.read(m_BGRImage);

			if (m_VideoFrameUpdate)
			{
				cv::GaussianBlur(m_ImageWrite, m_BGRImage, cv::Size(3, 3), 1.5, 1.5);

				// by deafult, opencv works in BGR, so we must convert to RGB because OpenGL in windows prefer
				cv::cvtColor(m_BGRImage, m_BGRImage, CV_BGR2RGB);

				// remove distorion in image
				cv::undistort(m_BGRImage, m_RGBImage, m_CameraParameters.CameraMatrix, m_CameraParameters.Distorsion);

				//detect markers
				m_PPDetector.detect(m_RGBImage, m_Markers, m_CameraParameters.CameraMatrix, cv::Mat(), TheMarkerSize, false);

				util::GetCameraProjectionMatrix(m_CameraParameters, m_BGRImage.size(), m_BGRImage.size(), m_ArucoProjMat, 0.01f, 100.0f, false);

				cv::flip(m_BGRImage, m_BGRImage, 0);

				m_VideoFrameUpdate = false;
			}

		timeToRead = 0.0f;
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void ArucoPlayground::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start rendering
	DrawVideoToTexture();

	renderer::OpenGL::GetInstance()->DepthMode(true, GL_LESS);

	glm::mat4 camView = glm::make_mat4(m_ArucoProjMat);
	renderer::OpenGL::GetInstance()->SetProjectionMatrix(camView);
	renderer::OpenGL::GetInstance()->SetViewMatrix(glm::mat4(1.0f));

	for (unsigned int m = 0; m < m_Markers.size(); m++)
	{
		util::GetMarkerModelViewMatrix(m_Markers[m], m_ArucoMdlViewMat);

		glm::mat4 modelView = glm::make_mat4(m_ArucoMdlViewMat);
		renderer::OpenGL::GetInstance()->SetModelMatrix(modelView);

		m_pPrimitivesDraw->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(TheMarkerSize, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_pPrimitivesDraw->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, TheMarkerSize, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		m_pPrimitivesDraw->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, TheMarkerSize), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		m_pPrimitivesDraw->DrawSphere(TheMarkerSize, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

int ArucoPlayground::Resize(int windowWidth, int windowHeight)
{
	m_WindowDims.x = windowWidth;
	m_WindowDims.y = windowHeight;

	renderer::OpenGL::GetInstance()->SetViewport(m_WindowDims.x, m_WindowDims.y);

	// create the FBO 
	m_FBOFinal.Release();

	m_FBOFinal.Initialise();
	m_FBOFinal.Create(m_WindowDims.x, m_WindowDims.y, (renderer::RTTFLAG_CREATE_COLOURBUFFER | renderer::RTTFLAG_CREATE_DEPTHBUFFER), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, false);
	m_FBOFinal.SetClearColour(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

	renderer::OpenGL::GetInstance()->UseProgram(m_FBOFinal.GetProgram());

	// cache the uniforms and attributes
	m_TextureSampler = m_FBOFinal.GetUniformLocation(renderer::Shader::TexUnit0);
	if (m_TextureSampler != -1)
	glUniform1i(m_TextureSampler, 0);

	m_VertexAttr = m_FBOFinal.GetAttributeLocation(renderer::Shader::Vertex);

	renderer::OpenGL::GetInstance()->UseProgram(0);

	// make sure video is correct size
	m_VideoCapturer.set(CV_CAP_PROP_FRAME_WIDTH, m_WindowDims.x);
	m_VideoCapturer.set(CV_CAP_PROP_FRAME_HEIGHT, m_WindowDims.y);

	return 0;
}

/////////////////////////////////////////////////////
/// Method: CaptureFrameThread
/// Params: None
///
/////////////////////////////////////////////////////
void ArucoPlayground::CaptureFrameThread()
{
	while (m_RunCapture)
	{
		m_CaptureMutex.lock();
		if (!m_VideoFrameUpdate)
		{
			m_VideoCapturer.read(m_ImageWrite);

			m_VideoFrameUpdate = true;
		}
		m_CaptureMutex.unlock();
	}
}

/////////////////////////////////////////////////////
/// Method: DrawVideoToTexture
/// Params: None
///
/////////////////////////////////////////////////////
void ArucoPlayground::DrawVideoToTexture()
{
	renderer::OpenGL::GetInstance()->UseProgram(m_FBOFinal.GetProgram());

	renderer::OpenGL::GetInstance()->DepthMode(false, GL_ALWAYS);
	renderer::OpenGL::GetInstance()->BlendMode(false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderer::OpenGL::GetInstance()->SetNearFarClip(-1.0f, 1.0f);
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

	renderer::OpenGL::GetInstance()->UseProgram(0);

	GL_CHECK
}

