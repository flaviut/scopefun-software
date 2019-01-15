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
#ifndef __OPENGL__SHADER__
#define __OPENGL__SHADER__

////////////////////////////////////////////////////////////////////////////////
// VertexType
////////////////////////////////////////////////////////////////////////////////
enum VertexType {
   VERTEX_TYPE_BINORMAL,
   VERTEX_TYPE_BLEND_INDICES,
   VERTEX_TYPE_BLEND_WEIGHT,
   VERTEX_TYPE_COLOR,
   VERTEX_TYPE_NORMAL,
   VERTEX_TYPE_POSITION,
   VERTEX_TYPE_POSITIONT,
   VERTEX_TYPE_PSIZE,
   VERTEX_TYPE_TANGENT,
   VERTEX_TYPE_TEXCOORD
};

////////////////////////////////////////////////////////////////////////////////
// VertexFormat
////////////////////////////////////////////////////////////////////////////////
enum VertexFormat {
   VERTEX_FORMAT_FLOAT2,
   VERTEX_FORMAT_FLOAT3,
   VERTEX_FORMAT_FLOAT4,
   VERTEX_FORMAT_COLOR,
};

////////////////////////////////////////////////////////////////////////////////
//
// GrVertexElement
//
////////////////////////////////////////////////////////////////////////////////
class GrVertexElement {
public:
   const char*  name;
   VertexType   elType;
   int          index;
   VertexFormat elFormat;
public:
   GrVertexElement(const char* id,VertexType type,int idx,VertexFormat format)
   {
      name     = id;
      elType   = type;
      index    = idx;
      elFormat = format;
   }
};

////////////////////////////////////////////////////////////////////////////////
//
// GrVertexDeclaration
//
////////////////////////////////////////////////////////////////////////////////
class GrVertexDeclaration {
public:
   Array<GrVertexElement*,8> element;
public:
   GrVertexDeclaration(){};
   GrVertexDeclaration(GrVertexElement *first,...)
   {
      GrVertexElement *el = first;
      va_list marker;
      va_start( marker, first );
      while( el != 0 )
      {
         element.pushBack(el);
         //
         el = va_arg( marker, GrVertexElement* );
      }
      va_end( marker );
   }
};

////////////////////////////////////////////////////////////////////////////////
//
// GrShaderDefine
//
////////////////////////////////////////////////////////////////////////////////
class GrShaderDefine {
public:
   Array<String,16> define;
public:
   GrShaderDefine() {};
   GrShaderDefine(const char* first,...)
   {
      char* el = (char*)first;
      va_list marker;
      va_start( marker, first );
      while( el != 0 )
      {
         define.pushBack( /*String(*/el/*)*/ );
         //
         el = (char*)va_arg( marker, char* );
      }
      va_end( marker );
   }
};

////////////////////////////////////////////////////////////////////////////////
//
// GrConstantBuffer
//
////////////////////////////////////////////////////////////////////////////////
class GrConstantBuffer {
public:
   uint                id;
   uint                uboIndex;
   Array<Vector4,256> element;
public:
   GrConstantBuffer();
public:
   void* Lock();
   void  Unlock();
};

////////////////////////////////////////////////////////////////////////////////
//
// GrVertexShader
//
////////////////////////////////////////////////////////////////////////////////
class GrVertexShader {
public:
   uint                 index;
   GrVertexDeclaration  declaration;
public:
   GrVertexShader();
};

////////////////////////////////////////////////////////////////////////////////
//
// GrPixelShader
//
////////////////////////////////////////////////////////////////////////////////
class GrPixelShader {
public:
    uint index;
public:
   GrPixelShader();
};

////////////////////////////////////////////////////////////////////////////////
//
// GrShader
//
////////////////////////////////////////////////////////////////////////////////
class GrShader {
public:
   uint   uniformVecSet[32];
   uint   uniformVecLocation[32];
   uint   uniformMatSet[32];
   uint   uniformMatLocation[32];
public:
   uint   constantBuffer[4];
   uint   psSampler[4];
   uint   vsSampler[4];
   uint   program;
public:
   GrVertexShader* vertexShader;
   GrPixelShader*  pixelShader;
public:
   GrShader();
public:
   void compile(const char* file,GrVertexDeclaration vertexDeclaration,GrShaderDefine defines);
   void activate();
public:
   void SetUniform4(int index, const char* name, const Vector4& vector);
   void SetUniform4x4(int index, const char* name, const Matrix4x4& mat);
};

////////////////////////////////////////////////////////////////////////////////
//
// functions
//
////////////////////////////////////////////////////////////////////////////////
GrConstantBuffer* grCreateConstantBuffer(int id,int size);
GrShader*         grCreateShader();

////////////////////////////////////////////////////////////////////////////////
//
// functions
//
////////////////////////////////////////////////////////////////////////////////
void grSetVertexShaderTexture(GrShader* shader,int index,GrTexture *texture);
void grSetPixelShaderTexture(GrShader* shader,int index,GrTexture *texture);

////////////////////////////////////////////////////////////////////////////////
//
// functions
//
////////////////////////////////////////////////////////////////////////////////
void grActivateShader(uint id);


#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
