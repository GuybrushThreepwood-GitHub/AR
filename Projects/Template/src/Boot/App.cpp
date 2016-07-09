
/*===================================================================
	File: App.cpp
=====================================================================*/

#include "Boot/Includes.h"

#include "App.h"

using namespace core::app;

App::App()
{
	InitClass();
}

App::App(int &argc, char **argv)
{
	InitClass();
	m_CmdLine.Create(argc, argv);
}

App::~App()
{

}

void App::InitClass()
{
	m_Window = nullptr;
	m_Context = nullptr;
	m_TotalFrames = 0;
	m_StartTicks = 0;
	m_FPS = 0;
	m_ElapsedTime = 0.0f;

	m_ProjMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	m_ModlMatrix = glm::mat4(1.0f);
	m_ClipMatrix = glm::mat4(1.0f);

	m_FOV = 60.0f;
	m_AspectRatio = 1.0f;
	m_NearClip = 1.0f;
	m_FarClip = 1000.0f;
}

void App::ToggleFullscreen() 
{
	Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN;

	bool IsFullscreen = (SDL_GetWindowFlags(m_Window) & fullscreenFlag) != 0;

	SDL_SetWindowFullscreen(m_Window, IsFullscreen ? 0 : fullscreenFlag);

	//SDL_ShowCursor(IsFullscreen);
}

void App::UpdateFrame()
{
	Uint32 currTicks = SDL_GetTicks();

	m_ElapsedTime = static_cast<float>( (currTicks - m_LastTicks) ) / 1000.0f;
	m_FPS = (m_TotalFrames / (float)(currTicks - m_StartTicks)) * 1000.0f;

	m_LastTicks = currTicks;

	Update();
}

void App::RenderFrame()
{
	Render();
}

int App::Execute(int windowWidth, int windowHeight, int frameWidth, int frameHeight, bool fullscreen)
{
	if (windowWidth <= 0)
		windowWidth = 1;

	if (windowHeight <= 0)
		windowHeight = 1;

	m_WindowDims = glm::ivec2( windowWidth, windowHeight );
	m_FrameDims = glm::ivec2( frameWidth, frameHeight );

	std::string title;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}
	else
	{
#ifdef USE_OPENGL21
		// Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
		// Use OpenGL 4.1 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
		// Create window
		Uint32 flags = SDL_WINDOW_OPENGL;
		if (fullscreen)
			flags |= SDL_WINDOW_FULLSCREEN;

		m_Window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_WindowDims.x, m_WindowDims.y, flags);
		if (m_Window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return 1;
		}
		else
		{
			m_Context = SDL_GL_CreateContext(m_Window);
			if (m_Context == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				return 1;
			}
			else
			{
				// initialise OpenGL
				renderer::OpenGL::Initialise();

				// some default GL values
				renderer::OpenGL::GetInstance()->Init();

				renderer::OpenGL::GetInstance()->SetupPerspectiveView(m_FrameDims.x, m_FrameDims.y);
				renderer::OpenGL::GetInstance()->SetNearFarClip(1.0f, 10000.0f);
				renderer::OpenGL::GetInstance()->ClearColour(0.0f, 0.282f, 0.415f, 1.0f);

				Initialise();
			}

			// Main loop flag
			bool quit = false;

			// Event handler
			SDL_Event e;

			m_TotalFrames = 0;
			m_LastTicks = m_StartTicks = SDL_GetTicks();

			//While application is running
			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					switch (e.type)
					{
						//User requests quit 
						case SDL_QUIT:
						{
							quit = true;
						}break;
						case SDL_KEYDOWN:
						{
							// key press 
							switch (e.key.keysym.sym)
							{
								case SDLK_RETURN:
								{
									ToggleFullscreen();
								}break;
								case SDLK_ESCAPE:
								{
									quit = true;
								}break;

								default:
									break;
							}
						}break;
						case SDL_KEYUP:
						{

						}break;

						default:
							break;
					}
				}

				UpdateFrame();

				//renderer::OpenGL::GetInstance()->ClearScreen();

				RenderFrame();

				GL_CHECK

				SDL_GL_SwapWindow(m_Window);

				title = "FPS: " + std::to_string(m_FPS) + " | FrameTime: " + std::to_string(m_ElapsedTime);
				SDL_SetWindowTitle(m_Window, title.c_str());
				m_TotalFrames++;
			}
		}


	}

	Shutdown();

	// release OpenGL
	renderer::OpenGL::Shutdown();

	SDL_GL_DeleteContext(m_Context);

	// Destroy window
	SDL_DestroyWindow(m_Window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}

void App::ResizeWindow(int windowWidth, int windowHeight)
{
	if (windowWidth <= 0)
		windowWidth = 1;

	if (windowHeight <= 0)
		windowHeight = 1;

	m_Viewport[0] = 0;
	m_Viewport[1] = 0;
	m_Viewport[2] = m_WindowDims.x;
	m_Viewport[3] = m_WindowDims.y;

	m_WindowDims.x = windowWidth;
	m_WindowDims.y = windowHeight;

	SDL_SetWindowSize(m_Window, m_WindowDims.x, m_WindowDims.y);

	Resize(m_WindowDims.x, m_WindowDims.y);
}
