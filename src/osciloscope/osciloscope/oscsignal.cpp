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
// OsciloscopeFrame
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeFrame::OsciloscopeFrame()
{
   SDL_zero(*this);
}

ishort OsciloscopeFrame::getAnalogShort(uint channel, uint sample)
{
   uint count = analog[channel].getCount();
   if (!count) return 0;
   uint      idx = clamp<uint>(sample, 0, count - 1);
   ishort ivalue = analog[channel][idx];
   return ivalue;
}

float OsciloscopeFrame::getAnalog(uint channel,uint sample)
{
   uint count = analog[channel].getCount();
   if( !count ) return 0.f;
   int       idx = clamp<uint>( sample, 0, count-1 );
   ishort ivalue = analog[channel][idx];
   float  fvalue = float(ivalue)/MAXOSCVALUE;
   return clamp( fvalue, -1.f, 1.f );
}

double OsciloscopeFrame::getAnalogDouble(uint channel, uint sample)
{
   uint count = analog[channel].getCount();
   if (!count) return 0.0;
   int       idx = clamp<uint>(sample, 0, count - 1);
   ishort ivalue = analog[channel][idx];
   double  fvalue = double(ivalue) / double( MAXOSCVALUE );
   return clamp(fvalue, -1.0, 1.0);
}

ishort OsciloscopeFrame::getDigital(uint channel,uint sample)
{
   uint count = digital.getCount();
   if( !count ) return 0;
   int       idx = clamp<uint>( sample, 0, count-1 );
   ishort ivalue = digital[idx];
   ishort    bit = (ivalue >> channel)&0x0001;
   return bit;
}

ushort OsciloscopeFrame::getDigitalChannels(uint sample)
{
   uint count = digital.getCount();
   if (!count) return 0;
   int       idx = clamp<uint>(sample, 0, count - 1);
   ushort value = digital[idx];
   return value;
}

float rand_FloatRange(float a, float b)
{
   return ((b-a)*((float)rand()/RAND_MAX))+a;
}

void OsciloscopeFrame::generate(double dt,uint amount,double captureStart,double captureFreq)
{
   if( pOsciloscope->signalMode != SIGNAL_MODE_GENERATOR ) return;

   deltaTime[0] += dt;
   deltaTime[1] += dt;

   //////////////////////////////////////////////////
   // ets
   //////////////////////////////////////////////////
   ets = rand()%pOsciloscope->settings.ets;

   //////////////////////////////////////////////////
   // analog
   //////////////////////////////////////////////////
   for(int i=0;i<2;i++)
   {
       // on/off
      if( pOsciloscope->window.softwareGenerator.channel[i].onOff == 0) continue;

      // capture
      float captureTime = pOsciloscope->window.horizontal.Capture*float(NUM_SAMPLES);
      float captureVolt = 0;
      if( i==0 ) captureVolt = pOsciloscope->window.channel01.Capture;
      if( i==1 ) captureVolt = pOsciloscope->window.channel02.Capture;

      // channels
      SoftwareGeneratorChannel *pGenerate = 0;
      if( i==0 ) pGenerate = &pOsciloscope->window.softwareGenerator.channel[0];
      if( i==1 ) pGenerate = &pOsciloscope->window.softwareGenerator.channel[1];
      GenerateType type       = pGenerate->type;
      float        period     = pGenerate->period;
      float        peaktopeak = pGenerate->peakToPeak;
      float        speed      = pGenerate->speed;

      // generate only on specified time
      if( deltaTime[i] < pGenerate->every ) continue;
      if (!pOsciloscope->window.horizontal.ETS)
            generateTime[i] += deltaTime[i]*speed;

      deltaTime[i] = 0;

      // min/max peak
      float minpeak = -peaktopeak/2.f;
      float maxpeak = peaktopeak/2.f;

      // frame size
      float displayFrameSize = float(NUM_SAMPLES);

      // generate
      srand( (uint)SDL_GetPerformanceCounter() );
      float value = 0;
      for(uint j=0;j<amount;j++)
      {
         switch( type ) {
         case GENERATE_SINUS:
         case GENERATE_COSINUS:
            {
               float  normalizedTime = float(j)/float(displayFrameSize) + float(generateTime[i]);
               float  signaltime  = (normalizedTime*captureTime);
               float            t = (signaltime / period);
               float    angle2Pi  = 2.f*PI*( t );
               switch( type  ) {
               case GENERATE_SINUS   : value = maxpeak*sinf( angle2Pi ); break;
               case GENERATE_COSINUS : value = maxpeak*cosf( angle2Pi ); break;
               default:
                  break;
               };
            }
            break;
         case GENERATE_DECREMENT:
         case GENERATE_INCREMENT:
            {
               float  normalizedTime   = float(j)/float(displayFrameSize) + float(generateTime[i]);
               float  normalizedPeriod = period/captureTime;
               double input            = normalizedPeriod;
               float mod               = (float)modf(normalizedTime,&input);
               float div               = mod/(normalizedPeriod);
               switch( type  ) {
               case GENERATE_INCREMENT : value = div*peaktopeak+minpeak; break;
               case GENERATE_DECREMENT : value = (1.f-div)*peaktopeak+minpeak; break;
               default:
                  break;
               };
            }
            break;
         case GENERATE_CONSTANT:
            value = peaktopeak;
            break;
         case GENERATE_RADOM:
            value = rand_FloatRange(-1.f,1.f)*(peaktopeak/2.f);
            break;
         case GENERATE_SQUARE:
            {
             float  normalizedTime = float(j)/float(displayFrameSize) + generateTime[i];
             float  normalizedPeriod = period/captureTime;
             float  time = fmod(normalizedTime,1.0f);
             if( time < normalizedPeriod )
                value = peaktopeak;
             else
                value = 0.f;
            }
            break;
         case GENERATE_DELTA:
            {
                float  normalizedTime = float(j)/float(displayFrameSize) + generateTime[i];
                float  normalizedPeriod = period/captureTime;
                float  time = fmod(normalizedTime,1.0f);
                float      t = time / normalizedPeriod/2;
                if( time < normalizedPeriod/2 )    value = 4.0*t*peaktopeak;
                else if( time < normalizedPeriod ) value = 4.0*(0.5f-t)*peaktopeak;
                else value = 0.f;
            }
            break;
         };
         float normalized = float(value)/(5.f*float(captureVolt));
         normalized = clamp<float>( normalized,-1.0,1.0);
         ishort val = (ishort)( float(normalized)*MAXOSCVALUE );
         if( analog[i].getCount() < NUM_SAMPLES )
            analog[i].pushBack( val );
      }
   }

   //////////////////////////////////////////////////
   // digital
   //////////////////////////////////////////////////
   srand( (uint)SDL_GetPerformanceCounter() );
   for(uint i=0;i<amount;i++)
   {
      byte digital0 = rand()%2;
      byte digital1 = rand()%2;
      byte digital2 = rand()%2;
      byte digital3 = rand()%2;
      byte digital4 = rand()%2;
      byte digital5 = rand()%2;
      byte digital6 = rand()%2;
      byte digital7 = rand()%2;
      byte digital8 = rand()%2;
      byte digital9 = rand()%2;
      byte digital10 = rand()%2;
      byte digital11 = rand()%2;
      byte digital12 = rand()%2;
      byte digital13 = rand()%2;
      byte digital14 = rand()%2;
      byte digital15 = rand()%2;
      ushort bits   = digital0|(digital1<<1)|(digital2<<2)|(digital3<<3)|(digital4<<4)|(digital5<<5)|(digital6<<6)|(digital7<<7);
             bits  |= digital8<<8|(digital9<<9)|(digital10<<10)|(digital11<<11)|(digital12<<12)|(digital13<<13)|(digital14<<14)|(digital15<<15);
      if( digital.getCount() < NUM_SAMPLES )
         digital.pushBack( bits );
   }
   // attrobutes
   for(uint i=0;i<amount;i++)
   {
      attr.pushBack(0);
   }
   // debug
   debug.clear();
   for(uint i=0;i<416;i++)
   {
      debug.pushBack( rand()%256 );
   }
   triggerTime = 0;
   time(&utc);
   thisFrame   = firstFrame = SDL_GetPerformanceCounter();
   captureTime = double( SDL_GetPerformanceCounter() - captureStart ) / double( captureFreq );
}

ishort leadBitShift(ushort value)
{
   ishort isLeadBit = value&0x200;
   if( isLeadBit )
   {
      return (value|0xFE00);
   }
   return value;
}

////////////////////////////////////////////////////////////////
//
// time
//
////////////////////////////////////////////////////////////////
void OsciloscopeFrame::getTime(char *buffer, int size)
{
   FORMAT_BUFFER();
   struct tm* local  = localtime((time_t*)&utc);
   if( local )
   {
      ularge     offset = (this->thisFrame - this->firstFrame) + this->triggerTime;
      FORMAT("%d|%d|%d - %d|%d|%d - %llu", (local->tm_year + 1900), (local->tm_mon + 1), (local->tm_mday), (local->tm_hour + 1), (local->tm_min + 1), (local->tm_sec + 1), offset);
   }
   else
   {
      FORMAT("%s", "triggerTime zero missing!" );
   }
   memcpy(buffer, formatBuffer, min(size, FORMAT_BUFFER_SIZE));
}

////////////////////////////////////////////////////////////////
//
// header  - 2464 bytes
// data    - 60000 bytes
//
////////////////////////////////////////////////////////////////
bool OsciloscopeFrame::captureHeader(byte *src, uint size,ularge captureStart,ularge captureFreq)
{
   // src
   if ( !src ) return false;

   // size
   if ( size < CAPTURE_BUFFER_HEADER )
   {
      return false;
   }

   // debug
   debug.clear();
   for (uint i = 0; i<CAPTURE_BUFFER_HEADER; i++)
   {
      debug.pushBack(src[i]);
   }

   // magic DDDD
   ushort header = *(ushort*)src;
   if (header != 56797)
   {
      return false;
   }

   // ets
   int index = clamp(pOsciloscope->settings.etsByteIndex, 0, debug.getCount() - 1);
   ets = debug[index];


   // trigger
   triggerTime = *(ularge*)(src + 2);
   if (triggerTime == 0)
   {
      firstFrame = SDL_GetPerformanceCounter();
      utc        = ::time(0);
   }
   thisFrame = SDL_GetPerformanceCounter();

   // captureTime
   captureTime = double(thisFrame - captureStart) / double(captureFreq);

   return true;
}

bool OsciloscopeFrame::captureData(byte *src, uint& pos, uint size)
{
   // src
   if (!src) return false;

   byte *shortBuffer = src  + pos;
   uint   numBytes   = size - pos;
   uint numSamples   = numBytes / 6;
   for (uint i = 0; i<numSamples;i++)
   {
      // raw data
      ushort short0 = *(ushort*)&shortBuffer[0];
      ushort short1 = *(ushort*)&shortBuffer[2];
      ushort short2 = *(ushort*)&shortBuffer[4];

      shortBuffer += 6;

      // attribute
      byte attribute = 0;
      if (short0 & 0x8000) attribute |= FRAME_ATTRIBUTE_HIDE_SIGNAL;
      if (short0 & 0x4000) attribute |= FRAME_ATTRIBUTE_TRIGGERED_LED;
      if (short0 & 0x2000) attribute |= FRAME_ATTRIBUTE_ROLL_DISPLAY;
      if (attr.getCount() < NUM_SAMPLES)
         attr.pushBack(attribute);

      // analog
      ishort singedShort0 = leadBitShift(short0 & 0x000003FF);
      ishort singedShort1 = leadBitShift(short1 & 0x000003FF);
      if (analog[0].getCount() < NUM_SAMPLES)
         analog[0].pushBack(singedShort0);
      if (analog[1].getCount() < NUM_SAMPLES)
         analog[1].pushBack(singedShort1);

      // digital
      if (digital.getCount() < NUM_SAMPLES)
         digital.pushBack(short2);
   }

   pos += numSamples * 6;

   uint endByte = pos + (size - pos);
   if (endByte / 6 == NUM_SAMPLES)
      return true;

   return false;
}

int OsciloscopeFrame::isFull()
{
   int numSamples = int(NUM_SAMPLES); //pOsciloscope->control.frameSize;
   if( analog[0].getCount() >= numSamples ) return 1;
   if( analog[1].getCount() >= numSamples ) return 1;
   if( digital.getCount()   >= numSamples ) return 1;
   return 0;
}

void OsciloscopeFrame::clear()
{
   analog[0].clear();
   analog[1].clear();
   digital.clear();
   attr.clear();
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
