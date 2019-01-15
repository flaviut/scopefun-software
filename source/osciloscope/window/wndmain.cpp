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
// MWProgress
//
////////////////////////////////////////////////////////////////////////////////
MWProgress::MWProgress()
{
   uiActive = 0;
   uiValue  = 0;
   uiRange  = 0;
   uiPulse  = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// MWVertical
//
////////////////////////////////////////////////////////////////////////////////
MWVertical::MWVertical()
{
   Default();
}

void MWVertical::Default()
{
   Capture   = 2.f;
   Scale     = 0.f;
   Display   = 2.f;
   YPosition = 0;
   OscOnOff  = 1;
   FFTOnOff  = 0;
   Invert = 0;
   Ground = 0;
   AcDc   = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// MWFunction
//
////////////////////////////////////////////////////////////////////////////////
MWFunction::MWFunction()
{
   Default();
}

void MWFunction::Default()
{
   Type = 0;
   OscOnOff = 0;
   FFTOnOff = 0;
   xyGraph = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// MWHorizontal
//
////////////////////////////////////////////////////////////////////////////////
MWHorizontal::MWHorizontal()
{
   Default();
}

void MWHorizontal::Default()
{
   Capture  = 0.00000001f;
   Display  = 0.00000001f;
   Position = 0.f;
   Mode     = SIGNAL_MODE_PAUSE;
   Control  = 0;
   Frame = 0;
   FrameSize = NUM_SAMPLES;
   FFTSize   = NUM_SAMPLES;
   ETS = 0;
   //
   uiActive = 0;
   uiRange  = 8;
   uiValue  = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// MWTrigger
//
////////////////////////////////////////////////////////////////////////////////
MWTrigger::MWTrigger()
{
   Default();
}
void MWTrigger::Default()
{
   Source  = 0;
   Slope   = 0;
   Mode    = 0;
   Level   = 0;
   His     = 0;
   Percent = 0;
   Holdoff = 0;
   //
   stage = 0;
   stageStart = 0;
   stageMode = 0;
   stageChannel = 0;
   for(int i=0;i<4;i++)
   {
      delay[i] = 0;
      mask[i].setCount(16);
      pattern[i].setCount(16);
      for(int j=0;j<16;j++)
      {
         mask[i][j] = 0;
         pattern[i][j] = 0;
      }
   }
}


////////////////////////////////////////////////////////////////////////////////
//
// MWDigital
//
////////////////////////////////////////////////////////////////////////////////
MWDigital::MWDigital()
{
    Default();
}

void MWDigital::Default()
{
   digital.setCount(16);
   output.setCount(16);
   for(int i=0;i<16;i++)
   {
      digital[i] = 0;
      output[i]  = 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// MWDigitalSetup
//
////////////////////////////////////////////////////////////////////////////////
MWDigitalSetup::MWDigitalSetup()
{
   Default();
}

void MWDigitalSetup::Default()
{
   divider = 0;
   voltage = 1.25;
   inputOutput15 = 1;
   inputOutput7  = 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// MWMeasure
//
////////////////////////////////////////////////////////////////////////////////
MeasureData::MeasureData()
{
   Clear();
}

void MeasureData::Clear()
{
   memset(&pick, 0, sizeof(pick));
   memset(&history, 0, sizeof(history));
}

double MeasurePickData::RowD(EValueGrid2 r)
{
   if( r < Last2 )
      return row[r];
   return 0;
}

ushort MeasurePickData::RowU(EValueGrid2 r)
{
   if (r == DigitalX0) return DigitalX0;
   if (r == DigitalX1) return DigitalX1;
   return 0;
}

double MeasureChannelData::Row(EValueGrid1 r)
{
   if( r < Last1 )
      return row[r];
   return 0;
}

void MeasureChannelData::SetValue(double value)
{
   for (int i = 0; i < Last1; i++)
      row[i] = value;
}

void MeasureChannelData::Minimum(MeasureChannelData& other)
{
   for (int i = 0; i < Last1; i++)
      row[i] = min(row[i], other.row[i]);
}

void MeasureChannelData::Maximum(MeasureChannelData& other)
{
   for (int i = 0; i < Last1; i++)
      row[i] = max(row[i], other.row[i]);
}
void MeasureChannelData::Average(MeasureChannelData& other)
{
   //double div = (averageN + 1.0);
   //double mul = averageN / (averageN + 1);
   //  averageN = averageN + 1.0;

   //  /*for (int i = 0; i < Last1; i++)
   //     row[i] = row[i]*mul + other.row[i]/div;*/

   for (int i = 0; i < Last1; i++)
      row[i] = (row[i] + other.row[i]) / 2;
}

void MeasureChannelData::Clear()
{
   averageN   = 0;

   for (int i = 0; i < Last1; i++)
   {
      row[i] = 0;
      display[i] = false;
   }
}

uint MeasurePos::getXIndex(uint frameSize)
{
   return uint( (x)*double( (frameSize-1) ) );
}

void MeasurePos::setXTime(double time)
{
   double maxTime = double(pOsciloscope->window.horizontal.Capture) * double(pOsciloscope->window.horizontal.FrameSize);
   xTime = time;
   x  = time / maxTime;
   x += double(pOsciloscope->window.trigger.Percent / 100.0);
}

void MeasurePos::setXFreq(double freq)
{
   xFreq = freq;

   double      time = pOsciloscope->window.horizontal.Capture;
   double  maxHertz = (1.f / time)*0.5f;
   iint           p = pOsciloscope->window.horizontal.FFTSize;
   int        count = p / 2 + 1;
   double       max = double(count - 1);

   if (pOsciloscope->window.display.fftLogFreq > 0)
   {
      double norm  = freq / maxHertz;
      double hertz = norm * max;
      x = log2(hertz) / log2(max) - 0.5;
   }
   else
   {
      x = double(freq) / double(maxHertz) - 0.5;
   }
}

void MeasurePos::setYVolt(double volt,int channel)
{
   switch(channel) {
   case 0: yVolt = volt; y = volt / (double(pOsciloscope->window.channel01.Capture) * double(pOsciloscope->grid.yCount)); break;
   case 1: yVolt = volt; y = volt / (double(pOsciloscope->window.channel02.Capture) * double(pOsciloscope->grid.yCount)); break;
   };
}

double MeasurePos::getXTime()
{
   double  maxTime = double(pOsciloscope->window.horizontal.Capture) * double(pOsciloscope->window.horizontal.FrameSize);
   xTime  = x * maxTime - maxTime*(double(pOsciloscope->window.trigger.Percent) / 100.0);
   return xTime;
}

double freqFromPosition(int index, int maxIndex, double maxHertz);

double MeasurePos::getXFreq()
{
   double      time = pOsciloscope->window.horizontal.Capture;
   double   maxHertz = (1.f / time)*0.5f;
   iint           p = pOsciloscope->window.horizontal.FFTSize;
   int        count = p / 2 + 1;
   double       max = float(count - 1);

   if (pOsciloscope->window.display.fftLogFreq > 0)
   {
      double pos = (x + 0.5f);

      double hertz = float(pos) * maxHertz;
      double max = float(count - 1);

      hertz = pow(max, pos);
      double norm = hertz / max;
      hertz = norm*maxHertz;
      xFreq = hertz;
   }
   else
   {
      xFreq = (x + 0.5)*double(maxHertz);
   }

   return xFreq;
}

double MeasurePos::getYVolt(int channel)
{
   switch (channel) {
   case 0: yVolt = y * double(pOsciloscope->window.channel01.Capture) * double(pOsciloscope->grid.yCount);
   case 1: yVolt = y * double(pOsciloscope->window.channel02.Capture) * double(pOsciloscope->grid.yCount);
   };
   return yVolt;
}

float MeasurePos::FrameToZ(int iframe)
{
   int maximum = pOsciloscope->settings.historyFrameCount;
   return float(iframe%maximum) / float(maximum - 1);
}

int MeasurePos::ZToFrame(float fz)
{
   int maximum = pOsciloscope->settings.historyFrameCount;
   return int(fz*float(maximum - 1));
}

void MeasurePos::setZ(int frame)
{
   int maximum = pOsciloscope->settings.historyFrameCount;
   z = float( frame%maximum ) / float(maximum-1);
}

int MeasurePos::getZ()
{
   int maximum = pOsciloscope->settings.historyFrameCount;
   return int( z*float(maximum-1) );
}

MWMeasure::MWMeasure()
{
   Default();
}

void MWMeasure::Default()
{
   uiOpen = 0;
   automaticDataClear = true;
   automaticDataClearTrigger = false;
   ClearCapture();
}

////////////////////////////////////////////////////////////////////////////////
//
// WndMain
//
////////////////////////////////////////////////////////////////////////////////
WndMain::WndMain()
{
   Default();
}

void WndMain::Default()
{
   speed = USB_SPEED_AUTOMATIC;

   display.Default();
   thermal.Default();
   softwareGenerator.Default();
   hardwareGenerator.Default();

   measure.Default();
   channel01.Default();
   channel02.Default();
   function.Default();
   horizontal.Default();
   trigger.Default();
   digital.Default();
   digitalSetup.Default();

   fftDigital.raise(VIEW_SELECT_DIGITAL);
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
