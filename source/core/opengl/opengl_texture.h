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
#ifndef __OPENGL__TEXTURE__
#define __OPENGL__TEXTURE__

enum RenderTargetType {
   RENDER_TARGET_TYPE_RGBA,
   RENDER_TARGET_TYPE_FLOAT32,
   RENDER_TARGET_TYPE_FLOAT16
};

enum DepthStencilType{
   DEPTH_STENCIL_TYPE_Z24S8,
   DEPTH_STENCIL_TYPE_Z32
};


////////////////////////////////////////////////////////////////////////////////
//
// DdsPixelFormat
//
////////////////////////////////////////////////////////////////////////////////
class DdsPixelFormat {
public:
  uint dwSize;
  uint dwFlags;
  uint dwFourCC;
  uint dwRGBBitCount;
  uint dwRBitMask;
  uint dwGBitMask;
  uint dwBBitMask;
  uint dwABitMask;
};

////////////////////////////////////////////////////////////////////////////////
//
// DDSHeader
//
////////////////////////////////////////////////////////////////////////////////
class DdsHeader {
public:
  uint            dwSize;
  uint            dwFlags;
  uint            dwHeight;
  uint            dwWidth;
  uint            dwLinearSize;
  uint            dwDepth;
  uint            dwMipMapCount;
  uint            dwReserved1[11];
  DdsPixelFormat  ddpf;
  uint            dwCaps;
  uint            dwCaps2;
  uint            dwCaps3;
  uint            dwCaps4;
  uint            dwReserved2;
};

////////////////////////////////////////////////////////////////////////////////
//
// texture
//
////////////////////////////////////////////////////////////////////////////////
class GrTexture {
public:
   uint    frameBufferIndex;
   uint    textureIndex;
   byte*   memory;
   int     mips;
   uint    width;
   uint    height;
   uint    format;
   int     internalFormat;
   uint    type;
public:
   GrTexture();
};

////////////////////////////////////////////////////////////////////////////////
//
// functions
//
////////////////////////////////////////////////////////////////////////////////
GrTexture* grCreateRenderTarget(RenderTargetType type,int width,int height);
GrTexture* grCreateDepthStencil(DepthStencilType type,int width,int height);
GrTexture* grCreateTexture(const char* filename);

////////////////////////////////////////////////////////////////////////////////
//
// functions
//
////////////////////////////////////////////////////////////////////////////////
void grSetRenderTarget(GrTexture *depth,GrTexture *pColor);
void grClearRenderTarget(GrTexture *rt,int color);
void grClearDepthStencil(GrTexture *depth,int flags,float z,int stencil);
void grCopyToBackBuffer(GrTexture *color,int x,int y,int w,int h);
void grCopyRenderTarget(GrTexture *dest,GrTexture *src);
void grResizeRenderTarget(GrTexture *target,int width,int height);
void grResizeDepthStencil(GrTexture *target,int width,int height);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
