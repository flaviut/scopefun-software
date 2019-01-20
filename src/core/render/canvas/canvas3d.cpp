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
MANAGER_REGISTER(Canvas3d);


Vector4 vecColorWhite(1, 1, 1, 1);

////////////////////////////////////////////////////////////////////////////////
// globals - vertex declaration
///////////////////////////////////////////////////////////////////////////////
static GrVertexElement elPosition( "inPos",  VERTEX_TYPE_POSITION, 0, VERTEX_FORMAT_FLOAT4 );
static GrVertexElement elNormal("inNormal", VERTEX_TYPE_NORMAL, 0, VERTEX_FORMAT_FLOAT4);
static GrVertexElement elColor("inColor", VERTEX_TYPE_COLOR,   0, VERTEX_FORMAT_FLOAT4 );
static GrVertexElement elColorUint("inColor", VERTEX_TYPE_COLOR, 0, VERTEX_FORMAT_COLOR);
static GrVertexElement elUV( "inUV", VERTEX_TYPE_TEXCOORD, 0, VERTEX_FORMAT_FLOAT2 );
GrVertexDeclaration    declPos3D( &elPosition, 0 );
GrVertexDeclaration    declPos3DNormal(&elPosition, &elNormal, 0);
GrVertexDeclaration    declPosUV3D( &elPosition, &elUV, 0 );

////////////////////////////////////////////////////////////////////////////////
//
// Canvas3D
//
////////////////////////////////////////////////////////////////////////////////
Canvas3dManager::Canvas3dManager()
{
   vertexbufferSize = 16 * 1024 * 1024;
   threadBuffer = MAX_THREAD;
}

void Canvas3dManager::beginBatch(uint threadId, Canvas3dBatch type,int count)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&   currentSlot   = thread[threadId].slot[mode][slotId];

   currentSlot.batchtype    = type;
   currentSlot.elementCount = count;
   switch(currentSlot.batchtype){
   case CANVAS3D_BATCH_LINE:
      currentSlot.elementSize = 2 * sizeof(Vector4);
      break;
   case CANVAS3D_BATCH_POINT:
      currentSlot.elementSize = 1 * sizeof(Vector4);
      break;
   case CANVAS3D_BATCH_RECTZ:
      currentSlot.elementSize = 6 * sizeof(PosUv3d);
      break;
   case CANVAS3D_BATCH_TRIANGLE:
      currentSlot.elementSize = 3 * sizeof(PosNormal);
      break;
   case CANVAS3D_BATCH_TRIANGLEUV:
      currentSlot.elementSize = 3 * sizeof(PosUv3d);
      break;
   case CANVAS3D_BATCH_STRIP:
      currentSlot.elementSize = 2 * sizeof(Vector4);
      break;
   case CANVAS3D_BATCH_STRIPUV:
      currentSlot.elementSize = 2 * sizeof(PosUv3d);
      break;
   };

   currentThread.pLock      = (byte*)currentThread.pBatchMesh->lock(currentSlot.elementSize*currentSlot.elementCount);
   currentSlot.lockedBytes  = currentThread.pBatchMesh->lockedBytes;
   currentSlot.lockedOffset = currentThread.pBatchMesh->lockedOffset;
}

void Canvas3dManager::bLine(uint threadId, const Vector4 &start, const Vector4 &end)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&   currentSlot   = thread[threadId].slot[mode][slotId];

   if( currentSlot.batchtype != CANVAS3D_BATCH_LINE )
      CORE_ABORT("missmatched batch type",0);

   if (currentSlot.lockedBytes > 0)
   {
      Vector4 *pVertex = (Vector4*)currentThread.pLock;
      pVertex[0] = start;
      pVertex[1] = end;

      currentThread.pLock     += 2*sizeof(Vector4);
      currentSlot.lockedBytes -= 2*sizeof(Vector4);
   }
   else
   {
      int debug = 1;
   }
}

void Canvas3dManager::bPoint(uint threadId,  const Vector4 &point)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&   currentSlot = thread[threadId].slot[mode][slotId];

   if(currentSlot.batchtype != CANVAS3D_BATCH_POINT )
      CORE_ABORT("missmatched batch type",0);

   if (currentSlot.lockedBytes > 0)
   {
      Vector4 *pVertex = (Vector4*)currentThread.pLock;
      pVertex[0] = point;
      currentThread.pLock       += sizeof(Vector4);
      currentSlot.lockedBytes -= sizeof(Vector4);
   }
}

void Canvas3dManager::bRectangleZ(uint threadId,  const Vector4 &min,const Vector4 &max)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&     currentSlot = thread[threadId].slot[mode][slotId];

   if(currentSlot.batchtype != CANVAS3D_BATCH_RECTZ )
      CORE_ABORT("missmatched batch type",0);

   if (currentSlot.lockedBytes > 0)
   {
      PosUv3d *pVertex = (PosUv3d*)currentThread.pLock;

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
      pVertex[0] = PosUv3d(minx, miny, 0.f, 1.f, minz, minw + dv);
      pVertex[1] = PosUv3d(minx + dx, miny + dy, 0.f, 1.f, minz + du, minw);
      pVertex[2] = PosUv3d(minx, miny + dy, 0.f, 1.f, minz, minw);

      // 2. triangle
      pVertex[3] = PosUv3d(minx, miny, 0.f, 1.f, minz, minw + dv);
      pVertex[4] = PosUv3d(minx + dx, miny, 0.f, 1.f, minz + du, minw + dv);
      pVertex[5] = PosUv3d(minx + dx, miny + dy, 0.f, 1.f, minz + du, minw);

      currentThread.pLock       += 6*sizeof(PosUv3d);
      currentSlot.lockedBytes -= 6*sizeof(PosUv3d);
   }
}

void Canvas3dManager::bTriangle(uint threadId,  const Vector4 &v0, const Vector4 &v1, const Vector4 &v2)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&     currentSlot = thread[threadId].slot[mode][slotId];

   if(currentSlot.batchtype != CANVAS3D_BATCH_TRIANGLE )
      CORE_ABORT("missmatched batch type",0);

   if (currentSlot.lockedBytes > 0)
   {
      Vector4 *pVertex = (Vector4*)currentThread.pLock;

      pVertex[0] = v0;
      pVertex[1] = v1;
      pVertex[2] = v2;

      currentThread.pLock     += sizeof(Vector4)*3;
      currentSlot.lockedBytes -= sizeof(Vector4)*3;
   }
}

void Canvas3dManager::bTriangleNormal(uint threadId,  const Vector4 &v0, const Vector4 &v1, const Vector4 &v2)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&     currentSlot = thread[threadId].slot[mode][slotId];

   if(currentSlot.batchtype != CANVAS3D_BATCH_TRIANGLE )
      CORE_ABORT("missmatched batch type",0);

   if (currentSlot.lockedBytes > 0)
   {
      PosNormal *pVertex = (PosNormal*)currentThread.pLock;

      Vector4 a = v0- v1;
      Vector4 b = v0- v2;
      Vector4 n = vecCross3d(a, b);
      n = vecNormalize3d(n);

      pVertex[0] = PosNormal(v0.x, v0.y, v0.z, v0.w, n.x, n.y, n.z, n.w);
      pVertex[1] = PosNormal(v1.x, v1.y, v1.z, v1.w, n.x, n.y, n.z, n.w);
      pVertex[2] = PosNormal(v2.x, v2.y, v2.z, v2.w, n.x, n.y, n.z, n.w);

      currentThread.pLock     += sizeof(PosNormal)*3;
      currentSlot.lockedBytes -= sizeof(PosNormal)*3;
   }
}

void Canvas3dManager::bTriangleNormal(uint threadId,  const Vector4 &v0, const Vector4 &v1, const Vector4 &v2, const Vector4 &n0, const Vector4 &n1, const Vector4 &n2)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&     currentSlot = thread[threadId].slot[mode][slotId];

   if (currentSlot.batchtype != CANVAS3D_BATCH_TRIANGLE)
      CORE_ABORT("missmatched batch type", 0);

   if (currentSlot.lockedBytes > 0)
   {
      PosNormal *pVertex = (PosNormal*)currentThread.pLock;

      pVertex[0] = PosNormal(v0.x, v0.y, v0.z, v0.w, n0.x, n0.y, n0.z, n0.w);
      pVertex[1] = PosNormal(v1.x, v1.y, v1.z, v1.w, n1.x, n1.y, n1.z, n1.w);
      pVertex[2] = PosNormal(v2.x, v2.y, v2.z, v2.w, n2.x, n2.y, n2.z, n2.w);

      currentThread.pLock       += sizeof(PosNormal)* 3;
      currentSlot.lockedBytes -= sizeof(PosNormal)* 3;
   }
}


void Canvas3dManager::bTriangleUV(uint threadId,  const PosUv3d &v0,const PosUv3d &v1,const PosUv3d &v2)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&     currentSlot = thread[threadId].slot[mode][slotId];

   if(currentSlot.batchtype != CANVAS3D_BATCH_TRIANGLEUV )
      CORE_ABORT("missmatched batch type",0);

   if (currentSlot.lockedBytes > 0)
   {
      PosUv3d *pVertex = (PosUv3d*)currentThread.pLock;
      pVertex[0] = v0;
      pVertex[1] = v1;
      pVertex[2] = v2;

      currentThread.pLock     += sizeof(PosUv3d)*3;
      currentSlot.lockedBytes -= sizeof(PosUv3d)*3;
   }
}

void Canvas3dManager::bStrip(uint threadId,  const Vector4 &v0 )
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&     currentSlot = thread[threadId].slot[mode][slotId];

   if(currentSlot.batchtype != CANVAS3D_BATCH_STRIP )
      CORE_ABORT("missmatched batch type",0);

   if (currentSlot.lockedBytes > 0)
   {
      Vector4 *pVertex = (Vector4*)currentThread.pLock;
      pVertex[0] = v0;

      currentThread.pLock       += sizeof(Vector4);
      currentSlot.lockedBytes -= sizeof(Vector4);
   }
}

void Canvas3dManager::bStripUV(uint threadId, const Vector4 &v0,float u,float v)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&     currentSlot = thread[threadId].slot[mode][slotId];

   if(currentSlot.batchtype != CANVAS3D_BATCH_STRIPUV )
      CORE_ABORT("missmatched batch type",0);

   if (currentSlot.lockedBytes > 0)
   {
      PosUv3d *pPosUV = (PosUv3d*)currentThread.pLock;

      pPosUV[0].x = v0.x;
      pPosUV[0].y = v0.y;
      pPosUV[0].z = v0.z;
      pPosUV[0].w = v0.w;
      pPosUV[0].u = u;
      pPosUV[0].v = v;

      currentThread.pLock       += sizeof(PosUv3d);
      currentSlot.lockedBytes -= sizeof(PosUv3d);
   }
}

void Canvas3dManager::endBatch(uint threadId, const Matrix4x4 &final, int color, GrTexture *pTexture, BlendMode bm, Canvas3dShader shdType, GrShader *shader, SamplerState ss)
{
   uint   mode = thread[threadId].mode;
   uint slotId = thread[threadId].slotCounter[mode];

   Canvas3dThread& currentThread = thread[threadId];
   Canvas3dSlot&     currentSlot = thread[threadId].slot[mode][slotId];

   currentThread.pBatchMesh->unlock();

   currentSlot.finalMatrix = final;
   currentSlot.color       = color;
   currentSlot.blendMode   = bm;
   currentSlot.sampler     = ss;
   currentSlot.pTexture    = pTexture;
   switch (shdType) {
   case CANVAS3D_SHADER_CUSTOM:
      currentSlot.pShader = shader;
      break;
   default:
      currentSlot.pShader = shader3d[shdType];
      break;
   };

   thread[threadId].slotCounter[mode]++;
}

void Canvas3dManager::bind(uint threadId)
{
   Canvas3dThread& currentThread = thread[threadId];
   currentThread.pBatchMesh->bind();
}

void Canvas3dManager::unbind(uint threadId)
{
   Canvas3dThread& currentThread = thread[threadId];
   currentThread.pBatchMesh->unbind();
}

void Canvas3dManager::addConstant(uint threadId, Vector4 constant)
{
   Canvas3dThread& currentThread = thread[threadId];
   uint                     mode = thread[threadId].mode;
   uint                   slotId = thread[threadId].slotCounter[mode];
   Canvas3dSlot&     currentSlot = currentThread.slot[mode][slotId];
   currentSlot.constant[currentSlot.constantCount] = constant;
   currentSlot.constantCount++;
   currentSlot.constantCount = clamp( currentSlot.constantCount, 0U, MAX_CANVAS3D_CONSTANT-1);
}

void Canvas3dManager::render(uint threadId,uint mode)
{
   Canvas3dThread& currentThread = thread[threadId];

   if (mode == 1 && currentThread.slotCounter[mode]==0)
   {
      int debug = 1;
   }

   for (uint slotId = 0; slotId < currentThread.slotCounter[mode]; slotId++)
   {
      Canvas3dSlot& currentSlot = currentThread.slot[mode][slotId];

      // constants
      float        oow = 1.f / (float)pRender->width;
      float        ooh = 1.f / (float)pRender->height;
      if (grGetMode() == OPENGL_MODE_32)
      {
         Vector4 *pVector = (Vector4*)pRender->constantBuffer[0]->Lock();
         pVector[0] = Vector4((float)pRender->width, (float)pRender->height, oow, ooh);
         pVector[1] = vecFromColor(currentSlot.color);
         pVector[2] = currentSlot.finalMatrix.RowX();
         pVector[3] = currentSlot.finalMatrix.RowY();
         pVector[4] = currentSlot.finalMatrix.RowZ();
         pVector[5] = currentSlot.finalMatrix.Pos();
         pVector[6] = dirLight;
         pRender->constantBuffer[0]->Unlock();

         pVector = (Vector4*)pRender->constantBuffer[1]->Lock();
         for (uint i = 0; i < currentSlot.constantCount; i++)
            pVector[i] = currentSlot.constant[i];
         pRender->constantBuffer[1]->Unlock();
      }

      // shader
      currentSlot.pShader->activate();
      if (grGetMode() == OPENGL_MODE_21)
      {
         currentSlot.pShader->SetUniform4(0, "gPSRenderTarget", Vector4((float)pRender->width, (float)pRender->height, oow, ooh));
         currentSlot.pShader->SetUniform4(1, "gPSColor", vecFromColor(currentSlot.color));
         currentSlot.pShader->SetUniform4(2, "gPSDirectionalLight", dirLight);
         currentSlot.pShader->SetUniform4x4(0, "gVSFinalMatrix", currentSlot.finalMatrix);
      }

      // texture
      if (currentSlot.pTexture)
         grSetPixelShaderTexture(currentSlot.pShader, 0, currentSlot.pTexture);

      // texture state
      grStateBlendMode(currentSlot.blendMode);
      grStateSampler(0, currentSlot.sampler);

      // draw
      uint elementCount = currentSlot.elementCount;
      uint vertexstart = currentSlot.lockedOffset;
      switch (currentSlot.batchtype) {
      case CANVAS3D_BATCH_LINE:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader, PRIMITIVE_TYPE_LINE_LIST, 2 * elementCount, vertexstart);
         break;
      case CANVAS3D_BATCH_POINT:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader, PRIMITIVE_TYPE_POINT_LIST, elementCount, vertexstart);
         break;
      case CANVAS3D_BATCH_RECTZ:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader, PRIMITIVE_TYPE_TRIANGLE_LIST, 3 * elementCount, vertexstart);
         break;
      case CANVAS3D_BATCH_TRIANGLE:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader, PRIMITIVE_TYPE_TRIANGLE_LIST, 3 * elementCount, vertexstart);
         break;
      case CANVAS3D_BATCH_TRIANGLEUV:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader, PRIMITIVE_TYPE_TRIANGLE_LIST, 3 * elementCount, vertexstart);
         break;
      case CANVAS3D_BATCH_STRIP:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader, PRIMITIVE_TYPE_TRIANGLE_STRIP, elementCount, vertexstart);
         break;
      case CANVAS3D_BATCH_STRIPUV:
         currentThread.pBatchMesh->draw(currentSlot.pShader->vertexShader, PRIMITIVE_TYPE_TRIANGLE_STRIP, elementCount, vertexstart);
         break;
      };
   }
}

void Canvas3dManager::setMode(uint threadId, uint mode)
{
   Canvas3dThread& currentThread = thread[threadId];
   currentThread.mode = min( MAX_CANVAS3D_MODE, mode );
}

void Canvas3dManager::threadBegin(uint threadId)
{
   Canvas3dThread& currentThread = thread[threadId];

   currentThread.pLock = 0;

   for (int i = 0; i < MAX_CANVAS3D_MODE; i++)
   {
      currentThread.slotCounter[i] = 0;
      for (uint j = 0; j < MAX_CANVAS3D_SLOT; j++)
      {
         SDL_zero(currentThread.slot[i][j]);
      }
   }

   currentThread.pBatchMesh->frameStart();
}

void Canvas3dManager::threadEnd(uint threadId)
{

}

void Canvas3dManager::mainBegin(uint threadId)
{
   Canvas3dThread& currentThread = thread[threadId];
   currentThread.pBatchMesh->bind();
}

void Canvas3dManager::mainEnd(uint threadId)
{
}

void Canvas3dManager::setVertexBufferSize(uint size)
{
   vertexbufferSize = size;
}

void Canvas3dManager::setThreadBuffer(uint cnt)
{
   threadBuffer = cnt;
}

int Canvas3dManager::start()
{
   shader3d[0] = 0;
   for (int i = 1; i < CANVAS3D_SHADER_LAST;i++)
      shader3d[i] = grCreateShader();

   shader3d[CANVAS3D_SHADER_DEFAULT]->compile("canvas/canvas3D.fx", declPos3D, GrShaderDefine(0));
   shader3d[CANVAS3D_SHADER_DEFAULT_COLOR]->compile("canvas/canvas3D.fx", declPos3D, GrShaderDefine("COLOR",0));
   shader3d[CANVAS3D_SHADER_NORMAL]->compile("canvas/canvas3D.fx", declPos3DNormal, GrShaderDefine("NORMAL", 0));
   shader3d[CANVAS3D_SHADER_NORMAL_COLOR]->compile("canvas/canvas3D.fx", declPos3DNormal, GrShaderDefine( "NORMAL", "COLOR", 0));
   shader3d[CANVAS3D_SHADER_NORMAL_COLOR_LIGHT]->compile("canvas/canvas3D.fx", declPos3DNormal, GrShaderDefine("NORMAL", "COLOR", "LIGHT", 0));
   shader3d[CANVAS3D_SHADER_TEXTURE]->compile("canvas/canvas3D.fx", declPosUV3D, GrShaderDefine("TEXTURE", 0));

   for (uint threadId = 0; threadId < threadBuffer; threadId++)
   {
      Canvas3dThread& currentThread = thread[threadId];
      currentThread.pBatchMesh = grCreateStaticMesh();
      currentThread.pBatchMesh->createBuffer( vertexbufferSize );
      currentThread.pBatchMesh->bind();
   }
   return 0;
}

int Canvas3dManager::update(float dt)
{
   return 0;
}

int Canvas3dManager::stop()
{
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
