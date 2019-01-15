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
// constant buffer
//
////////////////////////////////////////////////////////////////////////////////
GrConstantBuffer::GrConstantBuffer()
{
}

void *GrConstantBuffer::Lock()
{
   return &element[0];
}

void GrConstantBuffer::Unlock()
{
   glBindBuffer(GL_UNIFORM_BUFFER, uboIndex );
   glBufferSubData(GL_UNIFORM_BUFFER, 0, element.getSize(), &element[0] );
}

////////////////////////////////////////////////////////////////////////////////
//
// Shader Compiler
//
////////////////////////////////////////////////////////////////////////////////
void CompileShader(const char* shaderName,GLuint &shader,const char* file,ilarge size,GrShaderDefine &def)
{
   ////////////////////////////////////////////////////////////////////////////////
   // macros
   ////////////////////////////////////////////////////////////////////////////////
   if( def.define >= 64 )
      CORE_ABORT("max number shader defines reached",0);
   //
   char*   macro[64] = {0};
   String  string[64];
   if (grGetMode() == OPENGL_MODE_21)
   {
      macro[0] = (char*)"#version 110\n";
   }
   else
   {
      macro[0] = (char*)"#version 150 core\n";
   }
   int a=1;
   for(;a<def.define.getCount()+1;a++)
   {
      string[a] = def.define.index(a-1);
      string[a].insert(0,"#define ");
      string[a].insert( string[a].getLength(),"\n");
      //
      macro[a] = string[a].asChar();
   }
   macro[a] = (char*)file;
   a++;

   ////////////////////////////////////////////////////////////////////////////////
   // compile
   ////////////////////////////////////////////////////////////////////////////////
   glShaderSource(shader, a, (const char**)macro, 0);
   glCompileShader(shader);

   ////////////////////////////////////////////////////////////////////////////////
   // errors
   ////////////////////////////////////////////////////////////////////////////////
   GLint compiled;
   glGetObjectParameterivARB(shader, GL_COMPILE_STATUS, &compiled);
   if( !compiled )
   {
      GLint blen = 0;
      GLsizei slen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &blen);
      if(blen > 1)
      {
         GLchar* compiler_log = (GLchar*)memoryAllocate( blen );
         glGetInfoLogARB( shader, blen, &slen, compiler_log );
         CORE_ERROR(" %s \n %s", shaderName, compiler_log);
         memoryFree(compiler_log);
      }
   }
}


////////////////////////////////////////////////////////////////////////////////
//
// Vertex Shader
//
////////////////////////////////////////////////////////////////////////////////
GrVertexShader::GrVertexShader()
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Pixel Shader
//
////////////////////////////////////////////////////////////////////////////////
GrPixelShader::GrPixelShader()
{
}

////////////////////////////////////////////////////////////////////////////////
//
// grConstantBufferCreate
//
////////////////////////////////////////////////////////////////////////////////
GrConstantBuffer  *grCreateConstantBuffer(int id,int size)
{
   GrConstantBuffer *cb = new GrConstantBuffer();
   //
   cb->element.setCount( size );
   cb->id = id;
   //
   glGenBuffers( 1, &cb->uboIndex );
   glBindBuffer(GL_UNIFORM_BUFFER, cb->uboIndex );
   glBufferData(GL_UNIFORM_BUFFER, size*sizeof(Vector4), 0, GL_DYNAMIC_DRAW);
   glBindBufferBase(GL_UNIFORM_BUFFER, cb->id, cb->uboIndex );
   glBindBuffer(GL_UNIFORM_BUFFER, 0);
   return cb;
}

////////////////////////////////////////////////////////////////////////////////
//
// OpenGlShader
//
////////////////////////////////////////////////////////////////////////////////
void GrShader::compile(const char* file,GrVertexDeclaration decl,GrShaderDefine def)
{
   static String dfx = ".fx";
   static String dvs = ".vs";
   static String dps = ".ps";
   static String shd21 = "data/shader21/";
   static String shd32 = "data/shader32/";
   String vsName = file;
          vsName.replace(dfx,dvs);
   String psName = file;
          psName.replace(dfx,dps);

   if ( grGetMode() == OPENGL_MODE_21 )
   {
      vsName.insert(0, shd21);
      psName.insert(0, shd21);
   }
   else
   {
      vsName.insert(0, shd32);
      psName.insert(0, shd32);
   }

   FORMAT_BUFFER();
   FORMAT_PATH( vsName.asChar() );
   vsName = formatBuffer;
   FORMAT_PATH( psName.asChar() );
   psName = formatBuffer;

   ////////////////////////////////////////////////////////
   // vertex shader
   ////////////////////////////////////////////////////////
   GrVertexShader* vs = vertexShader;
   vs->declaration = decl;
   {
      char*  memory = 0;
      ilarge memorySize = 0;
      fileLoadString(vsName.asChar(), &memory, &memorySize);

      CompileShader(file, vs->index, memory, memorySize, def);

      fileFree(memory);
   }

   ////////////////////////////////////////////////////////
   // pixel shader
   ////////////////////////////////////////////////////////
   GrPixelShader* ps = pixelShader;
   {
      char*  memory = 0;
      ilarge memorySize = 0;
      fileLoadString(psName.asChar(), &memory, &memorySize);

      CompileShader(file,ps->index, memory, memorySize, def);

      fileFree(memory);
   }
   //
   program = glCreateProgram();
   //
   glAttachShader(program, vs->index);
   glAttachShader(program, ps->index);
   //
   glDeleteShader(vs->index);
	glDeleteShader(ps->index);
	//
   for (int i = 0; i < decl.element.getCount(); i++)
   {            
      glBindAttribLocation(program, i/*decl.element[i]->index*/, decl.element[i]->name );
   }   

   ////////////////////////////////////////////////////////
   // link
   ////////////////////////////////////////////////////////
   glLinkProgram(program);
   GLint link_ok;
   glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
   if(!link_ok)
   {
      GLint blen = 0;
      GLsizei slen = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH , &blen);
      if(blen > 1)
      {
         GLchar* link_log = (GLchar*)memoryAllocate(blen);
         glGetInfoLogARB( program, blen, &slen, link_log );
         CORE_ERROR( "%s", link_log );
         memoryFree(link_log);
      }
   }

   ////////////////////////////////////////////////////////
   // sampler indices
   ////////////////////////////////////////////////////////
   for(int i=0;i<4;i++)
   {
      FORMAT_BUFFER();

      FORMAT( "gVSTexture%d", i );
      vsSampler[i] = glGetUniformLocation(program, formatBuffer );

      FORMAT( "gPSTexture%d", i );
      psSampler[i] = glGetUniformLocation(program, formatBuffer );
   }

   if (grGetMode() == OPENGL_MODE_32)
   {
      ////////////////////////////////////////////////////////
      // cb indices
      ////////////////////////////////////////////////////////
      for (int i = 0; i < 4; i++)
      {
         FORMAT_BUFFER();
         FORMAT("VSCBuffer%d", i);
         constantBuffer[i] = glGetUniformBlockIndex(program, formatBuffer);
         if (constantBuffer[i] != GL_INVALID_INDEX)
            glUniformBlockBinding(program, constantBuffer[i], i);
      }
      for (int i = 0; i < 4; i++)
      {
         FORMAT_BUFFER();
         FORMAT("PSCBuffer%d", i);
         constantBuffer[i] = glGetUniformBlockIndex(program, formatBuffer);
         if (constantBuffer[i] != GL_INVALID_INDEX)
            glUniformBlockBinding(program, constantBuffer[i], i);
      }
   }
}

void GrShader::SetUniform4(int index, const char* name, const Vector4& vector)
{
   int i = clamp(index, 0, 32);
   if ( uniformVecSet[i] == 0 )
   {
      uniformVecLocation[i] = (uint)glGetUniformLocation(program, name);
      uniformVecSet[i] = 1;
   }

   glUniform4f( uniformVecLocation[i], vector.x, vector.y, vector.z, vector.w);
}

void GrShader::SetUniform4x4(int index, const char* name, const Matrix4x4& mat)
{
   int i = clamp(index, 0, 32);
   if (uniformMatSet[i] == 0)
   {
      uniformMatLocation[i] = (uint)glGetUniformLocation(program, name);
      uniformMatSet[i] = 1;
   }

   glUniformMatrix4fv(uniformMatLocation[i], 1, false, &mat.m11);
}

void GrShader::activate()
{
   glUseProgram(program);
}

////////////////////////////////////////////////////////////////////////////////
//
// grCreateShader
//
////////////////////////////////////////////////////////////////////////////////
GrShader  *grCreateShader()
{
   GrShader *shd = new GrShader();
   //
   GrVertexShader *glVS = new GrVertexShader();
   glVS->index = glCreateShader(GL_VERTEX_SHADER);
   shd->vertexShader = glVS;
   //
   GrPixelShader *glPS = new GrPixelShader();
   glPS->index = glCreateShader(GL_FRAGMENT_SHADER);
   shd->pixelShader = glPS;
   //
   return shd;
}

////////////////////////////////////////////////////////////////////////////////
//
// OpenGlShader
//
////////////////////////////////////////////////////////////////////////////////
GrShader::GrShader()
{
   memset(uniformVecSet, 0, sizeof(uniformVecSet));
   memset(uniformVecLocation, 0, sizeof(uniformVecLocation));

   memset(uniformMatSet, 0, sizeof(uniformMatSet));
   memset(uniformMatLocation, 0, sizeof(uniformMatLocation));

   memset(constantBuffer, 0, sizeof(constantBuffer));

   memset(psSampler, 0, sizeof(psSampler));
   memset(vsSampler, 0, sizeof(vsSampler));

   program = 0;

   vertexShader = 0;
   pixelShader = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// textures
//
////////////////////////////////////////////////////////////////////////////////
void grSetVertexShaderTexture(GrShader* shader,int slot,GrTexture *pTexture)
{
   glActiveTexture(GL_TEXTURE0 + slot);
   if( pTexture )
      glBindTexture( GL_TEXTURE_2D, pTexture->textureIndex );
   if( shader )
      glUniform1i( shader->vsSampler[slot], slot );
}

void grSetPixelShaderTexture(GrShader* shader,int slot,GrTexture *pTexture)
{
   glActiveTexture(GL_TEXTURE0 + slot );
   if( pTexture )
      glBindTexture( GL_TEXTURE_2D, pTexture->textureIndex );
   if( shader )
      glUniform1i( shader->psSampler[slot], slot );
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
