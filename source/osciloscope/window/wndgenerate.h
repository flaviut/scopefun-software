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
#ifndef __OSCILOSCOPE__GENERATE__WINDOW__
#define __OSCILOSCOPE__GENERATE__WINDOW__

#define GENERATOR_ANALOG_CHANNELS  2
#define GENERATOR_ANALOG_SAMPLES   4096
#define GENERATOR_DIGITAL_SAMPLES  4096
#define GENERATOR_DIGITAL_CHANNELS 16

////////////////////////////////////////////////////////////////////////////////
//
// GenerateType
//
////////////////////////////////////////////////////////////////////////////////
enum GenerateType {
   GENERATE_SINUS,
   GENERATE_COSINUS,
   GENERATE_INCREMENT,
   GENERATE_DECREMENT,
   GENERATE_CONSTANT,
   GENERATE_RADOM,
   GENERATE_SQUARE,
   GENERATE_DELTA
};

////////////////////////////////////////////////////////////////////////////////
//
// HardwareType
//
////////////////////////////////////////////////////////////////////////////////
enum HardwareType {
   HARDWARE_CUSTOM_FILE,
   HARDWARE_SIN,
   HARDWARE_TRIANGLE,
   HARDWARE_SAW,
   HARDWARE_SQUEARE,
   HARDWARE_DELTA,
   HARDWARE_DC,
   HARDWARE_NOISE
};

////////////////////////////////////////////////////////////////////////////////
//
// SoftwareGeneratorChannel
//
////////////////////////////////////////////////////////////////////////////////
class SoftwareGeneratorChannel {
public:
   GenerateType     type;
   float            period;
   float            peakToPeak;
   float            every;
   float            speed;
   int              onOff;
public:
   SoftwareGeneratorChannel();
public:
   void Default();
};

class WndSoftwareGenerator {
public:
   SoftwareGeneratorChannel channel[2];
public:
	WndSoftwareGenerator();
public:
   void Default();
};

#ifdef PLATFORM_WIN
   #pragma pack(push,1)
#endif

class CustomData {
public:
   ishort                   analog[GENERATOR_ANALOG_CHANNELS][GENERATOR_ANALOG_SAMPLES];
   byte                     digital[GENERATOR_DIGITAL_CHANNELS][(GENERATOR_DIGITAL_SAMPLES / 8)];
}

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
__attribute__((packed));
#else
;
#endif

#ifdef PLATFORM_WIN
   #pragma pack(pop)
#endif

class WndHardwareGenerator {
public:
   uint                     type0;
   float                    frequency0;
   int                      voltage0;
   int                      offset0;
   int                      squareDuty0;
   uint                     sawSlopePositive0;
   uint                     onOff0;

public:
   uint                     type1;
   float                    frequency1;
   int                      voltage1;
   int                      offset1;
   int                      squareDuty1;
   uint                     sawSlopePositive1;
   uint                     onOff1;
public:
   CustomData               custom;
public:
	WndHardwareGenerator();
public:
   void loadCustomData(int idx,const char* path);
   void loadCustomDigital(const char* path);
   void upload();
   void uploadDigital();
public:
   void Default();
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
