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

OscSettings::OscSettings()
{
   speedLow    = 60;
   speedMedium = 200;
   speedHigh   = 500;

   threadDelayCaptureProducer = 1;
   threadDelayPause   = 33;
   threadDelayUpdateConsumer = 10;
   threadDelayUpdateProducer = 1;
   threadDelayRender = 0;
   threadDelayIdle = 0;
   threadPriorityCapture = 2;
   threadPriorityUpdate  = 1;
   threadPriorityRender  = 0;
   threadPriorityMain    = 0;
   updateTimer = 0.016;
   renderTimer = 0.001;

   threadCount    = 2;
   threadBuffer   = 16;
   fps = 0;

   historyFrameCount     = 10000;
   historyFrameDisplay   = 16;
   historyFrameLoadSave  = 16;
   historyFrameClipboard = 16;
   shaders21        = 0;
   vertexBufferSizeMegaByte = 16;
   depthBuffer = 0;

   debug = 0;
   displayWidth  = 500;
   displayHeight = 650;

   generatorFs = 457142.81;

   ets = 32;
   etsByteIndex = 3;

   middleOffset = 0.02;

   usbVendorId  = 0;
   usbProductId = 0;
   usbEp2TimeOut = 20;
   usbEp4TimeOut = 20;
   usbEp6TimeOut = 20;
   usbXferSize  = 300;

   generatorVoltagePerStep = 0.0;
   generatorReference = 0;
   generatorOffset[0] = 0;
   generatorOffset[1] = 0;
   gainExitPercentage = 0;
   gainMaxIterations  = 0;
   analogChannelReference = 0;

   digitalVoltageCoeficient = 116.0;

   for(int i=0;i<2;i++)
      for(int j=0;j<vcLast;j++)
      {
         analogChannelOffsets[i][j] = 0.0;
         analogChannelYVoltageStep[i][j] = 0.0;
      }


   for (int ch=0;ch<2;ch++)
   {
      for (int i = 0; i < vcLast; i++)
      {
         gainValue[ch][i] = 0;
         gainAttr[i] = 0;
      }
   }

   for (int i = 0; i < vcLast; i++)
   {
      gainMin[i] = 0;
      gainMax[i] = 0;
      gainGenOffset[i] = 0;
      gainRefVoltage[i] = 0;
      gainRefValue[i] = 0;
   }
}

int jsonToInt(cJSON* json)
{
   if( json->type == cJSON_Number ) return json->valueint;
   CORE_ERROR("Wrong json type",0);
   return 0;
}

double jsonToDouble(cJSON* json)
{
   if( json->type == cJSON_Number ) return json->valuedouble;
   CORE_ERROR("Wrong json type",0);
   return 0;
}

const char* jsonToString(cJSON* json)
{
   if( json->type == cJSON_String ) return json->valuestring;
   CORE_ERROR("Wrong json type",0);
   return 0;
}

void fillGuid(UsbGuid &usbGuid, const char* str )
{
   uint data[11] = { 0 };
   sscanf(str, "0x%8x,0x%4x,0x%4x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x", &data[0],
                                                                                    &data[1],
                                                                                    &data[2],
                                                                                    &data[3],
                                                                                    &data[4],
                                                                                    &data[5],
                                                                                    &data[6],
                                                                                    &data[7],
                                                                                    &data[8],
                                                                                    &data[9],
                                                                                    &data[10]);
   usbGuid.data1 = data[0];
   usbGuid.data2 = data[1];
   usbGuid.data3 = data[2];
   usbGuid.data4[0] = data[3];
   usbGuid.data4[1] = data[4];
   usbGuid.data4[2] = data[5];
   usbGuid.data4[3] = data[6];
   usbGuid.data4[4] = data[7];
   usbGuid.data4[5] = data[8];
   usbGuid.data4[6] = data[9];
   usbGuid.data4[7] = data[10];
}



void VoltageFromJson(cJSON* json, ushort* output)
{
   if (json && output)
   {
      cJSON* volt2   = cJSON_GetObjectItem(json, "volt2");
      cJSON* volt1   = cJSON_GetObjectItem(json, "volt1");
      cJSON* mili500 = cJSON_GetObjectItem(json, "mili500");
      cJSON* mili200 = cJSON_GetObjectItem(json, "mili200");
      cJSON* mili100 = cJSON_GetObjectItem(json, "mili100");
      cJSON* mili50  = cJSON_GetObjectItem(json, "mili50");
      cJSON* mili20  = cJSON_GetObjectItem(json, "mili20");
      cJSON* mili10  = cJSON_GetObjectItem(json, "mili10");

      if (volt2)   output[vc2Volt]   = jsonToDouble(volt2);
      if (volt1)   output[vc1Volt]   = jsonToDouble(volt1);
      if (mili500) output[vc500Mili] = jsonToDouble(mili500);
      if (mili200) output[vc200Mili] = jsonToDouble(mili200);
      if (mili100) output[vc100Mili] = jsonToDouble(mili100);
      if (mili50)  output[vc50Mili]  = jsonToDouble(mili50);
      if (mili20)  output[vc20Mili]  = jsonToDouble(mili20);
      if (mili10)  output[vc10Mili]  = jsonToDouble(mili10);
   }
}

void VoltageFromJson(cJSON* json, double* output)
{
   if (json && output)
   {
      cJSON* volt2 = cJSON_GetObjectItem(json, "volt2");
      cJSON* volt1 = cJSON_GetObjectItem(json, "volt1");
      cJSON* mili500 = cJSON_GetObjectItem(json, "mili500");
      cJSON* mili200 = cJSON_GetObjectItem(json, "mili200");
      cJSON* mili100 = cJSON_GetObjectItem(json, "mili100");
      cJSON* mili50 = cJSON_GetObjectItem(json, "mili50");
      cJSON* mili20 = cJSON_GetObjectItem(json, "mili20");
      cJSON* mili10 = cJSON_GetObjectItem(json, "mili10");

      if (volt2)   output[vc2Volt]   = jsonToDouble(volt2);
      if (volt1)   output[vc1Volt]   = jsonToDouble(volt1);
      if (mili500) output[vc500Mili] = jsonToDouble(mili500);
      if (mili200) output[vc200Mili] = jsonToDouble(mili200);
      if (mili100) output[vc100Mili] = jsonToDouble(mili100);
      if (mili50)  output[vc50Mili]  = jsonToDouble(mili50);
      if (mili20)  output[vc20Mili]  = jsonToDouble(mili20);
      if (mili10)  output[vc10Mili]  = jsonToDouble(mili10);
   }
}

void VoltageFromJson(cJSON* json, int* output)
{
   if (json && output)
   {
      cJSON* volt2 = cJSON_GetObjectItem(json, "volt2");
      cJSON* volt1 = cJSON_GetObjectItem(json, "volt1");
      cJSON* mili500 = cJSON_GetObjectItem(json, "mili500");
      cJSON* mili200 = cJSON_GetObjectItem(json, "mili200");
      cJSON* mili100 = cJSON_GetObjectItem(json, "mili100");
      cJSON* mili50 = cJSON_GetObjectItem(json, "mili50");
      cJSON* mili20 = cJSON_GetObjectItem(json, "mili20");
      cJSON* mili10 = cJSON_GetObjectItem(json, "mili10");

      if (volt2)   output[vc2Volt] = jsonToDouble(volt2);
      if (volt1)   output[vc1Volt] = jsonToDouble(volt1);
      if (mili500) output[vc500Mili] = jsonToDouble(mili500);
      if (mili200) output[vc200Mili] = jsonToDouble(mili200);
      if (mili100) output[vc100Mili] = jsonToDouble(mili100);
      if (mili50)  output[vc50Mili] = jsonToDouble(mili50);
      if (mili20)  output[vc20Mili] = jsonToDouble(mili20);
      if (mili10)  output[vc10Mili] = jsonToDouble(mili10);
   }
}

void VoltageToJson(cJSON* json, ushort* input )
{
   if( json && input )
   {
      cJSON_AddItemToObject(json, "volt2",   cJSON_CreateNumber(input[vc2Volt]));
      cJSON_AddItemToObject(json, "volt1",   cJSON_CreateNumber(input[vc1Volt]));
      cJSON_AddItemToObject(json, "mili500", cJSON_CreateNumber(input[vc500Mili]));
      cJSON_AddItemToObject(json, "mili200", cJSON_CreateNumber(input[vc200Mili]));
      cJSON_AddItemToObject(json, "mili100", cJSON_CreateNumber(input[vc100Mili]));
      cJSON_AddItemToObject(json, "mili50",  cJSON_CreateNumber(input[vc50Mili]));
      cJSON_AddItemToObject(json, "mili20",  cJSON_CreateNumber(input[vc20Mili]));
      cJSON_AddItemToObject(json, "mili10",  cJSON_CreateNumber(input[vc10Mili]));
   }
}


void VoltageToJson(cJSON* json, double* input)
{
   if (json && input)
   {
      cJSON_AddItemToObject(json, "volt2", cJSON_CreateNumber(input[vc2Volt]));
      cJSON_AddItemToObject(json, "volt1", cJSON_CreateNumber(input[vc1Volt]));
      cJSON_AddItemToObject(json, "mili500", cJSON_CreateNumber(input[vc500Mili]));
      cJSON_AddItemToObject(json, "mili200", cJSON_CreateNumber(input[vc200Mili]));
      cJSON_AddItemToObject(json, "mili100", cJSON_CreateNumber(input[vc100Mili]));
      cJSON_AddItemToObject(json, "mili50", cJSON_CreateNumber(input[vc50Mili]));
      cJSON_AddItemToObject(json, "mili20", cJSON_CreateNumber(input[vc20Mili]));
      cJSON_AddItemToObject(json, "mili10", cJSON_CreateNumber(input[vc10Mili]));
   }
}


void VoltageToJson(cJSON* json, int* input)
{
   if (json && input)
   {
      cJSON_AddItemToObject(json, "volt2", cJSON_CreateNumber(input[vc2Volt]));
      cJSON_AddItemToObject(json, "volt1", cJSON_CreateNumber(input[vc1Volt]));
      cJSON_AddItemToObject(json, "mili500", cJSON_CreateNumber(input[vc500Mili]));
      cJSON_AddItemToObject(json, "mili200", cJSON_CreateNumber(input[vc200Mili]));
      cJSON_AddItemToObject(json, "mili100", cJSON_CreateNumber(input[vc100Mili]));
      cJSON_AddItemToObject(json, "mili50", cJSON_CreateNumber(input[vc50Mili]));
      cJSON_AddItemToObject(json, "mili20", cJSON_CreateNumber(input[vc20Mili]));
      cJSON_AddItemToObject(json, "mili10", cJSON_CreateNumber(input[vc10Mili]));
   }
}

double OscSettings::getAnalogChannelYVoltageStep(int ch, VoltageCapture capture)
{
   ch = clamp(ch, 0, 1);
   int voltage = analogChannelYVoltageStep[ch][capture];
   return double(voltage)*DOUBLE_NANO;
}

void OscSettings::setAnalogChannelYVoltageStep(double value,int ch, VoltageCapture capture)
{
   ch = clamp(ch, 0, 1);
   analogChannelYVoltageStep[ch][capture] = double(value)*DOUBLE_GIGA;
}

void OscSettings::load()
{
   FORMAT_BUFFER();
   FORMAT_PATH("data/startup/settings.json");
   char*  memory = 0;
   ilarge memorySize = 0;
   fileLoadString(formatBuffer,&memory,&memorySize);

   json = cJSON_Parse((const char*)memory);
   if (!json)
   {
      CORE_MESSAGE("settings.json error before: [%.256s]\n", cJSON_GetErrorPtr());
   }

   fileFree(memory);

   cJSON* render = cJSON_GetObjectItem(json, "render");
   if (render)
   {
      cJSON* shd21           = cJSON_GetObjectItem(render, "shaders21");
      cJSON* vbsize          = cJSON_GetObjectItem(render, "vertexBufferSizeMegaByte");
      cJSON* depth           = cJSON_GetObjectItem(render, "depthBuffer");
      cJSON* framesPerSecond = cJSON_GetObjectItem(render, "fps");
      cJSON* tCount          = cJSON_GetObjectItem(render, "threadCount");
      cJSON* tBuffer         = cJSON_GetObjectItem(render, "threadBuffer");
      cJSON* tDelayCapture         = cJSON_GetObjectItem(render, "threadDelayCaptureProducer");
      cJSON* tDelayPause           = cJSON_GetObjectItem(render, "threadDelayPause");
      cJSON* tDelayUpdateConsumer  = cJSON_GetObjectItem(render, "threadDelayUpdateConsumer");
      cJSON* tDelayUpdateProducer  = cJSON_GetObjectItem(render, "threadDelayUpdateProducer");
      cJSON* tDelayRender          = cJSON_GetObjectItem(render, "threadDelayRender");
      cJSON* tDelayIdle            = cJSON_GetObjectItem(render, "threadDelayIdle");
      cJSON* tPriorityCapture      = cJSON_GetObjectItem(render, "threadPriorityCapture");
      cJSON* tPriorityUpdate       = cJSON_GetObjectItem(render, "threadPriorityUpdate");
      cJSON* tPriorityRender       = cJSON_GetObjectItem(render, "threadPriorityRender");
      cJSON* tPriorityMain         = cJSON_GetObjectItem(render, "threadPriorityMain");
      cJSON* timerUpdate           = cJSON_GetObjectItem(render, "updateTimer");
      cJSON* timerRender           = cJSON_GetObjectItem(render, "renderTimer");

      if (shd21)           shaders21                = jsonToInt(shd21);
      if (vbsize)          vertexBufferSizeMegaByte = jsonToInt(vbsize);
      if (depth)           depthBuffer              = jsonToInt(depth);
      if (framesPerSecond) fps                      = jsonToInt(framesPerSecond);
      if (tCount)          threadCount              = jsonToInt(tCount);
      if (tBuffer)         threadBuffer             = jsonToInt(tBuffer);
      if (tDelayCapture)         threadDelayCaptureProducer = jsonToInt(tDelayCapture);
      if (tDelayPause)           threadDelayPause          = jsonToInt(tDelayPause);
      if (tDelayUpdateProducer)  threadDelayUpdateProducer = jsonToInt(tDelayUpdateProducer);
      if (tDelayUpdateConsumer)  threadDelayUpdateConsumer = jsonToInt(tDelayUpdateConsumer);
      if (tDelayRender)          threadDelayRender         = jsonToInt(tDelayRender);
      if (tDelayIdle)            threadDelayIdle           = jsonToInt(tDelayIdle);
      if (tPriorityCapture)      threadPriorityCapture     = jsonToInt(tPriorityCapture);
      if (tPriorityUpdate)       threadPriorityUpdate      = jsonToInt(tPriorityUpdate);
      if (tPriorityRender)       threadPriorityRender      = jsonToInt(tPriorityRender);
      if (tPriorityMain)         threadPriorityMain        = jsonToInt(tPriorityMain);

      if (timerUpdate) updateTimer = jsonToDouble(timerUpdate);
      if (timerRender) renderTimer = jsonToDouble(timerRender);
   }

   cJSON* speed = cJSON_GetObjectItem(json, "speed");
   if (speed)
   {
      cJSON* low    = cJSON_GetObjectItem(speed, "low");
      cJSON* medium = cJSON_GetObjectItem(speed, "medium");
      cJSON* high   = cJSON_GetObjectItem(speed, "high");
      if(low)    speedLow    = jsonToInt(low);
      if(medium) speedMedium = jsonToInt(medium);
      if(high)   speedHigh   = jsonToInt(high);
   }

   cJSON* history = cJSON_GetObjectItem(json,"history");
   if( history )
   {
      cJSON* frameCount = cJSON_GetObjectItem(history,"frameCount");
      if( frameCount ) historyFrameCount = jsonToInt(frameCount);

      cJSON* frameDisplay = cJSON_GetObjectItem(history, "frameDisplay");
      if (frameDisplay) historyFrameDisplay = jsonToInt(frameDisplay);

      cJSON* frameLoadSave = cJSON_GetObjectItem(history, "frameLoadSave");
      if (frameLoadSave) historyFrameLoadSave = jsonToInt(frameLoadSave);

      cJSON* frameClipboard = cJSON_GetObjectItem(history, "frameClipboard");
      if (frameClipboard) historyFrameClipboard = jsonToInt(frameClipboard);
   }

   cJSON* window = cJSON_GetObjectItem(json,"window");
   if(window)
   {
      cJSON* debugWindow = cJSON_GetObjectItem(window,"debug");
      cJSON* dw = cJSON_GetObjectItem(window,"displayWidth");
      cJSON* dh = cJSON_GetObjectItem(window,"displayHeight");
      cJSON* cw = cJSON_GetObjectItem(window,"controlWidth");
      cJSON* ch = cJSON_GetObjectItem(window,"controlHeight");
      if( debugWindow ) debug = jsonToInt( debugWindow );
      if( dw ) displayWidth  = jsonToInt( dw );
      if( dh ) displayHeight = jsonToInt( dh );
      if( cw ) controlWidth  = jsonToInt( cw );
      if( ch ) controlHeight = jsonToInt( ch );
   }
   cJSON* update = cJSON_GetObjectItem(json,"update");
   if( update )
   {
      cJSON* time = cJSON_GetObjectItem(update,"time");
      if( time ) updateTime = (float)jsonToDouble(time);
   }

   cJSON*     generator = cJSON_GetObjectItem(json,"generator");
   if( generator )
   {
      cJSON*  fs = cJSON_GetObjectItem(generator,"fs");
      if( fs ) generatorFs = jsonToDouble(fs);
   }

   cJSON* etsJson = cJSON_GetObjectItem(json,"ets");
   if( etsJson )
   {
      cJSON*     frames = cJSON_GetObjectItem(etsJson,"frames");
      cJSON*  byteIndex = cJSON_GetObjectItem(etsJson,"byteIndex");
      if( frames )             ets = jsonToInt(frames);
      if( byteIndex ) etsByteIndex = jsonToInt(byteIndex);
   }

   cJSON*     usb   = cJSON_GetObjectItem(json,"usb");
   cJSON* ep2timeOut= cJSON_GetObjectItem(usb,"ep2TimeOut");
   cJSON* ep4timeOut= cJSON_GetObjectItem(usb,"ep4TimeOut");
   cJSON* ep6timeOut= cJSON_GetObjectItem(usb,"ep6TimeOut");
   cJSON* timeOut   = cJSON_GetObjectItem(usb,"timeOut");
   cJSON* xferSize  = cJSON_GetObjectItem(usb,"xferSize");
   cJSON* guid      = cJSON_GetObjectItem(usb,"guid");
   cJSON* vendorId  = cJSON_GetObjectItem(usb,"vendorId");
   cJSON* productId = cJSON_GetObjectItem(usb,"productId");
   cJSON* serialId  = cJSON_GetObjectItem(usb, "serialId");
   cJSON* checkSerialId  = cJSON_GetObjectItem(usb, "checkSerialId");
   cJSON* factoryVendorId = cJSON_GetObjectItem(usb,  "factoryVendorId");
   cJSON* factoryProductId = cJSON_GetObjectItem(usb, "factoryProductId");

   if(ep2timeOut) usbEp2TimeOut = jsonToInt( ep2timeOut );
   if(ep4timeOut) usbEp4TimeOut = jsonToInt( ep4timeOut );
   if(ep6timeOut) usbEp6TimeOut = jsonToInt( ep6timeOut );
   if( xferSize ) usbXferSize   = jsonToInt( xferSize );
   const char* str = jsonToString( guid );

   fillGuid(usbGuid, str);

   usbVendorId     = jsonToInt( vendorId );
   usbProductId    = jsonToInt( productId );
   usbSerialId     = jsonToInt( serialId );
   usbCheckSerialId    = jsonToInt( checkSerialId );
   usbFactoryVendorId  = jsonToInt(factoryVendorId);
   usbFactoryProductId = jsonToInt(factoryProductId);

   cJSON* measure = cJSON_GetObjectItem(json, "measure");
   if (measure)
   {
      cJSON* offset = cJSON_GetObjectItem(measure, "middleOffset");
      middleOffset  = jsonToDouble(offset);
   }

   cJSON* jsonDigital = cJSON_GetObjectItem(json, "digital");
   if( jsonDigital )
   {
      cJSON* coeficient = cJSON_GetObjectItem(jsonDigital, "voltageCoeficient");
      if( coeficient )
      {
         digitalVoltageCoeficient = jsonToDouble(coeficient);
      }
   }

   cJSON* jsonCallibration = cJSON_GetObjectItem(json, "callibrate");
   if (jsonCallibration)
   {
      cJSON* invCh0 = cJSON_GetObjectItem(jsonCallibration, "invertCh0");
      cJSON* invCh1 = cJSON_GetObjectItem(jsonCallibration, "invertCh1");
      cJSON* frames = cJSON_GetObjectItem(jsonCallibration, "framesPerCapture");
      cJSON* maxIt = cJSON_GetObjectItem(jsonCallibration, "gainMaxIterations");
      cJSON* exitPerc = cJSON_GetObjectItem(jsonCallibration, "gainExitPercentage");
      cJSON* channelRef = cJSON_GetObjectItem(jsonCallibration, "analogChannelReference");
      cJSON* generatorRef = cJSON_GetObjectItem(jsonCallibration, "generatorReference");
      cJSON* generatorStep = cJSON_GetObjectItem(jsonCallibration, "generatorStep");

      if (invCh0)        invertCh[0] = jsonToInt(invCh0);
      if (invCh1)        invertCh[1] = jsonToInt(invCh1);
      if (frames)        framesPerCapture = jsonToInt(frames);
      if (maxIt)         gainMaxIterations = jsonToInt(maxIt);
      if (exitPerc)      gainExitPercentage = jsonToDouble(exitPerc);
      if (channelRef)    analogChannelReference = jsonToInt(channelRef);
      if (generatorRef)  generatorReference = jsonToInt(generatorRef);
      if (generatorStep) generatorVoltagePerStep = jsonToDouble(generatorStep);

      cJSON* jsonGainRefValue = cJSON_GetObjectItem(jsonCallibration, "gainRefValue");
      VoltageFromJson(jsonGainRefValue, (ushort*)gainRefValue);

      cJSON* jsonGainRefVoltage = cJSON_GetObjectItem(jsonCallibration, "gainRefVoltage");
      VoltageFromJson(jsonGainRefVoltage, (double*)gainRefVoltage);

      // calculate gainGenOffset from generatorVoltageStep
      {
         gainGenOffset[vc2Volt]   = gainRefVoltage[vc2Volt] / generatorVoltagePerStep;
         gainGenOffset[vc1Volt]   = gainRefVoltage[vc1Volt] / generatorVoltagePerStep;
         gainGenOffset[vc500Mili] = gainRefVoltage[vc500Mili] / generatorVoltagePerStep;
         gainGenOffset[vc200Mili] = gainRefVoltage[vc200Mili] / generatorVoltagePerStep;
         gainGenOffset[vc100Mili] = gainRefVoltage[vc100Mili] / generatorVoltagePerStep;
         gainGenOffset[vc50Mili]  = gainRefVoltage[vc50Mili] / generatorVoltagePerStep;
         gainGenOffset[vc20Mili]  = gainRefVoltage[vc20Mili] / generatorVoltagePerStep;
         gainGenOffset[vc10Mili]  = gainRefVoltage[vc10Mili] / generatorVoltagePerStep;
      }


      cJSON* gMin = cJSON_GetObjectItem(jsonCallibration, "gainMin");
      VoltageFromJson(gMin, (ushort*)gainMin);


      cJSON* gMax = cJSON_GetObjectItem(jsonCallibration, "gainMax");
      VoltageFromJson(gMax, (ushort*)gainMax);

      cJSON* gAttr = cJSON_GetObjectItem(jsonCallibration, "gainAttr");
      VoltageFromJson(gAttr, (ushort*)gainAttr);
   }


   FORMAT_PATH("data/firmware/fx2.bin");
   fx2FileName  = formatBuffer;
   FORMAT_PATH("data/firmware/fpga.bin");
   fpgaFileName = formatBuffer;

   cJSON_Delete(json);

   loadCallibrate();
}

void OscSettings::loadCallibrate()
{
   FORMAT_BUFFER();
   FORMAT_PATH("data/startup/callibrate.json");

   SDL_RWops* ctx = SDL_RWFromFile(formatBuffer, "rb");
   if (!ctx)
   {
      return;
   }
   SDL_RWclose(ctx);

   char*  memory = 0;
   ilarge memorySize = 0;
   fileLoadString(formatBuffer, &memory, &memorySize);

   json = cJSON_Parse((const char*)memory);
   if (!json)
   {
      CORE_MESSAGE("callibrate.json error before: [%.256s]\n", cJSON_GetErrorPtr());
   }

   fileFree(memory);

   // callibrate
   cJSON* jsonAnalog = cJSON_GetObjectItem(json, "callibrate");
   if (jsonAnalog)
   {
      for (int i = 0; i<2; i++)
      {
         FORMAT("offsetsChannel%d", i);
         cJSON* jsonOffsetsChannel = cJSON_GetObjectItem(jsonAnalog, formatBuffer);
         VoltageFromJson(jsonOffsetsChannel, (int*)analogChannelOffsets[i]);
      }

      for (int i = 0; i<2; i++)
      {
         FORMAT("voltageStepChannel%d", i);
         cJSON* jsonStep = cJSON_GetObjectItem(jsonAnalog, formatBuffer);
         VoltageFromJson(jsonStep, analogChannelYVoltageStep[i]);
      }


      cJSON* jsonGenerator = cJSON_GetObjectItem(jsonAnalog, "generatorOffsets");
      if (jsonGenerator)
      {
         cJSON* ch0 = cJSON_GetObjectItem(jsonGenerator, "channel0");
         cJSON* ch1 = cJSON_GetObjectItem(jsonGenerator, "channel1");

         if (ch0) generatorOffset[0] = jsonToInt(ch0);
         if (ch1) generatorOffset[1] = jsonToInt(ch1);
      }

      for (int i = 0; i < 2; i++)
      {
         FORMAT("gainValue%d", i);
         cJSON* jsonGainValue = cJSON_GetObjectItem(jsonAnalog, formatBuffer);
         VoltageFromJson(jsonGainValue, (ushort*)gainValue[i]);
      }

      cJSON* jsonGainAttr = cJSON_GetObjectItem(jsonAnalog, "gainAttr");
      VoltageFromJson(jsonGainAttr, (ushort*)gainAttr);
   }
}

void OscSettings::save()
{
   cJSON* jsonRoot = cJSON_CreateObject();

   cJSON* jsonRender = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "render", jsonRender );
   cJSON_AddItemToObject(jsonRender, "shaders21", cJSON_CreateNumber(this->shaders21));
   cJSON_AddItemToObject(jsonRender, "vertexBufferSizeMegaByte", cJSON_CreateNumber(this->vertexBufferSizeMegaByte));
   cJSON_AddItemToObject(jsonRender, "depthBuffer", cJSON_CreateNumber(this->depthBuffer));
   cJSON_AddItemToObject(jsonRender, "fps", cJSON_CreateNumber(this->fps));
   cJSON_AddItemToObject(jsonRender, "threadCount",    cJSON_CreateNumber(this->threadCount));
   cJSON_AddItemToObject(jsonRender, "threadBuffer",   cJSON_CreateNumber(this->threadBuffer));
   cJSON_AddItemToObject(jsonRender, "threadDelayCaptureProducer",  cJSON_CreateNumber(this->threadDelayCaptureProducer));
   cJSON_AddItemToObject(jsonRender, "threadDelayPause",    cJSON_CreateNumber(this->threadDelayPause));
   cJSON_AddItemToObject(jsonRender, "threadDelayUpdateProducer",  cJSON_CreateNumber(this->threadDelayUpdateProducer));
   cJSON_AddItemToObject(jsonRender, "threadDelayUpdateConsumer",  cJSON_CreateNumber(this->threadDelayUpdateConsumer));
   cJSON_AddItemToObject(jsonRender, "threadDelayRender",   cJSON_CreateNumber(this->threadDelayRender));
   cJSON_AddItemToObject(jsonRender, "threadDelayIdle", cJSON_CreateNumber(this->threadDelayIdle));
   cJSON_AddItemToObject(jsonRender, "threadPriorityCapture", cJSON_CreateNumber(this->threadPriorityCapture));
   cJSON_AddItemToObject(jsonRender, "threadPriorityUpdate", cJSON_CreateNumber(this->threadPriorityUpdate));
   cJSON_AddItemToObject(jsonRender, "threadPriorityRender", cJSON_CreateNumber(this->threadPriorityRender));
   cJSON_AddItemToObject(jsonRender, "threadPriorityMain", cJSON_CreateNumber(this->threadPriorityMain));
   cJSON_AddItemToObject(jsonRender, "updateTimer", cJSON_CreateNumber(this->updateTimer));
   cJSON_AddItemToObject(jsonRender, "renderTimer", cJSON_CreateNumber(this->renderTimer));

   // speed
   cJSON* jsonSpeed = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "speed", jsonSpeed );
   cJSON_AddItemToObject(jsonSpeed, "low",    cJSON_CreateNumber(this->speedLow));
   cJSON_AddItemToObject(jsonSpeed, "medium", cJSON_CreateNumber(this->speedMedium));
   cJSON_AddItemToObject(jsonSpeed, "high",   cJSON_CreateNumber(this->speedHigh));

   // history
   cJSON* jsonHistory = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "history", jsonHistory);
   cJSON_AddItemToObject(jsonHistory, "frameCount",     cJSON_CreateNumber(this->historyFrameCount));
   cJSON_AddItemToObject(jsonHistory, "frameDisplay",   cJSON_CreateNumber(this->historyFrameDisplay));
   cJSON_AddItemToObject(jsonHistory, "frameLoadSave",  cJSON_CreateNumber(this->historyFrameLoadSave));
   cJSON_AddItemToObject(jsonHistory, "frameClipboard", cJSON_CreateNumber(this->historyFrameClipboard));


   // window
   cJSON* jsonWindow = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "window", jsonWindow);
   cJSON_AddItemToObject(jsonWindow, "debug", cJSON_CreateNumber(this->debug));
   cJSON_AddItemToObject(jsonWindow, "displayWidth", cJSON_CreateNumber(this->displayWidth));
   cJSON_AddItemToObject(jsonWindow, "displayHeight", cJSON_CreateNumber(this->displayHeight));
   cJSON_AddItemToObject(jsonWindow, "controlWidth", cJSON_CreateNumber(this->controlWidth));
   cJSON_AddItemToObject(jsonWindow, "controlHeight", cJSON_CreateNumber(this->controlHeight));


   // update
   cJSON* jsonUpdate = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "update", jsonUpdate);
   cJSON_AddItemToObject(jsonUpdate, "time", cJSON_CreateNumber(this->updateTime));


   // generator
   cJSON* jsonGenerator = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "generator", jsonGenerator);
   cJSON_AddItemToObject(jsonGenerator, "fs", cJSON_CreateNumber(this->generatorFs));


   // ets
   cJSON* jsonETS = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "ets", jsonETS);
   cJSON_AddItemToObject(jsonETS, "frames", cJSON_CreateNumber(this->ets));
   cJSON_AddItemToObject(jsonETS, "byteIndex", cJSON_CreateNumber(this->etsByteIndex));

   // usb
   cJSON* jsonUSB = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "usb", jsonUSB);
   cJSON_AddItemToObject(jsonUSB, "ep2TimeOut",  cJSON_CreateNumber(this->usbEp2TimeOut));
   cJSON_AddItemToObject(jsonUSB, "ep4TimeOut",  cJSON_CreateNumber(this->usbEp4TimeOut));
   cJSON_AddItemToObject(jsonUSB, "ep6TimeOut",  cJSON_CreateNumber(this->usbEp6TimeOut));
   cJSON_AddItemToObject(jsonUSB, "xferSize",    cJSON_CreateNumber(this->usbXferSize));
   cJSON_AddItemToObject(jsonUSB, "guid",        cJSON_CreateString(this->usbGuid.asString().asChar()));
   cJSON_AddItemToObject(jsonUSB, "vendorId",    cJSON_CreateNumber(this->usbVendorId));
   cJSON_AddItemToObject(jsonUSB, "productId",   cJSON_CreateNumber(this->usbProductId));
   cJSON_AddItemToObject(jsonUSB, "serialId",         cJSON_CreateNumber(this->usbSerialId));
   cJSON_AddItemToObject(jsonUSB, "checkSerialId",    cJSON_CreateNumber(this->usbCheckSerialId));
   cJSON_AddItemToObject(jsonUSB, "factoryVendorId",  cJSON_CreateNumber(this->usbFactoryVendorId));
   cJSON_AddItemToObject(jsonUSB, "factoryProductId", cJSON_CreateNumber(this->usbFactoryProductId));

   // measure
   cJSON* jsonMeasure = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "measure", jsonMeasure);
   cJSON_AddItemToObject(jsonUSB, "middleOffset", cJSON_CreateNumber(this->middleOffset));

   // digital
   cJSON* jsonDigital = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "digital", jsonDigital);
   cJSON_AddItemToObject(jsonDigital, "voltageCoeficient", cJSON_CreateNumber(this->digitalVoltageCoeficient));

   // callibrate
   cJSON* jsonCallibrate = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "callibrate", jsonCallibrate);
   cJSON_AddItemToObject(jsonCallibrate, "invertCh0", cJSON_CreateNumber(this->invertCh[0]));
   cJSON_AddItemToObject(jsonCallibrate, "invertCh1", cJSON_CreateNumber(this->invertCh[1]));
   cJSON_AddItemToObject(jsonCallibrate, "framesPerCapture", cJSON_CreateNumber(this->framesPerCapture));
   cJSON_AddItemToObject(jsonCallibrate, "gainMaxIterations", cJSON_CreateNumber(this->gainMaxIterations));
   cJSON_AddItemToObject(jsonCallibrate, "gainExitPercentage", cJSON_CreateNumber(this->gainExitPercentage));
   cJSON_AddItemToObject(jsonCallibrate, "analogChannelReference", cJSON_CreateNumber(this->analogChannelReference));
   cJSON_AddItemToObject(jsonCallibrate, "generatorReference", cJSON_CreateNumber(this->generatorReference));
   cJSON_AddItemToObject(jsonCallibrate, "generatorStep", cJSON_CreateNumber(this->generatorVoltagePerStep));

   {
      cJSON* jsonGainRefValue = cJSON_CreateObject();
      cJSON_AddItemToObject(jsonCallibrate, "gainRefValue", jsonGainRefValue);
      VoltageToJson(jsonGainRefValue, (ushort*)gainRefValue);
   }

   {
      cJSON* jsonGainRefVoltage = cJSON_CreateObject();
      cJSON_AddItemToObject(jsonCallibrate, "gainRefVoltage", jsonGainRefVoltage);
      VoltageToJson(jsonGainRefVoltage, (double*)gainRefVoltage);
   }

   {
      cJSON* jsonGainMin = cJSON_CreateObject();
      cJSON_AddItemToObject(jsonCallibrate, "gainMin", jsonGainMin);
      VoltageToJson(jsonGainMin, (ushort*)gainMin);
   }

   {
      cJSON* jsonGainMax = cJSON_CreateObject();
      cJSON_AddItemToObject(jsonCallibrate, "gainMax", jsonGainMax);
      VoltageToJson(jsonGainMax, (ushort*)gainMax);
   }

   // gainAttr
   cJSON* jsonGainAttr = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonCallibrate, "gainAttr", jsonGainAttr);
   VoltageToJson(jsonGainAttr, (ushort*)gainAttr);

   char* jsonString = cJSON_Print(jsonRoot);
   if (!jsonString)
   {
      CORE_MESSAGE("settings.json error before: [%.256s]\n", cJSON_GetErrorPtr());
   }

   FORMAT_BUFFER();
   FORMAT_PATH("data/startup/settings.json");
   fileSaveString(formatBuffer, jsonString);

   cJSON_Delete(jsonRoot);

   memoryFree(jsonString);
}


void OscSettings::saveCallibrate()
{
   cJSON* jsonRoot = cJSON_CreateObject();

   // analog
   cJSON* jsonAnalog = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonRoot, "callibrate", jsonAnalog);

   // offsetsChannel0
   cJSON* jsonOffsetChannel0 = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonAnalog, "offsetsChannel0", jsonOffsetChannel0);
   VoltageToJson(jsonOffsetChannel0, (int*)analogChannelOffsets[0]);


   // offsetsChannel1
   cJSON* jsonOffsetChannel1 = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonAnalog, "offsetsChannel1", jsonOffsetChannel1);
   VoltageToJson(jsonOffsetChannel1, (int*)analogChannelOffsets[1]);

   // stepChanne0
   cJSON* jsonStep0 = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonAnalog, "voltageStepChannel0", jsonStep0);
   VoltageToJson(jsonStep0, analogChannelYVoltageStep[0]);

   // stepChanne1
   cJSON* jsonStep1 = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonAnalog, "voltageStepChannel1", jsonStep1);
   VoltageToJson(jsonStep1, analogChannelYVoltageStep[1]);

   // generatorOffsets
   cJSON* jsonGeneratorOffsets = cJSON_CreateObject();
   cJSON_AddItemToObject(jsonAnalog, "generatorOffsets", jsonGeneratorOffsets);
   cJSON_AddItemToObject(jsonGeneratorOffsets, "channel0", cJSON_CreateNumber(this->generatorOffset[0]));
   cJSON_AddItemToObject(jsonGeneratorOffsets, "channel1", cJSON_CreateNumber(this->generatorOffset[1]));

   // gainValue0
   {
      cJSON* jsonGainVolt = cJSON_CreateObject();
      cJSON_AddItemToObject(jsonAnalog, "gainValue0", jsonGainVolt);
      VoltageToJson(jsonGainVolt, (ushort*)gainValue[0]);
   }

   // gainValue1
   {
      cJSON* jsonGainVolt = cJSON_CreateObject();
      cJSON_AddItemToObject(jsonAnalog, "gainValue1", jsonGainVolt);
      VoltageToJson(jsonGainVolt, (ushort*)gainValue[1]);
   }


   char* jsonString = cJSON_Print(jsonRoot);
   if (!jsonString)
   {
      CORE_MESSAGE("settings.json error before: [%.256s]\n", cJSON_GetErrorPtr());
   }

   FORMAT_BUFFER();
   FORMAT_PATH("data/startup/callibrate.json");
   fileSaveString(formatBuffer, jsonString);

   cJSON_Delete(jsonRoot);

   memoryFree(jsonString);
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
