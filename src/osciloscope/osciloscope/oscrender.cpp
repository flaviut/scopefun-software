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

float dBPower[] = { 0.1f,
                    0.01f,
                    0.001f,
                    0.0001f,
                    0.00001f,
                    0.000001f,
                    0.0000001f,
                    0.00000001f,
                    0.000000001f,
                    0.0000000001f,
                    0.00000000001f,
                    0.000000000001f };

float logFactors[] = {
	-0.5f,
	- 0.418617887f,
	- 0.311036585f,
	- 0.229654472f,
	- 0.148272358f,
	- 0.040691057f,
	0.040691057f,
	0.12207317f,
	0.229654472f,
	0.311036585f,
	0.392418698f,
	0.5f
};

float logFactorsGrid[] = {
	 -0.5f,
	 -0.418617887f,
	 -0.371012402f,
	 -0.337235774f,
	 -0.311036585f,
	 -0.289630289f,
	 -0.271531524f,
	 -0.25585366f,
	 -0.242024805f,
	 -0.229654472f,
	 -0.148272358f,
	 -0.100666874f,
	 -0.066890245f,
	 -0.040691057f,
	 -0.019284761f,
	 -0.001185996f,
	 0.014491868f,
	 0.028320724f,
	 0.040691057f,
	 0.12207317f,
	 0.169678654f,
	 0.203455283f,
	 0.229654472f,
	 0.251060768f,
	 0.269159533f,
	 0.284837396f,
	 0.298666252f,
	 0.311036585f,
	 0.392418698f,
	 0.440024183f,
	 0.473800811f,
	 0.5
};


////////////////////////////////////////////////////////////////////////////////
//
// renderAnalog
//
////////////////////////////////////////////////////////////////////////////////
float channelFunction(float ch0value, float ch1value, int function)
{
   switch (function) {
   case ANALOG_FUNCTION_MEDIUM:
      return (ch0value + ch1value) / 2;
      break;
   case ANALOG_FUNCTION_SUB_CH0_CH1:
      return ch0value - ch1value;
      break;
   case ANALOG_FUNCTION_SUB_CH1_CH0:
      return ch1value - ch0value;
      break;
   case ANALOG_FUNCTION_ADD:
      return ch0value + ch1value;
      break;
   case ANALOG_FUNCTION_MIN:
      return min(ch0value, ch1value);
      break;
   case ANALOG_FUNCTION_MAX:
      return max(ch0value, ch1value);
      break;
   }
   return 0.f;
}

/////////////////////////////////////////////////////////////////////////////
//
// osc:           x[ -0.5, 0.5 ], y[  -0.5, 0.5 ]
// fft/digital:   x[ -0.5, 0.5 ], y[  -0.5, 0.5 ]
//
/////////////////////////////////////////////////////////////////////////////


void OsciloscopeThreadRenderer::preOscRender(uint threadId, OsciloscopeThreadData& threadData)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if (wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
   {
      xMin = -0.5f;
      xMax = +0.5f;
      yMin = -0.5f;
      yMax = 0.5f;
   }
   else
   {
      xMin = render.cameraOsc.View.Pos().x - 0.5f;
      xMax = render.cameraOsc.View.Pos().x + 0.5f;
      yMin = -0.5f;
      yMax = 0.5f;
   }
}

void OsciloscopeThreadRenderer::preFftRender(uint threadId, OsciloscopeThreadData& threadData)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if (wndMain.fftDigital.is(VIEW_SELECT_FFT_3D) )
   {
      xMin = -0.5f;
      xMax = +0.5f;
      yMin = -0.5f;
      yMax = 0.5f;
   }
   else
   {
      xMin = render.cameraFFT.View.Pos().x - 0.5f;
      xMax = render.cameraFFT.View.Pos().x + 0.5f;
      yMin = -0.5f;
      yMax = 0.5f;
   }
}

void OsciloscopeThreadRenderer::renderFps(uint threadId, OsciloscopeThreadData& threadData)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if( pOsciloscope->settings.fps )
   {
      FORMAT_BUFFER();

      int y = 225;

      FORMAT("generate: %d", pTimer->getFps(TIMER_GENERATE));
      pFont->setSize(threadId,0.75f);
      pFont->writeText(threadId,  200, y, formatBuffer);

      y +=  25;

      FORMAT("capture: %d", pTimer->getFps(TIMER_CAPTURE));
      pFont->setSize(threadId, 0.75f);
      pFont->writeText(threadId, 200, y, formatBuffer);

      y +=  25;

      for (uint i = 0; i < pRender->getThreadCount();i++)
      {
         FORMAT("update_%d: %d", i, pTimer->getFps(i+TIMER_UPDATE0));
         pFont->setSize(threadId, 0.75f);
         pFont->writeText(threadId, 200, y, formatBuffer );
         y +=  25;
      }

      FORMAT("main: %d", pTimer->getFps(TIMER_MAIN));
      pFont->setSize(threadId,0.75f);
      pFont->writeText(threadId,  200, y, formatBuffer);

      y +=  25;

      FORMAT("render: %d", pTimer->getFps(TIMER_RENDER));
      pFont->setSize(threadId,0.75f);
      pFont->writeText(threadId,  200, y, formatBuffer);
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderAnalogGrid
//
////////////////////////////////////////////////////////////////////////////////
void renderCircleXY(uint threadid,int n,float r, float z,Matrix4x4 final)
{
   Vector4 center = Vector4(0, 0, 0.5, 1);
   Vector4 a,b;
   pCanvas3d->beginBatch(threadid,CANVAS3D_BATCH_LINE, n);
   for (int i = 0; i < n; i++)
   {
      float angle0 = 360.f*(float(i) / float(n))*TO_RAD;
      float angle1 = 360.f*(float(i + 1) / float(n))*TO_RAD;
      float x0 = sinf(angle0);
      float y0 = cosf(angle0);
      float x1 = sinf(angle1);
      float y1 = cosf(angle1);
      a = center;
      b = center;
      a.x += r*x0;
      a.y += r*y0;
      a.z = z;
      a.w = 1;
      b.x += r*x1;
      b.y += r*y1;
      b.z = z;
      b.w = 1;
      pCanvas3d->bLine(threadid, a,b);
   }
   pCanvas3d->endBatch(threadid, final, COLOR_ARGB(255, 255, 255, 255) );
}

void OsciloscopeThreadRenderer::renderAnalogGrid(uint threadId, OsciloscopeThreadData& threadData)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   uint xCount = 10;
   uint yCount = 10;

   ////////////////////////////////////////////////////////////////////////////////
   // GRID
   ////////////////////////////////////////////////////////////////////////////////
   if( !wndMain.display.oscGrid ) return;

   // for (float z = 0; z <= 1.f; z += 1.f )
   float z = 0.f;
   {
      ////////////////////////////////////////////////////////////////////////////////
      // x fine grid lines
      ////////////////////////////////////////////////////////////////////////////////
      float xDelta = (xMax-xMin)/float(xCount);
      float x = xMin;
      pCanvas3d->beginBatch(threadId,CANVAS3D_BATCH_LINE,int(xCount)+1);
      for(uint i=0;i<=xCount;i++)
      {
         pCanvas3d->bLine(threadId,  Vector4(x, yMin, z, 1), Vector4(x, yMax, z, 1));
         x += xDelta;
      }
      pCanvas3d->endBatch(threadId,  render.cameraOsc.Final,COLOR_ARGB(255,25,25,25));

      ////////////////////////////////////////////////////////////////////////////////
      // y grid lines
      ////////////////////////////////////////////////////////////////////////////////
      float yDelta = (yMax-yMin)/float(yCount);
      float y = yMin;
      pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE,int(yCount+1));
      for(uint i=0;i<=yCount;i++)
      {
         pCanvas3d->bLine(threadId, Vector4(xMin, y, z, 1), Vector4(xMax, y, z, 1));
         y += yDelta;
      }
      pCanvas3d->endBatch(threadId, render.cameraOsc.Final, COLOR_ARGB(255, 25, 25, 25));
   }

   /*////////////////////////////////////////////////////////////////////////////////
   // x fine grid lines
   ////////////////////////////////////////////////////////////////////////////////
   float fxMin = xMin*2.5f;
   float fxMax = xMax*2.5f;
   float fyMin = yMin*2.5f;
   float fyMax = yMax*2.5f;

   float offsetZ = 0.5f;

   float xDelta = (fxMax - fxMin) / float(grid.xCount);
   float x = fxMin;
   pCanvas3d->beginBatch(CANVAS3D_BATCH_LINE, int(grid.xCount) + 1);
   for (int i = 0; i <= grid.xCount; i++)
   {
      pCanvas3d->bLine(Vector4(x, 0, fyMin + offsetZ, 1), Vector4(x, 0, fyMax + offsetZ, 1));
      x += xDelta;
   }
   pCanvas3d->endBatch(cameraOsc.ortho.Final, COLOR_ARGB(255, 25, 25, 25));

   ////////////////////////////////////////////////////////////////////////////////
   // y grid lines
   ////////////////////////////////////////////////////////////////////////////////
   float yDelta = (fyMax - fyMin) / float(grid.yCount);
   float y = fyMin;
   pCanvas3d->beginBatch(CANVAS3D_BATCH_LINE, int(grid.yCount + 1));
   for (int i = 0; i <= grid.yCount; i++)
   {
      pCanvas3d->bLine(Vector4(fxMin, 0, y + offsetZ, 1), Vector4(fxMax, 0, y + offsetZ, 1));
      y += yDelta;
   }
   pCanvas3d->endBatch(cameraOsc.ortho.Final, COLOR_ARGB(255, 25, 25, 25));*/

   if (wndMain.fftDigital.is(VIEW_SELECT_OSC_3D) )
   {
      float size = 0.5f;
      pCanvas3d->beginBatch(threadId,CANVAS3D_BATCH_LINE, 8);
         pCanvas3d->bLine(threadId,  Vector4(size,   size, 0, 1), Vector4(size,   size, 1, 1));
         pCanvas3d->bLine(threadId,  Vector4(size,  -size, 0, 1), Vector4(size,  -size, 1, 1));
         pCanvas3d->bLine(threadId,  Vector4(-size,  size, 0, 1), Vector4(-size,  size, 1, 1));
         pCanvas3d->bLine(threadId,  Vector4(-size, -size, 0, 1), Vector4(-size, -size, 1, 1));

         pCanvas3d->bLine(threadId,  Vector4(size,  -size,  1, 1), Vector4(size,  size, 1, 1));
         pCanvas3d->bLine(threadId,  Vector4(size,   size,  1, 1), Vector4(-size,  size, 1, 1));
         pCanvas3d->bLine(threadId,  Vector4(size,  -size, 1, 1), Vector4(-size, -size, 1, 1));
         pCanvas3d->bLine(threadId,  Vector4(-size, -size, 1, 1), Vector4(-size,  size, 1, 1));
      pCanvas3d->endBatch(threadId,  render.cameraOsc.Final, COLOR_ARGB(255, 0, 0, 0));
   }

   /**
   for (int i = 0; i < 8; i++)
   {
      float r = 1.f;
      float z = float(i) / 8.f;
      renderCircleXY(8, r, z, cameraOsc.ortho.Final);
   }
   /**/

   /**
   pCanvas3d->beginBatch(CANVAS3D_BATCH_LINE, 1);
   pCanvas3d->bLine(Vector4(0, 0, 0, 1), Vector4(1, 0, 0, 1));
   pCanvas3d->endBatch(cameraOsc.ortho.Final, COLOR_ARGB(255, 255, 0, 0));

   pCanvas3d->beginBatch(CANVAS3D_BATCH_LINE, 1);
   pCanvas3d->bLine(Vector4(0, 0, 0, 1), Vector4(0, 1, 0, 1));
   pCanvas3d->endBatch(cameraOsc.ortho.Final, COLOR_ARGB(255, 0, 255, 0));

   pCanvas3d->beginBatch(CANVAS3D_BATCH_LINE, 1);
   pCanvas3d->bLine(Vector4(0, 0, 0, 1), Vector4(0, 0, 1, 1));
   pCanvas3d->endBatch(cameraOsc.ortho.Final, COLOR_ARGB(255, 0, 0, 255));*/
}

////////////////////////////////////////////////////////////////////////////////
//
// renderAnalogAxis
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderAnalogAxis(uint threadId, OsciloscopeThreadData& threadData)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   ////////////////////////////////////////////////////////////////////////////////
   // Axis
   ////////////////////////////////////////////////////////////////////////////////
   if( !wndMain.display.oscAxis ) return;

   ////////////////////////////////////////////////////////////////////////////////
   // x axis
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->beginBatch(threadId,CANVAS3D_BATCH_LINE,1);
      pCanvas3d->bLine(threadId, Vector4(xMin,0,0,1), Vector4(xMax,0,0,1) );
   pCanvas3d->endBatch(threadId, render.cameraOsc.Final,COLOR_ARGB(255,255,0,0));

   ////////////////////////////////////////////////////////////////////////////////
   // y axis
   ////////////////////////////////////////////////////////////////////////////////
   pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE,1);
      pCanvas3d->bLine(threadId, Vector4(xMin,yMin,0,1), Vector4(xMin,yMax,0,1) );
   pCanvas3d->endBatch(threadId, render.cameraOsc.Final,COLOR_ARGB(255,0,255,0));
}

////////////////////////////////////////////////////////////////////////////////
//
// renderAnalogUnits
//
////////////////////////////////////////////////////////////////////////////////
double getTriggerVoltagePerStep(WndMain& window,float yCount)
{
   float  captureVoltage = 0.f;
   if (window.trigger.Source == 0) captureVoltage = window.channel01.Capture;
   else                            captureVoltage = window.channel02.Capture;
   return captureVoltage * yCount / 1024.0;
}

double getTriggerVoltage(WndMain& window)
{
   float  captureVoltage = 0.f;
   if (window.trigger.Source == 0) captureVoltage = window.channel01.Capture;
   else                            captureVoltage = window.channel02.Capture;
   return captureVoltage;
}

void OsciloscopeThreadRenderer::renderAnalogUnits(uint threadid, OsciloscopeThreadData& threadData)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   ////////////////////////////////////////////////////////////////////////////////
   // Units
   ////////////////////////////////////////////////////////////////////////////////
   if( !wndMain.display.oscUnits ) return;

   float timeDisplay = wndMain.horizontal.Capture*10000.0f;

   ////////////////////////////////////////////////////////////////////////////////
   // voltage / time
   ////////////////////////////////////////////////////////////////////////////////
   char bufferTime[1024]    = { 0 };
   char bufferVolt0[1024]   = { 0 };
   char bufferVolt1[1024]   = { 0 };
   char bufferTrigger[1024] = { 0 };
   ToolText::Time(bufferTime, 1024,  timeDisplay/10.f );
   ToolText::Volt(bufferVolt0, 1024, wndMain.channel01.Capture );
   ToolText::Volt(bufferVolt1, 1024, wndMain.channel02.Capture );
   pFont->setSize(threadid, 0.65f);

   FORMAT_BUFFER();
   FORMAT("Time : %s / div", bufferTime);
   pFont->writeText(threadid,15, 15, formatBuffer, -1);
   FORMAT("Ch0 : %s / div", bufferVolt0);
   pFont->writeText(threadid, 200,  15, formatBuffer, COLOR_ARGB(255, 255, 255, 0)   );
   FORMAT("Ch1 : %s / div", bufferVolt1);
   pFont->writeText(threadid, 350, 15, formatBuffer, COLOR_ARGB(255,   0, 255, 255) );

   ////////////////////////////////////////////////////////////////////////////////
   // size
   ////////////////////////////////////////////////////////////////////////////////
   pFont->setSize(threadid, 0.25f);
   float charHeight = pFont->getLineHeight(threadid);

   float sizeX = 0.035f*render.oscScaleX;
   float sizeY = 0.035f*render.oscScaleY;

   float yCount = 10.f;
   float xCount = 10.f;

   ////////////////////////////////////////////////////////////////////////////////
   // trigger
   ////////////////////////////////////////////////////////////////////////////////
   float triggerVoltage = float(double(wndMain.trigger.Level+wndMain.trigger.His) * getTriggerVoltagePerStep(wndMain,yCount));

  /* FORMAT("%2.0f", window.trigger.Percent);
   pFont->writeText3d(cameraOsc.ortho.Final, xMin - 2*sizeX, yMax, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, COLOR_ARGB(255, 255, 0, 0), oscScaleX, oscScaleY);

   FORMAT("%+.3f", triggerVoltage);
   pFont->writeText3d(cameraOsc.ortho.Final, xMin - 2.5*sizeX, yMax - 6*charHeight, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, COLOR_ARGB(255, 0, 255, 0), oscScaleX, oscScaleY);*/

   float triggerY = (triggerVoltage / (getTriggerVoltage(wndMain)*(yCount/2.f)))*0.5f;

   FORMAT("Trig", bufferTrigger);
   pFont->writeText3d(threadid, render.cameraOsc.Final, xMax, triggerY+charHeight/2, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, COLOR_ARGB(255, 0, 255, 0), render.oscScaleX, render.oscScaleY);

   pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, 3);
      pCanvas3d->bLine(threadid, Vector4(xMin- sizeX,              triggerY, 0, 1), Vector4(xMin, triggerY, 0, 1));
      pCanvas3d->bLine(threadid, Vector4(xMin- sizeX /4, triggerY- sizeY /4, 0, 1), Vector4(xMin, triggerY, 0, 1));
      pCanvas3d->bLine(threadid, Vector4(xMin- sizeX /4, triggerY+ sizeY /4, 0, 1), Vector4(xMin, triggerY, 0, 1));

   pCanvas3d->endBatch(threadid, render.cameraOsc.Final, COLOR_ARGB(128, 0, 255, 0));

   ////////////////////////////////////////////////////////////////////////////////
   // 0 volt for both channels
   ////////////////////////////////////////////////////////////////////////////////
   FORMAT("%s","0V");
   uint yellow = COLOR_ARGB(255, 255, 255, 0);
   uint blue   = COLOR_ARGB(255, 0, 0, 255);

   float zeroBlue   = 0.f;
   float zeroYellow = 0.f;
   for (int i = 0; i < 2;i++)
   {
      if (i == 0 && wndMain.channel01.OscOnOff == 0) continue;
      if (i == 1 && wndMain.channel02.OscOnOff == 0) continue;

      double step = 0.f;
      if(i==0) step = render.analogChannelYVoltageStep0;
      if(i==1) step = render.analogChannelYVoltageStep1;

      float zeroOffset = 0.f;
      if (i == 0) zeroOffset = render.analogChannelPositin0 - render.analogChannelOffsets0;
      if (i == 1) zeroOffset = render.analogChannelPositin1 - render.analogChannelOffsets1;

      float zeroVoltage = zeroOffset*step;
      if (i == 0) zeroBlue   = zeroVoltage;
      if (i == 1) zeroYellow = zeroVoltage;

      float zeroY = 0.f;
      if (i == 0) zeroY = (zeroVoltage / (wndMain.channel01.Capture*xCount));
      if (i == 1) zeroY = (zeroVoltage / (wndMain.channel02.Capture*xCount));

      pFont->writeText3d(threadid, render.cameraOsc.Final, xMax, zeroY + sizeY / 2, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, i==0?yellow:blue, render.oscScaleX, render.oscScaleY);

      pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, 3);
      pCanvas3d->bLine(threadid, Vector4(xMin - sizeX,                 zeroY, 0, 1), Vector4(xMin, zeroY, 0, 1));
      pCanvas3d->bLine(threadid, Vector4(xMin - sizeX / 4, zeroY - sizeY / 4, 0, 1), Vector4(xMin, zeroY, 0, 1));
      pCanvas3d->bLine(threadid, Vector4(xMin - sizeX / 4, zeroY + sizeY / 4, 0, 1), Vector4(xMin, zeroY, 0, 1));

      if (i == 0) pCanvas3d->endBatch(threadid, render.cameraOsc.Final, yellow);
      if (i == 1) pCanvas3d->endBatch(threadid, render.cameraOsc.Final, blue);
   }

   /*FORMAT("%+.3f", zeroYellow);
   pFont->writeText3d(cameraOsc.ortho.Final, xMin - 3 * sizeX, yMax - 8 * charHeight, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, COLOR_ARGB(255, 255, 255, 0), oscScaleX, oscScaleY);

   FORMAT("%+.3f", zeroBlue);
   pFont->writeText3d(cameraOsc.ortho.Final, xMin - 3 * sizeX, yMax - 10 * charHeight, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, COLOR_ARGB(255, 0, 0, 255), oscScaleX, oscScaleY);   */

   ////////////////////////////////////////////////////////////////////////////////
   // x units
   ////////////////////////////////////////////////////////////////////////////////
   char buffer[1024]    = { 0 };
   float charYmin       = yMin-charHeight;
   float charYmax       = yMax+charHeight;
   float xDelta         = (xMax-xMin)/float(xCount);
   float preTriggerZero = (double(wndMain.trigger.Percent) / 100.0) - 0.5;
   // units
   uint lines = 0;
   for(float x=xMin;x<xMax+xDelta;x=x+2*xDelta)
   {
      lines++;
      ToolText::Time( buffer, 1024, timeDisplay*(x-preTriggerZero) );
      pFont->writeText3d(threadid, render.cameraOsc.Final, x - sizeX, charYmin, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), buffer, 0xffffffff, render.oscScaleX, render.oscScaleY);
   }
   pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, 3*lines);
   for (float x = xMin; x<xMax + xDelta; x = x +2* xDelta)
   {
      pCanvas3d->bLine(threadid, Vector4(x,             charYmin, 0, 1), Vector4(x, yMin, 0, 1));
      pCanvas3d->bLine(threadid, Vector4(x + sizeX / 4, yMin - sizeY / 4, 0, 1), Vector4(x, yMin, 0, 1));
      pCanvas3d->bLine(threadid, Vector4(x - sizeX / 4, yMin - sizeY / 4, 0, 1), Vector4(x, yMin, 0, 1));
   }
   pCanvas3d->endBatch(threadid, render.cameraOsc.Final, COLOR_ARGB(255, 255, 255, 255));


   ////////////////////////////////////////////////////////////////////////////////
   // pre-trigger
   ////////////////////////////////////////////////////////////////////////////////
   FORMAT("%s", "Trig");
   float preTrigPos = (double(wndMain.trigger.Percent) / 100.0) - 0.5;

   pFont->writeText3d(threadid, render.cameraOsc.Final, preTrigPos, yMax + sizeY, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, COLOR_ARGB(255, 255, 0, 0), render.oscScaleX, render.oscScaleY);

   pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, 3);
   pCanvas3d->bLine(threadid, Vector4(preTrigPos, yMin - sizeY, 0, 1), Vector4(preTrigPos, yMin, 0, 1));
   pCanvas3d->bLine(threadid, Vector4(preTrigPos + sizeX / 4, yMin - sizeY / 4, 0, 1), Vector4(preTrigPos, yMin, 0, 1));
   pCanvas3d->bLine(threadid, Vector4(preTrigPos - sizeX / 4, yMin - sizeY / 4, 0, 1), Vector4(preTrigPos, yMin, 0, 1));

   pCanvas3d->endBatch(threadid, render.cameraOsc.Final, COLOR_ARGB(128, 255, 0, 0));


   //// minimums
   //for(float x=xMin;x<xMax+xDelta;x=x+2*xDelta)
   //{
   //   ToolText::Time( buffer, 1024, timeDisplay * (x + 0.5 - (double(wndMain.trigger.Percent)/100.0) ) );
   //   pFont->writeText3d(threadid, render.cameraOsc.Final, x, charYmin, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), buffer, 0xffffffff, render.oscScaleX, render.oscScaleY);
   //}
   //// maximums
   //for(float x=xMin+xDelta;x<xMax+xDelta;x=x+2*xDelta)
   //{
   //   ToolText::Time( buffer, 1024, timeDisplay * (x+0.5) );
   //   pFont->writeText3d(threadid, render.cameraOsc.Final, x, charYmax, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), buffer, 0xffffffff, render.oscScaleX, render.oscScaleY);
   //}

   ////////////////////////////////////////////////////////////////////////////////
   // y units
   ////////////////////////////////////////////////////////////////////////////////
   /*float        x = xMin - 2*charHeight;
   double  ydelta = 1.0/double(grid.yCount);
   for(double y=yMin;y<=yMax;y=y+ydelta)
   {
      ToolText::Volt( buffer, 1024, voltDisplay * float(y)*10.f );
      pFont->writeText3d(cameraOsc.ortho.Final, x, float(y), 0, Vector4(1.f, 0.f, 0.f, 1.f), Vector4(0.f, 1.f, 0.f, 1.f), buffer, -1, oscScaleX, oscScaleY );
   }*/
}

////////////////////////////////////////////////////////////////////////////////
//
// renderAnalog
//
////////////////////////////////////////////////////////////////////////////////
uint colorFromYPos(Vector4 color,float y,bool abgr=false)
{
   float sr = 1.f;
   float sg = 0.f;
   float sb = 0.f;
   float sa = 1.f;

   float fr = color.x;
   float fg = color.y;
   float fb = color.z;
   float fa = color.w;

   float norm = ((y / 0.5f) + 1.f)*0.5f; // 0.5f; // [ 0, 1 ]

   float r = lerp( sr, fr, norm);
   float g = lerp( sg, fg, norm);
   float b = lerp( sb, fb, norm);

   if(abgr) return COLOR_ARGB(255, int(b*255.f), int(g*255.f), int(r*255.f) );
   else     return COLOR_ARGB(255, int(r*255.f), int(g*255.f), int(b*255.f));
}

double freqFromPosition(int index, int maxIndex, double maxHertz);

void OsciloscopeThreadRenderer::measureSignal(uint threadId, OsciloscopeThreadData& threadData, MeasureData& measure, OsciloscopeFFT& fft)
{
   WndMain&               wndMain    = threadData.window;
   OsciloscopeRenderData&  render    = threadData.render;
   OsciloscopeFrame&       frame     = threadData.frame;
   MeasureChannelData&       current = measure.history[MEASURE_HISTORY_CURRENT];

   if (!wndMain.measure.uiOpen) return;

   double   etsDelta = (1.f / double(NUM_SAMPLES)) / double(render.maxEts);
   double  etsOffset = etsDelta*double(frame.ets);
   if (!wndMain.horizontal.ETS) etsOffset = 0;

   double  preTriggerZero = (double(wndMain.trigger.Percent) / 100.0);

   int     yposition0 = wndMain.channel01.YPosition - render.analogChannelOffsets0;
   int     yposition1 = wndMain.channel02.YPosition - render.analogChannelOffsets1;
   double  ch0ZeroVolt = double(yposition0)*render.analogChannelYVoltageStep0;
   double  ch1ZeroVolt = double(yposition1)*render.analogChannelYVoltageStep1;
   double  xposition  = (wndMain.horizontal.Position / 100.0) + etsOffset - preTriggerZero;
   double  yCount   = 10.0;
   double  yGridMax = yCount / 2.0;
   double  yfactor0 = wndMain.channel01.Capture*yGridMax;
   double  yfactor1 = wndMain.channel02.Capture*yGridMax;
   double minX = min(wndMain.measure.data.pickX0.position.x, wndMain.measure.data.pickX1.position.x );
   double maxX = max(wndMain.measure.data.pickX0.position.x, wndMain.measure.data.pickX1.position.x );
   double minY = min(wndMain.measure.data.pickY0.position.y, wndMain.measure.data.pickY1.position.y );
   double maxY = max(wndMain.measure.data.pickY0.position.y, wndMain.measure.data.pickY1.position.y );

   double  capture   = wndMain.horizontal.Capture;
   double  frameSize = wndMain.horizontal.FrameSize;
   double  maxTime   = captureTimeFromEnum(tc1s)*double(NUM_SAMPLES);
   wndMain.measure.data.pick.row[Ch0YV0] = maxTime;
   wndMain.measure.data.pick.row[Ch1YV0] = maxTime;
   wndMain.measure.data.pick.row[FunYV0] = maxTime;
   wndMain.measure.data.pick.row[Ch0YV1] = maxTime;
   wndMain.measure.data.pick.row[Ch1YV1] = maxTime;
   wndMain.measure.data.pick.row[FunYV1] = maxTime;
   current.row[Ch0Vmin]   = +yfactor0;
   current.row[Ch0Vmax]   = -yfactor0;
   current.row[Ch1Vmin]   = +yfactor1;
   current.row[Ch1Vmax]   = -yfactor1;
   current.row[FunVmin]   = max(-yfactor0, +yfactor1);
   current.row[FunVmax]   = min(-yfactor0, +yfactor1);
   current.row[Ch0VrmsDc] = 0;
   current.row[Ch0VrmsAc] = 0;
   current.row[Ch1VrmsDc] = 0;
   current.row[Ch1VrmsAc] = 0;
   current.row[FunVrmsDc] = 0;
   current.row[FunVrmsAc] = 0;
   current.row[Ch0Surface] = 0;
   current.row[Ch1Surface] = 0;
   current.row[FunSurface] = 0;

   double y0Avg = 0;
   double y1Avg = 0;
   double yFAvg = 0;

   int    y0MinFirst = 0;
   int    y0MaxFirst = 0;
   double y0MinTime = 0;
   double y0MaxTime = 0;
   double y0Min = +yfactor0;
   double y0Max = -yfactor0;

   int    y1MinFirst = 0;
   int    y1MaxFirst = 0;
   double y1MinTime = 0;
   double y1MaxTime = 0;
   double y1Min = +yfactor0;
   double y1Max = -yfactor0;

   int    yFMinFirst = 0;
   int    yFMaxFirst = 0;
   double yFMinTime = 0;
   double yFMaxTime = 0;
   double yFMin = +yfactor0;
   double yFMax = -yfactor0;


   double y0Middle = 0;
   double y1Middle = 0;
   double yFMiddle = 0;
   for (uint pt = 0; pt < NUM_SAMPLES; pt++)
   {
      uint idx = clamp<int>(pt, 0, NUM_SAMPLES - 1);

      if (idx < (uint)frame.attr.getCount() && frame.attr[idx] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

      double   yPosCh0 = frame.getAnalogDouble(0, idx);
      double   yPosCh1 = frame.getAnalogDouble(1, idx);
      double   yPosChF = channelFunction(yPosCh0, yPosCh1, wndMain.function.Type);

      double   y0 = yPosCh0*yfactor0 - ch0ZeroVolt;
      double   y1 = yPosCh1*yfactor1 - ch1ZeroVolt;
      double   yF = channelFunction(y0, y1, wndMain.function.Type);

      double xPos = (double(pt) / double(NUM_SAMPLES));
      double    x = xPos + xposition;
      double time = x*capture*frameSize;

      // avg
      y0Avg += y0;
      y1Avg += y1;
      yFAvg += yF;

      // recalc
      y0 = yPosCh0*yfactor0;
      y1 = yPosCh1*yfactor1;
      yF = channelFunction(y0, y1, wndMain.function.Type);

      // min
      if (y0 < y0Min) y0Min = y0;
      if (y1 < y1Min) y1Min = y1;
      if (yF < yFMin) yFMin = yF;

      // max
      if (y0 > y0Max) y0Max = y0;
      if (y1 > y1Max) y1Max = y1;
      if (yF > yFMax) yFMax = yF;
   }

   y0Avg /= frameSize;
   y1Avg /= frameSize;
   yFAvg /= frameSize;

   y0Middle = y0Min + (y0Max-y0Min)/2;
   y1Middle = y1Min + (y1Max-y1Min)/2;
   yFMiddle = yFMin + (yFMax-yFMin)/2;

   double sumTime0 = 0;
   double sumTime1 = 0;
   double sumTimeF = 0;

   int previous0 = 0;
   int previous1 = 0;
   int previousF = 0;

   double previousTime = 0;

   double positivePulseWidth = 0;
   double negativePulseWidth = 0;
   Array<double, NUM_SAMPLES> time0;
   Array<double, NUM_SAMPLES> time1;
   Array<double, NUM_SAMPLES> timeF;
   double offset0 = 0.025*yfactor0;
   double offset1 = 0.025*yfactor1;
   double offsetF = 0.025*channelFunction(offset0, offset1, wndMain.function.Type);
   for (uint pt = 0; pt < NUM_SAMPLES; pt++)
   {
      uint idx = clamp<int>(pt, 0, NUM_SAMPLES - 1);

      if (idx < (uint)frame.attr.getCount() && frame.attr[idx] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

      double   yPosCh0 = frame.getAnalogDouble(0, idx);
      double   yPosCh1 = frame.getAnalogDouble(1, idx);

      double   y0 = yPosCh0*yfactor0 /*- ch0ZeroVolt*/;
      double   y1 = yPosCh1*yfactor1 /*- ch1ZeroVolt*/;
      double   yF = channelFunction(y0, y1, wndMain.function.Type);

      double xPos = (double(pt) / double(NUM_SAMPLES));
      double    x = xPos + xposition;
      double time = x*capture*frameSize;
      double   dt = time - previousTime;
      previousTime = time;
      if (idx == 0) previousTime = 0;

      int current0 = 0;
      int current1 = 0;
      int currentF = 0;

      if ( y0 > y0Middle + offset0) current0 =  1;
      if ( y0 < y0Middle - offset0) current0 = -1;
      if ( y1 > y1Middle + offset1) current1 =  1;
      if ( y1 < y1Middle - offset1) current1 = -1;
      if ( yF > yFMiddle + offsetF) currentF =  1;
      if ( yF < yFMiddle - offsetF) currentF = -1;

      // init
      if ( previous0 == 0 )
      {
         previous0 = current0;
      }
      // + transition to  -
      if ( previous0 == 1 && current0 == -1 )
      {
         time0.pushBack(sumTime0);
         previous0 = current0;
         sumTime0  = 0;
      }
      // - transition to  +
      if ( previous0 == -1 && current0 == 1 )
      {
         time0.pushBack(sumTime0);
         previous0 = current0;
         sumTime0  = 0;
      }

      // init
      if (previous1 == 0)
      {
         previous1 = current1;
      }
      // + transition to  -
      if (previous1 == 1 && current1 == -1)
      {
         time1.pushBack(sumTime1);
         previous1 = current1;
         sumTime1  = 0;
      }
      // - transition to  +
      if (previous1 == -1 && current1 == 1)
      {
         time1.pushBack(sumTime1);
         previous1 = current1;
         sumTime1  = 0;
      }

      // init
      if (previousF == 0)
      {
         previousF = currentF;
      }
      // + transition to  -
      if (previousF == 1 && currentF == -1)
      {
         timeF.pushBack(sumTimeF);
         previousF = currentF;
         sumTimeF = 0;
      }
      // - transition to  +
      if (previousF == -1 && currentF == 1)
      {
         timeF.pushBack(sumTimeF);
         previousF = currentF;
         sumTimeF  = 0;
      }

      sumTime0 += dt;
      sumTime1 += dt;
      sumTimeF += dt;
   }


   time0.popFront();
   time1.popFront();
   timeF.popFront();
   if (time0.getCount()%2 == 1) time0.popBack();
   if (time1.getCount()%2 == 1) time1.popBack();
   if (timeF.getCount()%2 == 1) timeF.popBack();

   double period0 = 0;
   double period1 = 0;
   double periodF = 0;
   for (int i = 0; i < time0.getCount();i++) period0 += time0[i];
   for (int i = 0; i < time1.getCount();i++) period1 += time1[i];
   for (int i = 0; i < timeF.getCount();i++) periodF += timeF[i];
   period0 /= (time0.getCount()/2);
   period1 /= (time1.getCount()/2);
   periodF /= (timeF.getCount()/2);

   current.row[Ch0Tperiod] = period0;
   current.row[Ch1Tperiod] = period1;
   current.row[FunTperiod] = periodF;

   // division by zero
   if (current.row[Ch0Tperiod] == 0.0) current.row[Ch0Tperiod] = 1.0;
   if (current.row[Ch1Tperiod] == 0.0) current.row[Ch1Tperiod] = 1.0;
   if (current.row[FunTperiod] == 0.0) current.row[FunTperiod] = 1.0;

   current.row[Ch0Tfreq] = 1.0 / current.row[Ch0Tperiod];
   current.row[Ch1Tfreq] = 1.0 / current.row[Ch1Tperiod];
   current.row[FunTfreq] = 1.0 / current.row[FunTperiod];

   // y - pick
   double pickCh0Y0    = wndMain.measure.data.pickY0.position.getYVolt(0);
   double pickCh0Y1    = wndMain.measure.data.pickY1.position.getYVolt(0);
   double pickCh1Y0    = wndMain.measure.data.pickY0.position.getYVolt(1);
   double pickCh1Y1    = wndMain.measure.data.pickY1.position.getYVolt(1);
   double pickFunY0    = channelFunction(pickCh0Y0, pickCh1Y0, wndMain.function.Type);
   double pickFunY1    = channelFunction(pickCh0Y1, pickCh1Y1, wndMain.function.Type);
   double pickCh0YMin  = min(pickCh0Y0, pickCh0Y1);
   double pickCh0YMax  = max(pickCh0Y0, pickCh0Y1);
   double pickCh1YMin  = min(pickCh1Y0, pickCh1Y1);
   double pickCh1YMax  = max(pickCh1Y0, pickCh1Y1);
   double pickFunYMin  = min(pickFunY0, pickFunY1);
   double pickFunYMax  = min(pickFunY0, pickFunY1);
   current.row[Ch0YV0] = pickCh0Y0;
   current.row[Ch0YV1] = pickCh0Y1;
   current.row[Ch1YV0] = pickCh1Y0;
   current.row[Ch1YV1] = pickCh1Y1;
   current.row[FunYV0] = pickFunY0;
   current.row[FunYV1] = pickFunY1;
   current.row[Ch0YT0] = capture*frameSize;
   current.row[Ch0YT1] = capture*frameSize;
   current.row[Ch1YT0] = capture*frameSize;
   current.row[Ch1YT1] = capture*frameSize;
   current.row[FunYT0] = capture*frameSize;
   current.row[FunYT1] = capture*frameSize;
   for (int pt = 0; pt < NUM_SAMPLES; pt++)
   {
      uint idx = clamp<uint>(pt, 0, NUM_SAMPLES - 1);

      if (idx < (uint)frame.attr.getCount() && frame.attr[idx] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

      double   yPosCh0 = frame.getAnalogDouble(0, idx);
      double   yPosCh1 = frame.getAnalogDouble(1, idx);
      double   y0 = yPosCh0*yfactor0 - ch0ZeroVolt;
      double   y1 = yPosCh1*yfactor1 - ch1ZeroVolt;
      double   yF = channelFunction(y0, y1, wndMain.function.Type);
      double    x = (double(pt) / double(NUM_SAMPLES));
                x = x + xposition;
      double time = x*capture*frameSize;

      ///////////////////////////////////////////////////////////////////////
      // rms
      ///////////////////////////////////////////////////////////////////////
      current.row[Ch0VrmsDc] += y0*y0;
      current.row[Ch1VrmsDc] += y1*y1;
      current.row[FunVrmsDc] += yF*yF;
      current.row[Ch0VrmsAc] += (y0 - y0Avg) *(y0 - y0Avg);
      current.row[Ch1VrmsAc] += (y1 - y1Avg) *(y1 - y1Avg);
      current.row[FunVrmsAc] += (yF - yFAvg) *(yF - yFAvg);

      ///////////////////////////////////////////////////////////////////////
      // at pick 0 ( x axis )
      ///////////////////////////////////////////////////////////////////////
      if (idx == wndMain.measure.data.pickX0.position.getXIndex(frameSize) )
      {
         current.row[Ch0XT0] = time;
         current.row[Ch1XT0] = time;
         current.row[FunXT0] = time;

         current.row[Ch0XV0] = y0;
         current.row[Ch1XV0] = y1;
         current.row[FunXV0] = yF;
      }

      ///////////////////////////////////////////////////////////////////////
      // at pick 1 ( x axis )
      ///////////////////////////////////////////////////////////////////////
      if ( idx == wndMain.measure.data.pickX1.position.getXIndex(frameSize) )
      {
         current.row[Ch0XT1] = time;
         current.row[Ch1XT1] = time;
         current.row[FunXT1] = time;

         current.row[Ch0XV1] = y0;
         current.row[Ch1XV1] = y1;
         current.row[FunXV1] = yF;
      }

      ///////////////////////////////////////////////////////////////////////
      // x range [ pick0, pick1 ]
      ///////////////////////////////////////////////////////////////////////
      if( x >= xMin && x <= xMax )
      {
         current.row[Ch0Vavg] += y0;
         current.row[Ch1Vavg] += y1;
         current.row[FunVavg] += yF;
         current.row[Ch0Vmin] = min( y0, current.row[Ch0Vmin] );
         current.row[Ch0Vmax] = max( y0, current.row[Ch0Vmax] );
         current.row[Ch1Vmin] = min( y1, current.row[Ch1Vmin] );
         current.row[Ch1Vmax] = max( y1, current.row[Ch1Vmax] );
         current.row[FunVmin] = min( yF, current.row[FunVmin] );
         current.row[FunVmax] = max( yF, current.row[FunVmax] );
         current.row[Ch0Surface] += fabs(y0)*capture;
         current.row[Ch1Surface] += fabs(y1)*capture;
         current.row[FunSurface] += fabs(yF)*capture;
      }

      // y pick 0
      if (y0 >= pickCh0Y0 )
      {
         current.row[Ch0YT0] = min(current.row[Ch0YT0], time );
      }
      if (y1 >= pickCh1Y0 )
      {
         current.row[Ch1YT0] = min(current.row[Ch1YT0], time);
      }
      if (yF >= pickFunY0 )
      {
         current.row[FunYT0] = min(current.row[FunYT0], time);
      }

      // y pick 1
      if (y0 >= pickCh0Y1)
      {
         current.row[Ch0YT1] = min(current.row[Ch0YT1], time);
      }
      if (y1 >= pickCh1Y1)
      {
         current.row[Ch1YT1] = min(current.row[Ch1YT1], time);
      }
      if (yF >= pickFunY1)
      {
         current.row[FunYT1] = min(current.row[FunYT1], time);
      }
   }
   current.row[Ch0YTD]    = max(current.row[Ch0YT0], current.row[Ch0YT1]) - min(current.row[Ch0YT0], current.row[Ch0YT1]);
   current.row[Ch1YTD]    = max(current.row[Ch1YT0], current.row[Ch1YT1]) - min(current.row[Ch1YT0], current.row[Ch1YT1]);
   current.row[FunYTD]    = max(current.row[FunYT0], current.row[FunYT1]) - min(current.row[FunYT0], current.row[FunYT1]);
   current.row[Ch0YInvTD] = 1.0 / current.row[Ch0YTD];
   current.row[Ch1YInvTD] = 1.0 / current.row[Ch1YTD];
   current.row[FunYInvTD] = 1.0 / current.row[FunYTD];
   current.row[Ch0XTD]    = max(current.row[Ch0XT0], current.row[Ch0XT1]   ) - min(current.row[Ch0XT0], current.row[Ch0XT1]   );
   current.row[Ch0XInvTD] = 1.0/current.row[Ch0XTD];
   current.row[Ch1XTD]    = max(current.row[Ch1XT0], current.row[Ch1XT1]) - min(current.row[Ch1XT0], current.row[Ch1XT1]);
   current.row[Ch1XInvTD] = 1.0/current.row[Ch1XTD];
   current.row[FunXTD]    = max(current.row[FunXT0], current.row[FunXT1]) - min(current.row[FunXT0], current.row[FunXT1]);
   current.row[FunXInvTD] = 1.0 / current.row[FunXTD];
   current.row[Ch0YVD] = max(current.row[Ch0YV0], current.row[Ch0YV1]) - min(current.row[Ch0YV0], current.row[Ch0YV1]);
   current.row[Ch1YVD] = max(current.row[Ch1YV0], current.row[Ch1YV1]) - min(current.row[Ch1YV0], current.row[Ch1YV1]);
   current.row[FunYVD] = max(current.row[FunYV0], current.row[FunYV1]) - min(current.row[FunYV0], current.row[FunYV1]);
   current.row[Ch0XVD] = max( current.row[Ch0XV0], current.row[Ch0XV1] ) - min(current.row[Ch0XV0], current.row[Ch0XV1]);
   current.row[Ch1XVD] = max( current.row[Ch1XV0], current.row[Ch1XV1] ) - min(current.row[Ch1XV0], current.row[Ch1XV1]);
   current.row[FunXVD] = max( current.row[FunXV0], current.row[FunXV1] ) - min(current.row[FunXV0], current.row[FunXV1]);
   current.row[Ch0Vavg] /= frameSize;
   current.row[Ch1Vavg] /= frameSize;
   current.row[FunVavg] /= frameSize;
   current.row[Ch0Vpp]    = current.row[Ch0Vmax] - current.row[Ch0Vmin];
   current.row[Ch1Vpp]    = current.row[Ch1Vmax] - current.row[Ch1Vmin];
   current.row[FunVpp]    = current.row[FunVmax] - current.row[FunVmin];
   current.row[Ch0VrmsDc] = sqrt(current.row[Ch0VrmsDc]/frameSize );
   current.row[Ch1VrmsDc] = sqrt(current.row[Ch1VrmsDc]/frameSize );
   current.row[FunVrmsDc] = sqrt(current.row[FunVrmsDc]/frameSize );
   current.row[Ch0VrmsAc] = sqrt(current.row[Ch0VrmsAc]/frameSize );
   current.row[Ch1VrmsAc] = sqrt(current.row[Ch1VrmsAc]/frameSize );
   current.row[FunVrmsAc] = sqrt(current.row[FunVrmsAc]/frameSize );

   if( wndMain.channel01.OscOnOff == 0 )
   {
      for (int i = Channel0 + 1; i < Channel1; i++)
         current.row[i] = 0;
   }
   if (wndMain.channel02.OscOnOff == 0)
   {
      for (int i = Channel1 + 1; i < Function; i++)
         current.row[i] = 0;
   }
   if (wndMain.function.OscOnOff == 0)
   {
      for (int i = Function + 1; i < FFT; i++)
         current.row[i] = 0;
   }


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // digital
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   int digitalCount = 0;
   int indicesArray[16] = { 0 };
   for (int i = 0; i<16; i++)
   {
      if (wndMain.digital.digital[i])
      {
         indicesArray[i] = 1;
         digitalCount++;
      }
   }

   if (digitalCount)
   {
      uint    count = frame.digital.getCount();
      double xScale = double(count) / double(NUM_SAMPLES);
      for (int i = 0; i<16; i++)
      {
         if (!indicesArray[i]) continue;

         double     ymin = (float(i) / 16.f) - 0.5f;
         double     ymax = (float(i + 1) / 16.f) - 0.5f - (1.f / 16.f)*0.5f;

         for (uint j = 0; j<count; j++)
         {
            int    idx = clamp(j, 0U, count - 1);
            ishort bit = frame.getDigital(i, idx);

            double delta = 1.f / double(count);
            double  xmin = (double(j) / double(count - 1))*xScale - 0.5 - delta / 2.0;
            double  xmax = (double(j) / double(count - 1))*xScale - 0.5 + delta / 2.0;

            if (wndMain.measure.data.pickX0.position.x >= xmin && wndMain.measure.data.pickX0.position.x <= xmax)
            {
               current.row[DigitalX1Bit0+i] = bit == 0 ? 1.0 : 0.0;

            }
            if (wndMain.measure.data.pickX1.position.x >= xmin && wndMain.measure.data.pickX1.position.x <= xmax)
            {
               current.row[DigitalX2Bit0 + i] = bit == 0 ? 1.0 : 0.0;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // FFT
   ////////////////////////////////////////////////////////////////////////////////
   for (int ch = 0; ch<3;ch++)
   {
      bool isFunction = (ch == 3) ? true : false;

      iint p = wndMain.horizontal.FFTSize;
      if (!p) return;

      if (ch == 0 && wndMain.channel01.FFTOnOff == 0) continue;
      if (ch == 1 && wndMain.channel02.FFTOnOff == 0) continue;
      if (ch == 2 && wndMain.function.FFTOnOff  == 0) continue;

      ////////////////////////////////////////////////////////////////////////////////
      // FFT input
      ////////////////////////////////////////////////////////////////////////////////
      iint n = 0;
      for (int j = 0; j<p; j++)
      {
         if (isFunction)
         {
            float ch0 = frame.getAnalog(0, j);
            float ch1 = frame.getAnalog(1, j);
            fft.aRe[j]   = channelFunction(ch0, ch1, wndMain.function.Type);
            fft.aIm[j]   = 0.0;
            fft.aAmpl[j] = 0.0;
            n = min( frame.analog[0].getCount(), frame.analog[1].getCount() );
         }
         else
         {
            n = frame.analog[ch].getCount();
            fft.aRe[j]   = frame.getAnalog(ch, j);
            fft.aIm[j]   = 0.0;
            fft.aAmpl[j] = 0.0;
         }
      }

      ////////////////////////////////////////////////////////////////////////////////
      // FFT calculate
      ////////////////////////////////////////////////////////////////////////////////
      fftCalculate(threadId,0, p, &fft.aRe[0], &fft.aIm[0]);
      fftAmplitude(n, p, &fft.aAmpl[0], &fft.aRe[0], &fft.aIm[0]);

      int count = p / 2 + 1;

      ////////////////////////////////////////////////////////////////////////////////
      // increment
      ////////////////////////////////////////////////////////////////////////////////
      float fresolution = (float)threadData.render.width;
      float fsignalpoints = (float)count;
      iint increment = (iint)((fsignalpoints / fresolution));
      increment = max<uint>(increment, 1);

      ////////////////////////////////////////////////////////////////////////////////
      // measure
      ////////////////////////////////////////////////////////////////////////////////
      double       time = pOsciloscope->window.horizontal.Capture;
      double   maxHertz = (1.f / time)*0.5f;
      int    maxIndex = count - 1;

      double freq0 = wndMain.measure.data.pickFFT0.position.getXFreq();
      double freq1 = wndMain.measure.data.pickFFT1.position.getXFreq();
      double freqMin = min(freq0,freq1);
      double freqMax = max(freq0,freq1);
      double measurePeriod    = 0.f;
      double measureAmplitude = 0.f;
      if (count)
      {
         float      delta = wndMain.display.fftWidth;
         float    yfactor = 1.0f;

         uint type = wndMain.display.fftType;

         float       time = wndMain.horizontal.Capture;
         for (int i = 0; i<count; i++)
         {
            float amplitude    = fft.aAmpl[i] * yfactor;
            float logAmplitude = amplitude;
            if (wndMain.display.fftDecibel >= 1)
            {
               // db60
               float power = dBPower[wndMain.display.fftDecibel - 1];
               float logMinimum = 10 * log10(power);
               logAmplitude = 10 * log10(amplitude);
               logAmplitude = clamp(logAmplitude, logMinimum, 0.f);
               amplitude = 1.f - (logAmplitude / logMinimum);
            }

            double    freq = 0;
            float position = float(i) / float(count - 1);
            if (wndMain.display.fftLogFreq > 0)
            {
               float max = float(count - 1);
               float value = position*float(count - 1);
               float log2Val = log2(value);
               float log2Max = log2(max);
               position = log2Val / log2Max;

               freq = freqFromPosition(i, maxIndex, maxHertz);
            }
            else
            {
               freq = position*maxHertz;
            }

            if (amplitude >= measureAmplitude)
            {
               measureAmplitude = amplitude;
               measurePeriod = 1.0 / freq;
            }

            // measure
            if ( freq <= freq0 )
            {
               switch(ch)
               {
               case 0:
                  {
                     current.row[FFTCh0F0]   = freq;
                     current.row[FFTCh0V0db] = logAmplitude;
                  }
                  break;
               case 1:
                  {
                     current.row[FFTCh1F0] = freq;
                     current.row[FFTCh1V0db] = logAmplitude;
                  }
                  break;
               case 2:
                  {
                     current.row[FFTFunF0]   = freq;
                     current.row[FFTFunV0db] = logAmplitude;
                  }
                  break;
               };
            }
            if (freq <= freq1)
            {
               switch (ch)
               {
               case 0:
               {
                  current.row[FFTCh0F1] = freq;
                  current.row[FFTCh0V1db] = logAmplitude;
               }
               break;
               case 1:
               {
                  current.row[FFTCh1F1] = freq;
                  current.row[FFTCh1V1db] = logAmplitude;
               }
               break;
               case 2:
               {
                  current.row[FFTFunF1] = freq;
                  current.row[FFTFunV1db] = logAmplitude;
               }
               break;
               };
            }
         }
      };

      switch (ch)
      {
      case 0:
         {
            current.row[FFTCh0VD] = max(current.row[FFTCh0V0db], current.row[FFTCh0V1db]) - min(current.row[FFTCh0V0db], current.row[FFTCh0V1db]);
            current.row[FFTCh0FD] = max(current.row[FFTCh0F0],   current.row[FFTCh0F1])   - min(current.row[FFTCh0F0],   current.row[FFTCh0F1]);
         }
         break;
      case 1:
         {
            current.row[FFTCh1VD] = max(current.row[FFTCh1V0db], current.row[FFTCh1V1db]) - min(current.row[FFTCh1V0db], current.row[FFTCh1V1db]);
            current.row[FFTCh1FD] = max(current.row[FFTCh1F0],   current.row[FFTCh1F1])   - min(current.row[FFTCh1F0],   current.row[FFTCh1F1]);
         }
         break;
      case 2:
         {
            current.row[FFTFunVD] = max(current.row[FFTFunV0db], current.row[FFTFunV1db]) - min(current.row[FFTFunV0db], current.row[FFTFunV1db]);
            current.row[FFTFunFD] = max(current.row[FFTFunF0],   current.row[FFTFunF1])   - min(current.row[FFTFunF0],   current.row[FFTFunF1]);
         }
         break;
      };
   };
}

void OsciloscopeThreadRenderer::renderAnalog(uint threadId, OsciloscopeThreadData& threadData,float z, uint channelId, uint shadow, OsciloscopeFrame& frame, float captureTime, float captureVolt,uint color,bool invert)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if (frame.analog[channelId].getCount() == 0)
      return;

   if (frame.analog[channelId].getCount() > NUM_SAMPLES)
      return;

   ////////////////////////////////////////////////////////////////////////////////
   // samples
   ////////////////////////////////////////////////////////////////////////////////
   uint  isamples = frame.analog[channelId].getCount();
   if (!isamples) return;

   ////////////////////////////////////////////////////////////////////////////////
   // start/end/increment/xgridsize
   ////////////////////////////////////////////////////////////////////////////////
   uint  start = 0;
   uint  end = NUM_SAMPLES - 1;
   uint  increment = wndMain.display.tessalation2d;

   ////////////////////////////////////////////////////////////////////////////////
   // render
   ////////////////////////////////////////////////////////////////////////////////
   end = isamples - 1;
   uint count = 0;
   for (uint point = start; point <= end; point += increment)
   {
      uint idx0 = clamp<uint>(point, 0, isamples);
      uint idx1 = clamp<uint>(point + increment, 0, isamples);

      if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;
      if (idx1 < (uint)frame.attr.getCount() && frame.attr[idx1] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

      count++;
   }

   uint type = wndMain.display.signalType;

   if (wndMain.horizontal.ETS)
   {
      pCanvas3d->beginBatch(threadId,CANVAS3D_BATCH_POINT, 2 * count);
   }
   else
   {
      if (type == 0)
         pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, count);
      else
         pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_STRIP, 2 * count);
   }

   float   etsDelta = (1.f / float(NUM_SAMPLES)) / float(render.maxEts);
   float   etsOffset = etsDelta*float(frame.ets);
   if (!wndMain.horizontal.ETS) etsOffset = 0;

   float   yGridMax = 0.5f;
   float    yfactor = yGridMax;
   float    xfactor = 1.f;
   float    xposition = (wndMain.horizontal.Position / 100.f)*(xfactor)+etsOffset*xfactor;

   if (invert) yfactor = -yfactor;

   double yOffset = 0;

   if (isamples)
   {
      for (uint point = start; point <= end; point += increment)
      {
         uint idx0 = clamp<uint>(point, 0, isamples);
         uint idx1 = clamp<uint>(point + increment, 0, isamples);

         if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;
         if (idx1 < (uint)frame.attr.getCount() && frame.attr[idx1] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

         float ystart = frame.getAnalog(channelId, idx0)*yfactor + float(yOffset);
         float yend   = frame.getAnalog(channelId, idx1)*yfactor + float(yOffset);

         float fstart = (float(point) / NUM_SAMPLES) - 0.5f;
         float fend = (float(point + increment) / NUM_SAMPLES) - 0.5f;

         float xstart = fstart*xfactor + xposition;
         float xend = fend*xfactor + xposition;

         Vector4 vstart = Vector4(xstart, ystart, z, 1.f);
         Vector4 vend = Vector4(xend, yend, z, 1.f);

         if (wndMain.horizontal.ETS)
         {
            pCanvas3d->bPoint(threadId,  vstart);
            pCanvas3d->bPoint(threadId,  vend);
            continue;
         }
         if (type == 0)
         {
            if (wndMain.fftDigital.is(VIEW_SELECT_OSC_3D)) pCanvas3d->bLine(threadId,  vstart, vend);
            else pCanvas3d->bLine(threadId,  vstart, vend);
            continue;
         }

         Vector4 vref0 = Vector4(0.f, 1.f, z, 1.f);
         Vector4 vref1 = Vector4(1.f, 0.f, z, 1.f);
         Vector4 vref;

         if (vecDot3d(vstart, vref0) == 0.000f) vref = vref1;
         else                                   vref = vref0;

         Vector4 vdir   = vend - vstart;
         Vector4 vcross = vecCross3d(vdir, vref);
         vcross = vecCross3d(vdir, vcross);
         vcross = vecNormalize3d(vcross);
         vcross = vcross * Vector4(wndMain.display.signalWidth);

         Vector4 v0 = vstart + vcross;
         Vector4 v1 = vstart - vcross;

         v0.w = v1.w = 1.f;

         pCanvas3d->bStrip(threadId,  v0);
         pCanvas3d->bStrip(threadId,  v1);
      }
   }

   if (shadow)
   {
      Vector4 constant = Vector4(wndMain.thermal.heating, 0.f, 0.f, 0.f);
      pCanvas3d->addConstant( threadId, constant );
      pCanvas3d->endBatch( threadId, render.cameraOsc.Final, COLOR_ARGB(255, 255, 255, 255), render.shadowTexture, BLEND_MODE_COPY, CANVAS3D_SHADER_CUSTOM, render.shadowLine3dShader);
   }
   else pCanvas3d->endBatch(threadId, render.cameraOsc.Final, color, 0, BLEND_MODE_COPY, wndMain.fftDigital.is(VIEW_SELECT_OSC_3D) ? CANVAS3D_SHADER_DEFAULT_COLOR : CANVAS3D_SHADER_DEFAULT );
}

void OsciloscopeThreadRenderer::renderAnalog3d(uint threadid, OsciloscopeThreadData& threadData, int frameIndex,float z, uint channelId, OsciloscopeFrame& frame, float captureTime, float captureVolt, uint color, bool invert)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if (frame.analog[channelId].getCount() == 0 || frame.analog[channelId].getCount() > NUM_SAMPLES )
   {
      return;
   }

   ////////////////////////////////////////////////////////////////////////////////
   // samples
   ////////////////////////////////////////////////////////////////////////////////
   uint  isamples = frame.analog[channelId].getCount();

   ////////////////////////////////////////////////////////////////////////////////
   // start/end/increment/xgridsize
   ////////////////////////////////////////////////////////////////////////////////
   uint  start = 0;
   uint  end = NUM_SAMPLES - 1;
   uint  increment = wndMain.display.tessalation3d;

   increment = pOsciloscope->max3dTesselation(increment,wndMain);

   ////////////////////////////////////////////////////////////////////////////////
   // render
   ////////////////////////////////////////////////////////////////////////////////
   end = isamples - 1;
   uint count = 0;
   for (uint point = start; point <= end; point += increment)
   {
      uint idx0 = clamp<uint>(point, 0, isamples);
      uint idx1 = clamp<uint>(point + increment, 0, isamples);

      if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;
      if (idx1 < (uint)frame.attr.getCount() && frame.attr[idx1] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

      count++;
   }

   uint type = wndMain.display.signalType;

   float   etsDelta = (1.f / float(NUM_SAMPLES)) / float(render.maxEts);

   float   etsOffset = etsDelta*float(frame.ets);
   if (!wndMain.horizontal.ETS) etsOffset = 0;

   float   yGridMax = 0.5f;
   float    yfactor = yGridMax;
   float    xfactor = 1.f;
   float    xposition = (wndMain.horizontal.Position / 100.f)*(xfactor)+etsOffset*xfactor;

   if (invert) yfactor = -yfactor;

   double yOffset = 0;

   if (isamples)
   {
      int     i      = 0;
      for (uint point = start; point <= end; point += increment)
      {
         uint idx0 = clamp<uint>(point, 0, isamples);
         if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

         // x
         float fstart  = (float(point) / NUM_SAMPLES) - 0.5f;
         float xstart  = fstart*xfactor + xposition;

         // y
         float ystart = frame.getAnalog(channelId, idx0)*yfactor + float(yOffset);

         if( channelId == 0 )       surfaceFrame0[frameIndex].point[i].pos = Vector4(xstart, ystart, z, 1.f);
         else                       surfaceFrame1[frameIndex].point[i].pos = Vector4(xstart, ystart, z, 1.f);
         i++;
      }
      if (channelId == 0)        surfaceFrame0[frameIndex].count = i;
      else                       surfaceFrame1[frameIndex].count = i;
   }
}


void OsciloscopeThreadRenderer::renderSurface3d(uint threadId, OsciloscopeThreadData& threadData, int channelId, uint color )
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   SurfaceFrame* surface = 0;
   if (channelId ==  0) surface = surfaceFrame0;
   if (channelId ==  1) surface = surfaceFrame1;
   if (channelId == -1) surface = surfaceFrameF;

   uint historyFrameDisplay = threadData.history.getCount();
   if (historyFrameDisplay < 16)
   {
      int debug = 1;
   }

   if (wndMain.display.solid3d == 0)
   {
      for (uint frame = 0; frame < historyFrameDisplay; frame++)
      {
         uint elCount = max( (int)surface[frame].count - 1, 0);
         uint loopCnt = max( (int)surface[frame].count,     0);
         pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, elCount);
         for (uint point = 1; point < loopCnt; point++)
         {
            Vector4 p0 = surface[frame].point[point - 1].pos;
            Vector4 p1 = surface[frame].point[point].pos;
            pCanvas3d->bLine(threadId,  p0, p1);
         }
         pCanvas3d->endBatch(threadId,  render.cameraOsc.Final, color, 0, BLEND_MODE_ALPHA, wndMain.display.light3d ? CANVAS3D_SHADER_DEFAULT : CANVAS3D_SHADER_DEFAULT_COLOR);
      }
   }
   else
   {
      for (uint frame = 1; frame < historyFrameDisplay; frame++)
      {
         uint elCount = max( int(2*surface[frame].count) - 2, 0);
         uint loopCnt = max( int(surface[frame].count)      , 0);
         pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_TRIANGLE, elCount );
         for (uint point = 1; point < loopCnt; point++)
         {
            Vector4 v[4];
            Vector4 n[4];

            uint iframeM1 = max<uint>(frame - 1, 0);
            uint ipointM1 = max<uint>(point - 1, 0);

            v[0] = surface[iframeM1].point[ipointM1].pos;
            n[0] = surface[iframeM1].point[ipointM1].normal;
            v[1] = surface[iframeM1].point[point].pos;
            n[1] = surface[iframeM1].point[point].normal;
            v[2] = surface[frame].point[point].pos;
            n[2] = surface[frame].point[point].normal;
            v[3] = surface[frame].point[ipointM1].pos;
            n[3] = surface[frame].point[ipointM1].normal;

            int indices[12] = { 0, 2, 1,
               2, 0, 3,
               2, 0, 1,
               0, 2, 3 };

            for (int i = 0; i < 2; i++)
            {
               pCanvas3d->bTriangleNormal(threadId, v[indices[i * 3 + 0]], v[indices[i * 3 + 1]], v[indices[i * 3 + 2]]);
            }
         }
         pCanvas3d->endBatch(threadId,  render.cameraOsc.Final, color, 0, BLEND_MODE_ALPHA, wndMain.display.light3d ? CANVAS3D_SHADER_NORMAL_COLOR_LIGHT : CANVAS3D_SHADER_NORMAL_COLOR);
      }
   }
}

void OsciloscopeThreadRenderer::renderAnalogFunction(uint threadid, OsciloscopeThreadData& threadData, float z,int function, OsciloscopeFrame& frame, float xCapture, float yCapture0, float yCapture1, uint color, bool invert0, bool invert1)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if (frame.analog[0].getCount() == 0)
      return;

   if (frame.analog[1].getCount() == 0)
      return;

   if (frame.analog[0].getCount() > NUM_SAMPLES)
      return;

   if (frame.analog[1].getCount() > NUM_SAMPLES)
      return;

   ////////////////////////////////////////////////////////////////////////////////
   // samples
   ////////////////////////////////////////////////////////////////////////////////
   uint  isamples0 = frame.analog[0].getCount();
   if (!isamples0) return;

   uint  isamples1 = frame.analog[1].getCount();
   if (!isamples1) return;

   if (isamples0 != isamples1) return;

   ////////////////////////////////////////////////////////////////////////////////
   // start/end/increment/xgridsize
   ////////////////////////////////////////////////////////////////////////////////
   uint  start = 0;
   uint  end = NUM_SAMPLES - 1;
   uint  increment = wndMain.display.tessalation2d;

   ////////////////////////////////////////////////////////////////////////////////
   // render
   ////////////////////////////////////////////////////////////////////////////////
   end = isamples0 - 1;
   uint count = 0;
   for (uint point = start; point <= end; point += increment)
   {
      uint idx0 = clamp<uint>(point, 0, isamples0);
      uint idx1 = clamp<uint>(point + increment, 0, isamples0);

      if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;
      if (idx1 < (uint)frame.attr.getCount() && frame.attr[idx1] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

      count++;
   }

   uint type = wndMain.display.signalType;


   pCanvas3d->beginBatch(threadid,CANVAS3D_BATCH_LINE, count);


   float   etsDelta = (1.f / float(NUM_SAMPLES)) / float(render.maxEts);
   float   etsOffset = etsDelta*float(frame.ets);
   if (!wndMain.horizontal.ETS) etsOffset = 0;

   float   yGridMax   = 0.5f;
   float    yfactor0  = yGridMax;
   float    yfactor1  = yGridMax;
   float    xfactor   = 1.f;
   float    xposition = (wndMain.horizontal.Position / 100.f)*(xfactor)+etsOffset*xfactor;

   if (invert0) yfactor0 = -yfactor0;
   if (invert1) yfactor1 = -yfactor1;


   float  xCount     = 10.f;
   double yNormalize = double(yCapture0)*double(xCount) / 2.0;
   double yOffset0 = 0;
   double yOffset1 = 0;
   //
   if (isamples0 && isamples1)
   {
      for (uint point = start; point <= end; point += increment)
      {
         uint idx0 = clamp<uint>(point, 0, isamples0);
         uint idx1 = clamp<uint>(point + increment, 0, isamples0);
         //
         if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;
         if (idx1 < (uint)frame.attr.getCount() && frame.attr[idx1] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

         float ystart  = 0;
         float yend    = 0;
         float ystart0 = frame.getAnalog(0, idx0)*yfactor0 + float(yOffset0);
         float yend0   = frame.getAnalog(0, idx1)*yfactor0 + float(yOffset0);
         float ystart1 = frame.getAnalog(1, idx0)*yfactor1 + float(yOffset1);
         float yend1   = frame.getAnalog(1, idx1)*yfactor1 + float(yOffset1);

         ystart = channelFunction(ystart0, ystart1, function);
         yend   = channelFunction(yend0, yend1, function);

         float fstart = (float(point) / NUM_SAMPLES) - 0.5f;
         float fend = (float(point + increment) / NUM_SAMPLES) - 0.5f;

         float xstart = fstart*xfactor + xposition;
         float xend = fend*xfactor + xposition;
         //
         Vector4 vstart = Vector4(xstart, ystart, 0.f, 1.f);
         Vector4 vend   = Vector4(xend, yend, 0.f, 1.f);

         pCanvas3d->bLine(threadid, vstart, vend);
      }
   }
   pCanvas3d->endBatch(threadid, render.cameraOsc.Final, color, 0, BLEND_MODE_COPY, CANVAS3D_SHADER_DEFAULT);
}


void OsciloscopeThreadRenderer::renderAnalogFunctionXY(uint threadid, OsciloscopeThreadData& threadData, OsciloscopeFrame& frame, float xCapture, float yCapture0, float yCapture1, uint color )
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if( !wndMain.function.xyGraph ) return;

   if (frame.analog[0].getCount() == 0)
      return;

   if (frame.analog[1].getCount() == 0)
      return;

   if (frame.analog[0].getCount() > NUM_SAMPLES)
      return;

   if (frame.analog[1].getCount() > NUM_SAMPLES)
      return;

   ////////////////////////////////////////////////////////////////////////////////
   // samples
   ////////////////////////////////////////////////////////////////////////////////
   uint  isamples0 = frame.analog[0].getCount();
   if (!isamples0) return;

   uint  isamples1 = frame.analog[1].getCount();
   if (!isamples1) return;

   if (isamples0 != isamples1) return;

   ////////////////////////////////////////////////////////////////////////////////
   // start/end/increment/xgridsize
   ////////////////////////////////////////////////////////////////////////////////
   uint  start = 0;
   uint  end = NUM_SAMPLES - 1;
   uint  increment = wndMain.display.tessalation2d;

   ////////////////////////////////////////////////////////////////////////////////
   // render
   ////////////////////////////////////////////////////////////////////////////////
   end = isamples0 - 1;
   uint count = 0;
   for (uint point = start; point <= end; point += increment)
   {
      uint idx0 = clamp<uint>(point, 0, isamples0);
      uint idx1 = clamp<uint>(point + increment, 0, isamples0);

      if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;
      if (idx1 < (uint)frame.attr.getCount() && frame.attr[idx1] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

      count++;
   }

   uint type = wndMain.display.signalType;

   pCanvas3d->beginBatch(threadid,CANVAS3D_BATCH_LINE, count);

   float   etsDelta = (1.f / float(NUM_SAMPLES)) / float(render.maxEts);
   float   etsOffset = etsDelta*float(frame.ets);
   if (!wndMain.horizontal.ETS) etsOffset = 0;

   float   yGridMax   = 0.5f;
   float    yfactor0  = yGridMax;
   float    yfactor1  = yGridMax;
   float    xfactor   = 1.f;
   float    xposition = (wndMain.horizontal.Position / 100.f)*(xfactor)+etsOffset*xfactor;

   float  xCount     = 10.f;
   double yNormalize = double(yCapture0)*double(xCount) / 2.0;
   double yOffset0 = 0;
   double yOffset1 = 0;

   double step0 = 0.f;
   double step1 = 0.f;
   step0 = render.analogChannelYVoltageStep0;
   step1 = render.analogChannelYVoltageStep1;

   float zeroOffset0 = 0.f;
   float zeroOffset1 = 0.f;
   zeroOffset0 = render.analogChannelPositin0 - render.analogChannelOffsets0;
   zeroOffset1 = render.analogChannelPositin1 - render.analogChannelOffsets1;

   float zeroVoltage0 = zeroOffset0*step0;
   float zeroVoltage1 = zeroOffset1*step1;

   yOffset0 = (zeroVoltage0 / (wndMain.channel01.Capture*xCount));
   yOffset1 = (zeroVoltage1 / (wndMain.channel02.Capture*xCount));

   if (isamples0 && isamples1)
   {
      for (uint point = start; point <= end; point += increment)
      {
         uint idx0 = clamp<uint>(point, 0, isamples0);
         uint idx1 = clamp<uint>(point + increment, 0, isamples0);
         //
         if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;
         if (idx1 < (uint)frame.attr.getCount() && frame.attr[idx1] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

         float x0 = frame.getAnalog(0, idx0)*yfactor0 - float(yOffset0);
         float y0 = frame.getAnalog(1, idx0)*yfactor0 - float(yOffset1);
         float x1 = frame.getAnalog(0, idx1)*yfactor1 - float(yOffset0);
         float y1 = frame.getAnalog(1, idx1)*yfactor1 - float(yOffset1);

         float fstart = (float(point) / NUM_SAMPLES) - 0.5f;
         float fend = (float(point + increment) / NUM_SAMPLES) - 0.5f;

         float xstart = fstart*xfactor + xposition;
         float xend   = fend*xfactor   + xposition;
         //
         Vector4 vstart = Vector4(x0, y0, 0.f, 1.f);
         Vector4 vend   = Vector4(x1, y1, 0.f, 1.f);

         pCanvas3d->bLine(threadid, vstart, vend);
      }
   }
   pCanvas3d->endBatch(threadid, render.cameraOsc.Final, color, 0, BLEND_MODE_COPY, CANVAS3D_SHADER_DEFAULT);
}

void OsciloscopeThreadRenderer::renderAnalogFunction3d(uint threadid, OsciloscopeThreadData& threadData, OsciloscopeFrame& frame, int frameIndex, float z, uint color)
{
   if (frame.analog[0].getCount() == 0 || frame.analog[1].getCount() == 0 || frame.analog[0].getCount() > NUM_SAMPLES || frame.analog[1].getCount() > NUM_SAMPLES)
   {
      return;
   }

   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   bool invert0 = wndMain.channel01.Invert;
   bool invert1 = wndMain.channel02.Invert;

   float xCapture = wndMain.horizontal.Capture;
   float yCapture = wndMain.channel01.Capture;


   ////////////////////////////////////////////////////////////////////////////////
   // samples
   ////////////////////////////////////////////////////////////////////////////////
   uint  isamples0 = frame.analog[0].getCount();
   if (!isamples0) return;

   uint  isamples1 = frame.analog[1].getCount();
   if (!isamples1) return;

   if (isamples0 != isamples1) return;

   ////////////////////////////////////////////////////////////////////////////////
   // start/end/increment/xgridsize
   ////////////////////////////////////////////////////////////////////////////////
   uint  start = 0;
   uint  end = NUM_SAMPLES - 1;
   uint  increment = wndMain.display.tessalation3d;

   increment = pOsciloscope->max3dTesselation(increment, wndMain );

   ////////////////////////////////////////////////////////////////////////////////
   // render
   ////////////////////////////////////////////////////////////////////////////////
   end = isamples0 - 1;
   uint count = 0;
   for (uint point = start; point <= end; point += increment)
   {
      uint idx0 = clamp<uint>(point, 0, isamples0);
      uint idx1 = clamp<uint>(point + increment, 0, isamples0);

      if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;
      if (idx1 < (uint)frame.attr.getCount() && frame.attr[idx1] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

      count++;
   }

   uint type = wndMain.display.signalType;

   float   etsDelta = (1.f / float(NUM_SAMPLES)) / float(render.maxEts);

   float   etsOffset = etsDelta*float(frame.ets);
   if (!wndMain.horizontal.ETS) etsOffset = 0;

   float    yGridMax  = 0.5f;
   float    yfactor0  = yGridMax;
   float    yfactor1  = yGridMax;
   float    xfactor   = 1.f;
   float    xposition = (wndMain.horizontal.Position / 100.f)*(xfactor)+etsOffset*xfactor;

   if (invert0) yfactor0 = -yfactor0;
   if (invert1) yfactor1 = -yfactor1;

   double yOffset0 = 0;
   double yOffset1 = 0;
   if (isamples0 && isamples1)
   {
      int i = 0;
      for (uint point = start; point <= end; point += increment)
      {
         uint idx0 = clamp<uint>(point, 0, isamples0);
         uint idx1 = clamp<uint>(point + increment, 0, isamples0);

         if (idx0 < (uint)frame.attr.getCount() && frame.attr[idx0] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;
         if (idx1 < (uint)frame.attr.getCount() && frame.attr[idx1] & FRAME_ATTRIBUTE_HIDE_SIGNAL) continue;

         // y
         float y0 = frame.getAnalog(0, idx0)*yfactor0 + float(yOffset0);
         float y1 = frame.getAnalog(1, idx0)*yfactor1 + float(yOffset1);
         float y      = channelFunction(y0, y1, wndMain.function.Type );

         // x
         float fpoint = (float(point) / NUM_SAMPLES) - 0.5f;
         float x      = fpoint*xfactor + xposition;

         // surface
         surfaceFrameF[frameIndex].point[i].pos = Vector4(x, y, z, 1.f);
         i++;
      }
      surfaceFrameF[frameIndex].count = i;
   }
}


void OsciloscopeThreadRenderer::renderMeasureFFT(uint threadId, OsciloscopeThreadData& threadData)
{
   OsciloscopeFrame&    frame = threadData.frame;
   WndMain&           wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   Matrix4x4   &final = render.cameraFFT.Final;

   float FFT0PickX = wndMain.measure.data.pickFFT0.position.x;
   float FFT0PickY = wndMain.measure.data.pickFFT0.position.y;
   float FFT0PickZ = wndMain.measure.data.pickFFT0.position.z;

   float FFT1PickX = wndMain.measure.data.pickFFT1.position.x;
   float FFT1PickY = wndMain.measure.data.pickFFT1.position.y;
   float FFT1PickZ = wndMain.measure.data.pickFFT1.position.z;

   if (wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
   {
      FFT0PickZ = 0.0f;
      FFT1PickZ = 0.0f;
   }

   if (wndMain.measure.data.pickFFT0.active )
   {
      pCanvas3d->beginBatch(threadId,CANVAS3D_BATCH_LINE, 1);
      pCanvas3d->bLine(threadId,  Vector4(FFT0PickX, yMin, FFT0PickZ, 1), Vector4(FFT0PickX, yMax, FFT0PickZ, 1));
      pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 255, 255, 0), 0, BLEND_MODE_ALPHA);
   }
   if (wndMain.measure.data.pickFFT1.active)
   {
      pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, 1);
      pCanvas3d->bLine(threadId,  Vector4(FFT1PickX, yMin, FFT1PickZ, 1), Vector4(FFT1PickX, yMax, FFT1PickZ, 1));
      pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 0, 255, 255), 0, BLEND_MODE_ALPHA);
   }

   if (wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
   {
      FFT0PickZ = 0.0f;
      FFT1PickZ = 1.0f;
   }

   int indices[12 * 3] = {  // bottom
      0, 1, 2,
      0, 2, 3,
      // top
      4, 5, 6,
      4, 6, 7,
      // left
      0, 3, 7,
      0, 7, 4,
      // right
      1, 2, 5,
      5, 2, 6,
      // front
      0, 1, 4,
      4, 1, 5,
      // back
      2, 3, 7,
      7, 2, 6 };

   ////////////////////////////////////////////////////////////////////////////////
   // X
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.measure.data.pickFFT0.active && wndMain.measure.data.pickFFT1.active )
   {
      if (wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
      {
         Vector4 v[8];
         v[0] = Vector4(FFT0PickX, yMin, FFT0PickZ, 1);
         v[1] = Vector4(FFT1PickX, yMin, FFT0PickZ, 1);
         v[2] = Vector4(FFT1PickX, yMin, FFT1PickZ, 1);
         v[3] = Vector4(FFT0PickX, yMin, FFT1PickZ, 1);
         v[4] = Vector4(FFT0PickX, yMax, FFT0PickZ, 1);
         v[5] = Vector4(FFT1PickX, yMax, FFT0PickZ, 1);
         v[6] = Vector4(FFT1PickX, yMax, FFT1PickZ, 1);
         v[7] = Vector4(FFT0PickX, yMax, FFT1PickZ, 1);

         pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 12);
         for (int i = 0; i < 12; i++)
            pCanvas3d->bTriangleNormal(threadId, v[indices[i * 3 + 0]], v[indices[i * 3 + 1]], v[indices[i * 3 + 2]]);
         pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
      }
      else
      {
         pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 2);
         pCanvas3d->bTriangleNormal(threadId,  Vector4(FFT0PickX, yMin, 0, 1), Vector4(FFT0PickX, yMax, 0, 1), Vector4(FFT1PickX, yMax, 0, 1));
         pCanvas3d->bTriangleNormal(threadId,  Vector4(FFT0PickX, yMin, 0, 1), Vector4(FFT1PickX, yMax, 0, 1), Vector4(FFT1PickX, yMin, 0, 1));
         pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
      }
   }
}

void OsciloscopeThreadRenderer::renderMeasure(uint threadId, OsciloscopeThreadData& threadData)
{
   OsciloscopeFrame&    frame = threadData.frame;
   WndMain&           wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   Matrix4x4   &final = render.cameraOsc.Final;

   float pre = wndMain.trigger.Percent / 100.0;

   float X0PickX = wndMain.measure.data.pickX0.position.x - 0.5;
   float X0PickY = wndMain.measure.data.pickX0.position.y;
   float X0PickZ = wndMain.measure.data.pickX0.position.z;

   float X1PickX = wndMain.measure.data.pickX1.position.x - 0.5;
   float X1PickY = wndMain.measure.data.pickX1.position.y;
   float X1PickZ = wndMain.measure.data.pickX1.position.z;

   float Y0PickX = wndMain.measure.data.pickY0.position.x - 0.5;
   float Y0PickY = wndMain.measure.data.pickY0.position.y;
   float Y0PickZ = wndMain.measure.data.pickY0.position.z;

   float Y1PickX = wndMain.measure.data.pickY1.position.x - 0.5;
   float Y1PickY = wndMain.measure.data.pickY1.position.y;
   float Y1PickZ = wndMain.measure.data.pickY1.position.z;

   if (wndMain.fftDigital.is(VIEW_SELECT_OSC_3D) )
   {
      X0PickZ = 0.0f;
      X1PickZ = 0.0f;

      Y0PickZ = 0.0f;
      Y1PickZ = 0.0f;
   }

   ////////////////////////////////////////////////////////////////////////////////
   // Y
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.measure.data.pickY0.active)
   {

      pCanvas3d->beginBatch(threadId,CANVAS3D_BATCH_LINE, 1);
      pCanvas3d->bLine(threadId,  Vector4(xMin, Y0PickY, Y0PickZ, 1), Vector4(xMax, Y0PickY, Y0PickZ, 1));
      pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 255, 255, 0), 0, BLEND_MODE_ALPHA);
   }
   if (wndMain.measure.data.pickY1.active)
   {
      pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, 1);
      pCanvas3d->bLine(threadId,  Vector4(xMin, Y1PickY, Y1PickZ, 1), Vector4(xMax, Y1PickY, Y1PickZ, 1));
      pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 0, 255, 255), 0, BLEND_MODE_ALPHA);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // X
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.measure.data.pickX0.active)
   {
      pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, 1);
      pCanvas3d->bLine(threadId,  Vector4(X0PickX, yMin, X0PickZ, 1), Vector4(X0PickX, yMax, X0PickZ, 1));
      pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 255, 255, 0), 0, BLEND_MODE_ALPHA);
   }
   if (wndMain.measure.data.pickX1.active)
   {
      pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, 1);
      pCanvas3d->bLine(threadId,  Vector4(X1PickX, yMin, X1PickZ, 1), Vector4(X1PickX, yMax, X1PickZ, 1));
      pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 0, 255, 255), 0, BLEND_MODE_ALPHA);
   }

   if (wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
   {
      X0PickZ = 0.0f;
      X1PickZ = 1.0f;

      Y0PickZ = 0.0f;
      Y1PickZ = 1.0f;
   }

   int indices[12*3] = {  // bottom
                        0, 1, 2,
                        0, 2, 3,
                        // top
                        4, 5, 6,
                        4, 6, 7,
                        // left
                        0, 3, 7,
                        0, 7, 4,
                        // right
                        1, 2, 5,
                        5, 2, 6,
                        // front
                        0, 1, 4,
                        4, 1, 5,
                        // back
                        2, 3, 7,
                        7, 2, 6 };

   ////////////////////////////////////////////////////////////////////////////////
   // X
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.measure.data.pickX0.active && wndMain.measure.data.pickX1.active)
   {
      if (wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
      {
         Vector4 v[8];
         v[0] = Vector4(X0PickX,   yMin, X0PickZ, 1);
         v[1] = Vector4(X1PickX,   yMin, X0PickZ, 1);
         v[2] = Vector4(X1PickX,   yMin, X1PickZ, 1);
         v[3] = Vector4(X0PickX,   yMin, X1PickZ, 1);
         v[4] = Vector4(X0PickX,   yMax, X0PickZ, 1);
         v[5] = Vector4(X1PickX,   yMax, X0PickZ, 1);
         v[6] = Vector4(X1PickX,   yMax, X1PickZ, 1);
         v[7] = Vector4(X0PickX,   yMax, X1PickZ, 1);

         pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 12);
            for (int i = 0; i < 12;i++)
               pCanvas3d->bTriangleNormal(threadId,  v[ indices[i*3+0] ] , v[ indices[i*3+1] ] , v[ indices[i*3+2] ] );
         pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
      }
      else
      {
         pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 2);
         pCanvas3d->bTriangleNormal(threadId,  Vector4(X0PickX, yMin, 0, 1), Vector4(X0PickX, yMax, 0, 1), Vector4(X1PickX, yMax, 0, 1));
         pCanvas3d->bTriangleNormal(threadId,  Vector4(X0PickX, yMin, 0, 1), Vector4(X1PickX, yMax, 0, 1), Vector4(X1PickX, yMin, 0, 1));
         pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // Y
   ////////////////////////////////////////////////////////////////////////////////
   if (wndMain.measure.data.pickY0.active && wndMain.measure.data.pickY1.active)
   {
      if (wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
      {
         Vector4 v[8];
         v[0] = Vector4(xMin, Y0PickY, Y0PickZ, 1);
         v[1] = Vector4(xMin, Y1PickY, Y0PickZ, 1);
         v[2] = Vector4(xMin, Y1PickY, Y1PickZ, 1);
         v[3] = Vector4(xMin, Y0PickY, Y1PickZ, 1);
         v[4] = Vector4(xMax, Y0PickY, Y0PickZ, 1);
         v[5] = Vector4(xMax, Y1PickY, Y0PickZ, 1);
         v[6] = Vector4(xMax, Y1PickY, Y1PickZ, 1);
         v[7] = Vector4(xMax, Y0PickY, Y1PickZ, 1);

         pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 12);
         for (int i = 0; i < 12; i++)
            pCanvas3d->bTriangleNormal(threadId,  v[indices[i * 3 + 0]], v[indices[i * 3 + 1]], v[indices[i * 3 + 2]]);
         pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);

      }
      else
      {
         pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 2);
         pCanvas3d->bTriangleNormal(threadId,  Vector4(xMin, Y0PickY, 0, 1), Vector4(xMax, Y0PickY, 0, 1), Vector4(xMin, Y1PickY, 0, 1));
         pCanvas3d->bTriangleNormal(threadId,  Vector4(xMin, Y1PickY, 0, 1), Vector4(xMax, Y1PickY, 0, 1), Vector4(xMax, Y0PickY, 0, 1));
         pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderFFTGrid
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderFFTGrid(uint threadid, OsciloscopeThreadData& threadData)
{
   OsciloscopeFrame&    frame = threadData.frame;
   WndMain&           wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   ////////////////////////////////////////////////////////////////////////////////
   // Grid
   ////////////////////////////////////////////////////////////////////////////////
   if( !wndMain.display.fftGrid ) return;

   int divisions = 8;

   float delta = 1.f/5.f;
   pCanvas3d->beginBatch(threadid,CANVAS3D_BATCH_LINE,5);
      for(float y=yMin;y<yMax;y=y+delta)
      {
         pCanvas3d->bLine(threadid,  Vector4(xMin,y,0,1), Vector4(xMax,y,0,1) );
      }
   pCanvas3d->endBatch(threadid,  render.cameraFFT.Final,COLOR_ARGB(255,25,25,25));

   pCanvas3d->beginBatch(threadid,  CANVAS3D_BATCH_LINE,2);
      pCanvas3d->bLine(threadid,  Vector4(xMin,yMin,0.f,1.f), Vector4(xMax,yMin,0.f,1.f) );
      pCanvas3d->bLine(threadid,  Vector4(xMin,yMax,0.f,1.f), Vector4(xMax,yMax,0.f,1.f) );
   pCanvas3d->endBatch(threadid,  render.cameraFFT.Final,COLOR_ARGB(255,25,25,25));

   if (wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
   {
      float size = 0.5f;
      pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, 8);
      pCanvas3d->bLine(threadid,Vector4(size, size, 0, 1), Vector4(size, size, 1, 1));
      pCanvas3d->bLine(threadid,Vector4(size, -size, 0, 1), Vector4(size, -size, 1, 1));
      pCanvas3d->bLine(threadid,Vector4(-size, size, 0, 1), Vector4(-size, size, 1, 1));
      pCanvas3d->bLine(threadid,Vector4(-size, -size, 0, 1), Vector4(-size, -size, 1, 1));

      pCanvas3d->bLine(threadid,Vector4(size, -size, 1, 1), Vector4(size, size, 1, 1));
      pCanvas3d->bLine(threadid,Vector4(size, size, 1, 1), Vector4(-size, size, 1, 1));
      pCanvas3d->bLine(threadid,Vector4(size, -size, 1, 1), Vector4(-size, -size, 1, 1));
      pCanvas3d->bLine(threadid,Vector4(-size, -size, 1, 1), Vector4(-size, size, 1, 1));
      pCanvas3d->endBatch(threadid,  render.cameraFFT.Final, COLOR_ARGB(255, 0, 0, 0));
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderFFTAxis
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderFFTAxis(uint threadid, OsciloscopeThreadData &threadData)
{
   OsciloscopeFrame&    frame = threadData.frame;
   WndMain&           wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   ////////////////////////////////////////////////////////////////////////////////
   // Axis
   ////////////////////////////////////////////////////////////////////////////////
   if( !wndMain.display.fftAxis ) return;

   pCanvas3d->beginBatch(threadid,  CANVAS3D_BATCH_LINE,1);
      pCanvas3d->bLine(threadid,  Vector4(xMin,-0.5f,0.0f,1.f), Vector4( xMax, -0.5f, 0.f,1.f) );
   pCanvas3d->endBatch(threadid,  render.cameraFFT.Final, COLOR_ARGB(255,255,0,0) );

   pCanvas3d->beginBatch(threadid,  CANVAS3D_BATCH_LINE,1);
      pCanvas3d->bLine(threadid,  Vector4(xMin,0.5f,0.f,1.f),  Vector4(xMin,-0.5f,0.f,1.f) );
   pCanvas3d->endBatch(threadid, render.cameraFFT.Final, COLOR_ARGB(255,0,255,0) );
}

////////////////////////////////////////////////////////////////////////////////
//
// renderFFTUnits
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderFFTUnits(uint threadid,OsciloscopeThreadData &threadData)
{
   OsciloscopeFrame&    frame = threadData.frame;
   MeasureData&          data = threadData.window.measure.data;
   WndMain&           wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if( frame.analog[0].getCount() > NUM_SAMPLES )
      return;

   iint n = frame.analog[0].getCount();

   pFont->setSize(threadid,0.25f );

   iint p = wndMain.horizontal.FFTSize;
   if( !p ) return;

   ////////////////////////////////////////////////////////////////////////////////
   // freq numbers
   ////////////////////////////////////////////////////////////////////////////////
   if( wndMain.display.fftUnits )
   {
      int   divisions  = 8;
      float charHeight = pFont->getLineHeight(threadid);
      //
      float      minX = render.cameraFFT.View.Pos().x - 0.5f*render.zoomFFT;
      float      maxX = render.cameraFFT.View.Pos().x + 0.5f*render.zoomFFT;
      if ( wndMain.fftDigital.is(VIEW_SELECT_FFT_3D) )
      {
         minX = xMin;
         maxX = xMax;
      }
      float xDeltaAbs = 1.f;
      float xDelta    = (maxX - minX);
      float xAdvance  = xDelta/float(divisions);
      float offset    = ( 1.f/float(n) )/2.f;
      FORMAT_BUFFER();
      int ypos=0;
      float       time = wndMain.horizontal.Capture;
      float   maxHertz = (1.f/time)*0.5f;
      int        count = p/2 + 1;
      Array<float,32> lines;
      lines.setCount(0);

      if (wndMain.display.fftLogFreq == 1)
      {
         for (int i = 0; i < 12; i++)
         {
            float x = logFactors[i];
            float pos = (x + 0.5f) / xDeltaAbs;

            float hertz = float(pos) * maxHertz;
            float max = float(count - 1);

            hertz = pow(max, pos);
            float norm = hertz / max;
            hertz = norm*maxHertz;
            ToolText::Hertz(formatBuffer, FORMAT_BUFFER_SIZE, hertz);

            if (ypos % 2 == 0) pFont->writeText3d(threadid, render.cameraFFT.Final, x, charHeight - 0.5f, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, 0xffffffff, render.fftScaleX, render.fftScaleY );
            else               pFont->writeText3d(threadid, render.cameraFFT.Final, x, -charHeight / 2 - 0.5f, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, 0xffffffff, render.fftScaleX, render.fftScaleY );
            ypos++;
         }
         for (int i = 0; i < 32; i++) lines.pushBack(logFactorsGrid[i]);
      }
      else
      {
         for (float x = minX; x<maxX + xAdvance; x = x + xAdvance)
         {
            float pos = (x + 0.5f) / xDeltaAbs;

            float hertz = float(pos) * maxHertz;
            ToolText::Hertz(formatBuffer, FORMAT_BUFFER_SIZE, hertz);

            lines.pushBack(x);
            if (ypos % 2 == 0) pFont->writeText3d(threadid, render.cameraFFT.Final, x, charHeight - 0.5f, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, 0xffffffff, render.fftScaleX, render.fftScaleY);
            else               pFont->writeText3d(threadid, render.cameraFFT.Final, x, -charHeight / 2 - 0.5f, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, 0xffffffff, render.fftScaleX, render.fftScaleY);
            ypos++;
         }
      }

      // fine x lines
      if( lines.getCount() )
      {
         pCanvas3d->beginBatch( threadid,  CANVAS3D_BATCH_LINE,lines.getCount());
         for(int i=0;i<lines.getCount();i++)
         {
            float x = lines[i];
            pCanvas3d->bLine(threadid, Vector4(x,0.5f,0.f,1.f),  Vector4(x,-0.5f,0.f,1.f) );
         }
         pCanvas3d->endBatch(threadid, render.cameraFFT.Final, COLOR_ARGB(255,25,25,25) );
      }

      // y units
      float    voltDisplay = wndMain.channel01.FFTOnOff ? wndMain.channel01.Capture : wndMain.channel02.Capture;
      float              x = xMin - charHeight*1.5f;
      double        ydelta = 1.f/5.f;
      char    buffer[1024] = {0};
      for(double y=yMin;y<=yMax+ydelta;y=y+ydelta)
      {
         if(wndMain.display.fftDecibel == 0 )
         {
            ToolText::Volt( buffer, 1024, voltDisplay * (y + yMax)*10.f );
         }
         else if (wndMain.display.fftDecibel >= 1 )
         {
            float  yNorm      = (y + yMax);
            float minValue    = dBPower[wndMain.display.fftDecibel-1];
            float minLogValue = 10*log10( minValue );
            float decibels    = (1.f-yNorm)*minLogValue;
            ToolText::Decibels( buffer, 1024, decibels );
         }
         pFont->writeText3d(threadid, render.cameraFFT.Final, x, y, 0, Vector4(1.f, 0.f, 0.f, 1.f), Vector4(0.f, 1.f, 0.f, 1.f), buffer, -1, render.fftScaleX, render.fftScaleY );
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderFFT
//
////////////////////////////////////////////////////////////////////////////////
double freqFromPosition(int index, int maxIndex, double maxHertz);

void OsciloscopeThreadRenderer::renderFFT(uint threadId, OsciloscopeThreadData& threadData, MeasureData& measure, OsciloscopeFFT& fft, OsciloscopeFrame& frame, bool funtion, float z, uint channelId,uint color)
{
   MeasureData&          data = measure;
   WndMain&           wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if( !frame.analog[channelId].getCount() ) return;

   if( frame.analog[channelId].getCount() > NUM_SAMPLES )
      return;

   iint n = frame.analog[channelId].getCount();

   iint p = wndMain.horizontal.FFTSize;
   if( !p ) return;

   ////////////////////////////////////////////////////////////////////////////////
   // FFT input
   ////////////////////////////////////////////////////////////////////////////////
   for(int i=0;i<p;i++)
   {
      if (funtion)
      {
         float ch0  = frame.getAnalog(0, i);
         float ch1  = frame.getAnalog(1, i);
         fft.aRe[i]   = channelFunction(ch0, ch1, wndMain.function.Type);
         fft.aIm[i]   = 0.0;
         fft.aAmpl[i] = 0.0;

      }
      else
      {
         fft.aRe[i] = frame.getAnalog(channelId, i);
         fft.aIm[i] = 0.0;
         fft.aAmpl[i] = 0.0;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // FFT calculate
   ////////////////////////////////////////////////////////////////////////////////
   fftCalculate(threadId, 0, p, &fft.aRe[0],   &fft.aIm[0] );
   fftAmplitude( n, p, &fft.aAmpl[0], &fft.aRe[0], &fft.aIm[0] );

   int count = p/2 + 1;

   ////////////////////////////////////////////////////////////////////////////////
   // increment
   ////////////////////////////////////////////////////////////////////////////////
   float fresolution   = (float)threadData.render.width;
   float fsignalpoints = (float)count;
   iint increment      = (iint)((fsignalpoints/fresolution));
   increment           = max<uint>( increment, 1 );

   ////////////////////////////////////////////////////////////////////////////////
   // Draw
   ////////////////////////////////////////////////////////////////////////////////
   double       time = pOsciloscope->window.horizontal.Capture;
   double   maxHertz = (1.f / time)*0.5f;
   int    maxIndex = count - 1;

   double freq0   = data.pickFFT0.position.getXFreq();
   double freq1   = data.pickFFT1.position.getXFreq();
   double measurePeriod    = 0.f;
   double measureAmplitude = 0.f;
   if(count)
   {
      float      delta = wndMain.display.fftWidth;
      float    yfactor = 1.0f;

      uint type = wndMain.display.fftType;

      if( type == 0 )
         pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE,count);
      else
         pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_TRIANGLE, 2*count );

      float       time = wndMain.horizontal.Capture;
      for (int i = 0; i<count; i++  )
      {
         float amplitude = fft.aAmpl[i]*yfactor;
         if( wndMain.display.fftDecibel >= 1 )
         {
            // db60
            float power = dBPower[wndMain.display.fftDecibel - 1];
            float logMinimum   = 10*log10( power );
            float logAmplitude = 10*log10( amplitude );
            logAmplitude = clamp( logAmplitude, logMinimum, 0.f );
            amplitude = 1.f-(logAmplitude/logMinimum);
         }

         double    freq = 0;
         float position = float(i)/float(count-1);
         if(wndMain.display.fftLogFreq > 0 )
         {
            float max   = float(count-1);
            float value = position*float(count-1);
            float log2Val = log2( value );
            float log2Max = log2( max );
            position      = log2Val / log2Max;

            freq = freqFromPosition(i, maxIndex, maxHertz);
         }
         else
         {
            freq = position*maxHertz;
         }

         if( amplitude >= measureAmplitude)
         {
            measureAmplitude = amplitude;
            measurePeriod    = 1.0/freq;
         }

         float      y0 = amplitude-0.5f;
         float      x0 = position-0.5f;
         float      x1 = x0 - delta;
         float      x2 = x0 + delta;
         if( type == 0 )
         {
            pCanvas3d->bLine(threadId,  Vector4(x0,-0.5f,z,1.f), Vector4(x0,(float)y0,z,1.f) );
         }
         else
         {
            pCanvas3d->bTriangle(threadId,  Vector4(x1, y0,z,1.f), Vector4(x1,-0.5f,z,1.f), Vector4(x2,-0.5f,z,1.f) );
            pCanvas3d->bTriangle(threadId,  Vector4(x1, y0,z,1.f), Vector4(x2,-0.5f,z,1.f), Vector4(x2,y0, z,1.f) );
         }
      }
      pCanvas3d->endBatch(threadId,  render.cameraFFT.Final, color, 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_DEFAULT );
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderDigitalAxis
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderDigitalAxis(uint threadid, OsciloscopeThreadData& threadData, uint xdivisions,uint ydivisions)
{
   OsciloscopeFrame&        frame = threadData.frame;
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if (!threadData.window.display.digitalAxis) return;

   pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE,1);
      pCanvas3d->bLine(threadid, Vector4(xMin, 0.f, 0.f,1.f), Vector4( xMax,  0.f, 0.f,1.f) );
   pCanvas3d->endBatch(threadid, render.cameraFFT.Final, COLOR_ARGB(255,255,0,0) );

   pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE,1);
      pCanvas3d->bLine(threadid, Vector4(xMin,0.5f,0.f,1.f),  Vector4(xMin,-0.5f,0.f,1.f) );
   pCanvas3d->endBatch(threadid, render.cameraFFT.Final, COLOR_ARGB(255,0,255,0) );
}

////////////////////////////////////////////////////////////////////////////////
//
// renderDigitalGrid
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderDigitalGrid(uint threadId,OsciloscopeThreadData& threadData, uint xdivisions,uint ydivisions)
{
   OsciloscopeFrame&        frame = threadData.frame;
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if (!threadData.window.display.digitalGrid) return;

   pCanvas3d->beginBatch( threadId,  CANVAS3D_BATCH_LINE, 16+3 );
      for(int i=0;i<=16;i++)
      {
         float y = float(i)/float(16.f) - 0.5f;
         pCanvas3d->bLine(threadId,  Vector4(xMin,y,0.f,1.f), Vector4(xMax,y,0.f,1.f) );
      }
      pCanvas3d->bLine(threadId,  Vector4(xMax,-0.5f,0.f,1.f),  Vector4(xMax,0.5f,0.f,1.f) );
      pCanvas3d->bLine(threadId,  Vector4(xMin,-0.5f,0.f,1.f),  Vector4(xMin,0.5f,0.f,1.f) );
   pCanvas3d->endBatch(threadId, render.cameraFFT.Final, COLOR_ARGB(255,25,25,25) );
}

////////////////////////////////////////////////////////////////////////////////
//
// renderDigitalUnit
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderDigitalUnit(uint threadid, OsciloscopeThreadData& threadData, uint xdivisions,uint ydivisions)
{
   OsciloscopeFrame&        frame = threadData.frame;
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if( !threadData.window.display.digitalUnits ) return;

   pFont->setSize(threadid, 0.25f );

   float charHeight = pFont->getLineHeight(threadid);
   uint       count = (xdivisions/8) + 1;
   float          x = xMin - charHeight - charHeight/2;
   float          d = 1.f/float(ydivisions);
   FORMAT_BUFFER();
   for(uint i=0;i<ydivisions;i++)
   {
      float y = float(i)/float(ydivisions) - yMax + d;
      FORMAT("%2d",i);
      pFont->writeText3d(threadid, render.cameraFFT.Final, x, y, 0, Vector4(1.f,0.f,0.f,1.f), Vector4(0.f,1.f,0.f,1.f), formatBuffer, 0xffffffff, render.fftScaleX, render.fftScaleY );
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderDigital
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderDigital(uint threadId,OsciloscopeThreadData& threadData, MeasureData& measure, uint xdivisions,uint ydivisions)
{
   OsciloscopeFrame&        frame = threadData.frame;
   MeasureData&              data = measure;
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   if(wndMain.digital.digital.getCount() == 0 ) return;

   int digitalCount     = 0;
   int indicesArray[16] = {0};
   for(int i=0;i<16;i++)
   {
      if( i < wndMain.digital.digital.getCount() && wndMain.digital.digital[i] )
      {
         indicesArray[i] = 1;
         digitalCount++;
      }
   }

   if( digitalCount )
   {
      uint    count = frame.digital.getCount();
      double xScale = double(count)/double(NUM_SAMPLES);
      pCanvas3d->beginBatch(threadId,CANVAS3D_BATCH_LINE, digitalCount*count*2 );
      for(int i=0;i<16;i++)
      {
         if( !indicesArray[i] ) continue;

         double     ymin = (float(i)/16.f)-0.5f;
         double     ymax = (float(i+1)/16.f)-0.5f-(1.f/16.f)*0.5f;

         for(uint j=0;j<count;j++)
         {
            int    idx = clamp( j, 0U, count-1 );
            ishort bit = frame.getDigital(i,idx);

            double delta =  1.f/double(count);
            double  xmin = (double(j)/double(count-1))*xScale - 0.5 - delta/2.0;
            double  xmax = (double(j)/double(count-1))*xScale - 0.5 + delta/2.0;

            if( bit )
            {
               pCanvas3d->bLine(threadId, Vector4(xmin,ymax,0.f,1.f), Vector4(xmax,ymax,0.f,1.f) );
            }
            else
            {
               pCanvas3d->bLine(threadId, Vector4(xmin,ymin,0.f,1.f), Vector4(xmax,ymin,0.f,1.f) );
            }
         }

         ishort previus = frame.getDigital(i,0);
         for(uint j=0;j<count;j++)
         {
            int    idx      = clamp( j, 0U, count-1 );
            ishort bit      = frame.getDigital(i,idx);

            double delta =  1.f/double(count);
            double  xmin = (double(j)/double(count-1))*xScale - 0.5 - delta/2.0;
            double  xmax = (double(j)/double(count-1))*xScale - 0.5 + delta/2.0;

            // draw
            if( previus == bit )
            {
               pCanvas3d->bLine(threadId,  Vector4(0,0,0.f,1.f), Vector4(0,0,0.f,1.f) );
            }
            else
            {
               pCanvas3d->bLine(threadId,  Vector4(xmin,ymin,0.f,1.f), Vector4(xmin,ymax,0.f,1.f) );
            }
            previus = bit;
         }
      }
      pCanvas3d->endBatch(threadId, render.cameraFFT.Final, COLOR_ARGB(255,255,255,255), 0, BLEND_MODE_COPY );
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderSlider
//
////////////////////////////////////////////////////////////////////////////////
void  OsciloscopeSlider::Rectangle(float& rectW, float& rectH, float& rectX, float& rectY, uint width, uint height, float size)
{
   rectW = width - 1;
   rectH = height / 42;
   rectX = 1.f;
   rectY = width*size - rectH / 2;
}

void OsciloscopeSlider::MinMax( float& xMinimum, float& xMaximum,uint width, uint height, float viewX,float size)
{
   float rectW = 0;
   float rectH = 0;
   float rectX = 0;
   float rectY = 0;
   Rectangle(rectW, rectH, rectX, rectY, width, height, size);
   float xNormalized = (viewX + 0.5f);
         xNormalized = clamp(1.f - xNormalized, 0.f, 1.f);
   float xPos        = (1.f-xNormalized)*(rectW);
   xMinimum    = xPos - (rectH / 2.f);
   xMaximum    = xPos + (rectH / 2.f);
}

void OsciloscopeThreadRenderer::renderSlider(uint threadId, OsciloscopeThreadData& threadData)
{
   WndMain&               wndMain = threadData.window;
   OsciloscopeRenderData&  render = threadData.render;

   float xNormalized = (-render.cameraFFT.View.Pos().x+0.5f);
         xNormalized = clamp(1.f-xNormalized,0.f,1.f);

   float posX = render.cameraFFT.View.Pos().x;

   if (wndMain.fftDigital.is(VIEW_SELECT_FFT_3D) )
   {
       xNormalized = 0.5f;
       posX        = 0.f;
   }

   float sliderRectW = 0;
   float sliderRectH = 0;
   float sliderRectX = 0;
   float sliderRectY = 0;
   float xMinimum = 0;
   float xMaximum = 0;
   float size = render.sliderSize;

   OsciloscopeSlider slider;
   slider.Rectangle(sliderRectW, sliderRectH, sliderRectX, sliderRectY, render.width, render.height, render.sliderSize );
   slider.MinMax(xMinimum, xMaximum, render.width, render.height, posX, render.sliderSize );

   pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_LINE, 4);
      pCanvas2d->bLine(threadId, sliderRectX,sliderRectY,sliderRectX+sliderRectW,sliderRectY);
      pCanvas2d->bLine(threadId, sliderRectX,sliderRectY+sliderRectH,sliderRectX+sliderRectW,sliderRectY+sliderRectH);
      pCanvas2d->bLine(threadId, sliderRectX,sliderRectY,sliderRectX,sliderRectY+sliderRectH);
      pCanvas2d->bLine(threadId, sliderRectX+sliderRectW-1,sliderRectY,sliderRectX+sliderRectW-1,sliderRectY+sliderRectH);
   pCanvas2d->endBatch(threadId, 0xffffffff );

   //float xPos = (1.f-xNormalized)*(sliderRectW);
   //xMinimum = xPos - (sliderRectH / 2.f);
   //xMaximum = xPos + (sliderRectH / 2.f);
   //xMinimum = clamp(xMinimum, sliderRectX, sliderRectX + sliderRectW - sliderRectH);
   //xMaximum = clamp(xMaximum, sliderRectX + sliderRectH, sliderRectX + sliderRectW);

   pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_RECTANGLE,1);
      pCanvas2d->bRectangle(threadId, xMinimum + 1, sliderRectY + 1, xMaximum - 1, sliderRectY + sliderRectH - 1 );
   pCanvas2d->endBatch(threadId, render.sliderMode == 1 ? 0xffffffff : 0xff000000 );
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
