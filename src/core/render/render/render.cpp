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
// globals
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER( Render );

////////////////////////////////////////////////////////////////////////////////
//
// RenderManager
//
////////////////////////////////////////////////////////////////////////////////
RenderManager::RenderManager()
{
   threadCount = 0;
}

void RenderManager::registerCallback(RenderCallback *renderCallback,iint priority)
{
   callback.insert( priority, renderCallback );
}

Flag64& RenderManager::GetFlag(uint id)
{
   id = clamp(id, 0U, MAX_THREAD);
   return renderFlag[id];
}

void RenderManager::render()
{
   ////////////////////////////////////////////////////////////////////////////////
   // render
   ////////////////////////////////////////////////////////////////////////////////
   for(int i=0;i<callback.getCount();i++)
   {
      callback.index(i)->render();
   }
}

void RenderManager::resize(uint width,uint height)
{
   int oldWidth  = this->width;
   int oldHeight = this->height;

   this->width  = width;
   this->height = height;
   this->aspect = (float)width/(float)height;
   for(int i=0;i<callback.getCount();i++)
   {
      callback.index(i)->onResize(width,height,oldWidth,oldHeight);
   }
}

uint RenderManager::getThreadCount()
{
   return threadCount;
}

void RenderManager::setThreadCount(uint count)
{
   threadCount = count;
}

int RenderManager::start()
{
   for(int i=0;i<4;i++)
      constantBuffer.pushBack( grCreateConstantBuffer( i, 256 ) );
   //
   return 0;
}

int RenderManager::onEvent(SDL_Event &e)
{
   switch( e.type ) {
   case SDL_WINDOWEVENT:
      if( e.window.event == SDL_WINDOWEVENT_RESIZED )
      {
         resize( e.window.data1, e.window.data2 );
      }
      break;
   };
   return 0;
}

int RenderManager::update(float dt)
{
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
