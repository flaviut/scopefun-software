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
#ifndef __CORE__TIMER__
#define __CORE__TIMER__

#define MAX_THREAD 16U

class ThreadData {
public:
   uint   frame;
public:
   double dt;
   double fpsTime;
public:
   uint   fps;
   uint   fpsCounter;
public:
   ularge frequency;
   ularge lasttime;
   ularge currenttime;
};

////////////////////////////////////////////////////////////////////////////////
//
// TimerManager
//
////////////////////////////////////////////////////////////////////////////////
class TimerManager  : public Manager {
private:
   ThreadData thread[MAX_THREAD];
public:
   uint   getFrame(uint threadId);
   double getDelta(uint threadId);
   uint   getFps(uint threadId);
   uint   getFpsCounter(uint threadId);
public:
   void deltaTime(uint threadId);
public:
   void init(uint threadId);
};

MANAGER_POINTER( Timer );

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
