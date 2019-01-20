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
#include<osciloscope/osciloscope.h>

void create()
{
   // managers
   MANAGER_CREATE(Manager);
   MANAGER_CREATE(Timer);
   MANAGER_CREATE(Render);
   MANAGER_CREATE(Canvas2d);
   MANAGER_CREATE(Canvas3d);
   MANAGER_CREATE(Font);
   MANAGER_CREATE(Camera);
   MANAGER_CREATE(Osciloscope);
   MANAGER_CREATE(Input);
}

void setup()
{
   // start
   pManager->addStart("Osciloscope");
   pManager->addStart("Timer");
   pManager->addStart("Input");

   // update
   pManager->addUpdate("Input");
   pManager->addUpdate("Render");
   pManager->addUpdate("Canvas2d");
   pManager->addUpdate("Canvas3d");
   pManager->addUpdate("Osciloscope");
   pManager->addUpdate("Camera");

   // stop
   pManager->addStop("Osciloscope");
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
