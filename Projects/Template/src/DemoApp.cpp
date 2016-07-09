
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
	m_PPDetector.setDictionary("ARUCO_MIP_36h12", 0.5f);

	m_RGBImage.create(m_BGRImage.size(), CV_8UC3);

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

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

		cv::GaussianBlur( m_BGRImage, m_BGRImage, cv::Size(5,5), 1.5, 1.5 );

		// by deafult, opencv works in BGR, so we must convert to RGB because OpenGL in windows prefer
		cv::cvtColor(m_BGRImage, m_BGRImage, CV_BGR2RGB);

		// remove distorion in image
		cv::undistort(m_BGRImage, m_RGBImage, m_CameraParameters.CameraMatrix, m_CameraParameters.Distorsion);

		//detect markers
		m_PPDetector.detect(m_RGBImage, m_Markers, m_CameraParameters.CameraMatrix, cv::Mat(), TheMarkerSize, false);

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
	double proj_matrix[16];
	m_CameraParameters.glGetProjectionMatrix(m_RGBImage.size(), m_RGBImage.size(), proj_matrix, 0.05, 10, false);
	glLoadIdentity();
	glLoadMatrixd(proj_matrix);

	//now, for each marker,
	double modelview_matrix[16];
	for (unsigned int m = 0; m<m_Markers.size(); m++)
	{
		m_Markers[m].glGetModelViewMatrix(modelview_matrix);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixd(modelview_matrix);

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
