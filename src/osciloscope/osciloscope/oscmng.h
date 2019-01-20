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
#ifndef __TOOL__OSCILOSCOPE__
#define __TOOL__OSCILOSCOPE__

enum ETimer {
    TIMER_MAIN_THREAD,
    TIMER_MAIN,
    TIMER_RENDER_THREAD,
    TIMER_RENDER,
    TIMER_CAPTURE,
    TIMER_GENERATE,
    TIMER_MEASURE,
    TIMER_UPDATE0,
    TIMER_UPDATE1,
    TIMER_UPDATE2,
    TIMER_UPDATE3,
    TIMER_UPDATE4,
    TIMER_UPDATE5,
    TIMER_UPDATE6,
    TIMER_UPDATE7,
};
////////////////////////////////////////////////////////////////////////////////
//
// AnalogFunction
//
////////////////////////////////////////////////////////////////////////////////
enum AnalogFunction {
   ANALOG_FUNCTION_MEDIUM = 0,
   ANALOG_FUNCTION_SUB_CH0_CH1,
   ANALOG_FUNCTION_SUB_CH1_CH0,
   ANALOG_FUNCTION_ADD,
   ANALOG_FUNCTION_MIN,
   ANALOG_FUNCTION_MAX,
};

////////////////////////////////////////////////////////////////////////////////
//
// AnalogFlag
//
////////////////////////////////////////////////////////////////////////////////
enum AnalogFlag {
   CHANNEL_ATTR_B    = BIT(0),
   CHANNEL_ATTR_A    = BIT(1),
   CHANNEL_B_GROUND  = BIT(2),
   CHANNEL_A_GROUND  = BIT(3),
   CHANNEL_B_ACDC    = BIT(4),
   CHANNEL_A_ACDC    = BIT(5),
   CHANNEL_A         = BIT(6),
   CHANNEL_B         = BIT(7),
   CHANNEL_ETS       = BIT(8),
};

////////////////////////////////////////////////////////////////////////////////
//
// GeneratorType
//
////////////////////////////////////////////////////////////////////////////////
enum GeneratorType {
   GENERATOR_CUSTOM,
   GENERATOR_SIN,
   GENERATOR_COS,
   GENERATOR_TRIANGLE,
   GENERATOR_RAMP_UP,
   GENERATOR_RAMP_DOWN,
   GENERATOR_SQUARE,
   GENERATOR_DELTA,
   GENERATOR_DC,
   GENERATOR_NOISE,
};

enum ControllType {
   CONTROLL_SHUTDOWN = 0x0000,
   CONTROLL_RESET    = 0x0A5A,
   CONTROLL_TEST     = 0x0640,
   CONTROLL_WAKEUP   = 0x0003,
   CONTROLL_NORMAL   = 0x0600,
};

enum CallibrateFreq {
   CALLIBRATE_1K,
   CALLIBRATE_5K,
   CALLIBRATE_10K,
   CALLIBRATE_100K,
   CALLIBRATE_200K,
   CALLIBRATE_500K,
   CALLIBRATE_1M,
   CALLIBRATE_2M,
};

enum DigitalPattern {
   DIGITAL_PATTERN_0,
   DIGITAL_PATTERN_1,
   DIGITAL_PATTERN_RISING,
   DIGITAL_PATTERN_FALLING,
};

enum DigitalStage {
   DIGITAL_STAGE_0,
   DIGITAL_STAGE_1,
   DIGITAL_STAGE_2,
   DIGITAL_STAGE_3,
};

enum DigitalBit {
   DIGITAL_BIT_0,
   DIGITAL_BIT_1,
   DIGITAL_BIT_2,
   DIGITAL_BIT_3,
   DIGITAL_BIT_4,
   DIGITAL_BIT_5,
   DIGITAL_BIT_6,
   DIGITAL_BIT_7,
   DIGITAL_BIT_8,
   DIGITAL_BIT_9,
   DIGITAL_BIT_10,
   DIGITAL_BIT_11,
   DIGITAL_BIT_12,
   DIGITAL_BIT_13,
   DIGITAL_BIT_14,
   DIGITAL_BIT_15,
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeControl
//
////////////////////////////////////////////////////////////////////////////////
#ifdef PLATFORM_WIN
	#pragma pack(push,1)
#endif

class OsciloscopeControl {
private:
    ushort control;
    ushort vgaina;
    ushort vgainb;
    short  offseta;
    short  offsetb;
    ushort siggen;
    ushort trigger;
    ushort analogswitch;
    ushort triggerMode;
    ushort triggerSource;
    ushort triggerSlope;
    ishort triggerLevel;
    ushort triggerHis;
    ushort triggerPercent;
    ushort xRange;
    ushort frameSize;
    ushort holdoffH;
    ushort holdoffL;
    ushort generatorType0;
    ushort generatorVoltage0;
    short  generatorOffset0;
    ushort generatorDeltaH0;
    ushort generatorDeltaL0;
    ushort generatorSqueareDuty0;
    ushort generatorType1;
    ushort generatorVoltage1;
    short  generatorOffset1;
    ushort generatorDeltaH1;
    ushort generatorDeltaL1;
    ushort generatorSqueareDuty1;
    ushort digitalPattern1a;
    ushort digitalPattern1b;
    ushort digitalMask1;
    ushort digitalPattern2a;
    ushort digitalPattern2b;
    ushort digitalMask2;
    ushort digitalPattern3a;
    ushort digitalPattern3b;
    ushort digitalMask3;
    ushort digitalPattern4a;
    ushort digitalPattern4b;
    ushort digitalMask4;
    ushort dt_delayMaxcnt1;
    ushort dt_delayMaxcnt2;
    ushort dt_delayMaxcnt3;
    ushort dt_delayMaxcnt4;
    ushort dt_control;
    ushort digitalVoltage;
    ushort digitalInputOutput;
    ushort digitalOutputBit;
    ushort digitalOutputMask;
    ushort digitalClkDivideH;
    ushort digitalClkDivideL;
public:
   OsciloscopeControl();
public:
   void Default();
public:
   void setAnalogSwitchBit(int bit, int value);
   void setEts(int enable);
   void setControl(uint control);
   void setYRangeScaleA(uint voltage,float scale);
   void setYRangeScaleA(uint gain, uint attr);
   void setYPositionA(int pos);
   void setYRangeScaleB(uint voltage,float scale);
   void setYRangeScaleB(uint gain,uint attr);
   void setYPositionB(int pos);
public:
   void setTriggerSource(int source);
   void setTriggerMode(int mode);
   void setTriggerSlope(int mode);
   void setTriggerReArm(bool on);
public:
   void setTriggerPre(float perc);
   void setTriggerHis(int perc);
   void setTriggerLevel(int perc);
public:
   void setXRange(ishort range);
   void setFrameSize(uint frameSize);
   void setHoldoff(double holdoff);
public:
   void setDigitalStart(int start);
   void setDigitalMode(int mode);
   void setDigitalChannel(int channel);
public:
   void setDigitalDelay(DigitalStage stage,ushort delay);
   void setDigitalMask(DigitalStage stage,DigitalBit bit,int value);
   void setDigitalPattern(DigitalStage stage,DigitalBit bit,DigitalPattern pattern);
public:
   void setGeneratorType0(GeneratorType type);
   void setGeneratorOn0(int onoff);
   void setGeneratorSlope0(int onoff);
   void setGeneratorVoltage0(int volt);
   void setGeneratorOffset0(int perc);
   void setGeneratorFrequency0(float freq,float fs=457142.81f);
   void setGeneratorSquareDuty0(int perc);
public:
   void setGeneratorType1(GeneratorType type);
   void setGeneratorOn1(int onoff);
   void setGeneratorSlope1(int onoff);
   void setGeneratorVoltage1(int volt);
   void setGeneratorOffset1(int perc);
   void setGeneratorFrequency1(float freq,float fs=457142.81f);
   void setGeneratorSquareDuty1(int perc);
public:
   void setDigitalVoltage(double volt);
   void setDigitalInputOutput(int inout15,int inout7);
   void setDigitalOutputBit(int bit,int onoff);
   void setDigitalClockDivide(uint divider);
public:
   int   getEts();
public:
   uint  getControl();
   uint  getYRangeA();
   float getYScaleA();
   int   getYPositionA();
public:
   uint  getYRangeB();
   float getYScaleB();
   int   getYPositionB();
public:
   float getTriggerPre();
   int   getTriggerHis();
   int   getTriggerLevel();
public:
   uint   getFrameSize();
public:
   ishort getXRange();
   double getHoldoff();
public:
   int   getDigitalStart();
   int   getDigitalMode();
   int   getDigitalChannel();
public:
   ushort         getDigitalDelay(DigitalStage stage);
   int            getDigitalMask(DigitalStage stage, DigitalBit bit);
   DigitalPattern getDigitalPattern(DigitalStage stage, DigitalBit bit);
public:
   GeneratorType getGeneratorType0();
   int   getGeneratorOn0();
   int   getGeneratorSlope0();
   int   getGeneratorVoltage0();
   int   getGeneratorOffset0();
   float getGeneratorFrequency0(float fs = 457142.81f);
   int   getGeneratorSquareDuty0();
public:
   GeneratorType getGeneratorType1();
   int   getGeneratorOn1();
   int   getGeneratorSlope1();
   int   getGeneratorVoltage1();
   int   getGeneratorOffset1();
   float getGeneratorFrequency1(float fs = 457142.81f);
   int   getGeneratorSquareDuty1();
public:
   double getDigitalVoltage();
   int    getDigitalInputOutput15();
   int    getDigitalInputOutput7();
   int    getDigitalOutputBit(int bit);
   uint   getDigitalClockDivide();
public:
   ushort getAttr(uint volt);
   ushort getGain(int channel,uint volt);
public:
   void transferData();
}
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
__attribute__ ((packed));
#else
;
#endif

#ifdef PLATFORM_WIN
	#pragma pack(pop)
#endif


////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeCamera
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeCamera {
public:
   float mouseStartX;
   float mouseStartY;
   float mouseEndX;
   float mouseEndY;
public:
   float  phi;
   float  theta;
   float  zoom;
   Camera ortho;
public:
   OsciloscopeCamera();
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeGrid
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeGrid {
public:
   float xCount;
   float xMax;
   float xMin;
public:
   float yCount;
   float yMax;
   float yMin;
public:
   void set(float xCount,float yCount);
};

////////////////////////////////////////////////////////////////////////////////
//
// OscFileThread
//
////////////////////////////////////////////////////////////////////////////////
class OscFileThread  {
public:
   String       file;
   SDL_Thread*  thread;
   SDL_atomic_t atomic;
public:
   OscFileThread();
public:
   int  isRunning();
};

////////////////////////////////////////////////////////////////////////////////
//
// Surface 3d
//
////////////////////////////////////////////////////////////////////////////////
struct SurfacePoint {
public:
   Vector4 pos;
   Vector4 normal;
};

struct SurfaceFrame {
public:
   SurfacePoint point[NUM_SAMPLES];
   int          count;
};


////////////////////////////////////////////////////////////////////////////////
//
// AutoCallibrate
//
////////////////////////////////////////////////////////////////////////////////
enum EAutoCallibrateMessageBox {
   acmbNone,
   acmbStart,
   acmbChannel0,
   acmbChannel1,
   acmbEnd,
   acmbOk,
   acmbCancel,
};

enum EAutoCallibrate {
   acStartMessageBox,
   acOffsetStart,
   acOffsetUpSetup,
   acOffsetUpCapture,
   acOffsetDownSetup,
   acOffsetDownCapture,
   acOffsetCalculate,
   acOffsetVoltageChange,
   acOffsetEnd,
   acGeneratorStart,
   acGeneratorMessageBox,
   acGeneratorUpSetup,
   acGeneratorUpCapture,
   acGeneratorDownSetup,
   acGeneratorDownCapture,
   acGeneratorCalculate,
   acGeneratorEnd,
   acGainStart,
   acGainSetup,
   acGainCapture,
   acGainCalculate,
   acGainVoltageChange,
   acGainEnd,
   acChannelChange,
   acEndMessageBox,
   acSaveSettings,
   acEnd,
};

class AutoCallibration {
public:
   int               active;
   int               generator;
   EAutoCallibrate   mode;
   VoltageCapture    voltage;
   int               frame;
   int               yposition;
   double            offsetUpVolt[2];
   double            offsetDownVolt[2];
   double            offsetVolt[2];
   double            generatorOffset;
   int               channel;
   int               gainIteration;
   int               gainMin;
   int               gainMax;
   double            gainVoltage;
public:
   EAutoCallibrateMessageBox messageBox;
public:
   bool              resetUI;
public:
   AutoCallibration()
   {
      resetUI = false;
      channel = 0;
      messageBox = acmbNone;
      active = 0;
      generator = 0;
      mode = acOffsetStart;
      voltage = vc2Volt;
      frame = 0;
      yposition = 0;
      offsetUpVolt[0] = 0;
      offsetUpVolt[1] = 0;
      offsetDownVolt[0] =  0;
      offsetDownVolt[1] = 0;
      offsetVolt[0] = 0;
      offsetVolt[1] = 0;
      gainIteration = 0;
      gainMin = 0;
      gainMax = 0;
      gainVoltage = 0;
   }
};

class OscThreadProduceConsume {
private:
   SDL_atomic_t headPos;
   SDL_atomic_t tailPos;
   SDL_atomic_t lockProducer;
   SDL_atomic_t lockConsumer;
   SDL_atomic_t lock[MAX_THREAD];
   uint         elCount;
public:
   OscThreadProduceConsume();
public:
   void setCount(uint value);
public:
   bool producerLock(uint& id,bool multiple);
   bool producerUnlock(uint id);
   bool consumerLock(uint& id,bool multiple);
   bool consumerUnlock(uint id);
};

class OscThreadLoop {
public:
   OscThreadProduceConsume capture;
   OscThreadProduceConsume update;
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeRenderData
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeRenderData {
public:
   Flag64 flags;
   uint   redrawEts;
public:
   GrTexture* shadowTexture;
public:
   GrShader* shadowLine3dShader;
   GrShader* shadowColorShader;
   GrShader* shadowCoolingShader;
public:
   uint width;
   uint height;
public:
   uint maxEts;
public:
   Camera cameraFFT;
   Camera cameraOsc;
   float  zoomFFT;
   float  zoomOsc;
public:
   float sliderSize;
   uint  sliderMode;
public:
   float fftScaleX;
   float fftScaleY;
   float oscScaleX;
   float oscScaleY;
public:
   double analogChannelYVoltageStep0;
   double analogChannelYVoltageStep1;
   double analogChannelOffsets0;
   double analogChannelOffsets1;
   double analogChannelPositin0;
   double analogChannelPositin1;
public:
   uint etsAttr;
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeThreadData
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeThreadData {
public:
   OsciloscopeFrame          etsClear;
   OsciloscopeFrame          frame;
   Ring<OsciloscopeFrame>    history;
   WndMain                   window;
   OsciloscopeRenderData     render;
};

////////////////////////////////////////////////////////////////////////////////
//
// RenderFlag
//
////////////////////////////////////////////////////////////////////////////////
enum RenderFlag {
   rfClearRenderTarget = BIT(0),
};

////////////////////////////////////////////////////////////////////////////////
//
// ETS
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeETS {
public:
   Array<OsciloscopeFrame, 32>  etsHistory;
   OsciloscopeFrame              etsClear;
   uint                          etsIndex;
   uint                          etsAttr;
public:
   void clear();
   void redraw(OsciloscopeRenderData& render);
   void onFrameChange(int framechange, Ring<OsciloscopeFrame> threadHistory, OsciloscopeRenderData& render);
   void onCapture(OsciloscopeFrame& frame, OsciloscopeRenderData& render);
   void onPause(OsciloscopeFrame& frame,WndMain& window);
};


////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeThreadRenderer
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeThreadRenderer {
public:
   float xMin;
   float xMax;
   float yMin;
   float yMax;
public:
   SurfaceFrame* surfaceFrame0;
   SurfaceFrame* surfaceFrame1;
   SurfaceFrame* surfaceFrameF;
public:
   ularge        cfgSize;
   byte*         cfgMem;
   kiss_fft_cpx* in;
   kiss_fft_cpx* out;
public:
   uint historyCount;
public:
   void init(uint max3dhistory)
   {
      surfaceFrame0 = (SurfaceFrame*)memoryAllocate(sizeof(SurfaceFrame)*max3dhistory);
      surfaceFrame1 = (SurfaceFrame*)memoryAllocate(sizeof(SurfaceFrame)*max3dhistory);
      surfaceFrameF = (SurfaceFrame*)memoryAllocate(sizeof(SurfaceFrame)*max3dhistory);
      historyCount  = max3dhistory;

      cfgSize = 0;
      kiss_fft_alloc(NUM_FFT, 0, 0, (size_t*)&cfgSize);
      cfgMem = (byte*)memoryAllocate(cfgSize);

      in  = (kiss_fft_cpx*)memoryAllocate( NUM_FFT*sizeof(kiss_fft_cpx) );
      out = (kiss_fft_cpx*)memoryAllocate( NUM_FFT*sizeof(kiss_fft_cpx) );
   }
   void clear()
   {
      SDL_memset(surfaceFrame0, 0, sizeof(SurfaceFrame)*historyCount);
      SDL_memset(surfaceFrame1, 0, sizeof(SurfaceFrame)*historyCount);
      SDL_memset(surfaceFrameF, 0, sizeof(SurfaceFrame)*historyCount);
   }
   void clearFast()
   {
      for (uint i = 0; i < historyCount; i++)
      {
         surfaceFrame0[i].count = 0;
         surfaceFrame1[i].count = 0;
         surfaceFrameF[i].count = 0;
      }
   }
public:
   void measureSignal(uint threadId, OsciloscopeThreadData& threadData, MeasureData& measure, OsciloscopeFFT& fft);
public:
   void preOscRender(uint threadId, OsciloscopeThreadData& threadData);
   void preFftRender(uint threadId, OsciloscopeThreadData& threadData);
   void renderFps(uint threadId, OsciloscopeThreadData& threadData);
   void renderAnalogGrid(uint threadId, OsciloscopeThreadData& threadData);
   void renderAnalogAxis(uint threadId, OsciloscopeThreadData& threadData);
   void renderAnalogUnits(uint threadid, OsciloscopeThreadData& threadData);
   void renderAnalog(uint threadId, OsciloscopeThreadData& threadData, float z, uint channelId, uint shadow, OsciloscopeFrame& frame, float captureTime, float captureVolt, uint color, bool invert);
   void renderAnalog3d(uint threadid, OsciloscopeThreadData& threadData, int frameIndex, float z, uint channelId, OsciloscopeFrame& frame, float captureTime, float captureVolt, uint color, bool invert);
   void renderSurface3d(uint threadId, OsciloscopeThreadData& threadData, int channelId, uint color);
   void renderAnalogFunction(uint threadid, OsciloscopeThreadData& threadData, float z, int function, OsciloscopeFrame& frame, float xCapture, float yCapture0, float yCapture1, uint color, bool invert0, bool invert1);
   void renderAnalogFunctionXY(uint threadid, OsciloscopeThreadData& threadData, OsciloscopeFrame& frame, float xCapture, float yCapture0, float yCapture1, uint color );
   void renderAnalogFunction3d(uint threadid, OsciloscopeThreadData& threadData, OsciloscopeFrame& frame, int frameIndex, float z, uint color);
   void renderMeasure(uint threadid, OsciloscopeThreadData& threadData);
   void renderMeasureFFT(uint threadId, OsciloscopeThreadData& threadData);
   void renderFFTGrid(uint threadid, OsciloscopeThreadData& threadData);
   void renderFFTAxis(uint threadid, OsciloscopeThreadData& threadData);
   void renderFFTUnits(uint threadid, OsciloscopeThreadData &threadData);
   void renderFFT(uint threadId, OsciloscopeThreadData& threadData, MeasureData& measure, OsciloscopeFFT& fft, OsciloscopeFrame& frame, bool function, float z, uint channelId, uint color);
   void renderDigitalAxis(uint threadid, OsciloscopeThreadData& threadData, uint xdiv, uint ydiv);
   void renderDigitalGrid(uint threadid, OsciloscopeThreadData& threadData, uint xdiv, uint ydiv);
   void renderDigitalUnit(uint threadid, OsciloscopeThreadData& threadData, uint xdiv, uint ydiv);
   void renderDigital(uint threadid, OsciloscopeThreadData& threadData, MeasureData& measure, uint xres, uint yres);
   void renderSlider(uint threadid, OsciloscopeThreadData& threadData);
public:
   void dftCalculate(iint n, double* inRe, double* inIm, double* outRe, double* outIm);
   void fftCalculate(uint threadId,uint backward, uint n, double* real, double* imag);
   void fftAmplitude(iint points, iint n, double* outAmpl, double* inRe, double* inIm);
};



enum ERenderMode {
  RENDER_MODE_THERMAL,
  RENDER_MODE_COLOR_ANALOG_3D,
  RENDER_MODE_COLOR_ANALOG_2D,
  RENDER_MODE_COLOR_DIGITAL,
  RENDER_MODE_COLOR_MEASURE,
  RENDER_MODE_COLOR_FFT3D,
  RENDER_MODE_COLOR_FFT2D,
  RENDER_MODE_COLOR_SLIDER,
  RENDER_MODE_COLOR_COOLING,
  RENDER_MODE_COLOR_BLEND_IN_HEATING,
  RENDER_MODE_LAST,
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeSlider
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeSlider{
public:
   void Rectangle(float& rectW, float& rectH, float& rectX, float& rectY, uint width, uint height, float size);
   void MinMax(float& xMinimum, float& xMaximum, uint width, uint height, float viewX, float size);
};

enum ThreadID {
    THREAD_ID_MAIN,
    THREAD_ID_CAPTURE,
    THREAD_ID_UPDATE,
    THREAD_ID_RENDER,
};

////////////////////////////////////////////////////////////////////////////////
//
// Osciloscope
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeManager : public Manager, public RenderCallback {
public:
   bool          scrollThread;
   SDL_atomic_t  contextCreated;
public:
   SDL_Window*   sdlWindow;
   SDL_GLContext sdlContext;
   int           sdlX;
   int           sdlY;
   int           sdlW;
   int           sdlH;
public:
   OscFileThread fileThread;
public:
   float frameTime;
public:
   AutoCallibration  callibrate;
   OscSettings       settings;
public:
    WndMain window;
    WndMain windowState[4];
    String  windowName[4];
    uint    windowSlot;
public:
   GrTexture* color;
   GrTexture* depth;
   GrTexture* aShadow[2];
public:
   GrShader* shadowLine3dShader;
   GrShader* shadowColorShader;
   GrShader* shadowCoolingShader;
public:
   OsciloscopeETS ets;
   uint           redrawEts;
public:
   float mouseWheel;
public:
   Vector4 nearStart;
   Vector4 farStart;
   Vector4 nearEnd;
   Vector4 farEnd;
   Vector4 out0;
   Vector4 out1;
public:
   int   sliderMode;
   float sliderSize;
   float sliderMin;
   float sliderMax;
   float sliderRectX;
   float sliderRectY;
   float sliderRectW;
   float sliderRectH;
   int   insideSliderBox;
   int   insideSliderArea;
   int   openglFocus;
public:
   float fftAspectX;
   float fftAspectY;
   float oscAspectX;
   float oscAspectY;
public:
   float oscScaleX;
   float oscScaleY;
   float fftScaleX;
   float fftScaleY;
public:
   float xMin;
   float xMax;
   float yMin;
   float yMax;
public:
   OsciloscopeFrame       display;
   SignalMode             signalMode;
   SDL_SpinLock           uploadFirmwareLock;
   SDL_SpinLock           displayLock;
public:
   bool                   renderSync;
   SDL_SpinLock           renderLock;
   WndMain                renderWindow;
   OsciloscopeRenderData  renderData;
   OscThreadLoop          threadLoop;
   OsciloscopeThreadData  captureData[MAX_THREAD];
   MeasureData            measureData[MAX_THREAD];
   OsciloscopeThreadData  captureTemp;
public:
   OsciloscopeFrame*      pTmpData;
   OsciloscopeFrame       tmpDisplay;
   Ring<OsciloscopeFrame> tmpHistory;
public:
   bool                   threadActive;
   OsciloscopeFrame*      pThreadData;
   OsciloscopeFrame       threadDisplay;
   Ring<OsciloscopeFrame> threadHistory;
public:
   OsciloscopeCamera cameraOsc;
   OsciloscopeCamera cameraFFT;
   float             analogWindowSize;
public:
   OsciloscopeGrid grid;
public:
   OsciloscopeControl control;
public:
   int clearRenderTarget;
   int clearThermal;
public:
   OsciloscopeManager();
public:
   void   setFrame(int frame);
   double getTriggerVoltagePerStep();
   double getTriggerVoltage();
public:
   int  saveToFile(const char *file);
   int  loadFromFile(const char *file);
public:
   void openUSB();
   void uploadFpga();
   void uploadFx2();
   void uploadFirmware();
   void ReadEEPROM(UsbEEPROM& out);
   void WriteEEPROM();
   void EraseEEPROM();
public:
   int  start();
   int  update(float dt);
   void startThreads();
   void renderMain(uint threadid);
   void renderThread(uint threadid, OsciloscopeThreadData& data, OsciloscopeThreadRenderer& renderer,OsciloscopeFFT& fft);
   void render() {};
   int  stop();
public:
   void onCallibrateFrameCaptured(OsciloscopeFrame& frame);
   void AutoCallibrate();
public:
   void oscCameraSetup(int enable);
   void fftCameraSetup(int enable);
public:
   uint max3dTesselation(uint value, WndMain& wnd);
public:
   void onApplicationInit();
   bool onApplicationIdle();
   void onResize(int width,int height,int oldWidth,int oldHeight);
public:
   void allocate();
public:
   void setThreadPriority(ThreadID id);
public:
   int onEvent(SDL_Event& event);
};

MANAGER_POINTER(Osciloscope);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
