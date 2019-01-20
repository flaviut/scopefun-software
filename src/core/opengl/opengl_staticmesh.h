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
#ifndef __OPENGL__STATICMESH__
#define __OPENGL__STATICMESH__


////////////////////////////////////////////////////////////////////////////////
//
// primitivetype
//
////////////////////////////////////////////////////////////////////////////////
enum PrimitiveType {
   PRIMITIVE_TYPE_POINT_LIST,
   PRIMITIVE_TYPE_LINE_LIST,
   PRIMITIVE_TYPE_LINE_STRIP,
   PRIMITIVE_TYPE_TRIANGLE_LIST,
   PRIMITIVE_TYPE_TRIANGLE_STRIP,
   PRIMITIVE_TYPE_LINE_LIST_ADJ,
   PRIMITIVE_TYPE_LINE_STRIP_ADJ,
   PRIMITIVE_TYPE_TRIANGLE_LIST_ADJ,
   PRIMITIVE_TYPE_TRIANGLE_STRIP_ADJ
};

////////////////////////////////////////////////////////////////////////////////
//
// StaticMesh
//
////////////////////////////////////////////////////////////////////////////////
class GrStaticMesh {
public:
   byte* meshMemory;
   uint  meshSize;
   uint  meshId;
public:
   uint  vao;
public:
   uint  lockedOffset;
   byte* lockedMemory;
   uint  lockedBytes;
public:
   GrStaticMesh();
public:
   void createBuffer(uint vbsize);
public:
   byte* lock(uint bytes);
   void  unlock();
   void  bind();
   void  unbind();
public:
   void draw(GrVertexShader* vs,PrimitiveType primtype,uint numvertices,uint startoffset);
   void frameStart();
public:
   void release();
};

////////////////////////////////////////////////////////////////////////////////
//
// functions
//
////////////////////////////////////////////////////////////////////////////////
GrStaticMesh* grCreateStaticMesh();


#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
