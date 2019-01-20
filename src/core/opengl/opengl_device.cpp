////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 David Košenina
//
//    This file is part of ScopeFun Oscilloscope.
//
//    ScopeFun Oscilloscope is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ScopeFun Oscilloscope is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
#include <GL/glew.h>

#include <core/core.h>


////////////////////////////////////////////////////////////////////////////////
//
// globals
//
////////////////////////////////////////////////////////////////////////////////
uint frameBufferIndex = 0;
OpenGLMode openGlMode = OPENGL_MODE_21;

////////////////////////////////////////////////////////////////////////////////
//
// GrDeviceCreate
//
////////////////////////////////////////////////////////////////////////////////
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
    #define APIENTRY
#endif

#if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    #define APIENTRY __stdcall
#endif

void APIENTRY openglCallbackFunction(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           void* userParam)
{
   CORE_MESSAGE("%s",message);
}


void grSetMode(OpenGLMode mode)
{
   openGlMode = mode;
}

OpenGLMode grGetMode()
{
   return openGlMode;
}

void grCreateDevice()
{
   // Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return;
	}

   const GLubyte* strig = glGetString(GL_VERSION);
	// int ret = glewIsSupported("glGenFramebuffers");

   if (glewIsSupported("GL_VERSION_2_0"))
   {
      int debug = 1;
   }

   if (glewIsSupported("GL_VERSION_3_2"))
   {
      int debug = 1;
   }

   if (glewIsSupported("GL_VERSION_4_0"))
   {
      int debug = 1;
   }

   if (GL_VERSION_4_0)
   {
      int debug = 1;
   }

   // debug
   #ifdef DEBUG
      glDebugMessageControlARB   = (PFNGLDEBUGMESSAGECONTROLARBPROC)SDL_GL_GetProcAddress("glDebugMessageControlARB");
      glDebugMessageCallbackARB  = (PFNGLDEBUGMESSAGECALLBACKARBPROC)SDL_GL_GetProcAddress("glDebugMessageCallbackARB");
      //
      glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
      glDebugMessageCallbackARB( &openglCallbackFunction, NULL);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
   #endif

   // CORE_MESSAGE( "%s ", glGetString(GL_VERSION) );
   // CORE_MESSAGE( "%s ", glGetString(GL_SHADING_LANGUAGE_VERSION) );

   // frame buffer index
   glGenFramebuffers(1, &frameBufferIndex);
}

void grViewport(float x,float y,float w,float h,float minz,float maxz)
{
   glViewport((int)x, (int)y, (int)w, (int)h);
   glDepthRange(minz,maxz);
}

////////////////////////////////////////////////////////////////////////////////
//
// GrDeviceClear
//
////////////////////////////////////////////////////////////////////////////////
void grClear(uint color)
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
