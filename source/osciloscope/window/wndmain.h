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
#ifndef __OSCILOSCOPE__MAIN__WINDOW__
#define __OSCILOSCOPE__MAIN__WINDOW__

////////////////////////////////////////////////////////////////////////////////
//
// ViewSelect
//
////////////////////////////////////////////////////////////////////////////////
enum ViewSelect {
   VIEW_SELECT_OSC_3D  = BIT(0),
   VIEW_SELECT_OSC_2D  = BIT(1),
   VIEW_SELECT_FFT_3D  = BIT(2),
   VIEW_SELECT_FFT_2D  = BIT(3),
   VIEW_SELECT_DIGITAL = BIT(4),
};

////////////////////////////////////////////////////////////////////////////////
//
// MWVertical
//
////////////////////////////////////////////////////////////////////////////////
class MWVertical {
public:
   int channelId;
public:
   float              Capture;
   float              Scale;
   float              Display;
   int                YPosition;
   int                OscOnOff;
   int                FFTOnOff;
   int                Invert;
   int                Ground;
   int                AcDc;
public:
	MWVertical();
public:
   void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWFunction
//
////////////////////////////////////////////////////////////////////////////////
class MWFunction {
public:
   int Type;
   int OscOnOff;
   int FFTOnOff;
   int xyGraph;
public:
   MWFunction();
public:
   void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWHorizontal
//
////////////////////////////////////////////////////////////////////////////////
class MWHorizontal {
public:
   float              Capture;
   float              Display;
   float              Position;
   uint               Mode;
   uint               Control;
   uint               Frame;
   uint               FrameSize;
   uint               FFTSize;
   uint               ETS;
public:
   int                uiActive;
   int                uiRange;
   int                uiValue;
public:
	MWHorizontal();
public:
   void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWTrigger
//
////////////////////////////////////////////////////////////////////////////////
class MWTrigger {
public:
   uint               Source;
   uint               Slope;
   uint               Mode;
   int                Level;
   int                His;
   double             Percent;
   double             Holdoff;
public:
   int                stage;
   int                stageStart;
   int                stageMode;
   int                stageChannel;
   ushort             delay[4];
   Array<int,16>     mask[4];
   Array<int,16>     pattern[4];
public:
	MWTrigger();
public:
   void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWDigital
//
////////////////////////////////////////////////////////////////////////////////
class MWDigital {
public:
   Array<int,16>          digital;
   Array<byte, 16>        output;
public:
	MWDigital();
public:
   void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWDigitalSetup
//
////////////////////////////////////////////////////////////////////////////////
class MWDigitalSetup {
public:
   double voltage;
   uint   divider;
   int    inputOutput15;
   int    inputOutput7;
public:
   MWDigitalSetup();
public:
   void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MeasureBits
//
////////////////////////////////////////////////////////////////////////////////
enum MeasureBits {
   MEASURE_CH0_PICK         = BIT(0),
   MEASURE_CH0_START        = BIT(1),
   MEASURE_CH0_START_TEXT   = BIT(2),
   MEASURE_CH0_END          = BIT(3),
   MEASURE_CH0_END_TEXT     = BIT(4),
   MEASURE_CH1_PICK         = BIT(5),
   MEASURE_CH1_START        = BIT(6),
   MEASURE_CH1_START_TEXT   = BIT(7),
   MEASURE_CH1_END          = BIT(8),
   MEASURE_CH1_END_TEXT     = BIT(9),
   MEASURE_CHF_PICK         = BIT(10),
   MEASURE_CHF_START        = BIT(11),
   MEASURE_CHF_START_TEXT   = BIT(12),
   MEASURE_CHF_END          = BIT(13),
   MEASURE_CHF_END_TEXT     = BIT(14),
   MEASURE_CH0_PICK_TEXT    = BIT(15),
   MEASURE_CH1_PICK_TEXT    = BIT(16),
   MEASURE_CHF_PICK_TEXT    = BIT(17),
   MEASURE_CH0_START_Z_TEXT = BIT(18),
   MEASURE_CH0_END_Z_TEXT   = BIT(19),
   MEASURE_CH1_START_Z_TEXT = BIT(20),
   MEASURE_CH1_END_Z_TEXT   = BIT(21),
   MEASURE_CHF_START_Z_TEXT = BIT(22),
   MEASURE_CHF_END_Z_TEXT   = BIT(23),
   MEASURE_CH0_PICK_Z_TEXT  = BIT(24),
   MEASURE_CH1_PICK_Z_TEXT  = BIT(25),
   MEASURE_CHF_PICK_Z_TEXT  = BIT(26),
};

enum LockFrameBits {
   MEASURE_LOCK_FRAME_CH0_START = BIT(0),
   MEASURE_LOCK_FRAME_CH1_START = BIT(1),
   MEASURE_LOCK_FRAME_CHF_START = BIT(2),
   MEASURE_LOCK_FRAME_CH0_END   = BIT(3),
   MEASURE_LOCK_FRAME_CH1_END   = BIT(4),
   MEASURE_LOCK_FRAME_CHF_END   = BIT(5),
   MEASURE_LOCK_FRAME_CH0_PICK  = BIT(6),
   MEASURE_LOCK_FRAME_CH1_PICK  = BIT(7),
   MEASURE_LOCK_FRAME_CHF_PICK  = BIT(8),
};

class MeasurePos {
public:
   double x;
   double y;
   double z;
   uint   frame;
   uint   index;
public:
   double xTime;
   double xFreq;
   double yVolt;
public:
   MeasurePos()
   {
      clear();
   }
public:
   void clear()
   {
      x = 0;
      y = 0;
      z = 0;
      frame = 0;
      index = 0;
      xTime = 0;
      xFreq = 0;
      yVolt = 0;
   }

   float FrameToZ(int iframe);
   int   ZToFrame(float fz);
public:
   uint  getXIndex(uint frameSize);
public:
   void setXTime(double time);
   void setXFreq(double freq);
   void setYVolt(double volt,int channel);
public:
   double getXTime();
   double getXFreq();
   double getYVolt(int channel);
public:
   void  setZ(int frame);
   int   getZ();
};

enum MeasureHistory {
   MEASURE_HISTORY_CURRENT,
   MEASURE_HISTORY_AVERAGE,
   MEASURE_HISTORY_MINIMUM,
   MEASURE_HISTORY_MAXIMUM,
   MEASURE_HISTORY_LAST,
};

class MeasurePick {
public:
   bool         active;
   bool         updateUI;
   bool         lineMove;
   MeasurePos   position;
public:
   MeasurePick()
   {
      clear();
   }
public:
   void onActivate()
   {
      active = true;
      lineMove = true;
   }

   void clear()
   {
      updateUI = false;
      active = false;
      lineMove = false;
      position.clear();
   }

   void onUpdate(double x, double y,double z,int frame)
   {
      if (active && lineMove)
      {
         position.x = x;
         position.y = y;
         position.z = z;
         position.frame = frame;
         updateUI = true;
      }
   }

   void onClick()
   {
      if( lineMove )
      {
         lineMove = false;
         updateUI = true;
      }
   }
};

enum EValueGrid1 {
// channel 0
Channel0,
Ch0Vavg,
Ch0VrmsDc,
Ch0VrmsAc,
Ch0Vmin,
Ch0Vmax,
Ch0Vpp,
Ch0Tperiod,
Ch0Tfreq,
Ch0Surface,
Ch0XCursor,
Ch0XV0,
Ch0XV1,
Ch0XVD,
Ch0XT0,
Ch0XT1,
Ch0XTD,
Ch0XInvTD,
Ch0YCursor,
Ch0YV0,
Ch0YV1,
Ch0YVD,
Ch0YT0,
Ch0YT1,
Ch0YTD,
Ch0YInvTD,
// channel 1
Channel1,
Ch1Vavg,
Ch1VrmsDc,
Ch1VrmsAc,
Ch1Vmin,
Ch1Vmax,
Ch1Vpp,
Ch1Tperiod,
Ch1Tfreq,
Ch1Surface,
Ch1XCursor,
Ch1XV0,
Ch1XV1,
Ch1XVD,
Ch1XT0,
Ch1XT1,
Ch1XTD,
Ch1XInvTD,
Ch1YCursor,
Ch1YV0,
Ch1YV1,
Ch1YVD,
Ch1YT0,
Ch1YT1,
Ch1YTD,
Ch1YInvTD,
// function
Function,
FunVavg,
FunVrmsDc,
FunVrmsAc,
FunVmin,
FunVmax,
FunVpp,
FunTperiod,
FunTfreq,
FunSurface,
FunXCursor,
FunXV0,
FunXV1,
FunXVD,
FunXT0,
FunXT1,
FunXTD,
FunXInvTD,
FunYCursor,
FunYV0,
FunYV1,
FunYVD,
FunYT0,
FunYT1,
FunYTD,
FunYInvTD,
// fft
FFT,
FFTCh0V0db,
FFTCh0V1db,
FFTCh0VD,
FFTCh0F0,
FFTCh0F1,
FFTCh0FD,
FFTCh1V0db,
FFTCh1V1db,
FFTCh1VD,
FFTCh1F0,
FFTCh1F1,
FFTCh1FD,
FFTFunV0db,
FFTFunV1db,
FFTFunVD,
FFTFunF0,
FFTFunF1,
FFTFunFD,
// digital
DigitalX1,
DigitalX1Bit0,
DigitalX1Bit1,
DigitalX1Bit2,
DigitalX1Bit3,
DigitalX1Bit4,
DigitalX1Bit5,
DigitalX1Bit6,
DigitalX1Bit7,
DigitalX1Bit8,
DigitalX1Bit9,
DigitalX1Bit10,
DigitalX1Bit11,
DigitalX1Bit12,
DigitalX1Bit13,
DigitalX1Bit14,
DigitalX1Bit15,
DigitalX2,
DigitalX2Bit0,
DigitalX2Bit1,
DigitalX2Bit2,
DigitalX2Bit3,
DigitalX2Bit4,
DigitalX2Bit5,
DigitalX2Bit6,
DigitalX2Bit7,
DigitalX2Bit8,
DigitalX2Bit9,
DigitalX2Bit10,
DigitalX2Bit11,
DigitalX2Bit12,
DigitalX2Bit13,
DigitalX2Bit14,
DigitalX2Bit15,
Last1,
};

class MeasureChannelData {
public:
   double averageN;
public:
   double row[Last1];
   bool   display[Last1];
public:
   double Row(EValueGrid1 r);
public:
   void Minimum(MeasureChannelData& other);
   void Maximum(MeasureChannelData& other);
   void Average(MeasureChannelData& other);
   void SetValue(double value);
   void Clear();
public:
   MeasureChannelData()
   {
      Clear();
   }
};

enum EValueGrid2 {
vg2First2    = 1,
vg2XT0       = 2,
vg2XT1       = 3,
vg2XTD       = 4,
vg2XInvTD    = 5,
vg2YCh0V0    = 8,
vg2YCh0V1    = 9,
vg2YCh0VD    = 10,
vg2YCh1V0    = 12,
vg2YCh1V1    = 13,
vg2YCh1VD    = 14,
vg2YFunV0    = 16,
vg2YFunV1    = 17,
vg2YFunVD    = 18,
vg2FFTV0db   = 20,
vg2FFTF0     = 21,
vg2FFTV1db   = 22,
vg2FFTF1     = 23,
vg2FFTVD     = 24,
vg2FFTFD     = 25,
vg2DigitalX0 = 28,
vg2DigitalX1 = 30,
Last2     = 31,
};

class MeasurePickData {
public:
   double row[Last2];
   bool   display[Last2];
public:
   ushort DigitalX0;
   ushort DigitalX1;
public:
   double RowD(EValueGrid2 r);
   ushort RowU(EValueGrid2 r);
public:
   MeasurePickData()
   {
      memset(row, 0, sizeof(row));
      memset(display, 0, sizeof(display));
      DigitalX0 = DigitalX1 = 0;
   }
};

class MeasureData {
public:
   MeasurePick pickX0;
   MeasurePick pickX1;
   MeasurePick pickY0;
   MeasurePick pickY1;
   MeasurePick pickFFT0;
   MeasurePick pickFFT1;
public:
   MeasurePickData    pick;
   MeasureChannelData history[MEASURE_HISTORY_LAST];
public:
   MeasureData();
public:
   void Clear();
};

class MWMeasure {
public:
   MeasureData             data;
public:
   bool                    automaticDataClear;
   bool                    automaticDataClearTrigger;
   int                     uiOpen;
public:
   void ClearCapture() { if (automaticDataClear) data.Clear(); };
   void ClearTrigger() { if (automaticDataClearTrigger) data.Clear(); };
public:
	MWMeasure();
public:
   void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWCallibrate
//
////////////////////////////////////////////////////////////////////////////////
class MWCallibrate {
public:
   int onoff;
   int freq;
public:
	MWCallibrate() { onoff = 0; freq = 0; };
};

////////////////////////////////////////////////////////////////////////////////
//
// MWProgress
//
////////////////////////////////////////////////////////////////////////////////
class MWProgress {
public:
   int uiActive;
   int uiValue;
   int uiRange;
   int uiPulse;
public:
	MWProgress();
};

enum UsbSpeed {
    USB_SPEED_AUTOMATIC,
    USB_SPEED_LOW,
    USB_SPEED_MEDIUM,
    USB_SPEED_HIGH,
};

////////////////////////////////////////////////////////////////////////////////
//
// WndMain
//
////////////////////////////////////////////////////////////////////////////////
class WndMain {
public:
   UsbSpeed speed;
public:
   WndDisplay			display;
   WndThermal			thermal;
   WndSoftwareGenerator softwareGenerator;
   WndHardwareGenerator hardwareGenerator;
public:
   MWProgress     progress;
   MWMeasure      measure;
   MWVertical     channel01;
   MWVertical     channel02;
   MWFunction     function;
   MWHorizontal   horizontal;
   MWTrigger      trigger;
   MWDigital      digital;
   MWDigitalSetup digitalSetup;
   MWCallibrate   callibrate;
public:
   Flag32  fftDigital;
public:
	WndMain();
public:
   void Default();
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
