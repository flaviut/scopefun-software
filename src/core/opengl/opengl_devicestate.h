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
#ifndef __OPENGL__DEVICESTATE__
#define __OPENGL__DEVICESTATE__

enum BlendMode {
   BLEND_MODE_COPY,
   BLEND_MODE_ADD,
   BLEND_MODE_MUL,
   BLEND_MODE_ALPHA,
   BLEND_MODE_LAST
};

enum Rasterizer {
   RASTERIZER_DEFAULT,
   RASTERIZER_DEFAULT_CW,
   RASTERIZER_CULLNONE,
   RASTERIZER_WIRE,
   RASTERIZER_LAST
};

enum SamplerState {
   SAMPLE_STATE_FONT,
   SAMPLE_STATE_CANVAS2D,
   SAMPLE_STATE_LAST
};

enum DepthStencil {
   DEPTH_STENCIL_ON,
   DEPTH_STENCIL_OFF,
   DEPTH_ON
};

////////////////////////////////////////////////////////////////////////////////
//
// functions
//
////////////////////////////////////////////////////////////////////////////////
void grStateRasterizer(Rasterizer r);
void grStateBlendMode(BlendMode bm);
void grStateDepthStencil(DepthStencil dm);
void grStateSampler(int index,SamplerState ss);

////////////////////////////////////////////////////////////////////////////////
//
// functions
//
////////////////////////////////////////////////////////////////////////////////
int OpenGl_DeviceStateCreate();
int OpenGl_DeviceStateRelease();

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
