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
// defines
////////////////////////////////////////////////////////////////////////////////
// #define MOUSE_DEBUG
#define MAXIMUM_HISTORY_SIZE  4*GIGABYTE
#define MINIMUM_HISTORY_COUNT 64

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER(Osciloscope);

int SDLCALL RenderThreadFunction(void *data);

////////////////////////////////////////////////////////////////////////////////
// variables
////////////////////////////////////////////////////////////////////////////////

OscFileThread::OscFileThread()
{
   SDL_AtomicSet(&atomic,0);
   thread=0;
}

int OscFileThread::isRunning()
{
   return SDL_AtomicGet(&atomic);
}

////////////////////////////////////////////////////////////////////////////////
// OscThreadProduceConsume
////////////////////////////////////////////////////////////////////////////////
OscThreadProduceConsume::OscThreadProduceConsume()
{
   SDL_AtomicSet(&headPos, 0);
   SDL_AtomicSet(&tailPos, 0);
   SDL_AtomicSet(&lockProducer, 0);
   SDL_AtomicSet(&lockConsumer, 0);

   for (int i = 0; i < MAX_THREAD; i++)
   {
      SDL_AtomicSet(&lock[i], 0);
   }

   elCount = MAX_THREAD;
}

void OscThreadProduceConsume::setCount(uint value)
{
   elCount = value;
}

bool OscThreadProduceConsume::producerLock(uint& id,bool multiple)
{
   SDL_MemoryBarrierAcquire();

      if( multiple )
      {
        if (SDL_AtomicCAS(&lockProducer, 0, 1) == SDL_FALSE)
        {
            SDL_MemoryBarrierRelease();
            return false;
        }
      }

      uint head = SDL_AtomicGet(&headPos);
      uint tail = SDL_AtomicGet(&tailPos);
             id = head % elCount;

      if (SDL_AtomicCAS(&lock[id], 0, 1) == SDL_FALSE)
      {
          if(multiple)
          {
            while (SDL_AtomicCAS(&lockProducer, 1, 0) == SDL_FALSE) {};
          }

          SDL_MemoryBarrierRelease();
          return false;
      };

      SDL_AtomicIncRef(&headPos);

      if( multiple )
      {
        while (SDL_AtomicCAS(&lockProducer, 1, 0) == SDL_FALSE) {};
      }

    SDL_MemoryBarrierRelease();
    return true;
}

bool OscThreadProduceConsume::producerUnlock(uint id)
{
      while (SDL_AtomicCAS(&lock[id], 1, 2) == SDL_FALSE) {};
      return true;
}

bool OscThreadProduceConsume::consumerLock(uint& id,bool multiple)
{
   SDL_MemoryBarrierAcquire();

      if( multiple )
      {
        if (SDL_AtomicCAS(&lockConsumer, 0, 1) == SDL_FALSE)
        {
            SDL_MemoryBarrierRelease();
            return false;
        }
      }


      uint head = SDL_AtomicGet(&headPos);
      uint tail = SDL_AtomicGet(&tailPos);
             id = tail % elCount;

      if (SDL_AtomicCAS(&lock[id], 2, 3) == SDL_FALSE)
      {
         if(multiple)
         {
            while (SDL_AtomicCAS(&lockConsumer, 1, 0) == SDL_FALSE) {};
         }
         SDL_MemoryBarrierRelease();
         return false;
      };

      SDL_AtomicIncRef(&tailPos);

      if( multiple )
      {
         while (SDL_AtomicCAS(&lockConsumer, 1, 0) == SDL_FALSE) {};
      }

    SDL_MemoryBarrierRelease();
    return true;
}

bool OscThreadProduceConsume::consumerUnlock(uint id)
{
    while (SDL_AtomicCAS(&lock[id], 3, 0) == SDL_FALSE) {};
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeControl
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeControl::OsciloscopeControl()
{
   Default();
}

void OsciloscopeControl::Default()
{
   memset(this, 0, sizeof(OsciloscopeControl));
   control = CONTROLL_NORMAL;
   vgaina = 0xff25;
   vgainb = 0xff25;
   frameSize = NUM_SAMPLES;
   setGeneratorType0(GENERATOR_SIN);
   setGeneratorFrequency0(50.f);
   setGeneratorType1(GENERATOR_SIN);
   setGeneratorFrequency1(50.f);
}

ushort OsciloscopeControl::getAttr(uint volt)
{
   return pOsciloscope->settings.gainAttr[volt];
}

ushort OsciloscopeControl::getGain(int channel,uint volt)
{
   channel = clamp(channel, 0, 1);
   return pOsciloscope->settings.gainValue[channel][volt];
}

uint getVolt(int channel,ushort gain)
{
   for (int i = 0; i < vcLast; i++)
   {
      if (pOsciloscope->settings.gainValue[channel][i] == gain) return i;
   }
   return 0;
}

void OsciloscopeControl::setAnalogSwitchBit(int bit, int value)
{
   Flag16 flags;
   flags.set(analogswitch);
   flags.bit(bit, value);
   analogswitch = flags.get();
}

void OsciloscopeControl::setEts(int enable)
{
   if( enable ) analogswitch = raiseFlag16( analogswitch, CHANNEL_ETS );
   else         analogswitch = lowerFlag16( analogswitch, CHANNEL_ETS );
}

void OsciloscopeControl::setYRangeScaleA(uint voltage,float scale)
{
   ushort   attr = getAttr(voltage);
   ushort   gain = getGain(0,voltage);
   if( attr ) analogswitch = raiseFlag16( analogswitch, CHANNEL_ATTR_A );
   else       analogswitch = lowerFlag16( analogswitch, CHANNEL_ATTR_A );
   vgaina = gain;
}

void OsciloscopeControl::setYRangeScaleA(uint gain, uint attr)
{
   if (attr) analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_A);
   else      analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_A);
   vgaina = gain;
}

void OsciloscopeControl::setYPositionA(int pos)
{
   pos = min(pos,1500);
   pos = max(pos,-1500);
   offseta = pos;
}

void OsciloscopeControl::setYRangeScaleB(uint voltage,float scale)
{
   ushort   attr = getAttr(voltage);
   ushort   gain = getGain(1,voltage);
   if( attr ) analogswitch = raiseFlag16( analogswitch, CHANNEL_ATTR_B );
   else       analogswitch = lowerFlag16( analogswitch, CHANNEL_ATTR_B );
   vgainb = gain;
}

void OsciloscopeControl::setYRangeScaleB(uint gain, uint attr)
{
   if (attr) analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_B);
   else      analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_B);
   vgainb = gain;
}

void OsciloscopeControl::setYPositionB(int pos)
{
   pos = min(pos,1500);
   pos = max(pos,-1500);
   offsetb = pos;
}

void OsciloscopeControl::setXRange(ishort range)
{
   xRange = range;
}

void OsciloscopeControl::setControl(uint selected)
{
   switch( selected ){
   case 0: control = CONTROLL_NORMAL;   break;
   case 1: control = CONTROLL_TEST;     break;
   case 2: control = CONTROLL_SHUTDOWN; break;
   // case 1 : control = CONTROLL_RESET;    break;
   // case 3 : control = CONTROLL_WAKEUP;   break;
   };
}

void OsciloscopeControl::setFrameSize(uint value)
{
   uint size = 50*(value/50);
   size = max(50u,size);
   size = min((uint)NUM_SAMPLES,size);
   frameSize = size;
}


void OsciloscopeControl::setTriggerSource(int value)
{
   triggerSource = value;
}

void OsciloscopeControl::setTriggerReArm(bool on)
{
   ushort mask = (1 << 2);
   if (on) triggerMode = triggerMode | mask;
   else    triggerMode = triggerMode & ~mask;
}

void OsciloscopeControl::setTriggerMode(int value)
{
   triggerMode = value;
}

void OsciloscopeControl::setTriggerSlope(int value)
{
   triggerSlope = value;
}

void OsciloscopeControl::setTriggerPre(float perc)
{
   triggerPercent = (perc/100.f)*float(NUM_SAMPLES-1);
}

void OsciloscopeControl::setTriggerHis(int perc)
{
   triggerHis = perc;
}

void OsciloscopeControl::setTriggerLevel(int perc)
{
   triggerLevel = perc;
}

void OsciloscopeControl::setHoldoff(double holdoff)
{
   double     N = double(holdoff)*double(NANO) / 10.0;
   uint   value = uint(N);
   holdoffH = (value & 0xFFFF0000) >> 16;
   holdoffL =  value & 0x0000FFFF;
}

///////////////////////////////////////////////////////////////////////
// Digital
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl::setDigitalStart(int start)
{
   Flag16 startBits;
   startBits.set(start);

   Flag16 controlBits;
   controlBits.set(dt_control);
   controlBits.bit(1<<8, startBits.is(1) );
   controlBits.bit(1<<9, startBits.is(2) );
   dt_control = controlBits.get();
}

void OsciloscopeControl::setDigitalMode(int mode)
{
   Flag16 modeBits;
   modeBits.set(mode);

   Flag16 controlBits;
   controlBits.set(dt_control);
   controlBits.bit(1 << 4, modeBits.is(1));
   dt_control = controlBits.get();
}

void OsciloscopeControl::setDigitalChannel(int channel)
{
   Flag16 channelBits;
   channelBits.set(channel);

   Flag16 controlBits;
   controlBits.set(dt_control);
   controlBits.bit(1 << 0, channelBits.is(1));
   controlBits.bit(1 << 1, channelBits.is(2));
   controlBits.bit(1 << 2, channelBits.is(4));
   controlBits.bit(1 << 3, channelBits.is(8));
   dt_control = controlBits.get();
}

///////////////////////////////////////////////////////////////////////
// setDigital - stage
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl::setDigitalDelay(DigitalStage stage,ushort delay)
{
   switch( stage ){
   case 0: dt_delayMaxcnt1 = delay; break;
   case 1: dt_delayMaxcnt2 = delay; break;
   case 2: dt_delayMaxcnt3 = delay; break;
   case 3: dt_delayMaxcnt4 = delay; break;
   default:
      CORE_ERROR("invalid digital stage",0);
   };
}

void OsciloscopeControl::setDigitalMask(DigitalStage stage,DigitalBit bit,int value)
{
   Flag16 mask;
   switch( stage ){
   case 0: mask.set(digitalMask1); break;
   case 1: mask.set(digitalMask2); break;
   case 2: mask.set(digitalMask3); break;
   case 3: mask.set(digitalMask4); break;
   default:
      CORE_ERROR("invalid digital stage",0);
   };
   //
   if( value ) mask.raise( BIT(bit) );
   else        mask.lower( BIT(bit) );
   //
   switch( stage ){
   case 0: digitalMask1 = mask.get(); break;
   case 1: digitalMask2 = mask.get(); break;
   case 2: digitalMask3 = mask.get(); break;
   case 3: digitalMask4 = mask.get(); break;
   default:
      CORE_ERROR("invalid digital stage",0);
   };
}

void OsciloscopeControl::setDigitalPattern(DigitalStage stage,DigitalBit bit,DigitalPattern pattern)
{
   Flag16 patternA;
   Flag16 patternB;
   switch( stage ){
   case 0:
      patternA.set(digitalPattern1a);
      patternB.set(digitalPattern1b);
      break;
   case 1:
      patternA.set(digitalPattern2a);
      patternB.set(digitalPattern2b);
      break;
   case 2:
      patternA.set(digitalPattern3a);
      patternB.set(digitalPattern3b);
      break;
   case 3:
      patternA.set(digitalPattern4a);
      patternB.set(digitalPattern4b);
      break;
   default:
      CORE_ERROR("invalid digital stage",0);
   };
   //
   switch( pattern ) {
   case DIGITAL_PATTERN_0: // 00
      patternA.lower( BIT(bit) );
      patternB.lower( BIT(bit) );
      break;
   case DIGITAL_PATTERN_1: // 11
      patternA.raise( BIT(bit) );
      patternB.raise( BIT(bit) );
      break;
   case DIGITAL_PATTERN_RISING: // 01
      patternA.lower( BIT(bit) );
      patternB.raise( BIT(bit) );
      break;
   case DIGITAL_PATTERN_FALLING: // 10
      patternA.raise( BIT(bit) );
      patternB.lower( BIT(bit) );
      break;
   default:
      CORE_ERROR("invalid digital stage",0);
   };
   //
   //
   switch( stage ){
   case 0:
      digitalPattern1a = patternA.get();
      digitalPattern1b = patternB.get();
      break;
   case 1:
      digitalPattern2a = patternA.get();
      digitalPattern2b = patternB.get();
      break;
   case 2:
      digitalPattern3a = patternA.get();
      digitalPattern3b = patternB.get();
      break;
   case 3:
      digitalPattern4a = patternA.get();
      digitalPattern4b = patternB.get();
      break;
   default:
      CORE_ERROR("invalid digital stage",0);
   };
}


///////////////////////////////////////////////////////////////////////
// generator 0
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl::setGeneratorType0(GeneratorType type)
{
   uint mask = 1<<8;
   if( generatorType0 & mask ) generatorType0 = type | mask;
   else                        generatorType0 = type;
}

void OsciloscopeControl::setGeneratorOn0(int onoff)
{
   uint mask = 1<<8;
   if( onoff ) generatorType0 = generatorType0 | mask;
   else        generatorType0 = generatorType0 & ~mask;
}

void OsciloscopeControl::setGeneratorSlope0(int onoff)
{

}

void OsciloscopeControl::setGeneratorVoltage0(int volt)
{
   uint voltage = volt;
   uint mask = 1<<12;
   if( generatorVoltage0 & mask ) generatorVoltage0 = voltage | mask;
   else                           generatorVoltage0 = voltage;
}

void OsciloscopeControl::setGeneratorOffset0(int perc)
{
   generatorOffset0 = perc;
}

void OsciloscopeControl::setGeneratorFrequency0(float freq,float fs)
{
   double delta = 0;
   uint mask = (1<<8);
   mask = ~mask;
   uint type = generatorType0 & mask;
   switch( type ){
   case GENERATOR_CUSTOM:
   case GENERATOR_SIN:
   case GENERATOR_COS:
   case GENERATOR_TRIANGLE:
   case GENERATOR_RAMP_UP:
   case GENERATOR_RAMP_DOWN:
   case GENERATOR_SQUARE:
   case GENERATOR_DELTA:
   case GENERATOR_DC:
   case GENERATOR_NOISE:
      delta = 8188.0*double(freq)/double(fs);
      break;
   // case GENERATOR_SAW:
   // case GENERATOR_DELTA:
   // case GENERATOR_DC:
   // case GENERATOR_NOISE:
   //   delta = 4094.0*double(freq)/double(fs);
   //   break;
   };
   //
   uint gDelta = uint( delta * 1048575.0 );
   generatorDeltaH0 = ( gDelta & 0xFFFF0000 ) >> 16;
   generatorDeltaL0 = gDelta & 0x0000FFFF;
}

void OsciloscopeControl::setGeneratorSquareDuty0(int perc)
{
   generatorSqueareDuty0 = perc;
}

///////////////////////////////////////////////////////////////////////
// generator 1
///////////////////////////////////////////////////////////////////////

void OsciloscopeControl::setGeneratorType1(GeneratorType type)
{
   uint mask = 1<<8;
   if( generatorType1 & mask ) generatorType1 = type | mask;
   else                        generatorType1 = type;
}

void OsciloscopeControl::setGeneratorOn1(int onoff)
{
   uint mask = 1<<8;
   if( onoff ) generatorType1 = generatorType1 | mask;
   else        generatorType1 = generatorType1 & ~mask;
}

void OsciloscopeControl::setGeneratorSlope1(int onoff)
{

}

void OsciloscopeControl::setGeneratorVoltage1(int volt)
{
   uint voltage = volt;
   uint mask = 1<<12;
   if( generatorVoltage1 & mask ) generatorVoltage1 = voltage | mask;
   else                           generatorVoltage1 = voltage;
}

void OsciloscopeControl::setGeneratorOffset1(int perc)
{
   generatorOffset1 = perc;
}

void OsciloscopeControl::setGeneratorFrequency1(float freq,float fs)
{
   double delta = 0;
   uint mask = (1<<8);
   mask = ~mask;
   uint type = generatorType1 & mask;
   switch( type ){
   case GENERATOR_CUSTOM:
   case GENERATOR_SIN:
   case GENERATOR_COS:
   case GENERATOR_TRIANGLE:
   case GENERATOR_RAMP_UP:
   case GENERATOR_RAMP_DOWN:
   case GENERATOR_SQUARE:
   case GENERATOR_DELTA:
   case GENERATOR_DC:
   case GENERATOR_NOISE:
      delta = 8188.0*double(freq)/double(fs);
      break;
   // case GENERATOR_SAW:
   // case GENERATOR_DELTA:
   // case GENERATOR_DC:
   // case GENERATOR_NOISE:
   //   delta = 4094.0*double(freq)/double(fs);
   //   break;
   };
   //
   uint gDelta = uint( delta * 1048575.0 );
   generatorDeltaH1 = ( gDelta & 0xFFFF0000 ) >> 16;
   generatorDeltaL1 = gDelta & 0x0000FFFF;
}

void OsciloscopeControl::setGeneratorSquareDuty1(int perc)
{
   generatorSqueareDuty1 = perc;
}

void OsciloscopeControl::setDigitalVoltage(double volt)
{
   double kDigital = pOsciloscope->settings.digitalVoltageCoeficient;
   double Vmin = 1.25 * ((0.0 / kDigital) + 1.0);
   double Vmax = 1.25 * ((255.0 / kDigital) + 1.0);

   double dVolt = clamp(double(volt), Vmin, Vmax);

   digitalVoltage = ushort( ((dVolt / 1.25) - 1.0)*kDigital );
}

void OsciloscopeControl::setDigitalInputOutput(int inout15,int inout7)
{
   // 00 = OUT, OUT
   // 01 = OUT, IN
   // 10 = IN, OUT
   // 11 = IN, IN
   digitalInputOutput = (inout15 << 1) | inout7;
}

void OsciloscopeControl::setDigitalOutputBit(int bit, int onoff)
{
   Flag16 output, mask;

   output.set(digitalOutputBit);
   mask.set(digitalOutputMask);

   if(onoff == 0 || onoff == 1 )
   {
      output.bit( BIT(bit), onoff);
      mask.bit( BIT(bit), 1);
   }
   else if( onoff == 2 )
   {
      output.bit( BIT(bit), onoff);
      mask.bit( BIT(bit), 0);
   }

   digitalOutputBit  = output.get();
   digitalOutputMask = mask.get();
}

void OsciloscopeControl::setDigitalClockDivide(uint divider)
{
   digitalClkDivideH = (divider & 0xFFFF0000) >> 16;
   digitalClkDivideL = divider & 0x0000FFFF;
}


///////////////////////////////////////////////////////////////////////
// get
///////////////////////////////////////////////////////////////////////
int OsciloscopeControl::getEts()
{
   Flag16 tmp;
   tmp.set(analogswitch);
   return tmp.is(CHANNEL_ETS);
}

uint OsciloscopeControl::getControl()
{
   switch (control) {
   case CONTROLL_NORMAL:   return 0;
   case CONTROLL_TEST:     return 1;
   case CONTROLL_SHUTDOWN: return 2;
   // CONTROLL_RESET;    break;
   // CONTROLL_WAKEUP;   break;
   };
   return 0;
}

uint OsciloscopeControl::getYRangeA()
{
   return getVolt(0,vgaina);
}

float OsciloscopeControl::getYScaleA()
{
   return 0.f;
}
int OsciloscopeControl::getYPositionA()
{
   return (int)offseta;
}

uint OsciloscopeControl::getYRangeB()
{
   return getVolt(1,vgainb);
}
float OsciloscopeControl::getYScaleB()
{
   return 0.f;
}
int OsciloscopeControl::getYPositionB()
{
   return (int)offsetb;
}

float OsciloscopeControl::getTriggerPre()
{
   return 100.f*( float(triggerPercent)/float(NUM_SAMPLES - 1) );
}

int OsciloscopeControl::getTriggerHis()
{
   return triggerHis;
}

int OsciloscopeControl::getTriggerLevel()
{
   return triggerLevel;
}

ishort OsciloscopeControl::getXRange()
{
   return xRange;
}

uint OsciloscopeControl::getFrameSize()
{
   return frameSize;
}

double OsciloscopeControl::getHoldoff()
{
   uint holdOffInt = uint(holdoffL) | ( uint(holdoffH) << 16);
   double value = double(holdOffInt) * 10.0 / double(NANO);
   return value;
}

int OsciloscopeControl::getDigitalStart()
{
   return (dt_control & 0x300) >> 8;
}

int OsciloscopeControl::getDigitalMode()
{

   return ( dt_control & 0x10 ) >> 4;
}

int OsciloscopeControl::getDigitalChannel()
{
   return dt_control & 0xF;
}

ushort OsciloscopeControl::getDigitalDelay(DigitalStage stage)
{
   switch (stage) {
   case 0: return dt_delayMaxcnt1;
   case 1: return dt_delayMaxcnt2;
   case 2: return dt_delayMaxcnt3;
   case 3: return dt_delayMaxcnt4;
   default:
      CORE_ERROR("invalid digital stage", 0);
   };
   return 0;
}

int OsciloscopeControl::getDigitalMask(DigitalStage stage, DigitalBit bit)
{
   Flag16 mask;
   switch (stage) {
   case 0: mask.set(digitalMask1); break;
   case 1: mask.set(digitalMask2); break;
   case 2: mask.set(digitalMask3); break;
   case 3: mask.set(digitalMask4); break;
   default:
      CORE_ERROR("invalid digital stage", 0);
   };

   return mask.is( BIT(bit) );
}

DigitalPattern OsciloscopeControl::getDigitalPattern(DigitalStage stage, DigitalBit bit)
{
   Flag16 patternA;
   Flag16 patternB;
   switch (stage) {
   case 0:
      patternA.set(digitalPattern1a);
      patternB.set(digitalPattern1b);
      break;
   case 1:
      patternA.set(digitalPattern2a);
      patternB.set(digitalPattern2b);
      break;
   case 2:
      patternA.set(digitalPattern3a);
      patternB.set(digitalPattern3b);
      break;
   case 3:
      patternA.set(digitalPattern4a);
      patternB.set(digitalPattern4b);
      break;
   default:
      CORE_ERROR("invalid digital stage", 0);
   };

   int bitA = patternA.is( BIT(bit) );
   int bitB = patternB.is( BIT(bit) );
   if ( bitA &&  bitB ) return DIGITAL_PATTERN_1;
   if (!bitA && !bitB ) return DIGITAL_PATTERN_0;
   if (!bitA &&  bitB)  return DIGITAL_PATTERN_RISING;
   if ( bitA && !bitB)  return DIGITAL_PATTERN_FALLING;
   return DIGITAL_PATTERN_1;
}

///////////////////////////////////////////////////////////////////////
// generator 0
///////////////////////////////////////////////////////////////////////

GeneratorType OsciloscopeControl::getGeneratorType0()
{
   uint mask = 1 << 8;
   if (generatorType0 & mask) return (GeneratorType)(generatorType0 & ~mask);
   else                       return (GeneratorType)(generatorType0);
}

int OsciloscopeControl::getGeneratorOn0()
{
   uint mask = 1 << 8;
   if ( generatorType0 & mask ) return 1;
   else return 0;
}

int OsciloscopeControl::getGeneratorSlope0()
{
   return 0;
}

int OsciloscopeControl::getGeneratorVoltage0()
{
   uint mask = 1 << 12;
   if (generatorVoltage0 & mask) return generatorVoltage0 & ~mask;
   else return generatorVoltage0;
}

int OsciloscopeControl::getGeneratorOffset0()
{
   return generatorOffset0;
}

float OsciloscopeControl::getGeneratorFrequency0(float fs)
{
   uint gDelta = uint(generatorDeltaL0) | uint(generatorDeltaH0 << 16);
   double delta = double(gDelta / 1048575.0);
   return float( delta*double(fs)/8188.0 );
}

int OsciloscopeControl::getGeneratorSquareDuty0()
{
   return generatorSqueareDuty0;
}


///////////////////////////////////////////////////////////////////////
// generator 1
///////////////////////////////////////////////////////////////////////

GeneratorType OsciloscopeControl::getGeneratorType1()
{
   uint mask = 1 << 8;
   if (generatorType1 & mask) return (GeneratorType)(generatorType1 & ~mask);
   else                       return (GeneratorType)(generatorType1);
}

int OsciloscopeControl::getGeneratorOn1()
{
   uint mask = 1 << 8;
   if (generatorType1 & mask) return 1;
   else return 0;
}

int OsciloscopeControl::getGeneratorSlope1()
{
   return 0;
}

int OsciloscopeControl::getGeneratorVoltage1()
{
   uint mask = 1 << 12;
   if (generatorVoltage1 & mask) return generatorVoltage1 & ~mask;
   else return generatorVoltage1;
}

int OsciloscopeControl::getGeneratorOffset1()
{
   return generatorOffset1;
}

float OsciloscopeControl::getGeneratorFrequency1(float fs )
{
   uint gDelta = uint(generatorDeltaL1) | uint(generatorDeltaH1 << 16);
   double delta = double(gDelta / 1048575.0);
   return float(delta*double(fs) / 8188.0);
}

int OsciloscopeControl::getGeneratorSquareDuty1()
{
   return generatorSqueareDuty1;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////

double OsciloscopeControl::getDigitalVoltage()
{
   double kDigital = pOsciloscope->settings.digitalVoltageCoeficient;
   double Vmin = 1.25 * ((0.0 / kDigital) + 1.0);
   double Vmax = 1.25 * ((255.0 / kDigital) + 1.0);
   double voltage = 1.25 * ((double(digitalVoltage) / kDigital) + 1.0);
   return clamp(voltage, Vmin, Vmax);
}

int OsciloscopeControl::getDigitalInputOutput15()
{
   return (digitalInputOutput >> 1) & 0x1;
}

int OsciloscopeControl::getDigitalInputOutput7()
{
   return digitalInputOutput & 0x1;
}

int OsciloscopeControl::getDigitalOutputBit(int bit)
{
   Flag16 bits;
   bits.set(digitalOutputBit);
   return bits.is(BIT(bit));
}

uint OsciloscopeControl::getDigitalClockDivide()
{
   return uint(digitalClkDivideL) | uint(digitalClkDivideH  << 16);
}

///////////////////////////////////////////////////////////////////////
// transferData
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl::transferData()
{
   int ret = usbTransferDataOut( 2, (byte*)this, sizeof(OsciloscopeControl), 0, pOsciloscope->settings.usbEp2TimeOut );
}

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeGrid
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeGrid::set(float x,float y)
{
   xCount = x;
   xMax   =  xCount/2.f;
   xMin   = -xCount/2.f;
   yCount = y;
   yMax   =  yCount/2.f;
   yMin   = -yCount/2.f;
}

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeCamera
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeCamera::OsciloscopeCamera()
{
   zoom  = 1.f;
   theta = 135.f;
   phi   = 45.f;
}

////////////////////////////////////////////////////////////////////////////////
//
// Osciloscope
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeManager::OsciloscopeManager()
{
   scrollThread = false;
   frameTime = 0.f;
   clearRenderTarget = 0;
   clearThermal = 0;
   analogWindowSize = 1.f;
   sliderMode  = 0;
   openglFocus = 1;
   redrawEts = 0;
   signalMode = SIGNAL_MODE_PAUSE;
   windowSlot = 0;
}

void *malloc_fn2(size_t sz)
{
   return memoryAllocate(sz);
}

void free_fn2(void *ptr)
{
   return memoryFree(ptr);
}

cJSON_Hooks hooks;

int usbHotPlugCallback(int flags)
{
   if (flags == USB_CALLBACK_DEVICE_ARIVED)
   {
      usbUploadFirmwareToFx2(0, 0, 0);

      char*  buffer = 0;
      ilarge bufferSize = 0;
      fileLoad(pOsciloscope->settings.fpgaFileName.asChar(), &buffer, &bufferSize);
         usbUploadFirmwareToFpga((byte*)buffer, bufferSize, 1);
      fileFree(buffer);

      pOsciloscope->control.transferData();
   }
   return 0;
}

int SDLCALL CaptureThreadFunction(void *data);
int SDLCALL UpdateThreadFunction(void *data);
int SDLCALL RenderThreadFunction(void *data);

int OsciloscopeManager::start()
{
   ////////////////////////////////////////////////
   // load settings
   ////////////////////////////////////////////////
   hooks.malloc_fn = malloc_fn2;
   hooks.free_fn = free_fn2;
   cJSON_InitHooks(&hooks);
   settings.load();

   ////////////////////////////////////////////////
   // apply settings
   ////////////////////////////////////////////////
   control.setYRangeScaleA(vc2Volt, 1.f);
   control.setYRangeScaleB(vc2Volt, 1.f);

   ////////////////////////////////////////////////
   // apply canvas allocation settings
   ////////////////////////////////////////////////
   pCanvas3d->setVertexBufferSize( settings.vertexBufferSizeMegaByte * 1024 * 1024);
   pCanvas3d->setThreadBuffer( settings.threadBuffer );
   pCanvas2d->setThreadBuffer( settings.threadBuffer );

   ////////////////////////////////////////////////
   // generator fs
   ////////////////////////////////////////////////
   control.setGeneratorFrequency0(50.f, settings.generatorFs);
   control.setGeneratorFrequency1(50.f, settings.generatorFs);

   ////////////////////////////////////////////////
   // custom signal
   ////////////////////////////////////////////////
   FORMAT_BUFFER();
   FORMAT_PATH("data/signal/custom.signal");
   window.hardwareGenerator.loadCustomData(0, formatBuffer);
   window.hardwareGenerator.loadCustomData(1, formatBuffer);

   //////////////////////////////////////////////////////////
   // init usb
   //////////////////////////////////////////////////////////
   usbInit();

   //////////////////////////////////////////////////////////
   // thread count
   //////////////////////////////////////////////////////////
   uint cpuCount = SDL_GetCPUCount();
   pRender->setThreadCount( min(settings.threadCount,MAX_THREAD) );

   //////////////////////////////////////////////////////////
   // allocate memory for signal
   //////////////////////////////////////////////////////////
   allocate();

   //////////////////////////////////////////////////////////
   // setup thread data
   //////////////////////////////////////////////////////////
   threadActive = true;
   threadLoop.capture.setCount(min(settings.threadBuffer, MAX_THREAD));
   threadLoop.update.setCount(min(settings.threadBuffer, MAX_THREAD));

   //////////////////////////////////////////////////////////
   // camera setup
   //////////////////////////////////////////////////////////
   cameraOsc.ortho.View.Pos() = Vector4(0.f, 0.f, -1.0f, 1.f);
   cameraOsc.zoom = 1.f;
   cameraFFT.ortho.View.Pos() = Vector4(0.f, 0.f, -1.0f, 1.f);
   cameraFFT.zoom = 1.f;
   grid.set(10.f, 10.f);

   ///////////////////////////////////////////////
   // launch render thread
   ///////////////////////////////////////////////
   SDL_AtomicUnlock(&renderLock);
   SDL_AtomicUnlock(&uploadFirmwareLock);
   SDL_AtomicUnlock(&displayLock);
   SDL_AtomicSet(&contextCreated, 0);
   SDL_CreateThread(RenderThreadFunction, "RenderThread", 0);
   while (SDL_AtomicCAS(&contextCreated, 1, 0) == SDL_FALSE) {};

   //////////////////////////////////////////////////////////
   // launch capture and update thread
   //////////////////////////////////////////////////////////
   startThreads();

   ////////////////////////////////////////////////
   // icon
   ////////////////////////////////////////////////
   FORMAT_PATH("data/icon/icon64.bmp")
      SDL_Surface* icon = SDL_LoadBMP(formatBuffer);
   SDL_SetWindowIcon(sdlWindow, icon);
   SDL_FreeSurface(icon);


   ////////////////////////////////////////////////
   // callbacks
   ////////////////////////////////////////////////
   pRender->registerCallback(this, 10);
   pCamera->registerCamera(&cameraOsc.ortho);
   pCamera->registerCamera(&cameraFFT.ortho);
   return 0;
}

void OsciloscopeManager::startThreads()
{
   SDL_zero(renderWindow);
   SDL_zero(renderData);

   // capture
   SDL_CreateThread(CaptureThreadFunction, "CaptureThread", this);

   // canvas
   FORMAT_BUFFER();
   for (uint i = 0; i < pRender->getThreadCount(); i++)
   {
      pCanvas3d->threadBegin(i);
      pCanvas2d->threadBegin(i);
   }

   // update
   static int index[MAX_THREAD] = { 0 };
   for (uint i = 0; i < pRender->getThreadCount(); i++)
   {
      index[i] = i;
      FORMAT("UpdateThread_%d", i);
      SDL_CreateThread(UpdateThreadFunction, formatBuffer, &index[i]);
   }
}


void OsciloscopeManager::oscCameraSetup(int enable)
{
   if (enable)
   {
      Vector4 pos = Vector4(1.f, 1.f, -1.f, 1.f);
      Vector4 at = Vector4(0.f, 0.f, 0.5f, 1.f);
      Vector4 up = Vector4(0.f, 1.f, 0.f, 1.f);
      cameraOsc.zoom = 1.f;
      cameraOsc.ortho.lookAt(pos, at, up);
   }
   else
   {
      Vector4 pos = Vector4(0.f, 0.f, -1.f, 1);
      Vector4 at = Vector4(0.f, 0.f, 0.f, 1.f);
      Vector4 up = Vector4(0.f, 1.f, 0.f, 1.f);
      cameraOsc.zoom = 1.f;
      cameraOsc.ortho.lookAt(pos, at, up);
   }

   cameraOsc.theta = 0;
   cameraOsc.phi = 0;
}

uint OsciloscopeManager::max3dTesselation(uint value,WndMain& wnd)
{
   int onOff = 0;
   if (wnd.channel01.OscOnOff) onOff++;
   if (wnd.channel02.OscOnOff) onOff++;
   if (wnd.function.OscOnOff)  onOff++;

   ularge triangleBytes = 32*3; // pos, normal
   ularge safetyBytes   = 2*MEGABYTE; // pos, normal
   while(1)
   {
        ularge neededBytes = (ularge(onOff*settings.historyFrameDisplay)*ularge(NUM_SAMPLES*2)*triangleBytes)/value;
        ularge actualBytes = (ularge(settings.vertexBufferSizeMegaByte)*MEGABYTE - safetyBytes);
        if( neededBytes < actualBytes || value > 1024 )
            return value;
        value++;
   }
   return value;
}

void OsciloscopeManager::fftCameraSetup(int enable)
{
   if (enable)
   {
      Vector4 pos = Vector4(1.f, 1.f, -1.f,  1.f);
      Vector4  at = Vector4(0.f, 0.f,  0.5f, 1.f);
      Vector4  up = Vector4(0.f, 1.f,  0.f,  1.f);
      cameraFFT.zoom = 1.f;
      cameraFFT.ortho.lookAt(pos, at, up);
   }
   else
   {
      Vector4 pos = Vector4(0.f, 0.f, -1.f, 1);
      Vector4 at = Vector4(0.f, 0.f, 0.f, 1.f);
      Vector4 up = Vector4(0.f, 1.f, 0.f, 1.f);
      cameraFFT.zoom = 1.f;
      cameraFFT.ortho.lookAt(pos, at, up);
   }

   cameraFFT.theta = 0;
   cameraFFT.phi = 0;
}

int OsciloscopeManager::update(float dt)
{
   if (fileThread.isRunning()) return 0;

   ////////////////////////////////////////////////////////////////////////////////
   // usb
   ////////////////////////////////////////////////////////////////////////////////
   usbUpdate();

   // if (!openglFocus) return 0;

   ////////////////////////////////////////////////////////////////////////////////
   // setup
   ////////////////////////////////////////////////////////////////////////////////
   clearRenderTarget = !window.horizontal.ETS;
   redrawEts = 0;

   ////////////////////////////////////////////////////////////////////////////////
   // mouse
   ////////////////////////////////////////////////////////////////////////////////
   int mX = pInput->getMouseX();
   int mY = pInput->getMouseY();
   int mRelX = pInput->getMouseRelX();
   int mRelY = pInput->getMouseRelY();
   int mWheel = pInput->getMouseWheel();

   ////////////////////////////////////////////////////////////////////////////////
   // window size
   ////////////////////////////////////////////////////////////////////////////////
   int wWidth = 0;
   int wHeight = 0;
   SDL_GetWindowSize(sdlWindow, &wWidth, &wHeight);

   ////////////////////////////////////////////////////////////////////////////////
   // relative mouse position
   ////////////////////////////////////////////////////////////////////////////////
   float fRelX = float(mRelX) / float(wWidth);
   float fRelY = float(mRelY) / float(wHeight);

   ////////////////////////////////////////////////////////////////////////////////
   // dimensions
   ////////////////////////////////////////////////////////////////////////////////
   float width = (float)pRender->width;
   float height = (float)pRender->height;
   float aspect = (float)pRender->aspect;

   ////////////////////////////////////////////////////////////////////////////////
   // speed
   ////////////////////////////////////////////////////////////////////////////////
   float moveSpeed = 22.f;
   float zoomSpeedFFT = cameraFFT.zoom*0.05f;
   float zoomSpeedOsc = cameraOsc.zoom*0.05f;

   ////////////////////////////////////////////////////////////////////////////////
   // move speed
   ////////////////////////////////////////////////////////////////////////////////
   Vector4 CamMoveSpeed = Vector4(moveSpeed*dt, moveSpeed*dt, 0.f, 0.f);

   ////////////////////////////////////////////////////////////////////////////////
   // View
   ////////////////////////////////////////////////////////////////////////////////
   if ( !window.fftDigital.is(VIEW_SELECT_OSC_3D) && !window.fftDigital.is(VIEW_SELECT_FFT_3D) )
   {
      if ((mX > 0 && mX < wWidth && mY > wWidth && mY < wHeight) ||
          (mX > 0 && mX < wWidth && mY >= 0 && mY < wWidth))
      {
         ////////////////////////////////////////////////////////////////////////////////
         // zoom
         ////////////////////////////////////////////////////////////////////////////////
         if (mWheel)
         {
            clearThermal = 1;
            clearRenderTarget = 1;
            redrawEts = 1;

            cameraFFT.zoom -= zoomSpeedFFT*dt*mWheel;
            cameraFFT.zoom = clamp<float>(cameraFFT.zoom, 0.001f, 1.f);

            cameraOsc.zoom -= zoomSpeedOsc*dt*mWheel;
            cameraOsc.zoom = clamp<float>(cameraOsc.zoom, 0.001f, 1.f);
         }

         ////////////////////////////////////////////////////////////////////////////////
         // measure
         ////////////////////////////////////////////////////////////////////////////////
         float mouseNormalized = float(mX) / float(pRender->width);
         if (aspect > 1) mouseNormalized = float(mX) / float(pRender->height);
         mouseNormalized *= 1.2f;
         mouseNormalized -= (0.5f*1.2f);

         float mouseNormalizedY = float(mY) / ( float(pRender->width)*analogWindowSize );
         mouseNormalizedY  = mouseNormalizedY*1.2f;
         mouseNormalizedY -= (0.5f*1.2f);

         float mouseOSC  =  mouseNormalized*cameraOsc.zoom + (cameraOsc.ortho.View.Pos().x+0.5);
         float mouseFFT  =  mouseNormalized*cameraFFT.zoom + (cameraFFT.ortho.View.Pos().x);
         float mouseY = -mouseNormalizedY;

         ////////////////////////////////////////////////////////////////////////////////
         // move
         ////////////////////////////////////////////////////////////////////////////////
         if (pInput->isClick(SDL_BUTTON_LEFT))
         {
            window.measure.data.pickX0.onClick();
            window.measure.data.pickX1.onClick();
            window.measure.data.pickY0.onClick();
            window.measure.data.pickY1.onClick();
            window.measure.data.pickFFT0.onClick();
            window.measure.data.pickFFT1.onClick();
         }

         window.measure.data.pickX0.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
         window.measure.data.pickX1.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
         window.measure.data.pickY0.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
         window.measure.data.pickY1.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
         window.measure.data.pickFFT0.onUpdate(mouseFFT, mouseY, 0.f, window.horizontal.Frame);
         window.measure.data.pickFFT1.onUpdate(mouseFFT, mouseY, 0.f, window.horizontal.Frame);

         if (pInput->isMouse(SDL_BUTTON_LEFT))
         {
            clearRenderTarget = 1;
            redrawEts = 1;

            OsciloscopeSlider slider;
            slider.Rectangle(sliderRectW, sliderRectH, sliderRectX, sliderRectY, pRender->width, pRender->height, analogWindowSize );
            slider.MinMax(sliderMin, sliderMax, pRender->width, pRender->height, cameraFFT.ortho.View.Pos().x, analogWindowSize);

            insideSliderBox = (mX > sliderMin && mX < sliderMax && mY >= sliderRectY && mY <= sliderRectY + sliderRectH);
            insideSliderArea = (mY >= sliderRectY && mY <= sliderRectY + sliderRectH);


            // slider
            if (sliderMode == 1 || insideSliderBox)
            {
               clearThermal = 1;

               sliderMode = 1;

               float xNormalized = (float(mX)) / (sliderRectW);
               xNormalized = clamp(1.f-xNormalized, 0.f, 1.f);
               cameraOsc.ortho.View.Pos().x = -(xNormalized - 0.5f);
               cameraFFT.ortho.View.Pos().x = -(xNormalized - 0.5f);
            }
            // up / down
            else if (insideSliderArea)
            {
               sliderMode = 2;
               analogWindowSize += (float(mRelY)) / (float(pRender->width));
            }
            else if(sliderMode == 2)
            {
               analogWindowSize += (float(mRelY)) / (float(pRender->width));
            }
            else
            {
               clearThermal = 1;
               float moveFactor = (cameraFFT.ortho.width / cameraOsc.ortho.width);
               Vector4 Move = Vector4(-fRelX, 0, 0, 1 );
               Vector4 MoveFFT = Move*CamMoveSpeed*Vector4(cameraFFT.zoom*moveFactor);
               Vector4 MoveOsc = Move*CamMoveSpeed*Vector4(cameraOsc.zoom);
               cameraOsc.ortho.View.Pos() += MoveFFT;
               cameraFFT.ortho.View.Pos() += MoveOsc;
            }
         }
         else
         {
            sliderMode = 0;
         }

         ////////////////////////////////////////////////////////////////////////////////
         // reset
         ////////////////////////////////////////////////////////////////////////////////
         if (pInput->isMouse(SDL_BUTTON_RIGHT))
         {
            clearThermal = 1;
            clearRenderTarget = 1;
            redrawEts = 1;

            cameraFFT.ortho.View.Pos() = Vector4(0, 0, -1.f, 1.f);
            cameraFFT.zoom = 1.f;

            cameraOsc.ortho.View.Pos() = Vector4(0, 0, -1.f, 1.f);
            cameraOsc.zoom = 1.f;
         }
      }
   }


//   ////////////////////////////////////////////////////////////////////////////////
//   // size
//   ////////////////////////////////////////////////////////////////////////////////
//   if (pInput->isKey(SDLK_w))
//   {
//      analogWindowSize += 0.25f*dt;
//   }
//   if (pInput->isKey(SDLK_s))
//   {
//      analogWindowSize -= 0.25f*dt;
//   }
//   if (pInput->isKey(SDLK_a))
//   {
//      window.measure.data.pickX0.position.x -= 0.1f*dt;
//   }
//   if (pInput->isKey(SDLK_d))
//   {
//      window.measure.data.pickX0.position.x += 0.1f*dt;
//   }
//   if (pInput->isKey(SDLK_SPACE))
//   {
//      analogWindowSize = (height / 2.f) / width;
//   }

   analogWindowSize = clamp(analogWindowSize, 0.f, 1.f / aspect);

   float oscW = width;
   float oscH = width*analogWindowSize;
   float oscA = oscW/oscH;
   float fftW = width;
   float fftH = height-width*analogWindowSize;
   float fftA = fftW/fftH;

   fftAspectX = width / (height - width*analogWindowSize);
   fftAspectY = (width - height*analogWindowSize) / height;
   oscAspectX = width / (width*analogWindowSize);
   oscAspectY = (height*analogWindowSize) / height;

   float scale = (0.5f / aspect);

   ////////////////////////////////////////////////////////////////////////////////
   // FFT Camera
   ////////////////////////////////////////////////////////////////////////////////
   float fftWidth  = 1.2f*cameraFFT.zoom;
   float fftHeight = 1.2f;
   fftScaleX = 1.f*scale*cameraFFT.zoom;
   fftScaleY = fftA*scale;
   cameraFFT.ortho.setOrthographic(fftWidth, fftHeight, 0.0001f, 1.2f);

   ////////////////////////////////////////////////////////////////////////////////
   // orthographics
   ////////////////////////////////////////////////////////////////////////////////
   float oscWidth  = 1.2f*cameraOsc.zoom;
   float oscHeight = 1.2f;
   oscScaleX = 1.f*scale*cameraFFT.zoom;
   oscScaleY = oscA*scale;
   cameraOsc.ortho.setOrthographic(oscWidth, oscHeight, 0.0001f, 1.2f);

   ////////////////////////////////////////////////////////////////////////////////
   // 3d fft
   ////////////////////////////////////////////////////////////////////////////////
   if (window.fftDigital.is(VIEW_SELECT_FFT_3D) )
   {
      // persepctive
      int sizeX       = 0;
      int sizeY       = 0;
      int sizeYOsc    = 0;
      int mouseInView = 0;
      {
         sizeX       = width;
         sizeY       = height - width*analogWindowSize;
         sizeYOsc    = width*analogWindowSize;
         mouseInView = mX > 0 && mX < sizeX && mY > width*analogWindowSize && mY < height;
      }
      float aspect = float(sizeX) / float(sizeY);
      cameraFFT.ortho.setPerspective(45.f, aspect, 0.1f, 7.f);

      // mouse
      if (mouseInView)
      {
         if (pInput->isClick(SDL_BUTTON_LEFT))
         {
            window.measure.data.pickFFT0.onClick();
            window.measure.data.pickFFT1.onClick();
         }

         Matrix4x4 proj = matMultiply(matInverse(cameraFFT.ortho.Perspective), cameraFFT.ortho.View);
         Vector4     l0 = matUnprojectScreenSpace(mX, sizeY - (mY - sizeYOsc), sizeX, sizeY, proj, 0.25f);
         Vector4     l1 = matUnprojectScreenSpace(mX, sizeY - (mY - sizeYOsc), sizeX, sizeY, proj, 0.75f);
         Vector4      l = vecNormalize3d(l1- l0);

         // pick X
         {
            Vector4      n = Vector4(0, 0, 1, 0);
            float dotUpper = -1.f * vecDot3d(l0, n);
            float dotlower = vecDot3d(l, n);
            if (dotlower != 0.f)
            {
               float d = dotUpper / dotlower;
               Vector4 pos = Vector4(d)*l + l0;

               window.measure.data.pickFFT0.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
               window.measure.data.pickFFT1.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
            }
         }

         if (pInput->isMouse(SDL_BUTTON_LEFT))
         {
            cameraFFT.theta =  TO_RAD*float(mRelX) / float(sizeX)*dt*10000.f;
            cameraFFT.phi   = -TO_RAD*float(mRelY) / float(sizeY)*dt*10000.f;
            clearThermal = 1;
            redrawEts = 1;
         }
         else
         {
            cameraFFT.theta = 0;
            cameraFFT.phi = 0;
         }
      }

      // rotate
      cameraFFT.ortho.View = matMultiply(cameraFFT.ortho.View, matRotationY(cameraFFT.theta));
      cameraFFT.ortho.View = matMultiply(cameraFFT.ortho.View, matRotationAxisAngle(cameraFFT.ortho.View.RowX(), cameraFFT.phi));

      // scale
      fftScaleX = 1.f;
      fftScaleY = 1.f;

      // wheel
      if (mWheel && mouseInView)
      {
         clearThermal = 1;
         clearRenderTarget = 1;

         redrawEts = 1;

         cameraFFT.zoom -= 0.01f*dt*mWheel;
         cameraFFT.zoom = clamp<float>(cameraFFT.zoom, 0.05f, 2.f);
      }
      float  cameraR = cameraFFT.zoom*2.0f;
      cameraFFT.ortho.View.Pos() = Vector4(0, 0, 0.5f, 1) + cameraFFT.ortho.View.RowZ() * Vector4(-cameraR, -cameraR, -cameraR, 1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // 3d osc
   ////////////////////////////////////////////////////////////////////////////////
   if (window.fftDigital.is(VIEW_SELECT_OSC_3D) )
   {
      // persepctive
      int sizeX       = 0;
      int sizeY       = 0;
      int mouseInView = 0;
      {
         sizeX = width;
         sizeY = width*analogWindowSize;
         mouseInView = mX > 0 && mX < sizeX && mY > 0 && mY < sizeY;
      }
      float aspect = float(sizeX) / float(sizeY);
      cameraOsc.ortho.setPerspective(45.f, aspect, 0.1f, 7.f);


      // mouse
      if (mouseInView)
      {
         ////////////////////////////////////////////////////////////////////////////////
         // move
         ////////////////////////////////////////////////////////////////////////////////
         if (pInput->isClick(SDL_BUTTON_LEFT))
         {
            window.measure.data.pickX0.onClick();
            window.measure.data.pickX1.onClick();
            window.measure.data.pickY0.onClick();
            window.measure.data.pickY1.onClick();
         }

         Matrix4x4 proj = matMultiply(matInverse(cameraOsc.ortho.Perspective), cameraOsc.ortho.View);
         Vector4     l0 = matUnprojectScreenSpace(mX, sizeY - mY, sizeX, sizeY, proj, 0.25f);
         Vector4     l1 = matUnprojectScreenSpace(mX, sizeY - mY, sizeX, sizeY, proj, 0.75f);
         Vector4      l = vecNormalize3d(l1- l0);

         // pick X
         {
            Vector4      n = Vector4(0, 0, 1, 0);
            float dotUpper = -1.f * vecDot3d(l0, n);
            float dotlower = vecDot3d(l, n);
            if (dotlower != 0.f)
            {
               float d = dotUpper / dotlower;
               Vector4 pos = Vector4(d)* l +  l0;

               window.measure.data.pickX0.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
               window.measure.data.pickX1.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
            }
         }

         // pick Y
         {
            Vector4     n = Vector4(0, 0, 1, 0);
            float dotUpper = -1.f * vecDot3d(l0, n);
            float dotlower = vecDot3d(l, n);
            if (dotlower != 0.f)
            {
               float d = dotUpper / dotlower;
               Vector4 pos = Vector4(d)* l + l0;

               window.measure.data.pickY0.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
               window.measure.data.pickY1.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
            }
         }

         if (pInput->isMouse(SDL_BUTTON_LEFT))
         {
            cameraOsc.theta =  TO_RAD*float(mRelX) / float(sizeX)*dt*10000.f;
            cameraOsc.phi   = -TO_RAD*float(mRelY) / float(sizeY)*dt*10000.f;
            clearThermal = 1;
            redrawEts = 1;
         }
         else
         {
            cameraOsc.theta = 0;
            cameraOsc.phi = 0;
         }
      }

      // rotate
      cameraOsc.ortho.View = matMultiply(cameraOsc.ortho.View, matRotationY(cameraOsc.theta));
      cameraOsc.ortho.View = matMultiply(cameraOsc.ortho.View, matRotationAxisAngle(cameraOsc.ortho.View.RowX(), cameraOsc.phi));

      // scale
      oscScaleX = 1.f;
      oscScaleY = 1.f;

      // wheel
      float zoomZoom = 0.f;
      if (mWheel && mouseInView)
      {
         zoomZoom = 0.1f*dt*float(mWheel);

         clearThermal = 1;

         redrawEts = 1;

         cameraOsc.zoom -= 0.01f*dt*mWheel;
         cameraOsc.zoom = clamp<float>(cameraOsc.zoom, 0.05f, 2.f);
      }

      // position
      float  cameraR = cameraOsc.zoom*2.0f;
      cameraOsc.ortho.View.Pos() = Vector4(0, 0, 0.5f, 1) + cameraOsc.ortho.View.RowZ() * Vector4(-cameraR, -cameraR, -cameraR, 1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // mouuse debug
   ////////////////////////////////////////////////////////////////////////////////
   #ifdef MOUSE_DEBUG
      int lheight = 25;
      pFont->setSize(0.5f);
      FORMAT_BUFFER();

      FORMAT("Mouse  : %d,%d",mX,mY);
      pFont->writeText(100,100,  formatBuffer );

      FORMAT("Mouse Rel : %d,%d",mRelX,mRelY);
      pFont->writeText(100, 100 + lheight, formatBuffer);

      FORMAT("Left   : %d",pInput->isMouse(SDL_BUTTON_LEFT));
      pFont->writeText(100,100+2*lheight,  formatBuffer );

      /*
      FORMAT("Middle : %d",pInput->isMouse(SDL_BUTTON_MIDDLE));
      pFont->writeText(100,100+lheight*3,formatBuffer );

      FORMAT("Right  : %d",pInput->isMouse(SDL_BUTTON_RIGHT));
      pFont->writeText(100,100+lheight*4,formatBuffer  );

      FORMAT("Wheel  : %d",mWheel);
      pFont->writeText(100,100+lheight*5, formatBuffer );*/
   #endif

   return 0;
}

void setAnalogViewport(int width,int height,float size)
{
   size = max(size, 0.000000001f);
   grViewport(0, float(height - width*size), (float)width, (float)width*size, 0.f, 1.f);
}

void setFFTViewport(int width,int height,float size)
{
   size = max(size, 0.000000001f);
   grViewport(0, 0, (float)width, (float)(height - width*size), 0.f, 1.f);
}



void OsciloscopeManager::renderThread(uint threadId,OsciloscopeThreadData& threadData, OsciloscopeThreadRenderer& renderer,OsciloscopeFFT& fft)
{
   WndMain&                 wndMain = threadData.window;
   OsciloscopeRenderData&    render = threadData.render;
   OsciloscopeFrame&          frame = threadData.frame;
   MeasureData&             measure = measureData[threadId];

   ////////////////////////////////////////////////////////////////////////////////
   // begin
   ////////////////////////////////////////////////////////////////////////////////
   pRender->GetFlag(threadId).set( render.flags.get() );
   pCanvas3d->threadBegin(threadId);
   pCanvas2d->threadBegin(threadId);
   pFont->threadStart(threadId);

   ////////////////////////////////////////////////////////////////////////////////
   // measure signal
   ////////////////////////////////////////////////////////////////////////////////
   renderer.measureSignal(threadId,threadData, measure,fft);

   ////////////////////////////////////////////////////////////////////////////////
   // Mode
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->setMode(threadId, RENDER_MODE_THERMAL);
   pCanvas2d->setMode(threadId, RENDER_MODE_THERMAL);

   ////////////////////////////////////////////////////////////////////////////////
   // preOscRender
   ////////////////////////////////////////////////////////////////////////////////
   renderer.preOscRender(threadId,threadData);

   ////////////////////////////////////////////////////////////////////////////////
   // width/height
   ////////////////////////////////////////////////////////////////////////////////
   float width  = (float)pRender->width;
   float height = (float)pRender->height;

   ////////////////////////////////////////////////////////////////////////////////
   // heating
   ////////////////////////////////////////////////////////////////////////////////
   if (grGetMode() == OPENGL_MODE_32)
   {
      if ( wndMain.horizontal.Mode != SIGNAL_MODE_PAUSE && wndMain.thermal.enabled )
      {
         ////////////////////////////////////////////////////////////////////////////////
         // Shadow Channel01
         ////////////////////////////////////////////////////////////////////////////////
         if ( wndMain.channel01.OscOnOff )
            renderer.renderAnalog(threadId, threadData, 0.f, 0, 1, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, COLOR_ARGB(255, 255, 255, 0), wndMain.channel01.Invert );

         ////////////////////////////////////////////////////////////////////////////////
         // Shadow Channel02
         ////////////////////////////////////////////////////////////////////////////////
         if (wndMain.channel02.OscOnOff)
            renderer.renderAnalog(threadId, threadData, 0.f, 1, 1, frame, wndMain.horizontal.Capture, wndMain.channel02.Capture, COLOR_ARGB(255, 0, 255, 255), wndMain.channel02.Invert );
      }
   }

   pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_COOLING);
   pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_COOLING);

   ////////////////////////////////////////////////////////////////////////////////
   // cooling
   ////////////////////////////////////////////////////////////////////////////////
   if (grGetMode() == OPENGL_MODE_32)
   {
      if (wndMain.horizontal.Mode != SIGNAL_MODE_PAUSE && wndMain.thermal.enabled )
      {
         pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_RECTANGLEUV, 1);
            pCanvas2d->bRectangleUV(threadId, Vector4(0.f, 0.f, 0.f, 1.f) , Vector4((float)render.width, (float)render.height, 1.f, 0.f ) );
            Vector4 vector;
            vector.x = window.thermal.cooling;
            vector.y = 0.f;
            vector.z = 0.f;
            vector.w = 0.f;
            pCanvas2d->addConstant(threadId, vector );
         pCanvas2d->endBatch(threadId, -1, render.shadowTexture, BLEND_MODE_COPY, SAMPLE_STATE_CANVAS2D, render.shadowCoolingShader );
      }
   }

   pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_BLEND_IN_HEATING);
   pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_BLEND_IN_HEATING);

   ////////////////////////////////////////////////////////////////////////////////
   // blend
   ////////////////////////////////////////////////////////////////////////////////
   if (render.shadowTexture != 0)
   {
      int debug = 1;
   }
   pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_RECTANGLEUV, 1);
      pCanvas2d->bRectangleUV(threadId, Vector4(0.f, 0.f, 0.f, 1.f), Vector4( (float)render.width, (float)render.height, 1.f, 0.f) );
   pCanvas2d->endBatch(threadId, -1, render.shadowTexture, BLEND_MODE_ALPHA, SAMPLE_STATE_CANVAS2D, render.shadowColorShader );

   ////////////////////////////////////////////////////////////////////////////////
   // Mode
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_3D);
   pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_3D);

   ////////////////////////////////////////////////////////////////////////////////
   // fps
   ////////////////////////////////////////////////////////////////////////////////
   renderer.renderFps(threadId,threadData);

   ////////////////////////////////////////////////////////////////////////////////
   // Signal 3d
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
   {
      if (!threadData.history.isEmpty())
      {
         if ( wndMain.channel01.OscOnOff || wndMain.channel02.OscOnOff || wndMain.function.OscOnOff )
         {
            Ring<OsciloscopeFrame> currentFrames = threadData.history;
            if (currentFrames.getCount())
            {
               float    zDelta = 1.f / (currentFrames.getCount() - 1);
               float         z = 1.f;
               static OsciloscopeFrame newFrame;
               uint count = currentFrames.getCount();
               for (uint i = 0; i < count; i++)
               {
                  currentFrames.read(newFrame);

                  if (wndMain.channel01.OscOnOff)
                     renderer.renderAnalog3d(threadId, threadData, i, z, 0, newFrame, wndMain.horizontal.Capture, wndMain.channel01.Capture, COLOR_ARGB(wndMain.display.alpha3dCh0, 255, 255, 0), wndMain.channel01.Invert);

                  if (wndMain.channel02.OscOnOff)
                     renderer.renderAnalog3d(threadId,threadData, i, z, 1, newFrame, wndMain.horizontal.Capture, wndMain.channel01.Capture, COLOR_ARGB(wndMain.display.alpha3dCh1, 0, 0, 255), wndMain.channel02.Invert);

                  if (wndMain.function.OscOnOff)
                     renderer.renderAnalogFunction3d(threadId, threadData, newFrame, i, z, COLOR_ARGB(wndMain.display.alpha3dFun, 255, 255, 255) );

                  z -= zDelta;
               }
               if (wndMain.channel01.OscOnOff)
                  renderer.renderSurface3d(threadId, threadData,  0, COLOR_ARGB(wndMain.display.alpha3dCh0, 255, 255, 0));
               if (wndMain.channel02.OscOnOff)
                  renderer.renderSurface3d(threadId, threadData,  1, COLOR_ARGB(wndMain.display.alpha3dCh1, 0, 0, 255));
               if (wndMain.function.OscOnOff)
                  renderer.renderSurface3d(threadId, threadData, -1, COLOR_ARGB(wndMain.display.alpha3dFun, 255, 255, 255));
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // Mode
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_2D);
   pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_2D);

   ////////////////////////////////////////////////////////////////////////////////
   // Grid
   ////////////////////////////////////////////////////////////////////////////////
   renderer.renderAnalogGrid(threadId, threadData);

   ////////////////////////////////////////////////////////////////////////////////
   // Axis
   ////////////////////////////////////////////////////////////////////////////////
   renderer.renderAnalogAxis(threadId, threadData);

   ////////////////////////////////////////////////////////////////////////////////
   // Units
   ////////////////////////////////////////////////////////////////////////////////
   renderer.renderAnalogUnits(threadId, threadData);

   ////////////////////////////////////////////////////////////////////////////////
   // ETS clear
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.horizontal.ETS)
   {
      if (wndMain.channel01.OscOnOff)
      {
         if (frame.etsAttr & ETS_CLEAR)
         {
            renderer.renderAnalog(threadId, threadData, 0.f, 0, 0, threadData.etsClear, wndMain.horizontal.Capture, wndMain.channel01.Capture, 0xff303030, wndMain.channel01.Invert);
         }
      }

      if(wndMain.channel02.OscOnOff)
      {
         if (frame.etsAttr & ETS_CLEAR)
         {
            renderer.renderAnalog(threadId, threadData, 0.f, 1, 0, threadData.etsClear, wndMain.horizontal.Capture, wndMain.channel02.Capture, 0xff303030, wndMain.channel02.Invert );
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // Signal Channel01
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.channel01.OscOnOff)
   {
      renderer.renderAnalog(threadId, threadData, 0.f, 0, 0, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture,  COLOR_ARGB(255, 255, 255, 0), wndMain.channel01.Invert);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // Signal Channel02
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.channel02.OscOnOff)
   {
      renderer.renderAnalog(threadId, threadData, 0.f, 1, 0, frame, wndMain.horizontal.Capture, wndMain.channel02.Capture, COLOR_ARGB(255, 0, 255, 255), wndMain.channel02.Invert);
   }


   ////////////////////////////////////////////////////////////////////////////////
   // Function
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.function.OscOnOff)
   {
      renderer.renderAnalogFunction(threadId, threadData, 0.f, wndMain.function.Type, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, wndMain.channel02.Capture, COLOR_ARGB(255, 255, 255, 255), wndMain.channel01.Invert, wndMain.channel02.Invert );
   }

   ////////////////////////////////////////////////////////////////////////////////
   // FunctionXY
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.function.xyGraph)
   {
      renderer.renderAnalogFunctionXY(threadId, threadData, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, wndMain.channel02.Capture, COLOR_ARGB(255, 64, 255, 64) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   // Mode
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_MEASURE);
   pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_MEASURE);

   ////////////////////////////////////////////////////////////////////////////////
   // measurre
   ////////////////////////////////////////////////////////////////////////////////
   renderer.renderMeasure(threadId,threadData);

   ////////////////////////////////////////////////////////////////////////////////
   // preFftRender
   ////////////////////////////////////////////////////////////////////////////////
   renderer.preFftRender(threadId,threadData);

   if (wndMain.fftDigital.is(VIEW_SELECT_DIGITAL))
   {
      ////////////////////////////////////////////////////////////////////////////////
      // Mode
      ////////////////////////////////////////////////////////////////////////////////
      pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_DIGITAL);
      pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_DIGITAL);

      ////////////////////////////////////////////////////////////////////////////////
      // digital
      ////////////////////////////////////////////////////////////////////////////////
      int bits = (int)pow(float(2), float(wndMain.display.digitalBits + 3));

      renderer.renderDigitalGrid( threadId, threadData, bits, 16 );
      renderer.renderDigitalAxis( threadId, threadData, bits, 16 );
      renderer.renderDigitalUnit( threadId, threadData, bits, 16 );
      renderer.renderDigital( threadId, threadData, measure, bits, 16 );
   }
   else
   {
      ////////////////////////////////////////////////////////////////////////////////
      // Mode
      ////////////////////////////////////////////////////////////////////////////////
      pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_FFT3D);
      pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_FFT3D);

      renderer.renderMeasureFFT(threadId,threadData);

      ////////////////////////////////////////////////////////////////////////////////
      // FFT 3d
      ////////////////////////////////////////////////////////////////////////////////
      if (wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
      {
         if (!threadData.history.isEmpty())
         {
            Ring<OsciloscopeFrame> currentFrames = threadData.history;
            int framesCount = currentFrames.getCount();
            float    zDelta = 1.f / framesCount;
            float         z = 1.f;
            static OsciloscopeFrame out;
            for (uint i= 0; i< (uint)framesCount; i++)
            {
               currentFrames.read(out);
               if (wndMain.channel01.FFTOnOff)
                  renderer.renderFFT( threadId, threadData, measure, fft, out, false, z, 0, COLOR_ARGB(25, 255, 255, 0) );
               if (wndMain.channel02.FFTOnOff)
                  renderer.renderFFT( threadId, threadData, measure, fft, out, false, z, 1, COLOR_ARGB(25, 0, 255, 255) );
               if (wndMain.function.FFTOnOff)
                  renderer.renderFFT(threadId, threadData, measure, fft, out, true, z, 0, COLOR_ARGB(25, 255, 255, 255) );
               z -= zDelta;
            }
         }
      }

      ////////////////////////////////////////////////////////////////////////////////
      // Mode
      ////////////////////////////////////////////////////////////////////////////////
      pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_FFT2D);
      pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_FFT2D);

      ////////////////////////////////////////////////////////////////////////////////
      // FFT Grid
      ////////////////////////////////////////////////////////////////////////////////
      renderer.renderFFTGrid(threadId,threadData);

      ////////////////////////////////////////////////////////////////////////////////
      // FFT Units
      ////////////////////////////////////////////////////////////////////////////////
      renderer.renderFFTUnits(threadId, threadData);

      ////////////////////////////////////////////////////////////////////////////////
      // FFT Axis
      ////////////////////////////////////////////////////////////////////////////////
      renderer.renderFFTAxis(threadId, threadData );

      ////////////////////////////////////////////////////////////////////////////////
      // FFT
      ////////////////////////////////////////////////////////////////////////////////
      if (wndMain.fftDigital.is(VIEW_SELECT_FFT_2D))
      {
         if (wndMain.channel01.FFTOnOff)
         {
            renderer.renderFFT(threadId, threadData, measure, fft, threadData.frame, false, 0.f, 0, COLOR_ARGB(255, 255, 255, 0));
         }
         if (wndMain.channel02.FFTOnOff)
         {
            renderer.renderFFT(threadId, threadData, measure, fft, threadData.frame, false, 0.f, 1, COLOR_ARGB(255, 0, 255, 255));
         }
         if (wndMain.function.FFTOnOff)
         {
            renderer.renderFFT(threadId, threadData, measure, fft, threadData.frame, true, 0.f, 1, COLOR_ARGB(25, 255, 255, 255));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // slider
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_SLIDER);
   pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_SLIDER);
   renderer.renderSlider(threadId, threadData);

   ////////////////////////////////////////////////////////////////////////////////
   // end
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->threadEnd(threadId);
   pCanvas2d->threadEnd(threadId);
}

void OsciloscopeManager::renderMain(uint threadId)
{
   ////////////////////////////////////////////////////////////////////////////////
   // setup
   ////////////////////////////////////////////////////////////////////////////////
   grSetRenderTarget(depth, color);
   grClearDepthStencil(depth, 0, 1.f, 0);

   ////////////////////////////////////////////////////////////////////////////////
   // clear
   ////////////////////////////////////////////////////////////////////////////////
   Flag64 flags = pRender->GetFlag(threadId);
   if (flags.is(rfClearRenderTarget))
   {
      grClearRenderTarget(0, 0xff303030);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // begin
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->mainBegin(threadId);
   pCanvas2d->mainBegin(threadId);

   ////////////////////////////////////////////////////////////////////////////////
   // depth stenci
   ////////////////////////////////////////////////////////////////////////////////
   grStateDepthStencil(DEPTH_STENCIL_OFF);
   grStateRasterizer(RASTERIZER_CULLNONE);

   ////////////////////////////////////////////////////////////////////////////////
   // frame [0-1]
   ////////////////////////////////////////////////////////////////////////////////
   int shadowFrame0 =  pTimer->getFrame(0)%2;
   int shadowFrame1 = (pTimer->getFrame(0)+1)%2;
   if( window.horizontal.Mode == SIGNAL_MODE_PAUSE )
   {
      shadowFrame0 = 0;
      shadowFrame1 = 1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   // clear
   ////////////////////////////////////////////////////////////////////////////////
   if( ( !window.thermal.enabled || clearThermal ) && grGetMode() == OPENGL_MODE_32 )
   {
      clearThermal = 0;

      grSetRenderTarget( depth, aShadow[0] );
      grClearRenderTarget( aShadow[0], 0x00000000 );

      grSetRenderTarget(depth, aShadow[1]);
      grClearRenderTarget( aShadow[1], 0x00000000 );
   }

   ////////////////////////////////////////////////////////////////////////////////
   // width, height
   ////////////////////////////////////////////////////////////////////////////////
   float width = (float)pRender->width;
   float height = (float)pRender->height;

   ////////////////////////////////////////////////////////////////////////////////
   // setup - heating
   ////////////////////////////////////////////////////////////////////////////////
   if (grGetMode() == OPENGL_MODE_32)
   {
      grSetRenderTarget(0, aShadow[shadowFrame0]);
      setAnalogViewport(width, height, analogWindowSize);

      ////////////////////////////////////////////////////////////////////////////////
      // render - heating
      ////////////////////////////////////////////////////////////////////////////////
      if ( window.horizontal.Mode != SIGNAL_MODE_PAUSE && window.thermal.enabled)
      {
         pCanvas3d->render(threadId, RENDER_MODE_THERMAL);
         pCanvas2d->render(threadId, RENDER_MODE_THERMAL);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // copy
   ////////////////////////////////////////////////////////////////////////////////
   if( window.horizontal.Mode != SIGNAL_MODE_PAUSE )
   {
      grViewport( 0, 0, width, height, 0.f, 1.f );
      grCopyRenderTarget( aShadow[shadowFrame1], aShadow[shadowFrame0] );
   }

   ////////////////////////////////////////////////////////////////////////////////
   // cooling
   ////////////////////////////////////////////////////////////////////////////////
   if (grGetMode() == OPENGL_MODE_32)
   {
      grSetRenderTarget(depth, aShadow[shadowFrame1]);
      if ( window.horizontal.Mode != SIGNAL_MODE_PAUSE && window.thermal.enabled)
      {
         pCanvas2d->render(threadId, RENDER_MODE_COLOR_COOLING);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // setup - color
   ////////////////////////////////////////////////////////////////////////////////
   grSetRenderTarget(depth, color);
   grViewport( 0, 0, width, height, 0.f, 1.f );

   ////////////////////////////////////////////////////////////////////////////////
   // blend in heating
   ////////////////////////////////////////////////////////////////////////////////
   if (grGetMode() == OPENGL_MODE_32)
   {
      pCanvas2d->render(threadId, RENDER_MODE_COLOR_BLEND_IN_HEATING);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // depth test on / off
   ////////////////////////////////////////////////////////////////////////////////
   if (window.display.depthTest3d) grStateDepthStencil(DEPTH_ON);

   ////////////////////////////////////////////////////////////////////////////////
   // setup - analog 3d
   ////////////////////////////////////////////////////////////////////////////////
   setAnalogViewport(width, height, analogWindowSize);
   pCanvas3d->dirLight = Vector4(-cameraOsc.ortho.View.RowZ().x, -cameraOsc.ortho.View.RowZ().y, -cameraOsc.ortho.View.RowZ().z, 0);

   ////////////////////////////////////////////////////////////////////////////////
   // render - analog 3d
   ////////////////////////////////////////////////////////////////////////////////
   grStateRasterizer(RASTERIZER_CULLNONE);
   pCanvas3d->render(threadId, RENDER_MODE_COLOR_ANALOG_3D);
   pCanvas2d->render(threadId, RENDER_MODE_COLOR_ANALOG_3D);

   ////////////////////////////////////////////////////////////////////////////////
   // cull off
   ////////////////////////////////////////////////////////////////////////////////
   grStateRasterizer(RASTERIZER_CULLNONE);
   pCanvas3d->dirLight = Vector4(0, 0.f, 1.f, 0.f);

   ////////////////////////////////////////////////////////////////////////////////
   // render - analog 3d
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->render(threadId, RENDER_MODE_COLOR_ANALOG_2D);
   pCanvas2d->render(threadId, RENDER_MODE_COLOR_ANALOG_2D);

   ////////////////////////////////////////////////////////////////////////////////
   // setup - measure
   ////////////////////////////////////////////////////////////////////////////////
   grStateDepthStencil(DEPTH_ON);

   ////////////////////////////////////////////////////////////////////////////////
   // render - measure
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->render(threadId, RENDER_MODE_COLOR_MEASURE);
   pCanvas2d->render(threadId, RENDER_MODE_COLOR_MEASURE);

   ////////////////////////////////////////////////////////////////////////////////
   // setup - digital
   ////////////////////////////////////////////////////////////////////////////////
   grStateDepthStencil(DEPTH_STENCIL_OFF);
   if (window.display.depthTest3d) grStateDepthStencil(DEPTH_ON);
   setFFTViewport(width, height, analogWindowSize);
   pCanvas3d->dirLight = Vector4(0, -1.f, 0.f, 0.f);

   ////////////////////////////////////////////////////////////////////////////////
   // render - digital
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->render(threadId, RENDER_MODE_COLOR_DIGITAL);
   pCanvas2d->render(threadId, RENDER_MODE_COLOR_DIGITAL);

   ////////////////////////////////////////////////////////////////////////////////
   // render - FFT3D
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->render(threadId, RENDER_MODE_COLOR_FFT3D);
   pCanvas2d->render(threadId, RENDER_MODE_COLOR_FFT3D);

   ////////////////////////////////////////////////////////////////////////////////
   // setup - FFT2D
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->dirLight = Vector4(0, 0.f, 1.f, 0.f);

   ////////////////////////////////////////////////////////////////////////////////
   // render - FFT2D
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->render(threadId, RENDER_MODE_COLOR_FFT2D );
   pCanvas2d->render(threadId, RENDER_MODE_COLOR_FFT2D );

   ////////////////////////////////////////////////////////////////////////////////
   // setup - slider
   ////////////////////////////////////////////////////////////////////////////////
   grViewport(0, 0, (float)width, (float)height, -1.f, 1.f);

   ////////////////////////////////////////////////////////////////////////////////
   // render - slider
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->render(threadId, RENDER_MODE_COLOR_SLIDER);
   pCanvas2d->render(threadId, RENDER_MODE_COLOR_SLIDER);

   ////////////////////////////////////////////////////////////////////////////////
   // end
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->mainEnd(threadId);
   pCanvas2d->mainEnd(threadId);

   ////////////////////////////////////////////////////////////////////////////////
   // output to screen
   ////////////////////////////////////////////////////////////////////////////////
   grCopyToBackBuffer(color, 0, 0, pRender->width, pRender->height);
   int ret = SDL_GL_SetSwapInterval(0);
   SDL_GL_SwapWindow(sdlWindow);
}

int OsciloscopeManager::stop()
{
   // usbExit();
   // SDL_Quit();
   // memoryFree(surfaceFrame);
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// uploadFirmware
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeManager::openUSB()
{
   usbClose();
   usbGuidVidPid(settings.usbGuid, settings.usbVendorId, settings.usbProductId, settings.usbSerialId);
   usbDevice* deviceList[4] = { 0 };
   usbFindList(deviceList, 4);
   usbOpenNormal(deviceList, 4);
   usbFreeList();
   if (!usbIsConnected())
   {
      usbGuidVidPid(settings.usbGuid, settings.usbFactoryVendorId, settings.usbFactoryProductId, settings.usbSerialId);
      usbDevice* factoryList[4] = { 0 };
      usbFindList(factoryList, 4);
      usbOpenNormal(factoryList, 4);
      usbFreeList();
   }
}
void OsciloscopeManager::uploadFx2()
{
   openUSB();
   usbUploadFirmwareToFx2(0, 0, 0);
   usbClose();
   SDL_Delay(3000);
   openUSB();
}

void OsciloscopeManager::uploadFpga()
{
   char*  buffer = 0;
   ilarge bufferSize = 0;
   int ret = fileLoad(settings.fpgaFileName.asChar(), &buffer, &bufferSize);
   usbUploadFirmwareToFpga((byte*)buffer, bufferSize, 1);
   memoryFree(buffer);
}

void OsciloscopeManager::uploadFirmware()
{
   SDL_AtomicLock(&uploadFirmwareLock);

   // fx2
   uploadFx2();

   // fpga
   uploadFpga();

   // control
   control.transferData();

   if (!usbIsConnected())
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Information","USB not connected.",0);

   SDL_AtomicUnlock(&uploadFirmwareLock);
}

void OsciloscopeManager::WriteEEPROM()
{
   SDL_AtomicLock(&uploadFirmwareLock);

   // fx2
   uploadFx2();

   // writeEEPROM
   UsbEEPROM eeprom;
   memset(&eeprom, 0, sizeof(UsbEEPROM));
   eeprom.bytes[0] = 0xC0;
   eeprom.bytes[1] = (settings.usbVendorId&0xff);
   eeprom.bytes[2] = (settings.usbVendorId>>8)&0xff;
   eeprom.bytes[3] = (settings.usbProductId&0xff);
   eeprom.bytes[4] = (settings.usbProductId>>8)&0xff;
   eeprom.bytes[5] = (settings.usbSerialId&0xff);
   eeprom.bytes[6] = (settings.usbSerialId&0xff);
   int ret = usbWriteEEPROM( eeprom.bytes, sizeof(UsbEEPROM), 0 );
   if( ret > 0) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Success","Write EEPROM was successfull.",0);
   else         SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Error","Write EEPROM failed.",0);

   usbClose();

   SDL_AtomicUnlock(&uploadFirmwareLock);
}


void OsciloscopeManager::EraseEEPROM()
{
   SDL_AtomicLock(&uploadFirmwareLock);

   // fx2
   uploadFx2();

   // eraseEEPROM
   UsbEEPROM eeprom;
   memset(&eeprom, 0, sizeof(UsbEEPROM));

   int ret = usbWriteEEPROM( eeprom.bytes, sizeof(UsbEEPROM), 0 );
   if( ret > 0) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Success","Erase EEPROM was successfull.",0);
   else         SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Error","Erase EEPROM failed.",0);

   usbClose();

   SDL_AtomicUnlock(&uploadFirmwareLock);
}

void OsciloscopeManager::ReadEEPROM(UsbEEPROM& out)
{
   SDL_AtomicLock(&uploadFirmwareLock);

   // fx2
   uploadFx2();

   // readEEPROM
   int ret = usbReadEEPROM(out.bytes, sizeof(UsbEEPROM));
   if( ret > 0) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Success","Read EEPROM was successfull.",0);
   else         SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Error","Read EEPROM failed.",0);

   usbClose();

   SDL_AtomicUnlock(&uploadFirmwareLock);
}

////////////////////////////////////////////////////////////////////////////////
//
// callbacks
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeManager::onApplicationInit()
{
   pOsciloscope->setThreadPriority(THREAD_ID_MAIN);

   int renderWidth  = pRender->width;
   int renderHeight = pRender->height;

   int ret = SDL_GL_SetSwapInterval(0);

   color = grCreateRenderTarget( RENDER_TARGET_TYPE_RGBA,  renderWidth, renderHeight );
   if( settings.depthBuffer ) depth = grCreateDepthStencil(DEPTH_STENCIL_TYPE_Z32, renderWidth, renderHeight);
   else depth = 0;
   for(int i=0;i<2;i++)
   {
      aShadow[i] = grCreateRenderTarget( RENDER_TARGET_TYPE_FLOAT32, renderWidth, renderHeight );
   }
}

int SDLCALL EventFilter(void *userdata, SDL_Event * event)
{
   pInput->onEvent(*event);
   return 0;
}

int SDLCALL RenderThreadFunction(void *data)
{
   pOsciloscope->setThreadPriority(THREAD_ID_RENDER);

   ////////////////////////////////////////////////
   // SDL
   ////////////////////////////////////////////////
   SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );

   pRender->height = pOsciloscope->settings.displayWidth;
   pRender->width  = pOsciloscope->settings.displayHeight;

   int renderWidth  = pOsciloscope->settings.displayWidth;
   int renderHeight = pOsciloscope->settings.displayHeight;

   SDL_DisplayMode mode;
   SDL_GetCurrentDisplayMode(0,&mode);
   int cx = mode.w/2;
   int cy = mode.h/2;

   pOsciloscope->sdlX = cx - (pOsciloscope->settings.controlWidth + pOsciloscope->settings.displayWidth) / 2;
   pOsciloscope->sdlY = cy -  pOsciloscope->settings.displayHeight / 2;
   pOsciloscope->sdlW = pOsciloscope->settings.displayWidth;
   pOsciloscope->sdlH = pOsciloscope->settings.displayHeight;

   pOsciloscope->sdlContext = 0;
   pOsciloscope->sdlWindow  = 0;
   if (pOsciloscope->settings.shaders21 == 0)
   {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      pOsciloscope->sdlWindow = SDL_CreateWindow("ScopeFun", pOsciloscope->sdlX, pOsciloscope->sdlY, renderWidth, renderHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
      pOsciloscope->sdlContext = SDL_GL_CreateContext(pOsciloscope->sdlWindow);

   }
   if (!pOsciloscope->sdlContext)
   {
      SDL_GL_ResetAttributes();
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1);
      pOsciloscope->sdlWindow = SDL_CreateWindow("ScopeFun", pOsciloscope->sdlX, pOsciloscope->sdlY, renderWidth, renderHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
      pOsciloscope->sdlContext = SDL_GL_CreateContext(pOsciloscope->sdlWindow);
      if (!pOsciloscope->sdlContext)
      {
         SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "error", "At least OpenGL 2.1(shaders) is requeired in order to run this program.", 0);
         return 1;
      }
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "warning", "Thermal rendering will be disabled.\nYou need OpenGL 3.2 graphics in order to use this feature.", 0);
      grSetMode(OPENGL_MODE_21);
   }
   else
   {
      grSetMode(OPENGL_MODE_32);
   }

   #if !defined(LINUX) && !defined(MAC)
      SDL_GetWindowSize(pOsciloscope->sdlWindow, &pOsciloscope->sdlW, &pOsciloscope->sdlH);
   #endif


   //////////////////////////////////////////////////
   //// glew
   //////////////////////////////////////////////////
   grCreateDevice();

   ////////////////////////////////////////////////////////////
   //// shaders
   ////////////////////////////////////////////////////////////
   if (grGetMode() == OPENGL_MODE_32)
   {
      pOsciloscope->shadowLine3dShader = grCreateShader();
      pOsciloscope->shadowColorShader = grCreateShader();
      pOsciloscope->shadowCoolingShader = grCreateShader();

      pOsciloscope->shadowLine3dShader->compile("shadow/line3d.fx", declPos3D, GrShaderDefine(0));
      pOsciloscope->shadowColorShader->compile("shadow/color.fx", declPosTex2D, GrShaderDefine(0));
      pOsciloscope->shadowCoolingShader->compile("shadow/cooling.fx", declPosTex2D, GrShaderDefine(0));
   }

   ////////////////////////////////////////////////////////////
   //// Canvas / Fonts
   ////////////////////////////////////////////////////////////
   pRender->start();
   pCanvas2d->start();
   pCanvas3d->start();
   pFont->start();

   //////////////////////////////////////////////////
   //// resize
   //////////////////////////////////////////////////
   pRender->resize(pOsciloscope->sdlW, pOsciloscope->sdlH);

   //////////////////////////////////////////////////
   //// init
   //////////////////////////////////////////////////
   pOsciloscope->onApplicationInit();

   //////////////////////////////////////////////////
   //// timer
   //////////////////////////////////////////////////
   pTimer->init(TIMER_MAIN);
   pTimer->init(TIMER_MAIN_THREAD);
   pTimer->init(TIMER_RENDER);
   pTimer->init(TIMER_RENDER_THREAD);

   while (SDL_AtomicCAS(&pOsciloscope->contextCreated, 0, 1) == SDL_FALSE) {};
   while (pOsciloscope->threadActive)
   {
     pOsciloscope->onApplicationIdle();
   }

   return 0;
}

bool OsciloscopeManager::onApplicationIdle()
{
   // update
   static double dtUpdate = 0.0;
   pTimer->deltaTime(TIMER_MAIN_THREAD);
   dtUpdate += pTimer->getDelta(TIMER_MAIN_THREAD);
   if (dtUpdate >= pOsciloscope->settings.updateTimer )
   {
      pTimer->deltaTime(TIMER_MAIN);

      pInput->clear();

      SDL_Event e;
      while( SDL_PollEvent(&e) )
      {
         pManager->onEvent(e);
      }

      pManager->update(dtUpdate);

      dtUpdate = 0.0;
   }

   // render
   static double dtRender= 0.0;
   pTimer->deltaTime(TIMER_RENDER_THREAD);
   dtRender += pTimer->getDelta(TIMER_RENDER_THREAD);

   // fps
   uint maxFps = 0;
   switch( window.speed ) {
   case USB_SPEED_AUTOMATIC:
       {
            double maxFrameTime = window.horizontal.Capture*double(NUM_SAMPLES);
            double maxFrameFps  = 1.0/maxFrameTime;
            maxFps = settings.speedHigh;
            if( maxFrameFps <= double(settings.speedMedium) ) maxFps = settings.speedMedium;
            if( maxFrameFps <= double(settings.speedLow)    ) maxFps = settings.speedLow;
       }
       break;
   case USB_SPEED_LOW:
        maxFps = settings.speedLow;
        break;
    case USB_SPEED_MEDIUM:
        maxFps = settings.speedMedium;
        break;
    case USB_SPEED_HIGH:
        maxFps = settings.speedHigh;
        break;
   };
   double        maxDelta = 1.0/double(maxFps);
   if ( dtRender >= maxDelta )
   {
      dtRender = 0.0;

      SDL_AtomicLock(&renderLock);

         // sync capture - > user interface
         window.horizontal.uiActive = renderWindow.horizontal.uiActive;
         window.horizontal.uiRange  = renderWindow.horizontal.uiRange;
         window.horizontal.uiValue  = renderWindow.horizontal.uiValue;
         if (signalMode != SIGNAL_MODE_PAUSE)
            window.horizontal.Frame = renderWindow.horizontal.Frame;

         // sync user interface -> capture
         renderWindow = window;
         renderData.cameraFFT = cameraFFT.ortho;
         renderData.cameraOsc = cameraOsc.ortho;
         renderData.width = pRender->width;
         renderData.height = pRender->height;
         renderData.sliderMode = sliderMode;
         renderData.sliderSize = analogWindowSize;
         renderData.fftScaleX = fftScaleX;
         renderData.fftScaleY = fftScaleY;
         renderData.oscScaleX = oscScaleX;
         renderData.oscScaleY = oscScaleY;
         renderData.zoomFFT = cameraFFT.zoom;
         renderData.zoomOsc = cameraOsc.zoom;
         renderData.maxEts = settings.ets;
         renderData.shadowTexture = aShadow[pTimer->getFrame(0) % 2];
         renderData.shadowLine3dShader = shadowLine3dShader;
         renderData.shadowColorShader = shadowColorShader;
         renderData.shadowCoolingShader = shadowCoolingShader;
         VoltageCapture capture0 = (VoltageCapture)captureVoltFromValue(window.channel01.Capture);
         VoltageCapture capture1 = (VoltageCapture)captureVoltFromValue(window.channel02.Capture);
         renderData.analogChannelYVoltageStep0 = settings.getAnalogChannelYVoltageStep(0, capture0);
         renderData.analogChannelYVoltageStep1 = settings.getAnalogChannelYVoltageStep(1, capture1);
         renderData.analogChannelOffsets0 = settings.analogChannelOffsets[0][captureVoltFromValue(window.channel01.Capture)];
         renderData.analogChannelOffsets1 = settings.analogChannelOffsets[1][captureVoltFromValue(window.channel02.Capture)];
         renderData.analogChannelPositin0 = control.getYPositionA();
         renderData.analogChannelPositin1 = control.getYPositionB();
         renderData.etsAttr = 0;
         renderData.flags.bit(rfClearRenderTarget, clearRenderTarget);
         renderData.redrawEts = redrawEts;
      SDL_AtomicUnlock(&renderLock);

      // lock
      uint renderId = 0;

      bool ret = false;
      while (!ret)
      {
          ret = pOsciloscope->threadLoop.update.consumerLock(renderId, false);
          if (ret)
          {
              pTimer->deltaTime(TIMER_RENDER);

              // render
              renderMain(renderId);

              // measure
              window.measure.data.pick = measureData[renderId].pick;
              window.measure.data.history[MEASURE_HISTORY_CURRENT] = measureData[renderId].history[MEASURE_HISTORY_CURRENT];
              window.measure.data.history[MEASURE_HISTORY_MINIMUM].Minimum(measureData[renderId].history[MEASURE_HISTORY_CURRENT]);
              window.measure.data.history[MEASURE_HISTORY_MAXIMUM].Maximum(measureData[renderId].history[MEASURE_HISTORY_CURRENT]);
              window.measure.data.history[MEASURE_HISTORY_AVERAGE].Average(measureData[renderId].history[MEASURE_HISTORY_CURRENT]);

              // unlock
              pOsciloscope->threadLoop.update.consumerUnlock(renderId);

              return true;
        }
        else
        {
            if (settings.threadDelayRender > 0)
                SDL_Delay(settings.threadDelayRender);
        }
      }
   }
   else
   {
      double diffMiliSeconds = (maxDelta - dtRender)*1000.0;
      SDL_Delay( uint(diffMiliSeconds) );
   }
   return false;
}

void OsciloscopeManager::onResize(int width,int height,int oldWidth,int oldHeight)
{
   grResizeRenderTarget( color,  width, height );
   if (settings.depthBuffer)
      grResizeDepthStencil(depth, width, height);
   for(int i=0;i<2;i++)
   {
      grResizeRenderTarget( aShadow[i], width, height );
   }

   float oldAspect = float(oldWidth) / float(oldHeight);
   float newAspect = float(width)    / float(height);

   analogWindowSize = analogWindowSize * ( oldAspect / newAspect );
}

void maximizeOglWindow(int &w,int &h);

int OsciloscopeManager::onEvent(SDL_Event& event)
{
   if (event.window.type == SDL_WINDOWEVENT)
   {
      if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
      {

      }

      if (event.window.event == SDL_WINDOWEVENT_RESTORED)
      {

      }

      if ( event.window.event == SDL_WINDOWEVENT_ENTER )
      {
         openglFocus = 1;
         sliderMode  = 0;
      }

      if (event.window.event == SDL_WINDOWEVENT_LEAVE)
      {
         openglFocus = 0;
         sliderMode  = 0;
      }
   }
   return 0;
}

void OsciloscopeManager::onCallibrateFrameCaptured(OsciloscopeFrame& frame)
{
   if (callibrate.active)
   {
      callibrate.frame++;

      int framesPerCapture = 0;
      switch (callibrate.mode) {
      case acOffsetUpCapture:
      case acOffsetDownCapture:
      case acGeneratorUpCapture:
      case acGeneratorDownCapture:
      case acGainCapture:
      case acEndMessageBox:
         framesPerCapture = settings.framesPerCapture;
         break;
      default:
         callibrate.frame = 0;
      };

      if ( callibrate.frame >= framesPerCapture )
      {
         switch( callibrate.mode ) {
         case acStartMessageBox:
            {
               if (callibrate.messageBox == acmbOk)
               {
                  callibrate.mode = acOffsetStart;
               }
               if (callibrate.messageBox == acmbCancel)
               {
                  callibrate.active  = 0;
                  callibrate.resetUI = true;
               }
            }
            break;
         case acOffsetStart:
            {
               callibrate.voltage = vc2Volt;
               control.setYRangeScaleA(settings.gainRefValue[callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
               control.setYRangeScaleB(settings.gainRefValue[callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
               callibrate.mode = acOffsetUpSetup;
            }
            break;
         case acOffsetUpSetup:
            {
               control.setYPositionA(settings.analogChannelReference);
               control.setYPositionB(settings.analogChannelReference);
               control.transferData();
               callibrate.mode = acOffsetUpCapture;
            }
            break;
         case acOffsetUpCapture:
            {
               double yGridMax = double(grid.yCount) / 2.0;
               for (int ch = 0; ch < 2; ch++)
               {
                  callibrate.offsetUpVolt[ch] = 0;
                  for (int sample = 0; sample < frame.analog[ch].getCount(); sample++)
                  {
                     callibrate.offsetUpVolt[ch] += frame.getAnalog(ch, sample);
                  }
                  callibrate.offsetUpVolt[ch] /= double(frame.analog[ch].getCount());
                  callibrate.offsetUpVolt[ch] *= yGridMax*double(captureVoltFromEnum(callibrate.voltage));
               }
               callibrate.mode = acOffsetDownSetup;
            }
            break;
         case acOffsetDownSetup:
            {
               control.setYPositionA(-settings.analogChannelReference);
               control.setYPositionB(-settings.analogChannelReference);
               control.transferData();
               callibrate.mode = acOffsetDownCapture;
            }
            break;
         case acOffsetDownCapture:
            {
               double yGridMax = double(grid.yCount) / 2.0;
               for (int ch = 0; ch < 2;ch++)
               {
                  callibrate.offsetDownVolt[ch] = 0;
                  for (int sample = 0; sample < frame.analog[ch].getCount(); sample++)
                  {
                     callibrate.offsetDownVolt[ch] += frame.getAnalog(ch,sample);
                  }
                  callibrate.offsetDownVolt[ch] /= double( frame.analog[ch].getCount() );
                  callibrate.offsetDownVolt[ch] *= yGridMax*double( captureVoltFromEnum(callibrate.voltage) );
               }
               callibrate.mode = acOffsetCalculate;
            }
            break;
         case acOffsetCalculate:
            {
               for (int ch = 0; ch < 2;ch++)
               {
                  double x1  =  settings.analogChannelReference;
                  double x2  = -settings.analogChannelReference;
                  double y1  = callibrate.offsetUpVolt[ch];
                  double y2  = callibrate.offsetDownVolt[ch];
                  int offset = int(-((y2*(x2 - x1) / (y2 - y1)) - x2));
                  settings.analogChannelOffsets[ch][callibrate.voltage] = offset;

                  double voltStep = -(callibrate.offsetDownVolt[ch] - callibrate.offsetUpVolt[ch]) / (2*double(settings.analogChannelReference));
                  if (settings.invertCh[callibrate.channel]) voltStep = -voltStep;
                  settings.setAnalogChannelYVoltageStep(voltStep, ch, callibrate.voltage);
               }
               callibrate.mode = acOffsetVoltageChange;
            }
         case acOffsetVoltageChange:
            {
               if (callibrate.voltage == vc10Mili)
               {
                  callibrate.mode = acOffsetEnd;
               }
               else
               {
                  callibrate.voltage = (VoltageCapture)((int)callibrate.voltage + 1);
                  control.setYRangeScaleA(settings.gainRefValue[callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                  control.setYRangeScaleB(settings.gainRefValue[callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                  control.transferData();
                  callibrate.mode = acOffsetUpSetup;
               }
            }
            break;
         case acOffsetEnd:
            {
               control.setYPositionA( window.channel01.YPosition + settings.analogChannelOffsets[0][ callibrate.voltage ] );
               control.setYPositionB( window.channel02.YPosition + settings.analogChannelOffsets[1][ callibrate.voltage ] );
               control.transferData();
               callibrate.mode = acGeneratorStart;
            }
         case acGeneratorStart:
            {
               callibrate.voltage    = vc2Volt;
               callibrate.channel    = 0;
               callibrate.messageBox = acmbChannel0;
               callibrate.mode       = acGeneratorMessageBox;
            }
            break;
         case acGeneratorMessageBox:
            {
               if (callibrate.messageBox == acmbOk)
               {
                  callibrate.mode    = acGeneratorUpSetup;
                  callibrate.voltage = vc100Mili;

                  window.hardwareGenerator.type0 = GENERATOR_DC;
                  window.hardwareGenerator.type1 = GENERATOR_DC;
                  window.hardwareGenerator.onOff0 = 1;
                  window.hardwareGenerator.onOff1 = 1;
                  control.setGeneratorType0(GENERATOR_DC);
                  control.setGeneratorType1(GENERATOR_DC);
                  control.setGeneratorOn0(1);
                  control.setGeneratorOn1(1);
                  control.setGeneratorOffset0(0);
                  control.setGeneratorOffset1(0);
                  control.setYRangeScaleA( settings.gainRefValue[callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                  control.setYRangeScaleB( settings.gainRefValue[callibrate.voltage] , (uint)control.getAttr(callibrate.voltage));

                  control.transferData();
               }
               if (callibrate.messageBox == acmbCancel)
               {
                  callibrate.active = 0;
                  callibrate.resetUI = true;
               }
            }
            break;
         case acGeneratorUpSetup:
            {
               control.setYPositionA(settings.analogChannelOffsets[0][callibrate.voltage]);
               control.setYPositionB(settings.analogChannelOffsets[1][callibrate.voltage]);

               control.setGeneratorOffset0( settings.generatorReference );
               control.setGeneratorOffset1( settings.generatorReference );

               window.channel01.Ground = 1;
               window.channel02.Ground = 1;
               control.setAnalogSwitchBit(CHANNEL_A_GROUND,0);
               control.setAnalogSwitchBit(CHANNEL_B_GROUND,0);
               window.channel01.AcDc = 0;
               window.channel02.AcDc = 0;
               control.setAnalogSwitchBit(CHANNEL_A_ACDC, 1);
               control.setAnalogSwitchBit(CHANNEL_B_ACDC, 1);

               control.transferData();

               callibrate.mode = acGeneratorUpCapture;
            }
            break;
         case acGeneratorUpCapture:
            {
               int          ch = callibrate.channel;
               double yGridMax = double(grid.yCount) / 2.0;

               callibrate.offsetUpVolt[ch] = 0;
               for (int sample = 0; sample < frame.analog[ch].getCount(); sample++)
               {
                  callibrate.offsetUpVolt[ch] += frame.getAnalog(ch, sample);
               }
               callibrate.offsetUpVolt[ch] /= double(frame.analog[ch].getCount());
               callibrate.offsetUpVolt[ch] *= yGridMax*double(captureVoltFromEnum(callibrate.voltage));

               callibrate.mode = acGeneratorDownSetup;
            }
            break;
         case acGeneratorDownSetup:
            {;
               control.setYPositionA( settings.analogChannelOffsets[0][callibrate.voltage] );
               control.setYPositionB( settings.analogChannelOffsets[1][callibrate.voltage] );
               control.setGeneratorOffset0(-settings.generatorReference);
               control.setGeneratorOffset1(-settings.generatorReference);
               control.transferData();

               callibrate.mode = acGeneratorDownCapture;
            }
            break;
         case acGeneratorDownCapture:
            {
               int          ch = callibrate.channel;
               double yGridMax = double(grid.yCount) / 2.0;

               callibrate.offsetDownVolt[ch] = 0;
               for (int sample = 0; sample < frame.analog[ch].getCount(); sample++)
               {
                  callibrate.offsetDownVolt[ch] += frame.getAnalog(ch, sample);
               }
               callibrate.offsetDownVolt[ch] /= double(frame.analog[ch].getCount());
               callibrate.offsetDownVolt[ch] *= yGridMax*double(captureVoltFromEnum(callibrate.voltage));

               callibrate.mode = acGeneratorCalculate;
            }
            break;
         case acGeneratorCalculate:
            {
               double x1 =  settings.generatorReference;
               double x2 = -settings.generatorReference;
               double y1 = callibrate.offsetUpVolt[callibrate.channel];
               double y2 = callibrate.offsetDownVolt[callibrate.channel];
               settings.generatorOffset[callibrate.channel] = int(-((y2*(x2 - x1) / (y2 - y1)) - x2));
               callibrate.mode = acGeneratorEnd;
            }
            break;
         case acGeneratorEnd:
            {
               callibrate.voltage = vc2Volt;
               callibrate.mode = acGainStart;
            }
            break;
         case acGainStart:
            {
               callibrate.gainMax       = settings.gainMax[callibrate.voltage];
               callibrate.gainMin       = settings.gainMin[callibrate.voltage];
               callibrate.gainIteration = 0;
               callibrate.mode = acGainSetup;
            }
            break;
         case acGainSetup:
            {
               control.setYPositionA(settings.analogChannelOffsets[0][callibrate.voltage]);
               control.setYPositionB(settings.analogChannelOffsets[1][callibrate.voltage]);
               control.setGeneratorOffset0(settings.gainGenOffset[callibrate.voltage] + settings.generatorOffset[0]);
               control.setGeneratorOffset1(settings.gainGenOffset[callibrate.voltage] + settings.generatorOffset[1]);
               control.setYRangeScaleA( callibrate.gainMin + (callibrate.gainMax - callibrate.gainMin)/2, (uint)control.getAttr(callibrate.voltage) );
               control.setYRangeScaleB( callibrate.gainMin + (callibrate.gainMax - callibrate.gainMin)/2, (uint)control.getAttr(callibrate.voltage) );
               control.transferData();
               callibrate.mode = acGainCapture;
            }
            break;
         case acGainCapture:
            {
               int          ch = callibrate.channel;
               double yGridMax = double(grid.yCount) / 2.0;
               callibrate.gainVoltage = 0;
               for (int sample = 0; sample < frame.analog[ch].getCount(); sample++)
               {
                  callibrate.gainVoltage += frame.getAnalog(ch, sample);
               }
               callibrate.gainVoltage /= double(frame.analog[ch].getCount());
               callibrate.gainVoltage *= yGridMax*double(captureVoltFromEnum(callibrate.voltage));

               callibrate.mode = acGainCalculate;
            }
            break;
         case acGainCalculate:
            {
               float referenceVoltage = settings.gainRefVoltage[callibrate.voltage];
               float percentage = 100.f;

               if (settings.invertCh[callibrate.channel]) percentage *=  ((referenceVoltage + callibrate.gainVoltage) / referenceVoltage);
               else                                       percentage *=  ((referenceVoltage - callibrate.gainVoltage) / referenceVoltage);

               if ( fabsf(percentage) < settings.gainExitPercentage || callibrate.gainIteration >= settings.gainMaxIterations )
               {
                  callibrate.mode = acGainVoltageChange;
               }
               else
               {
                  uint delta = (callibrate.gainMax - callibrate.gainMin) / 2;
                  if (percentage > 0.f) callibrate.gainMin += delta;
                  else                  callibrate.gainMax -= delta;
                  callibrate.gainIteration++;
                  callibrate.mode = acGainSetup;
               }
            }
            break;
         case acGainVoltageChange:
            {
               settings.gainValue[callibrate.channel][callibrate.voltage] = callibrate.gainMin + (callibrate.gainMax - callibrate.gainMin) / 2;
               if (callibrate.voltage == vc10Mili)
               {
                  callibrate.mode = acGainEnd;
               }
               else
               {
                  callibrate.voltage = (VoltageCapture)((int)callibrate.voltage + 1);
                  callibrate.mode = acGainStart;
               }
            }
            break;
         case acGainEnd:
            {
               callibrate.mode = acChannelChange;
            }
            break;
         case acChannelChange:
            {
               if (callibrate.channel == 1)
               {
                  callibrate.voltage = vc2Volt;
                  control.setYRangeScaleA( vc2Volt, 0.f );
                  control.setYRangeScaleB( vc2Volt, 0.f );
                  control.setGeneratorOffset0( 0 );
                  control.setGeneratorOffset1( 0 );
                  control.setYPositionA(settings.analogChannelOffsets[0][ vc2Volt]);
                  control.setYPositionB(settings.analogChannelOffsets[1][ vc2Volt]);
                  control.transferData();
                  callibrate.mode       = acEndMessageBox;
                  callibrate.messageBox = acmbEnd;
               }
               else
               {
                  callibrate.mode       = acGeneratorMessageBox;
                  callibrate.messageBox = acmbChannel1;
                  callibrate.channel    = 1;
                  control.transferData();
                  callibrate.mode = acGeneratorMessageBox;
               }
            }
            break;
         case acEndMessageBox:
            {
               if (callibrate.messageBox == acmbOk)
               {
                  callibrate.mode    = acSaveSettings;
                  callibrate.resetUI = true;
               }
               if (callibrate.messageBox == acmbCancel)
               {
                  callibrate.resetUI = true;
                  callibrate.active  = 0;
               }
            }
            break;
         case acSaveSettings:
            {
               callibrate.active = 0;
               settings.saveCallibrate();
            }
            break;
         break;
         }
      }

      // progress
      window.progress.uiValue = (callibrate.mode+1);
      window.progress.uiRange = (acEnd-1);
      window.progress.uiActive = 1;
   }
}

void OsciloscopeManager::AutoCallibrate()
{
   if( !usbIsConnected() )
      uploadFirmware();

   if( !usbIsConnected() )
   {
     SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Information","USB not connected.",0);
     return;
   }


   memset( settings.analogChannelOffsets, 0, sizeof(settings.analogChannelOffsets) );
   callibrate.active      = 1;
   callibrate.mode        = acStartMessageBox;
   callibrate.voltage     = vc2Volt;
   callibrate.frame       = 0;
   callibrate.generator   = 0;
   callibrate.messageBox  = acmbStart;

   signalMode = SIGNAL_MODE_CAPTURE;
}

double OsciloscopeManager::getTriggerVoltagePerStep()
{
   float  captureVoltage = 0.f;
   if (window.trigger.Source == 0) captureVoltage = window.channel01.Capture;
   else                            captureVoltage = window.channel02.Capture;
   return captureVoltage * grid.yCount / 1024.0;
}

double OsciloscopeManager::getTriggerVoltage()
{
   float  captureVoltage = 0.f;
   if (window.trigger.Source == 0) captureVoltage = window.channel01.Capture;
   else                            captureVoltage = window.channel02.Capture;
   return captureVoltage;
}

void OsciloscopeManager::setFrame(int frame)
{

}

void OsciloscopeManager::allocate()
{
   uint frameCount = settings.historyFrameCount;

   ularge toAllocate = frameCount*sizeof(OsciloscopeFrame);
   ularge     minSize = MINIMUM_HISTORY_COUNT*sizeof(OsciloscopeFrame);
   ularge       free = memoryMaximum(minSize, MAXIMUM_HISTORY_SIZE);
   ularge        max = free * 90 / 100;
   ularge        min = minSize;

   toAllocate = clamp(toAllocate, min, max);

   settings.historyFrameCount = toAllocate / sizeof(OsciloscopeFrame);
   if (settings.historyFrameCount == 0)
      CORE_ABORT("Not enough memory. Tried to allocate %d bytes.",toAllocate);

   pThreadData = (OsciloscopeFrame*)memoryAllocate(toAllocate);
   threadHistory.init((OsciloscopeFrame*)pThreadData, settings.historyFrameCount);

   toAllocate = settings.historyFrameDisplay*sizeof(OsciloscopeFrame);
   pTmpData = (OsciloscopeFrame*)memoryAllocate(toAllocate);
   tmpHistory.init((OsciloscopeFrame*)pTmpData, settings.historyFrameDisplay );

   for (uint i = 0; i < MAX_THREAD;i++)
   {
                            toAllocate = settings.historyFrameDisplay*sizeof(OsciloscopeFrame);
      OsciloscopeFrame* pThreadHistory = (OsciloscopeFrame*)memoryAllocate(toAllocate);
      captureData[i].history.init(pThreadHistory, settings.historyFrameDisplay );
   }
}

void OsciloscopeManager::setThreadPriority(ThreadID id)
{
    switch(id) {
    case THREAD_ID_CAPTURE:
        SDL_SetThreadPriority((SDL_ThreadPriority)settings.threadPriorityCapture);
        break;
    case THREAD_ID_UPDATE:
        SDL_SetThreadPriority((SDL_ThreadPriority)settings.threadPriorityUpdate);
        break;
    case THREAD_ID_MAIN:
        SDL_SetThreadPriority((SDL_ThreadPriority)settings.threadPriorityMain);
        break;
    case THREAD_ID_RENDER:
       SDL_SetThreadPriority((SDL_ThreadPriority)settings.threadPriorityRender);
       break;
    };
}


int SDLCALL UpdateThreadFunction(void *data)
{
   /*
   uint threadid = *(uint*)data;

   pOsciloscope->setThreadPriority(THREAD_ID_UPDATE);

   OsciloscopeThreadRenderer renderer;
   OsciloscopeFFT            fft;

   static SDL_SpinLock lock = { 0 };
   SDL_AtomicLock(&lock);
      OsciloscopeThreadData captureData;
      uint              toAllocate = pOsciloscope->settings.historyFrameDisplay*sizeof(OsciloscopeFrame);
      OsciloscopeFrame* memory     = (OsciloscopeFrame*)memoryAllocate(toAllocate);
      captureData.history.init( memory, pOsciloscope->settings.historyFrameDisplay );
      fft.init();
      renderer.init(pOsciloscope->settings.historyFrameDisplay);
   SDL_AtomicUnlock(&lock);

   renderer.clear();

   pTimer->init(threadid + TIMER_UPDATE0);
   while (pOsciloscope->threadActive)
   {
      // render frame
      uint captureId = 0;
      bool ret = pOsciloscope->threadLoop.capture.consumerLock(captureId,false);
      if(ret)
      {
         OsciloscopeThreadData* pThreadData = &pOsciloscope->captureData[captureId];
         uint          count = pThreadData->history.getCount();
         OsciloscopeFrame frame;
         captureData.history.clear();
         for (uint i = 0; i < count;i++)
         {
            pThreadData->history.read(frame);
            captureData.history.write(frame);
         }
         captureData.frame  = pThreadData->frame;
         captureData.render = pThreadData->render;
         captureData.window = pThreadData->window;

         pOsciloscope->threadLoop.capture.consumerUnlock(captureId);

         uint updateId = 0;
         while(pOsciloscope->threadActive)
         {
            ret = pOsciloscope->threadLoop.update.producerLock(updateId,false);
            if (ret)
            {
               pTimer->deltaTime(threadid + TIMER_UPDATE0);

               renderer.clearFast();
               fft.clear();
               pOsciloscope->renderThread(updateId, captureData, renderer, fft );
               pOsciloscope->threadLoop.update.producerUnlock(updateId);
               break;
            }
            else
            {
                //if(pOsciloscope->settings.threadDelayUpdateProducer==0) break;
                if(pOsciloscope->settings.threadDelayUpdateProducer>0)
                    SDL_Delay(pOsciloscope->settings.threadDelayUpdateProducer);
            }
         }
      }
      else
      {
          if(pOsciloscope->settings.threadDelayUpdateConsumer>0)
            SDL_Delay(pOsciloscope->settings.threadDelayUpdateConsumer);
      }
   }
   */
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ETS
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeETS::clear()
{
   etsHistory.clear();
   etsIndex = 0;
}

void OsciloscopeETS::redraw(OsciloscopeRenderData& render)
{
   render.flags.raise(rfClearRenderTarget);
   etsIndex = etsHistory.getCount() - 1;
   etsIndex = max(0U, etsIndex);
}

void OsciloscopeETS::onFrameChange(int framechange, Ring<OsciloscopeFrame> threadHistory,OsciloscopeRenderData& render)
{
   render.flags.raise(rfClearRenderTarget);

   // ets, display
   clear();

   // update
   Ring<OsciloscopeFrame> current = threadHistory;
   current.offset(framechange - 32);
   OsciloscopeFrame frame;
   for (uint i = 0; i < 32; i++)
   {
      if (current.isEmpty()) continue;

      current.read(frame);
      onCapture(frame,render);
   }

   redraw(render);
}

void OsciloscopeETS::onCapture(OsciloscopeFrame& frame,OsciloscopeRenderData& render)
{
   // attributes, index
   etsAttr  = 0;
   etsIndex = 0;

   // frame to clear
   for (int i = 0; i < etsHistory.getCount(); i++)
   {
      if (frame.ets == etsHistory[i].ets)
      {
         etsClear = etsHistory[i];
         etsHistory.remove(i);
         etsAttr = ETS_CLEAR;
         break;
      }
   }

   // add frames
   if (etsHistory.getCount() < etsHistory.getSize() &&
      etsHistory.getCount() < (int)pOsciloscope->settings.historyFrameCount)
   {
      etsHistory.pushBack(frame);
   }

   render.etsAttr = etsAttr;
}

void OsciloscopeETS::onPause(OsciloscopeFrame& frame,WndMain& window)
{
   if (window.horizontal.ETS)
   {
      etsAttr = 0;
      if (etsHistory.getCount())
      {
         if (etsIndex >= 0 && etsIndex < (uint)etsHistory.getCount())
         {
            frame = etsHistory[etsIndex];
            etsIndex--;
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// CaptureThread
//
////////////////////////////////////////////////////////////////////////////////
void SendToRenderer(OsciloscopeFrame& captureFrame,
                    WndMain& window,
                    OsciloscopeRenderData& render,
                    OsciloscopeETS& ets,
                    OsciloscopeThreadRenderer& renderer,
                    OsciloscopeFFT& fft,
                    OsciloscopeThreadData& captureData)
{
   SDL_AtomicLock(&pOsciloscope->displayLock);
      pOsciloscope->display = captureFrame;
   SDL_AtomicUnlock(&pOsciloscope->displayLock);

   // history
   pOsciloscope->tmpHistory.write(captureFrame);

   // ets
   ets.onCapture(captureFrame, render);

   // send to renderer
   uint captureId = 0;
   bool ret = false;
   while( !ret )
   {
       ret = pOsciloscope->threadLoop.update.producerLock(captureId,false);
       if( ret )
       {
            Ring<OsciloscopeFrame> history = pOsciloscope->tmpHistory;
            OsciloscopeFrame       frame;
            uint count = history.getCount();
            captureData.history.clear();
            for (uint i = 0; i < count; i++)
            {
                history.read(frame);
                captureData.history.write(frame);
            }
            captureData.etsClear = ets.etsClear;
            captureData.frame    = frame;
            captureData.render   = render;
            captureData.window   = window;

            // render
            renderer.clearFast();
            fft.clear();
            pOsciloscope->renderThread(captureId, captureData, renderer, fft );

            pOsciloscope->threadLoop.update.producerUnlock(captureId);

            pTimer->deltaTime(TIMER_CAPTURE);
        }
        else
        {
            if( pOsciloscope->settings.threadDelayCaptureProducer>0)
                SDL_Delay(pOsciloscope->settings.threadDelayCaptureProducer);
        }
   }
}

int SDLCALL CaptureThreadFunction(void *data)
{
   pOsciloscope->setThreadPriority(THREAD_ID_CAPTURE);

   pTimer->init(TIMER_CAPTURE);
   pTimer->init(TIMER_GENERATE);

   OsciloscopeFrame       captureFrame;
   WndMain                captureWindow;
   OsciloscopeRenderData  captureRender;
   byte   buffer[CAPTURE_BUFFER] = { 0 };
   uint   displayBytes           = 0;
   uint   dataOffset             = 0;
   bool   isHeader               = false;
   bool   isPadding              = false;
   bool   isData                 = false;
   ularge captureFreq            = SDL_GetPerformanceFrequency();
   ularge captureStart           = 0;
   ularge captureTime            = 0;

   uint       frame = 0;
   SignalMode mode  = SIGNAL_MODE_PAUSE;

   SDL_zero(captureWindow);
   SDL_zero(captureRender);
   SDL_zero(captureFrame);

    OsciloscopeThreadRenderer renderer;
    OsciloscopeFFT            fft;

    OsciloscopeThreadData captureData;
    uint              toAllocate = pOsciloscope->settings.historyFrameDisplay*sizeof(OsciloscopeFrame);
    OsciloscopeFrame* memory     = (OsciloscopeFrame*)memoryAllocate(toAllocate);
    captureData.history.init( memory, pOsciloscope->settings.historyFrameDisplay );
    fft.init();
    renderer.init(pOsciloscope->settings.historyFrameDisplay);
    renderer.clear();


   OsciloscopeETS&        ets = pOsciloscope->ets;
   Ring<OsciloscopeFrame> play;

   while (pOsciloscope->threadActive)
   {
      // sync user interface and render data with capture thread
      SDL_AtomicLock(&pOsciloscope->renderLock);

            ////////////////////////////////////////////////////////////////////
            // sync capture thread with user interface
            ////////////////////////////////////////////////////////////////////
            switch (mode) {
            case SIGNAL_MODE_PAUSE:
               pOsciloscope->renderWindow.horizontal.uiActive = 0;
               break;
            case SIGNAL_MODE_PLAY:
               pOsciloscope->renderWindow.horizontal.uiActive = 1;
               pOsciloscope->renderWindow.horizontal.uiValue  = play.getStart();
               pOsciloscope->renderWindow.horizontal.Frame    = play.getStart();
               pOsciloscope->renderWindow.horizontal.uiRange  = pOsciloscope->threadHistory.getCount();
               break;
            case SIGNAL_MODE_GENERATOR:
            case SIGNAL_MODE_CAPTURE:
               pOsciloscope->renderWindow.horizontal.uiActive = 1;
               pOsciloscope->renderWindow.horizontal.uiValue  = max<int>(0,pOsciloscope->threadHistory.getCount());
               pOsciloscope->renderWindow.horizontal.Frame    = max<int>(0,pOsciloscope->threadHistory.getCount()-1);
               pOsciloscope->renderWindow.horizontal.uiRange  = pOsciloscope->threadHistory.getCount();
               break;
            case SIGNAL_MODE_CLEAR:
               pOsciloscope->renderWindow.horizontal.uiActive = 1;
               pOsciloscope->renderWindow.horizontal.uiValue  = 0;
               pOsciloscope->renderWindow.horizontal.Frame    = 0;
               pOsciloscope->renderWindow.horizontal.uiRange  = 0;
               break;
            };

            ////////////////////////////////////////////////////////////////////
            // sync user interface data with capture thread
            ////////////////////////////////////////////////////////////////////
            captureWindow = pOsciloscope->renderWindow;
            captureRender = pOsciloscope->renderData;

      SDL_AtomicUnlock(&pOsciloscope->renderLock);

      ////////////////////////////////////////////////////////////////////
      // change
      ////////////////////////////////////////////////////////////////////
      if (pOsciloscope->signalMode != mode)
      {
         ////////////////////////////////////////////////////////////////////
         // play / stop
         ////////////////////////////////////////////////////////////////////
         switch (pOsciloscope->signalMode) {
         case SIGNAL_MODE_PLAY:
            if( captureWindow.horizontal.Frame + 1 >= pOsciloscope->threadHistory.getCount() - 1 )
               captureWindow.horizontal.Frame = 0;
            play = pOsciloscope->threadHistory;
            play.offset( captureWindow.horizontal.Frame - pOsciloscope->threadHistory.getStart() );
            if (!play.isEmpty())
            {
               play.read(captureFrame);
            }
            ets.clear();
            break;
         case SIGNAL_MODE_PAUSE:
            ets.redraw(captureRender);
            break;
         case SIGNAL_MODE_CLEAR:
            pOsciloscope->threadHistory.clear();
            pOsciloscope->tmpDisplay.clear();
            captureFrame.clear();
            break;
         case SIGNAL_MODE_CAPTURE:
         case SIGNAL_MODE_GENERATOR:
            ets.clear();
            captureFreq  = SDL_GetPerformanceFrequency();
            captureStart = SDL_GetPerformanceCounter();
            pTimer->init(TIMER_CAPTURE);
            pTimer->init(TIMER_GENERATE);
            break;
         };

         mode = pOsciloscope->signalMode;
      }

      // redraw ets ?
      if ( captureRender.redrawEts )
         ets.redraw(captureRender);

      ////////////////////////////////////////////////////////////////////
      // mode
      ////////////////////////////////////////////////////////////////////
      switch (mode)
      {
      case SIGNAL_MODE_PLAY:
         {
            // loop
            if (play.isEmpty())
               play = pOsciloscope->threadHistory;

            // advance frame
            if (!play.isEmpty())
            {
               play.read(captureFrame);

               SendToRenderer(captureFrame, captureWindow, captureRender, ets,renderer,fft,captureData);

               captureWindow.horizontal.Frame = play.getStart();
            }
         }
         break;
      case SIGNAL_MODE_CAPTURE:
         {
            // capture
            uint              bytesToRead = pOsciloscope->control.getFrameSize() * 6 + CAPTURE_BUFFER_HEADER + CAPTURE_BUFFER_PADDING;
            uint                     read = bytesToRead - displayBytes;
            int                       ret = 0;
            int                transfered = 0;
            byte*                    dest = buffer + displayBytes;

            SDL_AtomicLock(&pOsciloscope->uploadFirmwareLock);
               ret = usbTransferDataIn(6, dest, read, 0, pOsciloscope->settings.usbEp6TimeOut, transfered);
            SDL_AtomicUnlock(&pOsciloscope->uploadFirmwareLock);
            if (ret && transfered > 0)
            {
               if (captureFrame.isFull())
               {
                  pOsciloscope->threadHistory.write(captureFrame);
                  SendToRenderer(captureFrame, captureWindow, captureRender,ets,renderer,fft,captureData);
                  captureFrame.clear();
               }

               uint bufferSize = displayBytes + transfered;

               if (!isHeader && bufferSize >= CAPTURE_BUFFER_HEADER)
               {
                  isHeader = captureFrame.captureHeader(buffer, bufferSize,captureStart,captureFreq);
               }

               if (isHeader && bufferSize > (CAPTURE_BUFFER_HEADER + CAPTURE_BUFFER_PADDING))
               {
                  byte* data = buffer + CAPTURE_BUFFER_HEADER + CAPTURE_BUFFER_PADDING;
                  uint  size = bufferSize - CAPTURE_BUFFER_HEADER - CAPTURE_BUFFER_PADDING;
                  isData = captureFrame.captureData(data, dataOffset, size);
               }

               displayBytes += transfered;

               if (isData)
               {
                  isHeader = false;
                  isPadding = false;
                  isData = false;
                  displayBytes = 0;
                  dataOffset = 0;
               }
            }

            pOsciloscope->threadHistory.write(captureFrame);

            SendToRenderer(captureFrame, captureWindow, captureRender,ets,renderer,fft,captureData);

            pOsciloscope->onCallibrateFrameCaptured(captureFrame);
         }
         break;
      case SIGNAL_MODE_GENERATOR:
         {
            // dt
            pTimer->deltaTime(TIMER_GENERATE);

            // software generator
            if (captureFrame.isFull())
            {
               captureFrame.clear();
            }
            captureFrame.generate(pTimer->getDelta(TIMER_GENERATE),pOsciloscope->window.horizontal.FrameSize,captureStart,captureFreq);

            pOsciloscope->threadHistory.write(captureFrame);

            SendToRenderer(captureFrame, captureWindow, captureRender,ets,renderer,fft,captureData);
         }
         break;
      case SIGNAL_MODE_CLEAR:
      case SIGNAL_MODE_PAUSE:
         {
            if (frame != captureWindow.horizontal.Frame)
            {
               frame = captureWindow.horizontal.Frame;

               // history
               pOsciloscope->tmpHistory.clear();
               Ring<OsciloscopeFrame> current = pOsciloscope->threadHistory;
               current.offset( frame );
               current.offset( pOsciloscope->threadHistory.getSize() - pOsciloscope->tmpHistory.getSize() );
               uint count = pOsciloscope->tmpHistory.getSize();
               for (uint i = 0; i < count; i++)
               {
                  if (current.isEmpty()) continue;

                  current.read(pOsciloscope->tmpDisplay);
                  pOsciloscope->tmpHistory.write(pOsciloscope->tmpDisplay);
               }

               captureFrame = pOsciloscope->tmpDisplay;

               ets.onFrameChange(frame, pOsciloscope->threadHistory, captureRender );
            }

            ets.onPause(captureFrame, captureWindow);

            SendToRenderer(captureFrame, captureWindow, captureRender,ets,renderer,fft,captureData);

            if(pOsciloscope->settings.threadDelayPause>0)
                SDL_Delay( pOsciloscope->settings.threadDelayPause );
         }
         break;
      };
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
