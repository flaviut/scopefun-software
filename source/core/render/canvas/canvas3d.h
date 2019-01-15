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
#ifndef __CORE__CANVAS3D__
#define __CORE__CANVAS3D__

#define MAX_CANVAS3D_SLOT     512U
#define MAX_CANVAS3D_MODE     32U
#define MAX_CANVAS3D_CONSTANT 8U

////////////////////////////////////////////////////////////////////////////////
//
// PosUv3d
//
////////////////////////////////////////////////////////////////////////////////
class PosUv3d {
public:
   float x,y,z,w,u,v;
public:
   PosUv3d(float fx,float fy,float fz,float fw,float fu,float fv)
   {
      x=fx;
      y=fy;
      z=fz;
      w=fw;
      u=fu;
      v=fv;
   }
};

////////////////////////////////////////////////////////////////////////////////
//
// PosColor
//
////////////////////////////////////////////////////////////////////////////////
class PosNormal {
public:
   float x, y, z, w;
   float nx, ny, nz, nw;
public:
   PosNormal(float fx, float fy, float fz, float fw, float normx, float normy, float normz, float normw)
   {
      x = fx;
      y = fy;
      z = fz;
      w = fw;
      nx = normx;
      ny = normy;
      nz = normz;
      nw = normw;
   }
};

////////////////////////////////////////////////////////////////////////////////
//
// Canvas3dBatch
//
////////////////////////////////////////////////////////////////////////////////
enum Canvas3dBatch {
   CANVAS3D_BATCH_LINE,
   CANVAS3D_BATCH_POINT,
   CANVAS3D_BATCH_RECTZ,
   CANVAS3D_BATCH_TRIANGLE,
   CANVAS3D_BATCH_TRIANGLEUV,
   CANVAS3D_BATCH_STRIP,
   CANVAS3D_BATCH_STRIPUV,
};


////////////////////////////////////////////////////////////////////////////////
//
// Canvas3dShader
//
////////////////////////////////////////////////////////////////////////////////
enum Canvas3dShader {
   CANVAS3D_SHADER_CUSTOM,
   CANVAS3D_SHADER_DEFAULT,
   CANVAS3D_SHADER_DEFAULT_COLOR,
   CANVAS3D_SHADER_NORMAL,
   CANVAS3D_SHADER_NORMAL_COLOR,
   CANVAS3D_SHADER_NORMAL_COLOR_LIGHT,
   CANVAS3D_SHADER_TEXTURE,
   CANVAS3D_SHADER_LAST,
};

////////////////////////////////////////////////////////////////////////////////
//
// Canvas3dSlot
//
////////////////////////////////////////////////////////////////////////////////
class Canvas3dSlot {
public:
   Canvas3dBatch batchtype;
   uint          elementCount;
   uint          elementSize;
   uint          lockedBytes;
   uint          lockedOffset;
public:
   Matrix4x4     finalMatrix;
   uint          color;
   GrTexture*    pTexture;
   BlendMode     blendMode;
   GrShader*     pShader;
   SamplerState  sampler;
public:
   Vector4       constant[MAX_CANVAS3D_CONSTANT];
   uint          constantCount;
};

////////////////////////////////////////////////////////////////////////////////
//
// Canvas3d
//
////////////////////////////////////////////////////////////////////////////////
class Canvas3dThread {
public:
   Canvas3dSlot  slot[MAX_CANVAS3D_MODE][MAX_CANVAS3D_SLOT];
   byte*         pLock;
   GrStaticMesh* pBatchMesh;
   uint          slotCounter[MAX_CANVAS3D_MODE];
   uint          mode;
};

////////////////////////////////////////////////////////////////////////////////
//
// Canvas3D
//
////////////////////////////////////////////////////////////////////////////////
class Canvas3dManager : public Manager {
public:
   Vector4        dirLight;
   GrShader*      shader3d[CANVAS3D_SHADER_LAST];
   Canvas3dThread thread[MAX_THREAD];
   uint           vertexbufferSize;
   uint           threadBuffer;
public:
   Canvas3dManager();
public:
   void beginBatch(uint threadId,Canvas3dBatch type,int count);
   void bLine(uint threadId, const Vector4 &start,const Vector4 &end);
   void bPoint(uint threadId, const Vector4 &point);
   void bRectangleZ(uint threadId, const Vector4 &min,const Vector4 &max);
   void bTriangle(uint threadId, const Vector4 &v0, const Vector4 &v1, const Vector4 &v2);
   void bTriangleNormal(uint threadId,  const Vector4 &v0, const Vector4 &v1, const Vector4 &v2);
   void bTriangleNormal(uint threadId, const Vector4 &v0, const Vector4 &v1, const Vector4 &v2, const Vector4 &n0, const Vector4 &n1, const Vector4 &n2);
   void bTriangleUV(uint threadId, const PosUv3d &v0,const PosUv3d &v1,const PosUv3d &v2);
   void bStrip(uint threadId, const Vector4 &v0);
   void bStripUV(uint threadId, const Vector4 &v0,float u,float v);
   void endBatch(uint threadId, const Matrix4x4 &final,int color=0xffffffff,GrTexture *pTexture=0,BlendMode bm=BLEND_MODE_COPY,Canvas3dShader shdType = CANVAS3D_SHADER_DEFAULT,GrShader *shader=0,SamplerState ss = SAMPLE_STATE_FONT);
public:
   void addConstant(uint threadId, Vector4 constant);
public:
   void mainBegin(uint threadId);
   void mainEnd(uint threadId);
   void threadBegin(uint threadId);
   void threadEnd(uint threadId);
   void render(uint threadId,uint mode);
public:
   void bind(uint threadId);
   void unbind(uint threadId);
public:
   void setMode(uint threadId,uint mode);
   void setVertexBufferSize(uint size);
   void setThreadBuffer(uint cnt);
public:
   int start();
   int update(float dt);
   int stop();
};

MANAGER_POINTER(Canvas3d);

extern GrVertexDeclaration declPos3D;
extern GrVertexDeclaration declPosUV3D;

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
