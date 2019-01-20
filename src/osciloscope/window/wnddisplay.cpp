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

////////////////////////////////////////////////////////////////////////////////
//
// Display
//
////////////////////////////////////////////////////////////////////////////////
WndDisplay::WndDisplay()
{
    Default();
}

void WndDisplay::Default()
{
   oscGrid  = 1;
   oscAxis  = 0;
   oscUnits = 1;
   fftGrid  = 1;
   fftAxis  = 0;
   fftUnits = 1;
   digitalGrid    = 1;
   digitalAxis    = 0;
   digitalUnits   = 1;
   signalWidth    = MAX_TRIANGLE_SIZE/2.f;
   signalType     = SIGNAL_TYPE_LINE;
   fftWidth       = MAX_TRIANGLE_SIZE/2.f;
   fftType        = SIGNAL_TYPE_LINE;
   fftDecibel     = 10;
   fftLogFreq     = 1;
   digitalBits    = DIGITAL_BITS_32;
   solid3d        = 1;
   light3d        = 1;
   depthTest3d    = 1;
   tessalation3d  = 256;
   tessalation2d  = 1;
   alpha3dCh0     = 255;
   alpha3dCh1     = 255;
   alpha3dFun     = 255;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
