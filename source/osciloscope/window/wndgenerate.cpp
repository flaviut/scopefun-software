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
// SoftwareGeneratorChannel
//
////////////////////////////////////////////////////////////////////////////////
SoftwareGeneratorChannel::SoftwareGeneratorChannel()
{
   Default();
}

void SoftwareGeneratorChannel::Default()
{
   type = GENERATE_SINUS;
   period = 50.f*MICRO;
   peakToPeak = 17.f;
   speed = 0.1f;
   every = 0.0f;
   onOff = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// WndSoftwareGenerator
//
////////////////////////////////////////////////////////////////////////////////
WndSoftwareGenerator::WndSoftwareGenerator()
{

}

void WndSoftwareGenerator::Default()
{
	channel[0].Default();
	channel[1].Default();

   channel[1].speed      = 0.05f;
   channel[1].peakToPeak = 12.f;
   channel[1].period     = 42.f*MICRO;
}


////////////////////////////////////////////////////////////////////////////////
//
// WndHardwareGenerator
//
////////////////////////////////////////////////////////////////////////////////

WndHardwareGenerator::WndHardwareGenerator()
{
   Default();
}

void WndHardwareGenerator::loadCustomData(int idx, const char* path)
{
   char*  memory = 0;
   ilarge memorySize = 0;
   int ret = fileLoadString(path,&memory,&memorySize);
   if (ret) return;

   cJSON* root = cJSON_Parse( memory );

   cJSON* arrayValues = cJSON_GetObjectItem(root,"signal4096");
   for (uint i = 0; i<GENERATOR_ANALOG_SAMPLES; i++)
   {
      cJSON* val = cJSON_GetArrayItem( arrayValues, i );
      if( !val ) continue;

      if( idx == 0 || idx == 1 ) custom.analog[idx][i] = val->valueint;
   }

   cJSON_Delete(root);

   fileFree(memory);
}

void WndHardwareGenerator::loadCustomDigital(const char* path)
{
   char*  memory = 0;
   ilarge memorySize = 0;
   int ret = fileLoadString(path, &memory, &memorySize);
   if (ret) return;

   cJSON* root = cJSON_Parse(memory);

   FORMAT_BUFFER();
   for (int channel = 0; channel < GENERATOR_DIGITAL_CHANNELS; channel++)
   {
      FORMAT("digitalChannel%d", channel);
      cJSON* arrayValues = cJSON_GetObjectItem(root, formatBuffer);
      if (!arrayValues) continue;
      for (uint i = 0; i < GENERATOR_DIGITAL_SAMPLES / 8; i++)
      {
         byte value = 0;
         for (int bit = 0; bit < 8; bit++)
         {
            cJSON* val = cJSON_GetArrayItem(arrayValues, i*8 + bit );
            if (!val) continue;

            value = value | (val->valueint << bit);
         }
         custom.digital[channel][i] = value;
      }
   }

   cJSON_Delete(root);

   fileFree(memory);
}

void WndHardwareGenerator::upload()
{
   int ret = usbTransferDataOut( 4, (byte*)&custom, sizeof(custom), 0, pOsciloscope->settings.usbEp4TimeOut );
}

void WndHardwareGenerator::uploadDigital()
{
   int ret = usbTransferDataOut(4, (byte*)&custom, sizeof(custom), 0, pOsciloscope->settings.usbEp4TimeOut);
}

void WndHardwareGenerator::Default()
{
   // generator 0
   type0       = GENERATOR_SIN;
   frequency0  = 1000.f;
   voltage0    = 1024;
   offset0     = 0;
   squareDuty0 = 1024;
   sawSlopePositive0 = 1;
   onOff0 = 0;

   // generator 1
   type1       = GENERATOR_SIN;
   frequency1  = 1000.f;
   voltage1    = 1024;
   offset1     = 0;
   squareDuty1 = 1024;
   sawSlopePositive1 = 1;
   onOff1 = 0;

   // customData
   memset( &custom, 0, sizeof(custom) );
}


////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
