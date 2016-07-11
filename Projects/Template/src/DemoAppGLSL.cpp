
/*===================================================================
	File: DemopApp.cpp
=====================================================================*/

#include "Boot/Includes.h"

#include "ArucoPlayground.h"
#include "DemoAppGLSL.h"

namespace
{

}

DemoApp::DemoApp()
{

}

DemoApp::DemoApp(int &argc, char **argv )
	: App(argc, argv)
{

}

DemoApp::~DemoApp()
{

}

int DemoApp::Initialise()
{
	renderer::InititaliseFBOShader();

	// load the first state
	m_MainStateManager.ChangeState(new ArucoPlayground(m_MainStateManager));

	// grab the camera parameters
	aruco::CameraParameters params;
	params.readFromXMLFile("camera_aruco.yml");

	// match the camera size
	if (params.CamSize.width > 1 &&
		params.CamSize.height > 1)
	{
		ResizeWindow(params.CamSize.width, params.CamSize.height);
	}

	return 0;
}

int DemoApp::Resize(int windowWidth, int windowHeight)
{
	if (windowWidth * 3 % 4 != 0) {
		DBGLOG("DemoApp::Resize: changing width to avoid padding in cv");
		windowWidth += windowWidth * 3 % 4; //resize to avoid padding
		Resize(windowWidth, windowHeight);
	}

	// pass the size
	m_MainStateManager.Resize(windowWidth, windowHeight);

	return 0;
}

int DemoApp::Update()
{
	m_MainStateManager.Update(m_ElapsedTime);

	return 0;
}

int DemoApp::Render()
{
	m_MainStateManager.Render();

	return 0;
}

int DemoApp::Shutdown()
{
	m_MainStateManager.ChangeState(nullptr);

	return 0;
}


