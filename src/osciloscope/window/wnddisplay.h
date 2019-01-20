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
#ifndef __OSCILOSCOPE__DISPLAY__WINDOW__
#define __OSCILOSCOPE__DISPLAY__WINDOW__

#define MAX_TRIANGLE_SIZE 0.01f

enum DigitalBits {
    DIGITAL_BITS_8,
    DIGITAL_BITS_16,
    DIGITAL_BITS_32,
    DIGITAL_BITS_64,
};

enum SignalType {
    SIGNAL_TYPE_LINE,
    SIGNAL_TYPE_TRIANGLE,
};

////////////////////////////////////////////////////////////////////////////////
//
// WndDisplay
//
////////////////////////////////////////////////////////////////////////////////
class WndDisplay {
public:
   int           oscGrid;
   int           oscAxis;
   int           oscUnits;
public:
   int           fftGrid;
   int           fftAxis;
   int           fftUnits;
public:
   int           digitalGrid;
   int           digitalAxis;
   int           digitalUnits;
public:
   DigitalBits   digitalBits;
public:
   SignalType    signalType;
   float         signalWidth;
public:
   SignalType    fftType;
   uint          fftDecibel;
   uint          fftLogFreq;
   float         fftWidth;
public:
   int           solid3d;
   int           light3d;
   int           depthTest3d;
   uint          tessalation3d;
   uint          tessalation2d;
   uint          alpha3dCh0;
   uint          alpha3dCh1;
   uint          alpha3dFun;
public:
	WndDisplay();
public:
   void Default();
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
