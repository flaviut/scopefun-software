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

///////////////////////////////////////////////////////////////////////////////
//
// helper
//
////////////////////////////////////////////////////////////////////////////////
uint ConvertTopology( PrimitiveType type )
{
   switch( type ) {
   case PRIMITIVE_TYPE_POINT_LIST           : return GL_POINTS;
   case PRIMITIVE_TYPE_LINE_LIST            : return GL_LINES;
   case PRIMITIVE_TYPE_LINE_STRIP           : return GL_LINE_STRIP;
   case PRIMITIVE_TYPE_TRIANGLE_LIST        : return GL_TRIANGLES;
   case PRIMITIVE_TYPE_TRIANGLE_STRIP       : return GL_TRIANGLE_STRIP;
   case PRIMITIVE_TYPE_LINE_LIST_ADJ        : return GL_LINES_ADJACENCY;
   case PRIMITIVE_TYPE_LINE_STRIP_ADJ       : return GL_LINE_STRIP_ADJACENCY;
   case PRIMITIVE_TYPE_TRIANGLE_LIST_ADJ    : return GL_TRIANGLES_ADJACENCY;
   case PRIMITIVE_TYPE_TRIANGLE_STRIP_ADJ   : return GL_TRIANGLE_STRIP_ADJACENCY;
   default : return 0;
   };
}

uint ConvertFormatToDim( VertexFormat format )
{
   switch( format ) {
   case VERTEX_FORMAT_COLOR:    return 1;
   case VERTEX_FORMAT_FLOAT4 : return 4;
   case VERTEX_FORMAT_FLOAT3 : return 3;
   case VERTEX_FORMAT_FLOAT2 : return 2;
   default : return 0;
   };
}

unsigned int Offset( VertexFormat format )
{
   switch( format ) {
   case VERTEX_FORMAT_COLOR:    return 4;
   case VERTEX_FORMAT_FLOAT4 : return 16;
   case VERTEX_FORMAT_FLOAT3 : return 12;
   case VERTEX_FORMAT_FLOAT2 : return 8;
   default : return 0;
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Static Mesh
//
////////////////////////////////////////////////////////////////////////////////
GrStaticMesh::GrStaticMesh()
{
   meshMemory = 0;
   meshSize = 0;
   meshId = 0;
   //
   vao = 0;
   //
   lockedOffset = 0;
   lockedMemory = 0;
   lockedBytes = 0;
}

void GrStaticMesh::createBuffer(uint vbsize)
{
   meshMemory = (byte*)memoryAllocate(vbsize);
   SDL_memset((void*)meshMemory, 0, vbsize);
   meshSize   = vbsize;
   //
   glGenBuffers(1, &meshId);
   glBindBuffer(GL_ARRAY_BUFFER, meshId);
   glBufferData(GL_ARRAY_BUFFER, vbsize, meshMemory, GL_DYNAMIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   if (grGetMode() == OPENGL_MODE_32)
   {
      glGenVertexArrays(1, &vao);
   }
}

byte* GrStaticMesh::lock(uint bytes)
{
   if (lockedOffset + bytes > meshSize)
   {
      // CORE_ERROR("out of static mesh memory", 0);
      lockedOffset = 0;
      if (bytes > meshSize)
      {
         bytes = meshSize;
      }
   }
      
   lockedBytes  = bytes;

   lockedMemory = meshMemory + lockedOffset;
  
   return lockedMemory;
}

void GrStaticMesh::unlock()
{
   lockedOffset += lockedBytes;
}

void GrStaticMesh::bind()
{
   if (grGetMode() == OPENGL_MODE_32)
   {
      glBindBuffer(GL_ARRAY_BUFFER, meshId);
      void *dest = glMapBufferRange(GL_ARRAY_BUFFER, 0, lockedOffset, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
      SDL_memcpy(dest, meshMemory, lockedOffset);
      glUnmapBuffer(GL_ARRAY_BUFFER);
   }
   else
   {
      glBindBuffer(GL_ARRAY_BUFFER, meshId);
      glBufferSubData(GL_ARRAY_BUFFER, 0, lockedOffset, meshMemory);
   }
}

void GrStaticMesh::unbind()
{

}

void GrStaticMesh::frameStart()
{
   lockedOffset = 0;
   lockedMemory = 0;
   lockedBytes  = 0;
}

void GrStaticMesh::draw(GrVertexShader* vs,PrimitiveType type,uint numIndices,uint startoffset)
{
   // declaration
   GrVertexDeclaration &decl = vs->declaration;

   // stride
   uint stride = 0;
   for(int a=0;a<decl.element.getCount();a++)
   {
      if( decl.element[a] )
         stride += Offset( decl.element[a]->elFormat );
   }
  
   if (grGetMode() == OPENGL_MODE_32)
   {
      // vao
      glBindVertexArray(vao);
   }

   // buffer
   glBindBuffer(GL_ARRAY_BUFFER, meshId);

   // enable
   for (int a = 0; a < decl.element.getCount(); a++)
   {
      if (grGetMode() == OPENGL_MODE_32)
         glEnableVertexAttribArray(a);
   }

   // define vertex data
   uint offset = 0;
   for(int a=0;a<decl.element.getCount();a++)
   {      
      GrVertexElement* el = decl.element[a];
      uint dim = ConvertFormatToDim( el->elFormat );

      uint format = 0;
      uint normalized = GL_FALSE;
      if (el->elFormat == VERTEX_FORMAT_COLOR)
      {
         dim = 4;
         format = GL_UNSIGNED_BYTE;
         normalized = GL_TRUE;
      }       
      else format = GL_FLOAT;      

      switch( el->elType ) {
      case VERTEX_TYPE_BINORMAL:
         break;
      case VERTEX_TYPE_BLEND_INDICES:
      case VERTEX_TYPE_BLEND_WEIGHT:
      case VERTEX_TYPE_COLOR:
         if (grGetMode() == OPENGL_MODE_21)
         {
            glColorPointer(dim, format, stride, (GLvoid*)(startoffset + offset));
            glEnableClientState(GL_COLOR_ARRAY);
         }
         else
         {
            glVertexAttribPointer(a, dim, format, normalized, stride, (GLvoid*)(startoffset + offset));
         }
         break;
      case VERTEX_TYPE_NORMAL:
         if (grGetMode() == OPENGL_MODE_21)
         {
            glNormalPointer(GL_FLOAT, stride, (GLvoid*)(startoffset + offset));
            glEnableClientState(GL_NORMAL_ARRAY);
         }
         else
         {
            glVertexAttribPointer(a, dim, format, normalized, stride, (GLvoid*)(startoffset + offset));
         }
         break;
      case VERTEX_TYPE_POSITION:
         if (grGetMode() == OPENGL_MODE_21)
         {           
            glVertexPointer(dim, format, stride, (GLvoid*)(startoffset + offset));
            glEnableClientState(GL_VERTEX_ARRAY);
         }
         else
         {
            glVertexAttribPointer(a, dim, format, normalized, stride, (GLvoid*)(startoffset + offset));
         }         
         break;
      case VERTEX_TYPE_POSITIONT:
      case VERTEX_TYPE_PSIZE:
      case VERTEX_TYPE_TANGENT:
         break;
      case VERTEX_TYPE_TEXCOORD:
         if (grGetMode() == OPENGL_MODE_21)
         {
            glTexCoordPointer(dim, format, stride, (GLvoid*)(startoffset + offset));
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
         }
         else
         {
            glVertexAttribPointer(a, dim, format, normalized, stride, (GLvoid*)(startoffset + offset));
         }
         break;
      };
      offset += Offset( el->elFormat );
   }

   // draw
   glDrawArrays(ConvertTopology(type), 0, numIndices);

   // disable
   for(int a=0;a<decl.element.getCount();a++)
   {
         glDisableVertexAttribArray(a);
   };
   
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   if (grGetMode() == OPENGL_MODE_32)
   {
      glBindVertexArray(0);    
   }
}

void GrStaticMesh::release()
{
   memoryFree(meshMemory);
}

////////////////////////////////////////////////////////////////////////////////
//
// G_StaticMeshCreate
//
////////////////////////////////////////////////////////////////////////////////
GrStaticMesh *grCreateStaticMesh()
{
   return new GrStaticMesh();
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////

