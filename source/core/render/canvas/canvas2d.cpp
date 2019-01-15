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
#include<core/core.h>

////////////////////////////////////////////////////////////////////////////////
// globals - manager
///////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER(Canvas2d);

////////////////////////////////////////////////////////////////////////////////
// const
///////////////////////////////////////////////////////////////////////////////
const int CBatchSize = 10000;

////////////////////////////////////////////////////////////////////////////////
// globals - vertex declaration
///////////////////////////////////////////////////////////////////////////////
static GrVertexElement elPosition( "inPos", VERTEX_TYPE_POSITION, 0, VERTEX_FORMAT_FLOAT2 );
static GrVertexElement elUV( "inUV", VERTEX_TYPE_TEXCOORD, 0, VERTEX_FORMAT_FLOAT2 );
GrVertexDeclaration    declPos2D( &elPosition, 0 );
GrVertexDeclaration    declPosTex2D( &elPosition, &elUV, 0 );

////////////////////////////////////////////////////////////////////////////////
//
// Canvas2D
//
////////////////////////////////////////////////////////////////////////////////
Canvas2dManager::Canvas2dManager()
{
    threadBuffer = MAX_THREAD;
}

void Canvas2dManager::beginBatch(uint threadId,Canvas2dBatch type,int count)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas2dThread& currentThread = thread[threadId];
   Canvas2dSlot&   currentSlot   = thread[threadId].slot[mode][slotId];

   currentSlot.batchtype = type;
   uint lock        = 0;
   uint vertexcount = 0;
   switch(currentSlot.batchtype){
   case CANVAS2D_BATCH_LINE:
      lock        = 2*count*sizeof(Pos2d);
      vertexcount = 2*count;
      break;
   case CANVAS2D_BATCH_RECTANGLE:
      lock = 6*count*sizeof(Pos2d);
      vertexcount = 6*count;
      break;
   case CANVAS2D_BATCH_RECTANGLEUV:
      lock = 6*count*sizeof(PosUv2d);
      vertexcount = 6*count;
      break;
   };

   currentSlot.elementCount = count;
   currentSlot.elementSize  = lock / count;

   currentThread.pLock = (byte*)currentThread.pBatchMesh->lock(lock);
   currentSlot.lockedBytes  = currentThread.pBatchMesh->lockedBytes;
   currentSlot.lockedOffset = currentThread.pBatchMesh->lockedOffset;
}

void Canvas2dManager::bLine(uint threadId,  float x0,float y0,float x1,float y1)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas2dThread& currentThread = thread[threadId];
   Canvas2dSlot&   currentSlot = thread[threadId].slot[mode][slotId];

   if( currentSlot.batchtype != CANVAS2D_BATCH_LINE )
      CORE_ABORT("missmatched batch type",0);

   Pos2d *pVertex = (Pos2d*)currentThread.pLock;
      pVertex[0] = Pos2d(x0, y0);
      pVertex[1] = Pos2d(x1, y1);
   currentThread.pLock += 2*sizeof(Pos2d);
}

void Canvas2dManager::bRectangle(uint threadId,  float xmin,float ymin,float xmax,float ymax)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas2dThread& currentThread = thread[threadId];
   Canvas2dSlot&   currentSlot = thread[threadId].slot[mode][slotId];

   if(currentSlot.batchtype != CANVAS2D_BATCH_RECTANGLE )
      CORE_ABORT("missmatched batch type",0);

   Pos2d *pVertex = (Pos2d*)currentThread.pLock;

      float dx = xmax - xmin;
      float dy = ymax - ymin;

      // 1. triangle
      pVertex[0] = Pos2d(xmin, ymin);
      pVertex[1] = Pos2d(xmin + dx, ymin + dy);
      pVertex[2] = Pos2d(xmin, ymin + dy);

      // 2. triangle
      pVertex[3] = Pos2d(xmin, ymin);
      pVertex[4] = Pos2d(xmin + dx, ymin);
      pVertex[5] = Pos2d(xmin + dx, ymin + dy);

   currentThread.pLock += sizeof(Pos2d)*6;
}

void Canvas2dManager::bRectangleUV(uint threadId, const Vector4 &min, const Vector4 &max)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas2dThread& currentThread = thread[threadId];
   Canvas2dSlot&   currentSlot = thread[threadId].slot[mode][slotId];

   if(currentSlot.batchtype != CANVAS2D_BATCH_RECTANGLEUV )
      CORE_ABORT("missmatched batch type",0);

   PosUv2d *pVertex = (PosUv2d*)currentThread.pLock;

      float minx = min.x;
      float miny = min.y;
      float minz = min.z;
      float minw = min.w;

      float maxx = max.x;
      float maxy = max.y;
      float maxz = max.z;
      float maxw = max.w;

      float dx = maxx - minx;
      float dy = maxy - miny;
      float du = maxz - minz;
      float dv = maxw - minw;

      // 1. triangle
      pVertex[0] = PosUv2d(minx, miny, minz, minw);
      pVertex[1] = PosUv2d(minx + dx, miny + dy, minz + du, minw + dv);
      pVertex[2] = PosUv2d(minx, miny + dy, minz, minw + dv);

      // 2. triangle
      pVertex[3] = PosUv2d(minx, miny, minz, minw);
      pVertex[4] = PosUv2d(minx + dx, miny, minz + du, minw);
      pVertex[5] = PosUv2d(minx + dx, miny + dy, minz + du, minw + dv);

   currentThread.pLock += sizeof(PosUv2d)*6;
}


void Canvas2dManager::bRectangleUV(uint threadId, const Vector4 &v0, const Vector4 &v1, const Vector4 & v2, const Vector4 &v3)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];


   Canvas2dThread& currentThread = thread[threadId];
   Canvas2dSlot&   currentSlot = thread[threadId].slot[mode][slotId];

   if (currentSlot.batchtype != CANVAS2D_BATCH_RECTANGLEUV)
      CORE_ABORT("missmatched batch type", 0);

   Vector4 *pVertex = (Vector4*)currentThread.pLock;

   // 1. triangle
   pVertex[0] = v0;
   pVertex[1] = v1;
   pVertex[2] = v2;

   // 2. triangle
   pVertex[3] = v3;
   pVertex[4] = v2;
   pVertex[5] = v0;

   currentThread.pLock += sizeof(PosUv2d) * 6;
}

void Canvas2dManager::endBatch(uint threadId, int color, GrTexture *pTexture, BlendMode bm, SamplerState ss, GrShader *shader )
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas2dThread& currentThread = thread[threadId];
   Canvas2dSlot&   currentSlot = thread[threadId].slot[mode][slotId];

   currentThread.pBatchMesh->unlock();

   currentSlot.color = color;
   currentSlot.blendMode = bm;
   currentSlot.sampler = ss;
   currentSlot.pTexture = pTexture;
   if (shader)
   {
      currentSlot.pShader = shader;
   }
   else
   {
      if (pTexture) currentSlot.pShader = textureShader;
      else          currentSlot.pShader = normalShader;
   }

   thread[threadId].slotCounter[mode]++;
}

void Canvas2dManager::addConstant(uint threadId, Vector4 constant)
{
   Canvas2dThread& currentThread = thread[threadId];
   uint                     mode = thread[threadId].mode;
   uint                   slotId = thread[threadId].slotCounter[mode];
   Canvas2dSlot&     currentSlot = currentThread.slot[mode][slotId];
   currentSlot.constant[currentSlot.constantCount] = constant;
   currentSlot.constantCount++;
   currentSlot.constantCount = clamp(currentSlot.constantCount, 0U, MAX_CANVAS2D_CONSTANT - 1);
}

void Canvas2dManager::render(uint threadId,uint mode)
{
   Canvas2dThread& currentThread = thread[threadId];

   for (uint slotId = 0; slotId < currentThread.slotCounter[mode]; slotId++)
   {
      Canvas2dSlot&   currentSlot = currentThread.slot[mode][slotId];

      // constants
      float        oow =  2.f/(float)pRender->width;
      float        ooh = -2.f/(float)pRender->height;
      if (grGetMode() == OPENGL_MODE_32)
      {
         Vector4 *pVector = (Vector4*)pRender->constantBuffer[0]->Lock();
         pVector[0] = Vector4(oow, ooh, -1.f, 1.f);
         pVector[1] = vecFromColor(currentSlot.color);
         pRender->constantBuffer[0]->Unlock();

         pVector = (Vector4*)pRender->constantBuffer[1]->Lock();
         for (uint i = 0; i < currentSlot.constantCount; i++)
            pVector[i] = currentSlot.constant[i];
         pRender->constantBuffer[1]->Unlock();
      }

      // shader
      currentSlot.pShader->activate();

      // texture
      if( currentSlot.pTexture )
         grSetPixelShaderTexture(currentSlot.pShader,0, currentSlot.pTexture);

      if (grGetMode() == OPENGL_MODE_21)
      {
         currentSlot.pShader->SetUniform4(0, "gVSAdjust", Vector4(oow, ooh, -1.f, 1.f));
         currentSlot.pShader->SetUniform4(1, "gPSColor", vecFromColor(currentSlot.color));
      }

      grStateBlendMode(currentSlot.blendMode );
      grStateSampler( 0, currentSlot.sampler );

      switch( currentSlot.batchtype ) {
      case CANVAS2D_BATCH_LINE:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader,PRIMITIVE_TYPE_LINE_LIST, 2*currentSlot.elementCount, currentSlot.lockedOffset);
         break;
      case CANVAS2D_BATCH_RECTANGLE:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader,PRIMITIVE_TYPE_TRIANGLE_LIST, 6*currentSlot.elementCount, currentSlot.lockedOffset);
         break;
      case CANVAS2D_BATCH_RECTANGLEUV:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader,PRIMITIVE_TYPE_TRIANGLE_LIST, 6*currentSlot.elementCount, currentSlot.lockedOffset);
         break;
      };
   }
}

void Canvas2dManager::setMode(uint threadId,uint mode)
{
   Canvas2dThread& currentThread = thread[threadId];
   currentThread.mode = min( MAX_CANVAS2D_MODE, mode );
}

void Canvas2dManager::threadBegin(uint threadid)
{
   Canvas2dThread& currentThread = thread[threadid];

   currentThread.pLock = 0;
   for (int i = 0; i < MAX_CANVAS2D_MODE; i++)
   {
      currentThread.slotCounter[i] = 0;
      for (uint j = 0; j < MAX_CANVAS2D_SLOT; j++)
      {
         SDL_zero(currentThread.slot[i][j]);
      }
   }

   currentThread.pBatchMesh->frameStart();
}

void Canvas2dManager::threadEnd(uint threadid)
{

}

void Canvas2dManager::mainBegin(uint threadId)
{
   Canvas2dThread& currentThread = thread[threadId];
   currentThread.pBatchMesh->bind();
}

void Canvas2dManager::mainEnd(uint threadId)
{

}

void Canvas2dManager::setThreadBuffer(uint cnt)
{
   threadBuffer = cnt;
}

int Canvas2dManager::start()
{
   normalShader  = grCreateShader();
   textureShader = grCreateShader();

   normalShader->compile(  "canvas/canvas2D.fx", declPos2D, GrShaderDefine(0) );
   textureShader->compile( "canvas/canvas2D.fx", declPosTex2D, GrShaderDefine("TEXTURE",0) );

   for (uint i = 0; i <  threadBuffer;i++)
   {
      Canvas2dThread& currentThread = thread[i];
      SDL_zero(currentThread);
      currentThread.pBatchMesh = grCreateStaticMesh();
      currentThread.pBatchMesh->createBuffer( sizeof(Vector4)*CBatchSize );
      currentThread.pBatchMesh->bind();
   }
   return 0;
}

int Canvas2dManager::update(float dt)
{
   return 0;
}

int Canvas2dManager::stop()
{
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
