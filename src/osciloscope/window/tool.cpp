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
// Basic
//
////////////////////////////////////////////////////////////////////////////////
float multiplyerFromEnum(int enumerated)
{
   switch( enumerated ) {
   case 0: return 1.f;
   case 1: return MILI;
   case 2: return MICRO;
   case 3: return NANO;
   };
   return 0.f;
}

double multiplyerDoubleFromValue(double value)
{
   value = fabs(value);
   if (value < NANO)  return 1.0 / NANO;
   if (value < MICRO) return 1.0 / MICRO;
   if (value < MILI)  return 1.0 / MILI;

   if (value > GIGA)  return 1.0 / GIGA;
   if (value > MEGA)  return 1.0 / MEGA;
   if (value > KILO)  return 1.0 / KILO;

   return 1.0;
}

int multiplyerIndexFromValue(double value)
{
   value = fabs(value);
   if (value < NANO)  return  -3;
   if (value < MICRO) return  -2;
   if (value < MILI)  return  -1;

   if (value > GIGA)  return  3;
   if (value > MEGA)  return  2;
   if (value > KILO)  return  1;

   return 0;
}

uint  multiplyerFromValue(float value)
{
   if( value < MICRO ) return 3;
   if( value < MILI  ) return 2;
   if( value < 1.f   ) return 1;
   return 0;
}

float captureTimeFromEnum(int enumerated)
{
   switch( enumerated ) {
   case tc10ns:  return 10.f*NANO;
   case tc20ns:  return 20.f*NANO;
   case tc50ns:  return 50.f*NANO;
   case tc100ns: return 100.f*NANO;
   case tc200ns: return 200.f*NANO;
   case tc500ns: return 500.f*NANO;
   case tc1us:   return 1.f*MICRO;
   case tc2us:   return 2.f*MICRO;
   case tc5us:   return 5.f*MICRO;
   case tc10us:  return 10.f*MICRO;
   case tc20us:  return 20.f*MICRO;
   case tc50us:  return 50.f*MICRO;
   case tc100us: return 100.f*MICRO;
   case tc200us: return 200.f*MICRO;
   case tc500us: return 500.f*MICRO;
   case tc1ms:   return 1.f*MILI;
   case tc2ms:   return 2.f*MILI;
   case tc5ms:   return 5.f*MILI;
   case tc10ms:  return 10.f*MILI;
   case tc20ms:  return 20.f*MILI;
   case tc50ms:  return 50.f*MILI;
   case tc100ms: return 100.f*MILI;
   case tc200ms: return 200.f*MILI;
   case tc500ms: return 500.f*MILI;
   case tc1s:    return 1.f;
   };
   return 0.f;
}

uint captureTimeFromValue(float value)
{
   // if( value <=  10.f*NANO)
   if( value <  20.f*NANO)    return tc10ns;
   if( value <  50.f*NANO)    return tc20ns;
   if( value <  100.f*NANO)   return tc50ns;
   if( value <  200.f*NANO)   return tc100ns;
   if( value <  500.f*NANO)   return tc200ns;
   if( value <  1.f*MICRO)    return tc500ns;
   if( value <  2.f*MICRO)    return tc1us;
   if( value <  5.f*MICRO)    return tc2us;
   if( value <  10.f*MICRO)   return tc5us;
   if( value <  20.f*MICRO)   return tc10us;
   if( value <  50.f*MICRO)   return tc20us;
   if( value <  100.f*MICRO)  return tc50us;
   if( value <  200.f*MICRO)  return tc100us;
   if( value <  500.f*MICRO)  return tc200us;
   if( value <  1.f*MILI)     return tc500us;
   if( value <  2.f*MILI)     return tc1ms;
   if( value <  5.f*MILI)     return tc2ms;
   if( value <  10.f*MILI)    return tc5ms;
   if( value <  20.f*MILI)    return tc10ms;
   if( value <  50.f*MILI)    return tc20ms;
   if( value <  100.f*MILI)   return tc50ms;
   if( value <  200.f*MILI)   return tc100ms;
   if( value <  500.f*MILI)   return tc200ms;
   if( value <  1.f)          return tc500ms;
   return tc1s;
}

float captureVoltFromEnum(int enumerated)
{
   switch( enumerated ) {
   case vc2Volt   : return 2.f;
   case vc1Volt   : return 1.f;
   case vc500Mili : return 500.f*MILI;
   case vc200Mili : return 200.f*MILI;
   case vc100Mili : return 100.f*MILI;
   case vc50Mili  : return 50.f*MILI;
   case vc20Mili  : return 20.f*MILI;
   case vc10Mili  : return 10.f*MILI;
   };
   return 0;
}

uint captureVoltFromValue(float value)
{
   if( value < 0.02f ) return vc10Mili;
   if( value < 0.05f ) return vc20Mili;
   if( value < 0.1f  ) return vc50Mili;
   if( value < 0.2f  ) return vc100Mili;
   if( value < 0.5f  ) return vc200Mili;
   if( value < 1.f   ) return vc500Mili;
   if( value < 2.f   ) return vc1Volt;
   return vc2Volt;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ToolText::Time(char *buffer,int size,float value)
{
   double absvalue = fabsf(value);
   if( absvalue == 0.f )
   {
      formatString(buffer,size," 0.0 s");
   }
   else if( absvalue >= NANO && absvalue < MICRO )
   {
      formatString(buffer,size,"%.2f ns",value*GIGA);
   }
   else if( absvalue >= MICRO && absvalue < MILI )
   {
      formatString(buffer,size,"%.2f qs",value*MEGA);
   }
   else if( absvalue >= MILI && absvalue < 1.f )
   {
      formatString(buffer,size,"%.2f ms",value*KILO);
   }
   else if( absvalue >= 1.f && absvalue < 1000.f )
   {
      formatString(buffer,size,"%.2f s",value);
   }
   else
   {
      formatString(buffer,size,"%.2f ks",value*MILI);
   }
}

void ToolText::Hertz(char *buffer,int size,float value)
{
   if( value < PIKO )
   {
      formatString(buffer,size,"0 Hz");
   }
   if( value >= PIKO && value < NANO )
   {
      formatString(buffer,size,"%.3f nHz",value*GIGA);
   }
   if( value >= NANO && value < MICRO )
   {
      formatString(buffer,size,"%.3f qHz",value*MEGA);
   }
   else if( value >= MICRO && value < MILI )
   {
      formatString(buffer,size,"%.3f mHz",value*KILO);
   }
   else if( value >= MILI && value < KILO )
   {
      formatString(buffer,size,"%.3f Hz",value);
   }
   else if( value >= KILO && value < MEGA )
   {
      formatString(buffer,size,"%.3f KHz",value/KILO);
   }
   else if( value >= MEGA && value < GIGA )
   {
      formatString(buffer,size,"%.3f MHz",value/MEGA);
   }
   else if( value >= GIGA )
   {
      formatString(buffer,size,"%.3f GHz",value/GIGA);
   }
}

void ToolText::Decibels(char *buffer,int size,float value)
{
   formatString(buffer,size,"%.2f dB",value);
}

void ToolText::Volt(char *buffer,int size,float value)
{
   formatString(buffer,size,"%.2f V",value);
}


////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////

