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
// Globals
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER( Timer );

void TimerManager::init(uint id)
{
   thread[id].frequency   = SDL_GetPerformanceFrequency();
   thread[id].currenttime = 0;
   thread[id].lasttime    = SDL_GetPerformanceCounter();
   thread[id].frame       = 0;
   thread[id].fpsTime     = 0;
   thread[id].fpsCounter  = 0;
   thread[id].fps         = 0;
   thread[id].dt          = 0;
}

uint TimerManager::getFrame(uint id)
{
   return thread[id].frame;
}

double TimerManager::getDelta(uint id)
{
   return thread[id].dt;
}

uint TimerManager::getFps(uint id)
{
   return thread[id].fps;
}

uint TimerManager::getFpsCounter(uint id)
{
   return thread[id].fpsCounter;
}

void TimerManager::deltaTime(uint id)
{
   SDL_MemoryBarrierAcquire();
   thread[id].currenttime = SDL_GetPerformanceCounter();
   thread[id].dt = ((double)(thread[id].currenttime - thread[id].lasttime))/(double)thread[id].frequency;
   thread[id].lasttime = thread[id].currenttime;

   thread[id].fpsTime += thread[id].dt;
   if (thread[id].fpsTime > 1.0)
   {
      thread[id].fps = thread[id].fpsCounter;
      thread[id].fpsCounter = 0;
      thread[id].fpsTime = 0;
   }

   thread[id].fpsCounter++;
   thread[id].frame++;
   SDL_MemoryBarrierRelease();
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
