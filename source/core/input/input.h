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
#ifndef __CORE__INPUT__
#define __CORE__INPUT__

#define MAX_KEYS 1024

////////////////////////////////////////////////////////////////////////////////
//
// Input
//
////////////////////////////////////////////////////////////////////////////////
class InputManager : public Manager {
private:
   Flag32 Flag;
private:
   int   mouseX;
   int   mouseY;
   int   mouseRelX;
   int   mouseRelY;
   int   mouseWheel;
   uint  mouseButtons;
   uint  mouseClick;
private:
   byte keys[MAX_KEYS];
public:
   InputManager();
public:
   bool isKey(uint key);
   bool isMouse(uint key);
   bool isClick(uint key);
public:
   int getMouseX();
   int getMouseY();
   int getMouseRelX();
   int getMouseRelY();
   int getMouseWheel();
public:
   int start();
   int clear();
   int update(float dt);
public:
   int onEvent(SDL_Event& e);
};

MANAGER_POINTER(Input);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
