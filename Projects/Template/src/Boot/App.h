
#ifndef __APP_H__
#define __APP_H__

#include "Boot/Includes.h"

namespace core
{
	namespace app
	{
		class CmdParser // http://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c/868894#868894
		{
			public:
				CmdParser(){
					m_Tokens.clear();
				}

				CmdParser(int &argc, char **argv){
					Create(argc, argv);
				}

				void Create(int &argc, char **argv){

					m_Tokens.clear();

					for (int i = 1; i < argc; ++i)
						m_Tokens.push_back(std::string(argv[i]));
				}

				/// @author iain
				const std::string getCmdOption(const std::string &option) const{
					auto itr = std::find(m_Tokens.begin(), m_Tokens.end(), option);
					if (itr != m_Tokens.end() && ++itr != m_Tokens.end()){
						return *itr;
					}
					return "";
				}
				/// @author iain
				bool cmdOptionExists(const std::string &option) const{
					return std::find(m_Tokens.begin(), m_Tokens.end(), option) != m_Tokens.end();
				}
			private:
				std::vector<std::string> m_Tokens;
		};

		class App 
		{	
			public:
				App();
				App(int &argc, char **argv);
				virtual ~App();

				// override
				virtual int Initialise() = 0;
				virtual int Resize(int windowWidth, int windowHeight) = 0;
				virtual int Update() = 0;
				virtual int Render() = 0;
				virtual int Shutdown() = 0;

				// main launch
				int Execute(int windowWidth, int windowHeight, int frameWidth, int frameHeight, bool fullscreen);

				void ResizeWindow(int windowWidth, int windowHeight);

			private:
				void InitClass();

				void ToggleFullscreen();

				void UpdateFrame();
				void RenderFrame();

			private:
				SDL_Window* m_Window;
				SDL_GLContext m_Context;

				int m_TotalFrames;
				Uint32 m_StartTicks;
				Uint32 m_LastTicks;

			protected:
				float m_ElapsedTime;
				float m_FPS;
				CmdParser m_CmdLine;

				std::wstring m_SaveFilePath;

				glm::ivec2 m_WindowDims;
				glm::ivec2 m_FrameDims;

				/// last viewport
				GLint m_Viewport[4];

				/// projection matrix
				glm::mat4	m_ProjMatrix;
				/// view matrix
				glm::mat4	m_ViewMatrix;
				/// modelview matrix
				glm::mat4	m_ModlMatrix;
				/// clip matrix
				glm::mat4	m_ClipMatrix;

				/// GL field of view
				GLfloat	m_FOV;
				/// GL aspect ratio
				GLfloat m_AspectRatio;
				/// GL near clip
				GLfloat	m_NearClip;
				/// GL far clip
				GLfloat	m_FarClip;


		};

	} // namespace app

} // namespace core

#endif // __APP_H__

