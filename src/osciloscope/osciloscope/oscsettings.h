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
#ifndef __OSC__SETTINGS__
#define __OSC__SETTINGS__

////////////////////////////////////////////////////////////////////////////////
//
// OscSettings
//
////////////////////////////////////////////////////////////////////////////////
class OscSettings {
public:
   cJSON*      json;
public:
   uint  historyFrameCount;
   uint  historyFrameDisplay;
   uint  historyFrameLoadSave;
   uint  historyFrameClipboard;
   int   depthBuffer;
   int   shaders21;
   int   vertexBufferSizeMegaByte;
   float updateTime;
   uint  fps;
   uint  threadCount;
   uint  threadBuffer;
   uint  threadDelayCaptureProducer;
   uint  threadDelayPause;
   uint  threadDelayUpdateConsumer;
   uint  threadDelayUpdateProducer;
   uint  threadDelayRender;
   uint  threadDelayIdle;
   uint  threadPriorityCapture;
   uint  threadPriorityUpdate;
   uint  threadPriorityRender;
   uint  threadPriorityMain;
   double updateTimer;
   double renderTimer;
public:
    uint speedLow;
    uint speedMedium;
    uint speedHigh;
public:
   int debug;
   int displayWidth;
   int displayHeight;
   int controlWidth;
   int controlHeight;
public:
   double generatorFs;
public:
   int ets;
   int etsByteIndex;
public:
   double middleOffset;
public:
   UsbGuid   usbGuid;
   uint      usbVendorId;
   uint      usbProductId;
public:
   byte      usbSerialId;
   byte      usbCheckSerialId;
   uint      usbFactoryVendorId;
   uint      usbFactoryProductId;
public:
   uint      usbXferSize;
   uint      usbEp2TimeOut;
   uint      usbEp4TimeOut;
   uint      usbEp6TimeOut;
public:
   String    fx2FileName;
   String    fpgaFileName;
public:
   int    invertCh[2];
   int    analogChannelReference;
   int    analogChannelOffsets[2][vcLast];
   int    analogChannelYVoltageStep[2][vcLast];
   int    generatorReference;
   double generatorVoltagePerStep;
   int    generatorOffset[2];
public:
   int    framesPerCapture;
   double gainExitPercentage;
   int    gainMaxIterations;
   ushort gainValue[2][vcLast];
   ushort gainAttr[vcLast];
   ushort gainMin[vcLast];
   ushort gainMax[vcLast];
   ushort gainGenOffset[vcLast];
   ushort gainRefValue[vcLast];
   double gainRefVoltage[vcLast];
public:
   double digitalVoltageCoeficient;
public:
   double getAnalogChannelYVoltageStep(int ch, VoltageCapture capture);
   void   setAnalogChannelYVoltageStep(double value, int ch, VoltageCapture capture);
public:
   void load();
   void save();
   void loadCallibrate();
   void saveCallibrate();
public:
   OscSettings();
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
