
#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#ifdef  _WIN32
	#define _CRT_SECURE_NO_DEPRECATE // don't care about secure c functions
	#define _CRT_NONSTDC_NO_DEPRECATE // don't complain about non std c
	#define _CRT_SECURE_NO_WARNINGS // no warnings

	#define WIN32_LEAN_AND_MEAN // Windows specific includes and defines
	#define NOMINMAX // disable macro min/max

	#define snprintf _snprintf
#endif // _WIN32

#define DBGLOG printf
#define DBG_ASSERT assert

//#define USE_OPENGL21
//#define USE_OPENGL41

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <cassert>
#include <cmath>
#include <cerrno>
#include <fcntl.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <istream>
#include <map> // added to stop debug new operator causing compile problems 
#include <vector>
#include <string>
#include <array>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>

#include <sys/stat.h>

// opengl core
#ifdef USE_OPENGL21
	#include "Render/gl_core_2_1.h"
#endif

#ifdef USE_OPENGL41
	#include "Render/gl_core_4_1.h"
#endif

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/norm.hpp>

// sdl
#include <SDL.h>
#include <SDL_opengl.h>

// aruco 
#include "aruco.h"

// opencv
#include <opencv2/core/opengl.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// renderer
#include "Render/OpenGLCommon.h"

#ifdef USE_OPENGL21
	#include "Render/OpenGL21.h"
#endif
#ifdef USE_OPENGL41
	#include "Render/OpenGL41.h"
#endif

#include "Render/ShaderShared.h"
#include "Render/Shader.h"
#include "Render/Framebuffer.h"
#include "Render/Primitives.h"

#include "Util/IState.h"
#include "Util/StateManager.h"

#include "Util/ArucoUtils.h"

#endif // __INCLUDES_H__

