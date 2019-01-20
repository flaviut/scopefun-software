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
#ifndef __OSCILOSCOPE__THERMAL__WINDOW__
#define __OSCILOSCOPE__THERMAL__WINDOW__

#define MAX_HEAT 0.1f
#define MAX_COOL 0.05f

////////////////////////////////////////////////////////////////////////////////
//
// WndThermal
//
////////////////////////////////////////////////////////////////////////////////
class WndThermal {
public:
   float                   heating;
   float                   cooling;
   int                     enabled;
public:
	WndThermal();
public:
   void                    Default();
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
