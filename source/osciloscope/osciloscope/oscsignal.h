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
#ifndef __OSC__SIGNAL__
#define __OSC__SIGNAL__

#define MAXOSCVALUE    511.f
#define NUM_SAMPLES    10000
#define NUM_FFT        1048576
#define CAPTURE_BUFFER                62464
#define CAPTURE_BUFFER_HEADER          2048
#define CAPTURE_BUFFER_PADDING          416
#define CAPTURE_BUFFER_DATA           60000

class OsciloscopeFrame;

////////////////////////////////////////////////////////////////////////////////
//
// SignalMode
//
////////////////////////////////////////////////////////////////////////////////
enum SignalMode {
   SIGNAL_MODE_PLAY = 0,
   SIGNAL_MODE_PAUSE,
   SIGNAL_MODE_CAPTURE,
   SIGNAL_MODE_GENERATOR,
   SIGNAL_MODE_CLEAR,
};

////////////////////////////////////////////////////////////////////////////////
//
// FrameAttribute
//
////////////////////////////////////////////////////////////////////////////////
enum FrameAttribute {
   FRAME_ATTRIBUTE_HIDE_SIGNAL = 0,
   FRAME_ATTRIBUTE_TRIGGERED_LED,
   FRAME_ATTRIBUTE_ROLL_DISPLAY,
};

////////////////////////////////////////////////////////////////////////////////
//
// EtsAttribute
//
////////////////////////////////////////////////////////////////////////////////
enum Etsttribute {
   ETS_CLEAR = 1,
   ETS_PAUSE = 2,
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeFrame
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeFrame {
public:
   time_t                     utc;
   ularge                     firstFrame;
   ularge                     thisFrame;
   ularge                     triggerTime;
   double                     captureTime;
   double                     generateTime[2];
   double                     deltaTime[2];
public:
   Array<byte,NUM_SAMPLES>   attr;
   Array<ishort,NUM_SAMPLES> analog[2];
   Array<ushort,NUM_SAMPLES> digital;
   Array<byte,2464>          debug;
public:
   uint                       ets;
   uint                       etsAttr;
public:
   OsciloscopeFrame();
public:
   ishort getAnalogShort(uint channel, uint sample);
   float  getAnalog(uint channel,uint sample);
   double getAnalogDouble(uint channel, uint sample);
   ishort getDigital(uint channel,uint sample);
   ushort getDigitalChannels(uint sample);
public:
   int  isFull();
   void clear();
public:
   void getTime(char* buffer, int size);
   void generate(double dt,uint count,double captureStart,double captureFreq);
   bool captureHeader(byte *src,uint size,ularge captureStart,ularge captureFreq);
   bool captureData(byte *src, uint& pos, uint size);
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeFFT
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeFFT {
public:
   double* aRe;
   double* aIm;
   double* aAmpl;
public:
   void init()
   {
      aRe   = (double*)memoryAllocate(NUM_FFT*sizeof(double));
      aIm   = (double*)memoryAllocate(NUM_FFT*sizeof(double));
      aAmpl = (double*)memoryAllocate(NUM_FFT*sizeof(double));
   }

   void clear()
   {
      //SDL_memset( aRe,   0, sizeof(double)*NUM_FFT );
      //SDL_memset( aIm,   0, sizeof(double)*NUM_FFT );
      //SDL_memset( aAmpl, 0, sizeof(double)*NUM_FFT );
   }
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
