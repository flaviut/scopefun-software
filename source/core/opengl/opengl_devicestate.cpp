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
// OpenGl
//
////////////////////////////////////////////////////////////////////////////////
GLuint samplerCanvas2D = 0;
GLuint samplerFont     = 0;

////////////////////////////////////////////////////////////////////////////////
//
// OpenGl_DeviceStateCreate
//
////////////////////////////////////////////////////////////////////////////////
int OpenGl_DeviceStateCreate()
{
   if (grGetMode() == OPENGL_MODE_32)
   {
      glGenSamplers(1, &samplerCanvas2D);

      glSamplerParameteri(samplerCanvas2D, GL_TEXTURE_2D, GL_TRUE);
      glSamplerParameteri(samplerCanvas2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glSamplerParameteri(samplerCanvas2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glSamplerParameteri(samplerCanvas2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glSamplerParameteri(samplerCanvas2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glSamplerParameteri(samplerCanvas2D, GL_TEXTURE_MIN_LOD, 0);
      glSamplerParameteri(samplerCanvas2D, GL_TEXTURE_MAX_LOD, 12);
      //
      glGenSamplers(1, &samplerFont);
      glSamplerParameteri(samplerFont, GL_TEXTURE_2D, GL_TRUE);
      glSamplerParameteri(samplerFont, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glSamplerParameteri(samplerFont, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glSamplerParameteri(samplerFont, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glSamplerParameteri(samplerFont, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glSamplerParameteri(samplerFont, GL_TEXTURE_MIN_LOD, 0);
      glSamplerParameteri(samplerFont, GL_TEXTURE_MAX_LOD, 12);
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// OpenGl_DeviceStateRelease
//
////////////////////////////////////////////////////////////////////////////////
int OpenGl_DeviceStateRelease()
{
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// grStateRasterizer
//
////////////////////////////////////////////////////////////////////////////////
void grStateRasterizer(Rasterizer r)
{
   switch(r) {
   case RASTERIZER_DEFAULT:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      break;
   case RASTERIZER_DEFAULT_CW:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CW);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
   case RASTERIZER_CULLNONE:
      glDisable(GL_CULL_FACE);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      break;
   case RASTERIZER_WIRE:
      glDisable(GL_CULL_FACE);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      break;
   default:
      CORE_ERROR("invalid rasterizer state",0);
   };
}

////////////////////////////////////////////////////////////////////////////////
//
// grStateBlendMode
//
////////////////////////////////////////////////////////////////////////////////
void grStateBlendMode(BlendMode bm)
{
   switch(bm) {
   case BLEND_MODE_COPY:
      glEnable(GL_BLEND);
      glBlendFuncSeparate(GL_ONE, GL_ZERO,GL_ONE, GL_ZERO);
      glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
      break;
   case BLEND_MODE_ADD:
      glEnable(GL_BLEND);
      glBlendFuncSeparate(GL_ONE, GL_ONE,GL_ONE,GL_ONE);
      glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
      break;
   case BLEND_MODE_MUL:
      glEnable(GL_BLEND);
      glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO,GL_DST_ALPHA,GL_ZERO);
      glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
      break;
   case BLEND_MODE_ALPHA:
      glEnable(GL_BLEND);
      glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
      break;
   default:
      CORE_ERROR("invalid blend mode",0);
   };
}

////////////////////////////////////////////////////////////////////////////////
//
// grStateSampler
//
////////////////////////////////////////////////////////////////////////////////
void grStateSampler(int index,SamplerState ss)
{
   if (grGetMode() == OPENGL_MODE_21)
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   }

   if (grGetMode() == OPENGL_MODE_32)
   {
      switch (ss) {
      case SAMPLE_STATE_CANVAS2D:
         glBindSampler(index, samplerCanvas2D);
         break;
      case SAMPLE_STATE_FONT:
         glBindSampler(index, samplerFont);
         break;
      default:
         CORE_ERROR("invalid sampler state", 0);
      };
   }
}

void grStateDepthStencil(DepthStencil dm)
{
   switch( dm ) {
   case DEPTH_STENCIL_ON:
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_STENCIL_TEST);
      break;
   case DEPTH_STENCIL_OFF:
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_STENCIL_TEST);
      break;
   case DEPTH_ON:
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_STENCIL_TEST);
      glDepthFunc(GL_LEQUAL);
      break;
   };
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
