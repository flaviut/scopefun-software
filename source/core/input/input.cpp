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
#include <core/core.h>

////////////////////////////////////////////////////////////////////////////////
//
// Globals
//
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER( Input );

////////////////////////////////////////////////////////////////////////////////
//
// InputManager
//
////////////////////////////////////////////////////////////////////////////////
InputManager::InputManager()
{
   clear();
}

bool InputManager::isMouse(uint key)
{
   return mouseButtons & SDL_BUTTON( key );
}

bool InputManager::isClick(uint key)
{
   return mouseClick & SDL_BUTTON(key);
}

bool InputManager::isKey(uint key)
{
   uint scancode = SDL_GetScancodeFromKey(key);
   if(scancode < MAX_KEYS )
       return keys[scancode];
   return false;
}

int InputManager::getMouseX()
{
   return mouseX;
}
int InputManager::getMouseY()
{
   return mouseY;
}
int InputManager::getMouseRelX()
{
   return mouseRelX;
}
int InputManager::getMouseRelY()
{
   return mouseRelY;
}

int InputManager::getMouseWheel()
{
   return mouseWheel;
}

int InputManager::start()
{
   return 0;
}

int InputManager::clear()
{
   mouseX = 0;
   mouseY = 0;
   mouseRelX  = 0;
   mouseRelY  = 0;
   mouseWheel = 0;
   mouseButtons = 0;
   mouseClick = 0;
   SDL_zero(keys);
   return 0;
}

int InputManager::onEvent(SDL_Event &e)
{
   switch( e.type ) {
   case SDL_MOUSEWHEEL:
      mouseWheel += (e.wheel.y*100);
      break;
   };
   return 0;
}

int InputManager::update(float dt)
{
   mouseButtons = SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);
   mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

   // left click
   {
      int isMouse = mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT);
      int isClick = mouseClick   & SDL_BUTTON(SDL_BUTTON_LEFT);
      if (  isMouse && !mouseClick) mouseClick = mouseClick | (  SDL_BUTTON(SDL_BUTTON_LEFT) );
      if ( !isMouse &&  mouseClick) mouseClick = mouseClick & ( ~SDL_BUTTON(SDL_BUTTON_LEFT) );
   }

   // right click
   {
      int isMouse = mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT);
      int isClick = mouseClick   & SDL_BUTTON(SDL_BUTTON_RIGHT);
      if (isMouse && !mouseClick)  mouseClick = mouseClick |  (SDL_BUTTON(SDL_BUTTON_RIGHT));
      if (!isMouse &&  mouseClick) mouseClick = mouseClick & (~SDL_BUTTON(SDL_BUTTON_RIGHT));
   }

   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
