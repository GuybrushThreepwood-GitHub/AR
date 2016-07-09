
/*===================================================================
	File: main.cpp
=====================================================================*/

#include "DemoApp.h"

int SDL_main(int argc, char *argv[])
{
	DemoApp* pApp = new DemoApp(argc, argv);

	if (pApp != 0)
		pApp->Execute( 800, 600, 800, 600, false);

	delete pApp;

	return 0;
}