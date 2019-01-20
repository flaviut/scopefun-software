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
#ifndef __CORE__CANVAS2D__
#define __CORE__CANVAS2D__

#define MAX_CANVAS2D_SLOT     256U
#define MAX_CANVAS2D_MODE     16U
#define MAX_CANVAS2D_CONSTANT 4U

////////////////////////////////////////////////////////////////////////////////
//
// Pos_2D
//
////////////////////////////////////////////////////////////////////////////////
class Pos2d {
public:
   float x,y;
public:
   Pos2d(float fx,float fy)
   {
      x=fx;
      y=fy;
   }
};

////////////////////////////////////////////////////////////////////////////////
//
// PosUv2d
//
////////////////////////////////////////////////////////////////////////////////
class PosUv2d {
public:
   float x,y,u,v;
public:
   PosUv2d(float fx,float fy,float fu,float fv)
   {
      x=fx;
      y=fy;
      u=fu;
      v=fv;
   }
};


////////////////////////////////////////////////////////////////////////////////
//
// Canvas2dBatch
//
////////////////////////////////////////////////////////////////////////////////
enum Canvas2dBatch {
   CANVAS2D_BATCH_LINE,
   CANVAS2D_BATCH_RECTANGLE,
   CANVAS2D_BATCH_RECTANGLEUV,
};

////////////////////////////////////////////////////////////////////////////////
//
// Canvas2dSlot
//
////////////////////////////////////////////////////////////////////////////////
class Canvas2dSlot {
public:
   Canvas2dBatch batchtype;
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
   Vector4       constant[MAX_CANVAS2D_CONSTANT];
   uint          constantCount;
};

////////////////////////////////////////////////////////////////////////////////
//
// Canvas2d
//
////////////////////////////////////////////////////////////////////////////////
class Canvas2dThread {
public:
   Canvas2dSlot  slot[MAX_CANVAS2D_MODE][MAX_CANVAS2D_SLOT];
   byte*         pLock;
   GrStaticMesh* pBatchMesh;
   uint          slotCounter[MAX_CANVAS2D_MODE];
   uint          mode;
};

////////////////////////////////////////////////////////////////////////////////
//
// Canvas2D
//
////////////////////////////////////////////////////////////////////////////////
class Canvas2dManager : public Manager {
private:
   GrShader* normalShader;
   GrShader* textureShader;
private:
   Canvas2dThread thread[MAX_THREAD];
   uint           threadBuffer;
public:
   Canvas2dManager();
public:
   void beginBatch(uint threadId,Canvas2dBatch type,int count);
   void bLine(uint threadId, float x0,float y0,float x1,float y1);
   void bRectangle(uint threadId,  float xmin,float ymin,float xmax,float ymax);
   void bRectangleUV(uint threadId,  const Vector4 &min, const Vector4 &max);
   void bRectangleUV(uint threadId,  const Vector4 &v0, const Vector4 &v1, const Vector4 &v2, const Vector4 &v3);
   void endBatch(uint threadId, int color=0xffffffff,GrTexture *pTexture=0,BlendMode bm=BLEND_MODE_COPY,SamplerState ss=SAMPLE_STATE_CANVAS2D,GrShader *shader = 0 );
public:
   void mainBegin(uint threadId);
   void mainEnd(uint threadid);
   void threadBegin(uint threadId);
   void threadEnd(uint threadId);
   void render(uint threadId,uint mode);
public:
   void setMode(uint threadId,uint mode);
   void setThreadBuffer(uint cnt);
public:
   void addConstant(uint threadId, Vector4 constant);
public:
   int start();
   int update(float dt);
   int stop();
};

MANAGER_POINTER(Canvas2d);

extern GrVertexDeclaration  declPos2D;
extern GrVertexDeclaration  declPosTex2D;

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
