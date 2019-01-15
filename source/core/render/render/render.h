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
#ifndef __CORE__RENDER__
#define __CORE__RENDER__

////////////////////////////////////////////////////////////////////////////////
//
// RenderCallback
//
////////////////////////////////////////////////////////////////////////////////
class RenderCallback {
public:
   virtual void render()   = 0;
   virtual void onResize(int width,int height,int oldWidth,int oldHeight) {};
};


////////////////////////////////////////////////////////////////////////////////
//
// RenderManager
//
////////////////////////////////////////////////////////////////////////////////
class RenderManager : public Manager {
public:
   RenderManager();
public:
   Array<GrConstantBuffer*,4> constantBuffer;
public:
   iint  width;
   iint  height;
   float aspect;
private:
   uint    threadCount;
   Flag64  renderFlag[MAX_THREAD];
public:
   Flag64&  GetFlag(uint threadId);
public:
   Map<iint,RenderCallback*,16> callback;
public:
   void registerCallback(RenderCallback *pointer,iint priority=0);
public:
   void render();
public:
   void resize(uint width,uint height);
public:
   uint getThreadCount();
   void setThreadCount(uint count);
public:
   int  start();
   int  update(float dt);
   int  onEvent(SDL_Event &e);
};

MANAGER_POINTER(Render);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
