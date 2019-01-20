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


void largeToJson(cJSON* json,const char* name, ularge large )
{
   FORMAT_BUFFER();

   uint low  = large & 0xffffffff;
   uint high = (large >> 32) & 0xffffffff;

   FORMAT("%sLow", name);
   cJSON_AddNumberToObject(json, formatBuffer, low );

   FORMAT("%sHigh",name);
   cJSON_AddNumberToObject(json, formatBuffer, high );
}

void jsonToLarge(cJSON* json, const char* name, ularge& large )
{
   FORMAT_BUFFER();

   FORMAT("%sLow", name);
   uint low  = cJSON_GetObjectItem(json, formatBuffer)->valueint;

   FORMAT("%sHigh", name);
   uint high = cJSON_GetObjectItem(json, formatBuffer)->valueint;

   large = low | ( ularge(high) << 32);
}

////////////////////////////////////////////////////////////////////////////////
//
// Save
//
////////////////////////////////////////////////////////////////////////////////
int _saveToFile(void *param)
{
   OscFileThread* fileThread = (OscFileThread*)param;

   /////////////////////////////////////////////////////////
   // signal
   /////////////////////////////////////////////////////////
   Ring<OsciloscopeFrame> rHistory = pOsciloscope->threadHistory;

   /////////////////////////////////////////////////////////
   // root
   /////////////////////////////////////////////////////////
   cJSON* root = cJSON_CreateObject();

   /////////////////////////////////////////////////////////
   // Header
   /////////////////////////////////////////////////////////
   cJSON* header = cJSON_CreateObject();
   cJSON_AddItemToObject( root,"header",header);
   cJSON_AddStringToObject( header, "magic", "osciloscope" );
   cJSON_AddNumberToObject( header, "version", 1 );
   cJSON_AddNumberToObject(header, "frames", rHistory.getCount());

   FORMAT_BUFFER();
   /////////////////////////////////////////////////////////
   // history
   /////////////////////////////////////////////////////////
   int count = pOsciloscope->settings.historyFrameLoadSave;
   rHistory.offset( (rHistory.getEnd() - count)%rHistory.getSize() );
   for(int i=0;i<count;i++)
   {
      pOsciloscope->window.progress.uiActive = 1;
      pOsciloscope->window.progress.uiRange  = count;
      pOsciloscope->window.progress.uiValue  = int( ( float(i+1)/float(count-1) )*float(count-1) );

      OsciloscopeFrame frame;
      rHistory.read( frame );

      cJSON* jsonFrame = cJSON_CreateObject();
      cJSON_AddItemToObject( root, "frame", jsonFrame );
      cJSON_AddNumberToObject( jsonFrame, "id", i );
      cJSON_AddNumberToObject( jsonFrame, "size", NUM_SAMPLES );
      cJSON_AddNumberToObject( jsonFrame, "time", frame.captureTime );
      cJSON_AddNumberToObject( jsonFrame, "ets", frame.ets );

      largeToJson( jsonFrame, "utc",         frame.utc );
      largeToJson( jsonFrame, "firstFrame",  frame.firstFrame );
      largeToJson( jsonFrame, "thisFrame",   frame.thisFrame );
      largeToJson( jsonFrame, "triggerTime", frame.triggerTime );

      frame.getTime(formatBuffer, FORMAT_BUFFER_SIZE);
      cJSON_AddStringToObject(jsonFrame, "displayTime", formatBuffer);

      /////////////////////////////////////////////////////////
      // analog
      /////////////////////////////////////////////////////////
      for(int j=0;j<2;j++)
      {
         if (!frame.analog[j].getCount()) continue;

         int channelArray[NUM_SAMPLES] = {0};
         for (int k = 0; k<frame.analog[j].getCount(); k++)
         {
            channelArray[k] = frame.analog[j][k];
         }
         cJSON* arrayValues = cJSON_CreateIntArray(channelArray, frame.analog[j].getCount());

         FORMAT("analogChannel%d",j);
         cJSON_AddItemToObject( jsonFrame, formatBuffer, arrayValues );
      }

      /////////////////////////////////////////////////////////
      // digital
      /////////////////////////////////////////////////////////
      if (!frame.digital.getCount()) continue;
      for(int j=0;j<16;j++)
      {
         int digitalArray[NUM_SAMPLES] = {0};
         for (int k = 0; k<frame.digital.getCount(); k++)
         {
            digitalArray[k] = frame.getDigital(j, k);
         }
         cJSON* arrayValues = cJSON_CreateIntArray(digitalArray, frame.digital.getCount());

         FORMAT("digitalChannel%d",j);
         cJSON_AddItemToObject( jsonFrame, formatBuffer, arrayValues );
      }
   }

   char* text = cJSON_Print(root);
   fileSaveString(fileThread->file.asChar(), text);
   memoryFree( text );

   cJSON_Delete(root);

   pOsciloscope->window.progress.uiActive = 1;
   pOsciloscope->window.progress.uiRange  = 100;
   pOsciloscope->window.progress.uiValue  = 0;

   SDL_AtomicSet(&fileThread->atomic,0);

   return 0;
}

int OsciloscopeManager::saveToFile(const char *file)
{
   fileThread.file = file;
   if( fileThread.file.posReverse( ".osc" ) > 0 )
   {
      SDL_AtomicSet(&fileThread.atomic,1);
      fileThread.thread = SDL_CreateThread( _saveToFile, "OscFileSave", &fileThread );
      // _saveToFile( &fileThread );
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Load
//
////////////////////////////////////////////////////////////////////////////////
int _loadFromFile(void *param)
{
   OscFileThread* fileThread = (OscFileThread*)param;

   /////////////////////////////////////////////////////////
   // signal
   /////////////////////////////////////////////////////////
   Ring<OsciloscopeFrame> &rHistory = pOsciloscope->threadHistory;

   /////////////////////////////////////////////////////////
   // read
   /////////////////////////////////////////////////////////
   char*  memory     = 0;
   ilarge memorySize = 0;
   fileLoadString(fileThread->file.asChar(), &memory, &memorySize);

   /////////////////////////////////////////////////////////
   // parse
   /////////////////////////////////////////////////////////
   cJSON* root = cJSON_Parse( memory );
   fileFree(memory);

   cJSON*        header = cJSON_GetObjectItem(root,"header");
   uint         frames  = cJSON_GetObjectItem(header,"frames")->valueint;

   frames = clamp<uint>( frames, 0, pOsciloscope->settings.historyFrameLoadSave);

   //////////////////////////////////////////////////////////
   // history
   //////////////////////////////////////////////////////////
   FORMAT_BUFFER();
   cJSON* jsonFrame = header->next;
   rHistory.clear();
   for(uint i=0;i<frames;i++)
   {
      pOsciloscope->window.progress.uiActive = 1;
      pOsciloscope->window.progress.uiRange  = frames;
      pOsciloscope->window.progress.uiValue  = int( ( float(i+1)/float(frames-1) )*float(frames-1) );
      //
      OsciloscopeFrame frame;

      uint   id   = cJSON_GetObjectItem(jsonFrame,"id")->valueint;
      uint   size = cJSON_GetObjectItem(jsonFrame,"size")->valueint;
      double time = cJSON_GetObjectItem(jsonFrame,"time")->valuedouble;
      uint    ets = cJSON_GetObjectItem(jsonFrame,"ets")->valueint;

      jsonToLarge(jsonFrame, "utc", (ularge&)frame.utc);
      jsonToLarge(jsonFrame, "firstFrame", frame.firstFrame);
      jsonToLarge(jsonFrame, "thisFrame", frame.thisFrame);
      jsonToLarge(jsonFrame, "triggerTime", frame.triggerTime);

      if( id != i || size != NUM_SAMPLES )
      {
         CORE_MESSAGE("Error reading file %s",fileThread->file.asChar() );
         memoryFree(memory);
         return 1;
      }

      frame.ets         = ets;

      frame.captureTime = time;

      //////////////////////////////////////////////////////////
      // analog
      //////////////////////////////////////////////////////////
      for(int j=0;j<2;j++)
      {
         FORMAT("analogChannel%d",j);
         cJSON* arrayValues = cJSON_GetObjectItem(jsonFrame,formatBuffer);
         cJSON* element = arrayValues->child;
         for(uint k=0;k<size;k++)
         {
            if (element)
            {
               ishort value = element->valueint;
               frame.analog[j].pushBack(value);
               element = element->next;
            }
         }
      }
      //////////////////////////////////////////////////////////
      // digital
      //////////////////////////////////////////////////////////
      frame.digital.setCount( size );
      for(int j=0;j<16;j++)
      {
         FORMAT("digitalChannel%d",j);
         cJSON* arrayValues = cJSON_GetObjectItem(jsonFrame,formatBuffer);
         cJSON* element = arrayValues->child;
         for(uint k=0;k<size;k++)
         {
            if (element)
            {
               ishort bit = element->valueint;
               frame.digital[k] = frame.digital[k] | (bit << j);
               element = element->next;
            }
         }
      }

      // write frame
      rHistory.write( frame );
      //
      jsonFrame = jsonFrame->next;
   }

   cJSON_Delete(root);

   pOsciloscope->window.progress.uiActive = 1;
   pOsciloscope->window.progress.uiRange  = 100;
   pOsciloscope->window.progress.uiValue  = 0;

   SDL_AtomicSet(&fileThread->atomic,0);

   return 0;
}

int OsciloscopeManager::loadFromFile(const char *file)
{
   fileThread.file = file;
   if( fileThread.file.posReverse( ".osc" ) > 0 )
   {
      SDL_AtomicSet(&fileThread.atomic,1);
      fileThread.thread = SDL_CreateThread( _loadFromFile, "OscFileLoad", &fileThread );
      // _loadFromFile( &fileThread );
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
