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

extern uint frameBufferIndex;

////////////////////////////////////////////////////////////////////////////////
//
// OpenGl
//
////////////////////////////////////////////////////////////////////////////////
GrTexture::GrTexture()
{
   textureIndex = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// GrTextureCreate
//
////////////////////////////////////////////////////////////////////////////////
GrTexture *grCreateRenderTarget(RenderTargetType type,int width,int height)
{
   GrTexture *pGlTexture = (GrTexture*)new GrTexture();

   glGenTextures(1, &pGlTexture->textureIndex);
   glBindTexture(GL_TEXTURE_2D, pGlTexture->textureIndex);

   pGlTexture->width  = width;
   pGlTexture->height = height;
   pGlTexture->mips   = 0;

   switch( type ) {
   case RENDER_TARGET_TYPE_RGBA:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
      pGlTexture->format = GL_RGBA;
      pGlTexture->internalFormat = GL_RGBA8;
      pGlTexture->type = GL_UNSIGNED_BYTE;
      break;
   case RENDER_TARGET_TYPE_FLOAT32:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, 0);
      pGlTexture->format = GL_RED;
      pGlTexture->internalFormat = GL_R32F;
      pGlTexture->type = GL_FLOAT;
      break;
   case RENDER_TARGET_TYPE_FLOAT16:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, 0);
      pGlTexture->format = GL_RED;
      pGlTexture->internalFormat = GL_R16F;
      pGlTexture->type = GL_FLOAT;
      break;
   };

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glBindTexture(GL_TEXTURE_2D, 0);
   return pGlTexture;
}

GrTexture *grCreateDepthStencil(DepthStencilType type,int width,int height)
{
   GrTexture *pGlTexture = (GrTexture *)new GrTexture();

   pGlTexture->width  = width;
   pGlTexture->height = height;
   pGlTexture->mips   = 0;
   pGlTexture->format = GL_DEPTH_COMPONENT;
   pGlTexture->internalFormat = GL_DEPTH_COMPONENT32F;
   pGlTexture->type = GL_FLOAT;

   glGenRenderbuffers(1, &pGlTexture->textureIndex);
   glBindRenderbuffer(GL_RENDERBUFFER, pGlTexture->textureIndex);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height);
   glBindRenderbuffer(GL_RENDERBUFFER, 0);

   return pGlTexture;
}

GrTexture *grCreateTexture(const char* fileName)
{
   GrTexture* pGlTexture = (GrTexture*)new GrTexture();

   ilarge memorySize = 0;
   fileLoad(fileName, (char**)&pGlTexture->memory, &memorySize);

   DdsHeader *header = (DdsHeader*)( pGlTexture->memory + sizeof(uint) );
   byte*       data  = pGlTexture->memory + sizeof(DdsHeader) + sizeof(uint);

   // allocate a texture name
   glGenTextures( 1, &pGlTexture->textureIndex );

   // select our current texture
   glActiveTexture(GL_TEXTURE0);
   glBindTexture( GL_TEXTURE_2D, pGlTexture->textureIndex );

   uint offset    = 0;
   uint width     = header->dwWidth;
   uint height    = header->dwHeight;
   uint mips      = header->dwMipMapCount+1;

   uint   blockSize = 0;
   GLenum format    = 0;
   if( memcmp( &header->ddpf.dwFourCC, "DXT1", 4 ) == 0 )
   {
      format    = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      blockSize = 8;
   }
   if( memcmp( &header->ddpf.dwFourCC, "DXT3", 4 ) == 0 )
   {
      format    = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      blockSize = 16;
   }
   if( memcmp( &header->ddpf.dwFourCC, "DXT5", 4 ) == 0 )
   {
      format    = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      blockSize = 16;
   }

   pGlTexture->width  = width;
   pGlTexture->height = height;
   pGlTexture->mips   = mips;
   pGlTexture->format = format;

   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_LOD,0);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LOD,12);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,12);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   for(uint i=0;i<mips;i++)
   {
      uint buffSize = (((width + 3)/4) * ((height+3)/4))*blockSize;

      glCompressedTexImage2D(GL_TEXTURE_2D,  i,  format,  width,  height,  0,  buffSize, &data[offset] );

      offset +=  buffSize;
      width  /= 2;
      height /= 2;
      if( width  == 0 ) width  = 1;
      if( height == 0 ) height = 1;
   }

   glBindTexture(GL_TEXTURE_2D, 0);

   return pGlTexture;
}

void grSetRenderTarget(GrTexture *depth,GrTexture *color)
{
   if (grGetMode() == OPENGL_MODE_32)
   {
      if (depth == 0 && color == 0)
      {
         glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
      else
      {
         glBindFramebuffer(GL_FRAMEBUFFER, frameBufferIndex);

         if (color)
         {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color->textureIndex, 0);
         }
         if (depth)
         {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth->textureIndex);
         }
      }
   }
}

void grClearRenderTarget(GrTexture *rt,int color)
{
   Vector4 c = vecFromColor(color);
   glClearColor( c.x, c.y, c.z, c.w );
   glClear( GL_COLOR_BUFFER_BIT );
}

void grClearDepthStencil(GrTexture *depth,int flags,float z,int stencil)
{
   glClearDepth(z);
   glClearStencil(stencil);
   glClear( GL_DEPTH_BUFFER_BIT );
   glClear( GL_STENCIL_BUFFER_BIT );
}

void grCopyToBackBuffer(GrTexture *src,int x, int y,int width,int height)
{
   if (!src)return;
   if (grGetMode() == OPENGL_MODE_32)
   {
      grStateDepthStencil(DEPTH_STENCIL_OFF);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferIndex);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glDrawBuffer(GL_BACK);
      glBlitFramebuffer(0, 0, src->width, src->height, 0, 0, src->width, src->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
   }
}

void grCopyRenderTarget(GrTexture *dest,GrTexture *src)
{
   if (!dest)return;
   if (!src)return;
   if (grGetMode() == OPENGL_MODE_32)
   {
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dest->textureIndex, 0);
      glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src->textureIndex, 0);
      glBlitFramebuffer(0, 0, src->width, src->height, 0, 0, dest->width, dest->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
   }
}

void grResizeRenderTarget(GrTexture *target,int width,int height)
{
   if (!target)return;
   if (grGetMode() == OPENGL_MODE_32)
   {
      target->width = width;
      target->height = height;
      glBindFramebuffer(GL_FRAMEBUFFER, target->frameBufferIndex);
      glBindTexture(GL_TEXTURE_2D, target->textureIndex);
      glTexImage2D(GL_TEXTURE_2D, 0, target->internalFormat, width, height, 0, target->format, target->type, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }
}

void grResizeDepthStencil(GrTexture *target,int width,int height)
{
   if (!target)return;
   if (grGetMode() == OPENGL_MODE_32)
   {
      target->width = width;
      target->height = height;
      glBindFramebuffer(GL_FRAMEBUFFER, target->frameBufferIndex);
      glBindRenderbuffer(GL_RENDERBUFFER, target->textureIndex);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
