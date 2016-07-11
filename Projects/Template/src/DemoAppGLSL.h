
#ifndef __DEMOAPP_H__
#define __DEMOAPP_H__

#include "Boot/App.h"

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
		StateManager m_MainStateManager;
};

#endif // __DEMOAPP_H__

