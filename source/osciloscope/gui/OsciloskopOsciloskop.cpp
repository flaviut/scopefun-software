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
#include "OsciloskopOsciloskop.h"
#include "OsciloskopDisplay.h"
#include "OsciloskopThermal.h"
#include "OsciloskopSoftwareGenerator.h"
#include "OsciloskopHardwareGenerator.h"
#include "OsciloskopMeasure.h"
#include "OsciloskopInfo.h"
#include "OsciloskopDebug.h"

#include <wx/app.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include <wx/clipbrd.h>

extern "C" {
   extern char* xpm16_xpm[];
   extern char* xpm32_xpm[];
   extern char* xpm64_xpm[];
   extern char* xpm96_xpm[];
   extern char* xpm128_xpm[];
   extern char* xpm256_xpm[];
   extern char* xpm512_xpm[];
};

static OsciloskopMeasure*    pMeasure = 0;
static OsciloskopDebug*      pDebug = 0;
static OsciloskopInfo*       pInfo = 0;

int   checkAgree();
float rangeToSeconds(float range);

void LoadSlot(int slot,const char* file)
{
   slot = clamp(slot, 0, 3);
   SDL_RWops* ctx = SDL_RWFromFile(file, "rb");
   if (ctx)
   {
      SDL_RWread(ctx, &pOsciloscope->windowState[slot], 1, sizeof(WndMain));
      SDL_RWclose(ctx);
   }
}

void SaveSlot(int slot, const char* file)
{
   slot = clamp(slot, 0, 3);
   SDL_RWops* ctx = SDL_RWFromFile(file, "wb+");
   if (ctx)
   {
      SDL_RWwrite(ctx, &pOsciloscope->windowState[slot], 1, sizeof(WndMain));
      SDL_RWclose(ctx);
   }
}

wxFileName GetOscDataFolder()
{
    wxFileName fn;
#if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    fn = wxStandardPaths::Get().GetExecutablePath();
#endif
#ifdef PLATFORM_MAC
    fn = wxStandardPaths::Get().GetResourcesDir();
    fn.AppendDir("Resources");
#endif
#ifdef PLATFORM_LINUX
    fn = "/usr/lib/oscilloscope";
#endif
    return fn;
}

wxFileName GetOscLangFolder()
{
    wxFileName fn;
#if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    fn = wxStandardPaths::Get().GetExecutablePath();
    fn.AppendDir("locale");
#endif
#ifdef PLATFORM_MAC
    fn = wxStandardPaths::Get().GetResourcesDir();
    fn.AppendDir("Resources");
    fn.AppendDir("locale");
#endif
#ifdef PLATFORM_LINUX
    fn = "/usr/share/locale/";
#endif
    return fn;
}

OsciloskopOsciloskop::OsciloskopOsciloskop( wxWindow* parent )
:
Osciloskop( parent )
{
   pMeasure = 0;
   pDebug = 0;
   pInfo = 0;
}

void OsciloskopOsciloskop::onActivate( wxActivateEvent& event )
{
   // TODO: Implement onActivate
   static int once = 1;
   if( once )
   {
      if( pOsciloscope->settings.debug==0)
        m_menu7->Remove(m_menuItemDebug);

      once = 0;

      ////////////////////////////////////////////////////////////////////////////////////////
      // state
      ////////////////////////////////////////////////////////////////////////////////////////
      wxString stringSlot0;
      wxString stringSlot1;
      wxString stringSlot2;
      wxString stringSlot3;

      wxFileName fn = GetOscDataFolder();
      wxString string = fn.GetPath().append(_("/data/state/main.state"));
      SDL_RWops* ctx = SDL_RWFromFile(string.ToAscii().data(), "rb");
      if (ctx)
      {
         SDL_RWread(ctx, &pOsciloscope->windowName[0], 1, 4 * sizeof(String));
         SDL_RWclose(ctx);

         stringSlot0 = pOsciloscope->windowName[0].asChar();
         stringSlot1 = pOsciloscope->windowName[1].asChar();
         stringSlot2 = pOsciloscope->windowName[2].asChar();
         stringSlot3 = pOsciloscope->windowName[3].asChar();
      }
      else
      {
         stringSlot0 = fn.GetPath().append("/data/state/slot1.slot");
         stringSlot1 = fn.GetPath().append("/data/state/slot2.slot");
         stringSlot2 = fn.GetPath().append("/data/state/slot3.slot");
         stringSlot3 = fn.GetPath().append("/data/state/slot4.slot");
      }

      pOsciloscope->windowName[0] = stringSlot0.ToAscii().data();
      pOsciloscope->windowName[1] = stringSlot1.ToAscii().data();
      pOsciloscope->windowName[2] = stringSlot2.ToAscii().data();
      pOsciloscope->windowName[3] = stringSlot3.ToAscii().data();

      // load slot
      LoadSlot(0, stringSlot0.ToAscii().data());
      LoadSlot(1, stringSlot1.ToAscii().data());
      LoadSlot(2, stringSlot2.ToAscii().data());
      LoadSlot(3, stringSlot3.ToAscii().data());

      // load window
      loadWindow(3);
      loadWindow(2);
      loadWindow(1);
      loadWindow(0);

      ////////////////////////////////////////////////////////////////////////////////////////
      // license
      ////////////////////////////////////////////////////////////////////////////////////////
      int agree = checkAgree();
      if( !agree )
      {
         wxCommandEvent nullEvent;
         m_menuItemInfoOnMenuSelection( nullEvent );
      }

      ////////////////////////////////////////////////////////////////////////////////////////
      // icon
      ////////////////////////////////////////////////////////////////////////////////////////
      wxIconBundle icoBundle;
      wxIcon icon16(xpm16_xpm);
      wxIcon icon32(xpm32_xpm);
      wxIcon icon64(xpm64_xpm);
      wxIcon icon96(xpm96_xpm);
      wxIcon icon128(xpm128_xpm);
      wxIcon icon256(xpm256_xpm);
      wxIcon icon512(xpm512_xpm);
      icoBundle.AddIcon(icon16);
      icoBundle.AddIcon(icon32);
      icoBundle.AddIcon(icon64);
      icoBundle.AddIcon(icon96);
      icoBundle.AddIcon(icon128);
      icoBundle.AddIcon(icon512);
      SetIcons(icoBundle);
   }
}

void OsciloskopOsciloskop::onActivateApp( wxActivateEvent& event )
{
// TODO: Implement onActivateApp

}

void OsciloskopOsciloskop::onClose( wxCloseEvent& event )
{
   pOsciloscope->threadActive = false;

   // main.state save
   wxFileName fn = GetOscDataFolder();
   wxString string = fn.GetPath().append(_("/data/state/main.state"));
   SDL_RWops* ctx = SDL_RWFromFile( string.ToAscii().data(), "wb+");
   if (ctx)
   {
      SDL_RWwrite(ctx, &pOsciloscope->windowName[0], 1, 4 * sizeof(String));
      SDL_RWclose(ctx);
   }

   // save slot
   SaveSlot(0, pOsciloscope->windowName[0].asChar() );
   SaveSlot(1, pOsciloscope->windowName[1].asChar() );
   SaveSlot(2, pOsciloscope->windowName[2].asChar() );
   SaveSlot(3, pOsciloscope->windowName[3].asChar() );

   // exit
   Destroy();
   SDL_Quit();
   wxExit();
}

// wxDialog  *dlg = 0;

int SDLCALL PopulateFunction(void *data)
{
    ((OsciloskopMeasure*)data)->PopulateWithData();
    return 0;
}

void OsciloskopOsciloskop::OnIdle(wxIdleEvent& event)
{
   // reset gui
   if (pOsciloscope->callibrate.resetUI)
   {
      // slot
      wxFileName fn = GetOscDataFolder();
      wxString fileName = fn.GetPath().append(_("/data/callibrate/start.slot"));
      LoadSlot(getCurrentSlot(), fileName.ToAscii().data());
      loadWindow(getCurrentSlot());

      // callibrate
      pOsciloscope->settings.saveCallibrate();
      setupControl(pOsciloscope->window);

      // ui
      pOsciloscope->window.horizontal.Frame = 0;
      pOsciloscope->signalMode = SIGNAL_MODE_PAUSE;
      pOsciloscope->window.horizontal.Mode = SIGNAL_MODE_PAUSE;
      pOsciloscope->clearRenderTarget = 1;
      setupUI(pOsciloscope->window);

      // progress bar
      pOsciloscope->window.progress.uiActive = 1;
      pOsciloscope->window.progress.uiRange  = 0;
      pOsciloscope->window.progress.uiValue  = 0;
      pOsciloscope->window.progress.uiPulse  = 0;

      // transfer
      pOsciloscope->control.transferData();

      // exit
      pOsciloscope->callibrate.resetUI = false;
   }

   // message box
   switch (pOsciloscope->callibrate.messageBox) {
   case acmbStart:
   case acmbChannel0:
   case acmbChannel1:
   case acmbEnd:
       {
          wxString message;
          if (pOsciloscope->callibrate.messageBox == acmbChannel0) message = wxT("Cable connect generator 1(awg1) with channel 1(ch1). Then click OK to proceed.");
          if (pOsciloscope->callibrate.messageBox == acmbChannel1) message = wxT("Cable connect generator 2(awg2) with channel 2(ch2). Then click OK to proceed.");
          if (pOsciloscope->callibrate.messageBox == acmbStart)    message = wxT("This action will start automatic callibration. Disconnect all channels and generators. Click OK to proceed or Cancel to abort.");
          if (pOsciloscope->callibrate.messageBox == acmbEnd)      message = wxT("Automatic callibration was successfull. Click OK to save callibration data.");

          wxMessageDialog msgBox(this, message, wxT("Automatic Callibration"), wxCANCEL | wxOK | wxCENTRE);
          int ret = msgBox.ShowModal();
          if (ret == wxID_OK)
             pOsciloscope->callibrate.messageBox = acmbOk;
          if (ret == wxID_CANCEL)
             pOsciloscope->callibrate.messageBox = acmbCancel;
       }
       break;
   };

   // usb
   bool ret = usbIsConnected();
   if (m_checkBox26->GetValue() != ret)
      m_checkBox26->SetValue(ret);

   // progress bar
   if (pOsciloscope->display.attr.getCount())
   {
      if( pOsciloscope->display.attr.getCount() )
      {
         byte attr = pOsciloscope->display.attr[0];
         if (attr & FRAME_ATTRIBUTE_TRIGGERED_LED) m_radioBtnTriggered->SetValue(true);
         else m_radioBtnTriggered->SetValue(false);
      }
   }

   // pulse
   static int pulse = 0;
   if (pOsciloscope->window.progress.uiPulse)
   {
      if (pulse == 0)
      {
         // m_gauge1->SetIndeterminateMode();
      }
      pulse = 1;
   }
   else if (pulse == 1)
   {
      // m_gauge1->SetDeterminateMode();
      pulse = 0;
   }
   if (pulse) m_gauge1->Pulse();

   // gauge
   if (pOsciloscope->window.progress.uiActive)
   {
      m_gauge1->SetRange(pOsciloscope->window.progress.uiRange);
      m_gauge1->SetValue(pOsciloscope->window.progress.uiValue);
      pOsciloscope->window.progress.uiActive = 0;
   }

   // slider
   if (pOsciloscope->window.horizontal.uiActive)
   {
      m_sliderTimeFrame->SetMax(pOsciloscope->window.horizontal.uiRange);
      m_sliderTimeFrame->SetValue(pOsciloscope->window.horizontal.uiValue);
      m_textCtrlTimeFrame->SetValue(wxString::FromAscii(integerToString(pOsciloscope->window.horizontal.uiValue)));
      pOsciloscope->window.horizontal.uiActive = 0;
   }

   // measure
   pTimer->deltaTime(TIMER_MEASURE);
   if( pMeasure && pMeasure->IsShown() && pOsciloscope->scrollThread == false )
   {
      pMeasure->PopulateWithData();
   }

   if (pMeasure && pMeasure->IsShown() )
   {
      if (pOsciloscope->window.measure.data.pickX0.updateUI)
      {
         pOsciloscope->window.measure.data.pickX0.updateUI = false;
         pMeasure->setTextCtrlX(pMeasure->m_textCtrlX0, pMeasure->m_choiceX0, pOsciloscope->window.measure.data.pickX0.position.getXTime());
      }
      if (pOsciloscope->window.measure.data.pickX1.updateUI)
      {
         pOsciloscope->window.measure.data.pickX1.updateUI = false;
         pMeasure->setTextCtrlX(pMeasure->m_textCtrlX1, pMeasure->m_choiceX1, pOsciloscope->window.measure.data.pickX1.position.getXTime());
      }
      if (pOsciloscope->window.measure.data.pickY0.updateUI)
      {
         pOsciloscope->window.measure.data.pickY0.updateUI = false;
         pMeasure->setTextCtrlY(pMeasure->m_textCtrlY0, pOsciloscope->window.measure.data.pickY0.position.getYVolt(pMeasure->m_choiceY0->GetSelection()));
      }
      if (pOsciloscope->window.measure.data.pickY1.updateUI)
      {
         pOsciloscope->window.measure.data.pickY1.updateUI = false;
         pMeasure->setTextCtrlY(pMeasure->m_textCtrlY1, pOsciloscope->window.measure.data.pickY1.position.getYVolt(pMeasure->m_choiceY1->GetSelection()));
      }
      if (pOsciloscope->window.measure.data.pickFFT0.updateUI)
      {
         pOsciloscope->window.measure.data.pickFFT0.updateUI = false;
         pMeasure->setTextCtrlFreq(pMeasure->m_textCtrlFFT0, pMeasure->m_choiceFFT0, pOsciloscope->window.measure.data.pickFFT0.position.getXFreq());
      }
      if (pOsciloscope->window.measure.data.pickFFT1.updateUI)
      {
         pOsciloscope->window.measure.data.pickFFT1.updateUI = false;
         pMeasure->setTextCtrlFreq(pMeasure->m_textCtrlFFT1, pMeasure->m_choiceFFT1, pOsciloscope->window.measure.data.pickFFT1.position.getXFreq());
      }
   }

   // debug
   if( pDebug && pDebug->IsShown() )
   {
      if( pOsciloscope->signalMode != SIGNAL_MODE_PAUSE )
      {
          pDebug->m_textCtrl41->Clear();
          int count = min(32,pOsciloscope->display.debug.getCount());
          FORMAT_BUFFER();
          for( int i=0;i<count/2;i++)
          {
             ushort wordToPrint = ( ushort(pOsciloscope->display.debug[2*i]) << 8) | pOsciloscope->display.debug[2*i+1];
             FORMAT("%d HEX:%04x DEC:%04d \n",i,wordToPrint,wordToPrint);
             pDebug->m_textCtrl41->AppendText( wxString::FromAscii( formatBuffer ) );
          }
      }
   }

   if(pOsciloscope->settings.threadDelayIdle>0)
   {
      SDL_Delay(pOsciloscope->settings.threadDelayIdle);
      event.Skip();
      event.RequestMore();
   }
}

void OsciloskopOsciloskop::OnSetFocus( wxFocusEvent& event )
{
// TODO: Implement OnSetFocus
}

void maximizeOscWindow()
{
   // get top level window
   wxApp* pApp = (wxApp*)wxApp::GetInstance();
   wxFrame * topwindow = (wxFrame*)pApp->GetTopWindow();

   // window width & height
   int displayWidth = 0;
   int displayHeight = 0;
   wxDisplaySize(&displayWidth, &displayHeight);
   float osc = float(displayWidth) / (1.5f);
   float ogl = osc*0.5f;

   // border
   wxRect rect3 = topwindow->GetClientRect();
   wxRect rect4 = topwindow->GetScreenRect();
   int  borderX = (rect4.width - rect3.width) / 2;
   int  borderY = (rect4.height - rect3.height) / 2;

   // size & pos
   topwindow->SetSize(wxSize(osc-borderX, displayHeight));
   topwindow->SetPosition(wxPoint(ogl+borderX, 0));
}

void maximizeOglWindow(int &outW,int &outH)
{
   // get top level window
   wxApp* pApp = (wxApp*)wxApp::GetInstance();
   wxFrame * topwindow = (wxFrame*)pApp->GetTopWindow();

   // size
   int displayWidth = 0;
   int displayHeight = 0;
   wxDisplaySize(&displayWidth, &displayHeight);
   float osc = float(displayWidth) / (1.5f);
   float ogl = osc*0.5f;

   // border
   wxRect rect3 = topwindow->GetClientRect();
   wxRect rect4 = topwindow->GetScreenRect();
   int  borderX = (rect4.width - rect3.width) / 2;
   int  borderY = (rect4.height - rect3.height) / 2;

   // ogl
   outW = ogl - borderX;
   outH = displayHeight - borderY - borderX;
   SDL_SetWindowSize(pOsciloscope->sdlWindow, ogl - borderX, displayHeight - borderY - borderX);
   SDL_SetWindowPosition(pOsciloscope->sdlWindow, borderX, borderY);
}

void OsciloskopOsciloskop::OnSize( wxSizeEvent& event )
{
// TODO: Implement OnSize
   this->SetSize(event.GetSize().GetX(), event.GetSize().GetY());
   this->Layout();
   this->Refresh();
}

void OsciloskopOsciloskop::m_menuItem1OnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItem1OnMenuSelection
   wxFileDialog *LoadDialog = new wxFileDialog( this, _("Load File As _?"), wxEmptyString, wxEmptyString,
                                                _("*.osc"),
                                                wxFD_OPEN, wxDefaultPosition );

	// Creates a Save Dialog with 4 file types
	if (LoadDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
	{
		String file = LoadDialog->GetPath().ToAscii().data();
		pOsciloscope->loadFromFile( file.asChar() );
	}

	// Clean up after ourselves
	LoadDialog->Destroy();
	// delete LoadDialog;
}

void OsciloskopOsciloskop::m_menuItem2OnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItem2OnMenuSelection
   wxFileDialog *SaveDialog = new wxFileDialog( this, _("Save File As _?"), wxEmptyString, wxEmptyString,
                                                _("*.osc"),
                                                wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition );

	// Creates a Save Dialog with 4 file types
	if (SaveDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
	{
		String file = SaveDialog->GetPath().ToAscii().data();
		pOsciloscope->saveToFile( file.asChar() );
	}

	// Clean up after ourselves
    // delete 	SaveDialog;
	SaveDialog->Destroy();
}

void OsciloskopOsciloskop::m_menuItem3OnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItem3OnMenuSelection
    Close(true);
}

void OsciloskopOsciloskop::m_menuItem20OnMenuSelection(wxCommandEvent& event)
{
   pOsciloscope->window.fftDigital.raise(VIEW_SELECT_OSC_3D);
   pOsciloscope->window.fftDigital.lower(VIEW_SELECT_OSC_2D);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(0)->Check(1);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Check(0);
   pOsciloscope->oscCameraSetup(1);
}

void OsciloskopOsciloskop::m_menuItem21OnMenuSelection(wxCommandEvent& event)
{
   pOsciloscope->window.fftDigital.lower(VIEW_SELECT_OSC_3D);
   pOsciloscope->window.fftDigital.raise(VIEW_SELECT_OSC_2D);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(0)->Check(0);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Check(1);
   pOsciloscope->oscCameraSetup(0);
}

void OsciloskopOsciloskop::m_menuItem22OnMenuSelection(wxCommandEvent& event)
{
   pOsciloscope->window.fftDigital.raise(VIEW_SELECT_FFT_3D);
   pOsciloscope->window.fftDigital.lower(VIEW_SELECT_FFT_2D);
   pOsciloscope->window.fftDigital.lower(VIEW_SELECT_DIGITAL);

   GetMenuBar()->GetMenu(1)->FindItemByPosition(2)->Check(1);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(3)->Check(0);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(4)->Check(0);

   pOsciloscope->fftCameraSetup(1);
}

void OsciloskopOsciloskop::m_menuItem23OnMenuSelection(wxCommandEvent& event)
{
   pOsciloscope->window.fftDigital.lower(VIEW_SELECT_FFT_3D);
   pOsciloscope->window.fftDigital.raise(VIEW_SELECT_FFT_2D);
   pOsciloscope->window.fftDigital.lower(VIEW_SELECT_DIGITAL);

   GetMenuBar()->GetMenu(1)->FindItemByPosition(2)->Check(0);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(3)->Check(1);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(4)->Check(0);

   pOsciloscope->fftCameraSetup(0);
}

void OsciloskopOsciloskop::m_menuItem6OnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItem6OnMenuSelection
   pOsciloscope->window.fftDigital.lower(VIEW_SELECT_FFT_3D);
   pOsciloscope->window.fftDigital.lower(VIEW_SELECT_FFT_2D);
   pOsciloscope->window.fftDigital.raise(VIEW_SELECT_DIGITAL);

   GetMenuBar()->GetMenu(1)->FindItemByPosition(2)->Check(0);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(3)->Check(0);
   GetMenuBar()->GetMenu(1)->FindItemByPosition(4)->Check(1);

   pOsciloscope->fftCameraSetup(0);
}

void OsciloskopOsciloskop::m_menuItem8OnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItem8OnMenuSelection
   static OsciloskopDisplay* pDisplay = 0;
   if(!pDisplay)
      pDisplay = new OsciloskopDisplay(this);
   pDisplay->Show();
}

void OsciloskopOsciloskop::m_menuItem9OnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItem9OnMenuSelection
   static OsciloskopThermal* pThermal = 0;
   if(!pThermal)
      pThermal = new OsciloskopThermal(this);
   pThermal->Show();
}


void OsciloskopOsciloskop::m_menuItemSoftwareOnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItemSoftwareOnMenuSelection
   static OsciloskopSoftwareGenerator* pSoftwareGenerator = 0;
   if(!pSoftwareGenerator)
      pSoftwareGenerator = new OsciloskopSoftwareGenerator(this);
   pSoftwareGenerator->Show();
}

void OsciloskopOsciloskop::m_menuItem11OnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItem11OnMenuSelection
   static OsciloskopHardwareGenerator* pHardwareGenerator = 0;
   if(!pHardwareGenerator)
      pHardwareGenerator = new OsciloskopHardwareGenerator(this);
   pHardwareGenerator->Show();
}

void OsciloskopOsciloskop::m_menuItem12OnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItem12OnMenuSelection
   pOsciloscope->uploadFirmware();
}

void OsciloskopOsciloskop::m_menuItem15OnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItem15OnMenuSelection
   if(!pMeasure)
      pMeasure = new OsciloskopMeasure( this );
   pMeasure->Show();
}

void recreateGUI(int initial=0);
void saveLanguageToConfig(int id);
void setLocalization(int id);

void OsciloskopOsciloskop::m_menuItemSloveneOnMenuSelection(wxCommandEvent& event)
{
   setLocalization(wxLANGUAGE_SLOVENIAN);
   saveLanguageToConfig(wxLANGUAGE_SLOVENIAN);
   recreateGUI();
}

void OsciloskopOsciloskop::m_menuItemEnglishOnMenuSelection(wxCommandEvent& event)
{
   setLocalization(wxLANGUAGE_ENGLISH);
   saveLanguageToConfig(wxLANGUAGE_ENGLISH);
   recreateGUI();
}

class wxLanguageUserData : public wxObject
{
   wxDECLARE_ABSTRACT_CLASS(wxLanguageUserData);
public: wxLanguage data;
};



void OsciloskopOsciloskop::MenuLanguageSelection( wxCommandEvent& event )
{
   // #define RECREATE_GUI

   #if !defined(RECREATE_GUI)

   wxMessageDialog msgBox(this, wxT("Application needs to close. Continue?"), _T("Changing language"), wxCANCEL | wxOK | wxCENTRE);
   int ret = msgBox.ShowModal();
   if (ret == wxID_OK)

   #endif
   {
      wxLanguage langId = ((wxLanguageUserData*)(event.m_callbackUserData))->data;
      setLocalization(  langId );
      saveLanguageToConfig(langId);

      #if defined(RECREATE_GUI)
         recreateGUI();
      #else
         wxCloseEvent evnt;
         onClose(evnt);
      #endif
   }
}

void OsciloskopOsciloskop::m_menuItemDebugOnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItemDebugOnMenuSelection
   if(!pDebug)
      pDebug = new OsciloskopDebug( this  );
   pDebug->Show();
}

void OsciloskopOsciloskop::m_menuItemInfoOnMenuSelection( wxCommandEvent& event )
{
// TODO: Implement m_menuItemInfoOnMenuSelection
   if(!pInfo)
      pInfo = new OsciloskopInfo( this  );
   pInfo->Show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Time
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_comboBoxTimeCaptureOnCombobox(wxCommandEvent& event)
{
   // TODO: Implement m_comboBoxTimeCaptureOnCombobox
   pOsciloscope->window.horizontal.Capture = captureTimeFromEnum(m_comboBoxTimeCapture->GetSelection());
   pOsciloscope->control.setXRange(m_comboBoxTimeCapture->GetSelection());
   pOsciloscope->control.transferData();
   //
   pOsciloscope->window.horizontal.Display = pOsciloscope->window.horizontal.Capture;
   uint  multiEnum = multiplyerFromValue(pOsciloscope->window.horizontal.Display);
   float multiFloat = multiplyerFromEnum(multiEnum);
   m_comboBoxTimeDisplay->SetSelection(multiEnum);
   m_textCtrlTimeDisplay->SetValue(wxString::FromAscii(floatToString(pOsciloscope->window.horizontal.Display / multiFloat)));

   pOsciloscope->window.measure.ClearCapture();
}

void OsciloskopOsciloskop::m_checkBoxETSOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxETSOnCheckBox
   pOsciloscope->window.horizontal.ETS = m_checkBoxETS->GetValue();
   pOsciloscope->control.setEts( pOsciloscope->window.horizontal.ETS );
   pOsciloscope->control.transferData();
   pOsciloscope->redrawEts = 1;
   pOsciloscope->clearRenderTarget = 1;
}

void OsciloskopOsciloskop::m_textCtrlTimeDisplayOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlTimeDisplayOnTextEnter
   pOsciloscope->window.horizontal.Display = stringToFloat( m_textCtrlTimeDisplay->GetValue().ToAscii().data() )*multiplyerFromEnum( m_comboBoxTimeDisplay->GetSelection() );
}

void OsciloskopOsciloskop::m_comboBoxTimeDisplayOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxTimeDisplayOnCombobox
   pOsciloscope->window.horizontal.Display = stringToFloat( m_textCtrlTimeDisplay->GetValue().ToAscii().data() )*multiplyerFromEnum( m_comboBoxTimeDisplay->GetSelection() );
}

void OsciloskopOsciloskop::m_comboBoxTimeControlOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxTimeControlOnCombobox
   pOsciloscope->window.horizontal.Control = m_comboBoxTimeControl->GetSelection();
   pOsciloscope->control.setControl( pOsciloscope->window.horizontal.Control );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_textCtrlTimePositionOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlTimePositionOnTextEnter
   pOsciloscope->clearThermal = 1;
   pOsciloscope->window.horizontal.Position = stringToFloat( m_textCtrlTimePosition->GetValue().ToAscii().data() );
   m_sliderTimePosition->SetValue( pOsciloscope->window.horizontal.Position );
}

void OsciloskopOsciloskop::m_sliderTimePositionOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderTimePositionOnScroll
   pOsciloscope->clearThermal = 1;
   pOsciloscope->window.horizontal.Position = m_sliderTimePosition->GetValue();
   m_textCtrlTimePosition->SetValue( wxString::FromAscii( floatToString( pOsciloscope->window.horizontal.Position ) ));
}

void OsciloskopOsciloskop::m_textCtrlTimeFrameSizeOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlTimeFrameSizeOnTextEnter
   pOsciloscope->control.setFrameSize( atoi( m_textCtrlTimeFrameSize->GetValue().ToAscii().data() ) );
   pOsciloscope->window.horizontal.FrameSize = pOsciloscope->control.getFrameSize();
   pOsciloscope->window.horizontal.FFTSize   = pOsciloscope->window.horizontal.FrameSize;
   m_textCtrlTimeFFTSize->SetValue( wxString::FromAscii( integerToString(pOsciloscope->window.horizontal.FFTSize ) ) );
   m_textCtrlTimeFrameSize->SetValue( wxString::FromAscii( integerToString( pOsciloscope->control.getFrameSize() ) ) );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_textCtrlTimeFrameOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlTimeFrameOnTextEnter
   pOsciloscope->window.horizontal.Frame = stringToFloat( m_textCtrlTimeFrame->GetValue().ToAscii().data() );
   m_sliderTimeFrame->SetValue( pOsciloscope->window.horizontal.Frame );
   pOsciloscope->clearRenderTarget = 1;
}

void OsciloskopOsciloskop::m_sliderTimeFrameOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderTimeFrameOnScroll
   pOsciloscope->window.horizontal.Frame = m_sliderTimeFrame->GetValue();
   m_textCtrlTimeFrame->SetValue( wxString::FromAscii( floatToString( pOsciloscope->window.horizontal.Frame ) ) );
   // pOsciloscope->signal.changeFrame( pOsciloscope->window.horizontal.Frame );

   pOsciloscope->window.horizontal.Frame = pOsciloscope->window.horizontal.Frame;
   pOsciloscope->clearRenderTarget = 1;
}

void OsciloskopOsciloskop::m_textCtrlTimeFFTSizeOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlTimeFFTSizeOnTextEnter
   iint fftsize = atoi( m_textCtrlTimeFFTSize->GetValue().ToAscii().data() );
   fftsize = min( fftsize, NUM_FFT );
   pOsciloscope->window.horizontal.FFTSize = fftsize;
   m_textCtrlTimeFFTSize->SetValue( wxString::FromAscii( integerToString(fftsize)) );
}

void OsciloskopOsciloskop::m_radioBoxTimeModeOnRadioBox( wxCommandEvent& event )
{
// TODO: Implement m_radioBoxTimeModeOnRadioBox
   pOsciloscope->window.horizontal.Mode = m_radioBoxTimeMode->GetSelection();
   pOsciloscope->signalMode = (SignalMode)pOsciloscope->window.horizontal.Mode;
   // pOsciloscope->control.transferData();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Channel 0
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void _setYDisplay(float &volt,uint &unit,VoltageCapture selected)
{
   switch( selected ) {
   case  vc2Volt  : volt=2.f;   unit = (uint)vVolt; break;
   case  vc1Volt  : volt=1.f;   unit = (uint)vVolt; break;
   case  vc500Mili: volt=500.f; unit = (uint)vMili; break;
   case  vc200Mili: volt=200.f; unit = (uint)vMili; break;
   case  vc100Mili: volt=100.f; unit = (uint)vMili; break;
   case  vc50Mili : volt=50.f;  unit = (uint)vMili; break;
   case  vc20Mili : volt=20.f;  unit = (uint)vMili; break;
   case  vc10Mili : volt=10.f;  unit = (uint)vMili; break;
   default:
      CORE_ERROR("invalid volt value",0);
   };
}

void OsciloskopOsciloskop::m_comboBoxCh0CaptureOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxCh0CaptureOnCombobox

   double oldTriggerVoltagePerSteps = pOsciloscope->getTriggerVoltagePerStep();

   pOsciloscope->window.channel01.Capture = captureVoltFromEnum( m_comboBoxCh0Capture->GetSelection() );
   pOsciloscope->window.channel01.Scale   = stringToFloat( m_textCtrlCh0Scale->GetValue().ToAscii().data() );
   pOsciloscope->window.channel01.Display = pOsciloscope->window.channel01.Capture;
   pOsciloscope->control.setYRangeScaleA( m_comboBoxCh0Capture->GetSelection(), pOsciloscope->window.channel01.Scale );

   float volt;
   uint  unit;
   _setYDisplay( volt, unit, (VoltageCapture)m_comboBoxCh0Capture->GetSelection() );
   m_textCtrlCh0Display->SetValue( wxString::FromAscii( floatToString(volt) ) );
   m_comboBoxCh0Display->SetSelection( unit );

   double newTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();


   RecalculateTriggerPosition(oldTriggerVoltagePerSteps, newTriggerVoltagePerStep);

   pOsciloscope->control.setYPositionA(pOsciloscope->window.channel01.YPosition + pOsciloscope->settings.analogChannelOffsets[0][captureVoltFromValue(pOsciloscope->window.channel01.Capture)]);

   // transfer
   pOsciloscope->control.transferData();

   pOsciloscope->window.measure.ClearCapture();
}

void OsciloskopOsciloskop::m_textCtrlCh0ScaleOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlCh0ScaleOnTextEnter
   pOsciloscope->window.channel01.Scale = stringToFloat( m_textCtrlCh0Scale->GetValue().ToAscii().data() );
   pOsciloscope->control.setYRangeScaleA( m_comboBoxCh0Capture->GetSelection(), pOsciloscope->window.channel01.Scale );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_checkBoxFFT1OnCheckBox(wxCommandEvent& event)
{
   pOsciloscope->window.channel01.FFTOnOff = m_checkBoxFFT1->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxSignal1OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_radioBoxCh0OnOffOnRadioBox
   pOsciloscope->window.channel01.OscOnOff = m_checkBoxSignal1->IsChecked() ? 1 : 0;
   /*if( pOsciloscope->window.channel01.OnOff  ) pOsciloscope->control.analogswitch = raiseFlag16( pOsciloscope->control.analogswitch, CHANNEL_A );
   else                                        pOsciloscope->control.analogswitch = lowerFlag16( pOsciloscope->control.analogswitch, CHANNEL_A );
   pOsciloscope->control.transferData();*/
}

void OsciloskopOsciloskop::m_textCtrlCh0DisplayOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlCh0DisplayOnTextEnter
   pOsciloscope->window.channel01.Display = stringToFloat( m_textCtrlCh0Display->GetValue().ToAscii().data() ) * multiplyerFromEnum( m_comboBoxCh0Display->GetSelection() );
}

void OsciloskopOsciloskop::m_comboBoxCh0DisplayOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxCh0DisplayOnCombobox
   pOsciloscope->window.channel01.Display = stringToFloat( m_textCtrlCh0Display->GetValue().ToAscii().data() ) * multiplyerFromEnum( m_comboBoxCh0Display->GetSelection() );
}

void OsciloskopOsciloskop::m_checkBoxCh0InvertOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxCh0InvertOnCheckBox
   pOsciloscope->window.channel01.Invert = m_checkBoxCh0Invert->GetValue();
}

void OsciloskopOsciloskop::m_checkBoxCh0GroundOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxCh0GroundOnCheckBox
   pOsciloscope->window.channel01.Ground = m_checkBoxCh0Ground->GetValue();
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_A_GROUND, m_checkBoxCh0Ground->GetValue() );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_choiceCh0ACDCOnChoice( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxCh0ACDCOnCheckBox
   pOsciloscope->window.channel01.AcDc = m_choiceCh0ACDC->GetSelection();
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_A_ACDC, m_choiceCh0ACDC->GetSelection() );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_textCtrlCh0PositionOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlCh0PositionOnTextEnter
   VoltageCapture  capture = (VoltageCapture)captureVoltFromValue(pOsciloscope->window.channel01.Capture);
   int   position = stringToDouble(m_textCtrlCh0Position->GetValue().ToAscii().data()) / pOsciloscope->settings.getAnalogChannelYVoltageStep(0,capture);
   pOsciloscope->window.channel01.YPosition = position;
   pOsciloscope->control.setYPositionA( position + pOsciloscope->settings.analogChannelOffsets[0][capture] );
   pOsciloscope->control.transferData();

   m_sliderCh0Position->SetValue( position );
}

void OsciloskopOsciloskop::m_sliderCh0PositionOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderCh0PositionOnScroll
   VoltageCapture   capture = (VoltageCapture)captureVoltFromValue(pOsciloscope->window.channel01.Capture);

   int position = m_sliderCh0Position->GetValue();
   pOsciloscope->window.channel01.YPosition = position;
   pOsciloscope->control.setYPositionA( position + pOsciloscope->settings.analogChannelOffsets[0][capture]);
   pOsciloscope->control.transferData();

   double voltageY = double(position) * pOsciloscope->settings.getAnalogChannelYVoltageStep(0,capture);
   m_textCtrlCh0Position->SetValue( wxString::FromAscii( doubleToString(voltageY) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Channel 1
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_comboBoxCh1CaptureOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxCh1CaptureOnCombobox
   double oldTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();

   pOsciloscope->window.channel02.Capture = captureVoltFromEnum( m_comboBoxCh1Capture->GetSelection() );
   pOsciloscope->window.channel02.Scale   = stringToFloat( m_textCtrlCh1Scale->GetValue().ToAscii().data() );
   pOsciloscope->window.channel02.Display = pOsciloscope->window.channel02.Capture;
   pOsciloscope->control.setYRangeScaleB( m_comboBoxCh1Capture->GetSelection(), pOsciloscope->window.channel02.Scale );
   //
   float volt;
   uint  unit;
   _setYDisplay( volt, unit, (VoltageCapture)m_comboBoxCh1Capture->GetSelection() );
   //
   m_textCtrlCh1Display->SetValue( wxString::FromAscii( floatToString(volt) ));
   m_comboBoxCh1Display->SetSelection( unit );
   //
   double newTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();

   RecalculateTriggerPosition(oldTriggerVoltagePerStep, newTriggerVoltagePerStep);

   pOsciloscope->control.setYPositionB( pOsciloscope->window.channel02.YPosition + pOsciloscope->settings.analogChannelOffsets[1][captureVoltFromValue(pOsciloscope->window.channel02.Capture)]);

   pOsciloscope->control.transferData();

   pOsciloscope->window.measure.ClearCapture();
}

void OsciloskopOsciloskop::m_textCtrlCh1ScaleOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlCh1ScaleOnTextEnter
   pOsciloscope->window.channel02.Scale = stringToFloat( m_textCtrlCh1Scale->GetValue().ToAscii().data() );
   pOsciloscope->control.setYRangeScaleB( m_comboBoxCh1Capture->GetSelection(), pOsciloscope->window.channel02.Scale );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_checkBoxFFT2OnCheckBox(wxCommandEvent& event)
{
   pOsciloscope->window.channel02.FFTOnOff = m_checkBoxFFT2->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxSignal2OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_radioBoxCh1OnOffOnRadioBox
   pOsciloscope->window.channel02.OscOnOff = m_checkBoxSignal2->IsChecked() ? 1 : 0;
   /*if( pOsciloscope->window.channel02.OnOff  ) pOsciloscope->control.analogswitch = raiseFlag16( pOsciloscope->control.analogswitch, CHANNEL_B );
   else                                        pOsciloscope->control.analogswitch = lowerFlag16( pOsciloscope->control.analogswitch, CHANNEL_B );
   pOsciloscope->control.transferData();*/
}

void OsciloskopOsciloskop::m_textCtrlCh1DisplayOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlCh1DisplayOnTextEnter
   pOsciloscope->window.channel02.Display = stringToFloat( m_textCtrlCh1Display->GetValue().ToAscii().data() ) * multiplyerFromEnum( m_comboBoxCh1Display->GetSelection() );
}

void OsciloskopOsciloskop::m_comboBoxCh1DisplayOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxCh1DisplayOnCombobox
   pOsciloscope->window.channel02.Display = stringToFloat( m_textCtrlCh1Display->GetValue().ToAscii().data() ) * multiplyerFromEnum( m_comboBoxCh1Display->GetSelection() );
}

void OsciloskopOsciloskop::m_checkBoxCh1InvertOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxCh1InvertOnCheckBox
   pOsciloscope->window.channel02.Invert = m_checkBoxCh1Invert->GetValue();
}

void OsciloskopOsciloskop::m_checkBoxCh1GroundOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxCh1GroundOnCheckBox
   pOsciloscope->window.channel02.Ground = m_checkBoxCh1Ground->GetValue();
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_B_GROUND, m_checkBoxCh1Ground->GetValue());
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_choiceCh1ACDCOnChoice( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxCh1ACDCOnCheckBox
   pOsciloscope->window.channel02.AcDc = m_choiceCh1ACDC->GetSelection();
   pOsciloscope->control.setAnalogSwitchBit(CHANNEL_B_ACDC, m_choiceCh1ACDC->GetSelection() );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_textCtrlCh1PositionOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlCh1PositionOnTextEnter
   VoltageCapture  capture = (VoltageCapture)captureVoltFromValue(pOsciloscope->window.channel02.Capture);
   int   position = stringToDouble(m_textCtrlCh1Position->GetValue().ToAscii().data()) / pOsciloscope->settings.getAnalogChannelYVoltageStep(1,capture);
   pOsciloscope->window.channel02.YPosition = position;
   pOsciloscope->control.setYPositionB( position + pOsciloscope->settings.analogChannelOffsets[1][capture]);
   pOsciloscope->control.transferData();
   //
   m_sliderCh1Position->SetValue( position );
}

void OsciloskopOsciloskop::m_sliderCh1PositionOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderCh1PositionOnScroll
   VoltageCapture  capture = (VoltageCapture)captureVoltFromValue(pOsciloscope->window.channel02.Capture);

   int position = m_sliderCh1Position->GetValue();
   pOsciloscope->window.channel02.YPosition = position;
   pOsciloscope->control.setYPositionB( position + pOsciloscope->settings.analogChannelOffsets[1][capture]);
   pOsciloscope->control.transferData();

   double voltageY = double(position) * pOsciloscope->settings.getAnalogChannelYVoltageStep(1,capture);
   m_textCtrlCh1Position->SetValue(wxString::FromAscii(doubleToString(voltageY)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void OsciloskopOsciloskop::m_comboBoxFunctionTypeOnChoice( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxFunctionTypeOnChoice
   pOsciloscope->window.function.Type = m_comboBoxFunction->GetSelection();
}

void OsciloskopOsciloskop::m_checkBoxXYVoltageGraphOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.function.xyGraph = m_checkBoxXYVoltageGraph->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxFFTFOnCheckBox(wxCommandEvent& event)
{
   pOsciloscope->window.function.FFTOnOff = m_checkBoxFFTF->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxSignalFOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_radioBoxFunctionOffOnOnRadioBox
   pOsciloscope->window.function.OscOnOff = m_checkBoxSignalF->IsChecked() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Digital
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_checkBox1OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox1OnCheckBox
   pOsciloscope->window.digital.digital[0] = m_checkBox1->GetValue();
}

void OsciloskopOsciloskop::m_checkBox2OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox2OnCheckBox
   pOsciloscope->window.digital.digital[1] = m_checkBox2->GetValue();
}

void OsciloskopOsciloskop::m_checkBox3OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox3OnCheckBox
   pOsciloscope->window.digital.digital[2] = m_checkBox3->GetValue();
}

void OsciloskopOsciloskop::m_checkBox4OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox4OnCheckBox
   pOsciloscope->window.digital.digital[3] = m_checkBox4->GetValue();
}

void OsciloskopOsciloskop::m_checkBox5OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox5OnCheckBox
   pOsciloscope->window.digital.digital[4] = m_checkBox5->GetValue();
}

void OsciloskopOsciloskop::m_checkBox6OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox6OnCheckBox
   pOsciloscope->window.digital.digital[5] = m_checkBox6->GetValue();
}

void OsciloskopOsciloskop::m_checkBox7OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox7OnCheckBox
   pOsciloscope->window.digital.digital[6] = m_checkBox7->GetValue();
}

void OsciloskopOsciloskop::m_checkBox8OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox8OnCheckBox
   pOsciloscope->window.digital.digital[7] = m_checkBox8->GetValue();
}

void OsciloskopOsciloskop::m_checkBox9OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox9OnCheckBox
   pOsciloscope->window.digital.digital[8] = m_checkBox9->GetValue();
}

void OsciloskopOsciloskop::m_checkBox10OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox10OnCheckBox
   pOsciloscope->window.digital.digital[9] = m_checkBox10->GetValue();
}

void OsciloskopOsciloskop::m_checkBox11OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox11OnCheckBox
   pOsciloscope->window.digital.digital[10] = m_checkBox11->GetValue();
}

void OsciloskopOsciloskop::m_checkBox12OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox12OnCheckBox
  pOsciloscope->window.digital.digital[11] = m_checkBox12->GetValue();
}

void OsciloskopOsciloskop::m_checkBox13OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox13OnCheckBox
   pOsciloscope->window.digital.digital[12] = m_checkBox13->GetValue();
}

void OsciloskopOsciloskop::m_checkBox14OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox14OnCheckBox
   pOsciloscope->window.digital.digital[13] = m_checkBox14->GetValue();
}

void OsciloskopOsciloskop::m_checkBox15OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox15OnCheckBox
   pOsciloscope->window.digital.digital[14] = m_checkBox15->GetValue();
}

void OsciloskopOsciloskop::m_checkBox16OnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBox16OnCheckBox
   pOsciloscope->window.digital.digital[15] = m_checkBox16->GetValue();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Trigger Analog
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_comboBoxTriggerSourceOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxTriggerSourceOnCombobox
   double oldTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();

   pOsciloscope->window.trigger.Source = m_comboBoxTriggerSource->GetSelection();
   pOsciloscope->control.setTriggerSource( m_comboBoxTriggerSource->GetSelection() );

   double newTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();

   RecalculateTriggerPosition( oldTriggerVoltagePerStep, newTriggerVoltagePerStep );

   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_comboBoxTriggerSlopeOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxTriggerSlopeOnCombobox
   pOsciloscope->window.trigger.Slope = m_comboBoxTriggerSlope->GetSelection();
   pOsciloscope->control.setTriggerSlope( m_comboBoxTriggerSlope->GetSelection() );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_comboBoxTriggerOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxTriggerOnCombobox
   pOsciloscope->window.trigger.Mode = m_comboBoxTrigger->GetSelection();
   pOsciloscope->control.setTriggerMode( m_comboBoxTrigger->GetSelection() );
   if(m_comboBoxTrigger->GetSelection() == 3 )
   {
      pOsciloscope->window.trigger.Percent  = 0.f;
      pOsciloscope->control.setTriggerPre( pOsciloscope->window.trigger.Percent );
      m_textCtrlTriggerPre->SetValue( wxString::FromAscii( floatToString( pOsciloscope->window.trigger.Percent ) ) );
      m_sliderTriggerPre->SetValue( pOsciloscope->window.trigger.Percent );
      m_textCtrlTriggerPre->Disable();
      m_sliderTriggerPre->Disable();
   }
   else
   {
      m_textCtrlTriggerPre->Enable();
      m_sliderTriggerPre->Enable();
   }

   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_textCtrlTriggerLevelOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlTriggerLevelOnTextEnter
   double  triggerLevel = stringToDouble( m_textCtrlTriggerLevel->GetValue().ToAscii().data() );
   int    numberOfSteps = int( triggerLevel / pOsciloscope->getTriggerVoltagePerStep() );
   pOsciloscope->window.trigger.Level = numberOfSteps;
   pOsciloscope->control.setTriggerLevel( numberOfSteps );
   pOsciloscope->control.transferData();

   m_sliderTriggerLevel->SetValue( numberOfSteps );
}

void OsciloskopOsciloskop::m_sliderTriggerLevelOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderTriggerLevelOnScroll
   pOsciloscope->window.trigger.Level = m_sliderTriggerLevel->GetValue();
   pOsciloscope->control.setTriggerLevel( m_sliderTriggerLevel->GetValue() );
   pOsciloscope->control.transferData();

   double triggerLevel = pOsciloscope->getTriggerVoltagePerStep() * double(pOsciloscope->window.trigger.Level);
   m_textCtrlTriggerLevel->SetValue( wxString::FromAscii( doubleToString(triggerLevel) ) );
}

void OsciloskopOsciloskop::m_textCtrlTriggerHisteresisOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlTriggerHisteresisOnTextEnter
   double  hisLevel = stringToDouble(m_textCtrlTriggerHisteresis->GetValue().ToAscii().data());
   int    numberOfSteps = int(hisLevel / pOsciloscope->getTriggerVoltagePerStep());
   pOsciloscope->window.trigger.His = numberOfSteps;
   pOsciloscope->control.setTriggerHis(numberOfSteps);
   pOsciloscope->control.transferData();
   //
   m_sliderTriggerHisteresis->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_sliderTriggerHisteresisOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderTriggerHisteresisOnScroll
   pOsciloscope->window.trigger.His = m_sliderTriggerHisteresis->GetValue();
   pOsciloscope->control.setTriggerHis( m_sliderTriggerHisteresis->GetValue() );
   pOsciloscope->control.transferData();

   double hisLevel = pOsciloscope->getTriggerVoltagePerStep() * double(pOsciloscope->window.trigger.His);
   m_textCtrlTriggerHisteresis->SetValue( wxString::FromAscii( doubleToString(hisLevel) ));
}

void OsciloskopOsciloskop::m_textCtrlTriggerPreOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlTriggerPreOnTextEnter
   double preTrigger = max( stringToFloat( m_textCtrlTriggerPre->GetValue().ToAscii().data() ), 0.f );
   pOsciloscope->window.trigger.Percent = preTrigger;
   pOsciloscope->control.setTriggerPre( preTrigger );
   pOsciloscope->control.transferData();

   pOsciloscope->window.measure.data.pickX0.updateUI = true;
   pOsciloscope->window.measure.data.pickX1.updateUI = true;

   m_sliderTriggerPre->SetValue( pOsciloscope->window.trigger.Percent );
   m_textCtrlTriggerPre->SetValue( wxString::FromDouble( preTrigger ) );
}

void OsciloskopOsciloskop::m_sliderTriggerPreOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderTriggerPreOnScroll
   pOsciloscope->window.trigger.Percent = max( m_sliderTriggerPre->GetValue(), 0 );
   pOsciloscope->control.setTriggerPre( m_sliderTriggerPre->GetValue() );
   pOsciloscope->control.transferData();

   pOsciloscope->window.measure.data.pickX0.updateUI = true;
   pOsciloscope->window.measure.data.pickX1.updateUI = true;

   m_textCtrlTriggerPre->SetValue( wxString::FromAscii( floatToString( pOsciloscope->window.trigger.Percent ) ));
}

void OsciloskopOsciloskop::m_textCtrlTriggerHoldoffOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlTriggerHoldoffOnTextEnter
   pOsciloscope->window.trigger.Holdoff = stringToDouble( m_textCtrlTriggerHoldoff->GetValue().ToAscii().data() );
   pOsciloscope->control.setHoldoff( stringToDouble( m_textCtrlTriggerHoldoff->GetValue().ToAscii().data() ) );
   pOsciloscope->control.transferData();
   //
   m_sliderTriggerHoldoff->SetValue( pOsciloscope->window.trigger.Holdoff );
}

void OsciloskopOsciloskop::m_sliderTriggerHoldoffOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderTriggerHoldoffOnScroll
   pOsciloscope->window.trigger.Holdoff = m_sliderTriggerHoldoff->GetValue();
   pOsciloscope->control.setHoldoff( m_sliderTriggerHoldoff->GetValue() );
   pOsciloscope->control.transferData();
   //
   m_textCtrlTriggerHoldoff->SetValue( wxString::FromAscii(doubleToString( pOsciloscope->window.trigger.Holdoff ) ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Digital Trigger
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_comboBoxDigitalStageOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxDigitalStageOnCombobox
   int stage = m_comboBoxDigitalStage->GetSelection();

   pOsciloscope->window.trigger.stage = stage;

   ushort delay = pOsciloscope->window.trigger.delay[stage];
   m_textCtrDigitallDelay->SetValue(wxString::FromAscii(integerToString(delay)));

   m_comboBoxBit0->SetSelection( pOsciloscope->window.trigger.pattern[stage][0]  );
   m_comboBoxBit1->SetSelection( pOsciloscope->window.trigger.pattern[stage][1]  );
   m_comboBoxBit2->SetSelection( pOsciloscope->window.trigger.pattern[stage][2]  );
   m_comboBoxBit3->SetSelection( pOsciloscope->window.trigger.pattern[stage][3]  );
   m_comboBoxBit4->SetSelection( pOsciloscope->window.trigger.pattern[stage][4]  );
   m_comboBoxBit5->SetSelection( pOsciloscope->window.trigger.pattern[stage][5]  );
   m_comboBoxBit6->SetSelection( pOsciloscope->window.trigger.pattern[stage][6]  );
   m_comboBoxBit7->SetSelection( pOsciloscope->window.trigger.pattern[stage][7]  );
   m_comboBoxBit8->SetSelection( pOsciloscope->window.trigger.pattern[stage][8]  );
   m_comboBoxBit9->SetSelection( pOsciloscope->window.trigger.pattern[stage][9]  );
   m_comboBoxBit10->SetSelection(pOsciloscope->window.trigger.pattern[stage][10] );
   m_comboBoxBit11->SetSelection(pOsciloscope->window.trigger.pattern[stage][11] );
   m_comboBoxBit12->SetSelection(pOsciloscope->window.trigger.pattern[stage][12] );
   m_comboBoxBit13->SetSelection(pOsciloscope->window.trigger.pattern[stage][13] );
   m_comboBoxBit14->SetSelection(pOsciloscope->window.trigger.pattern[stage][14] );
   m_comboBoxBit15->SetSelection(pOsciloscope->window.trigger.pattern[stage][15] );

   if (!pOsciloscope->window.trigger.mask[stage][0])m_comboBoxBit0->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][1])m_comboBoxBit1->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][2])m_comboBoxBit2->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][3])m_comboBoxBit3->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][4])m_comboBoxBit4->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][5])m_comboBoxBit5->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][6])m_comboBoxBit6->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][7])m_comboBoxBit7->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][8])m_comboBoxBit8->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][9])m_comboBoxBit9->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][10])m_comboBoxBit10->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][11])m_comboBoxBit11->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][12])m_comboBoxBit12->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][13])m_comboBoxBit13->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][14])m_comboBoxBit14->SetSelection(4);
   if (!pOsciloscope->window.trigger.mask[stage][15])m_comboBoxBit15->SetSelection(4);
}

void OsciloskopOsciloskop::m_textCtrDigitallDelayOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrDigitallDelayOnTextEnter
   int stage = pOsciloscope->window.trigger.stage;
   ushort delay = clamp( (uint)stringToInteger( m_textCtrDigitallDelay->GetValue().ToAscii().data() ), 0U, 0xffffU );
   m_textCtrDigitallDelay->SetValue( wxString::FromAscii( integerToString(delay) ));
   pOsciloscope->window.trigger.delay[stage] = delay;
   pOsciloscope->control.setDigitalDelay( (DigitalStage)stage, delay );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_comboBoxDigitalStageStartOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxDigitalStageStartOnCombobox
   pOsciloscope->window.trigger.stageStart = m_comboBoxDigitalStageStart->GetSelection();
   pOsciloscope->control.setDigitalStart( pOsciloscope->window.trigger.stageStart );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_comboBoxDigitalModeOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxDigitalModeOnCombobox
   pOsciloscope->window.trigger.stageMode = m_comboBoxDigitalMode->GetSelection();
   pOsciloscope->control.setDigitalMode( pOsciloscope->window.trigger.stageMode );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_comboBoxDigitalSerialChannelOnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxDigitalSerialChannelOnCombobox
   pOsciloscope->window.trigger.stageChannel = m_comboBoxDigitalSerialChannel->GetSelection();
   pOsciloscope->control.setDigitalChannel( pOsciloscope->window.trigger.stageChannel );
   pOsciloscope->control.transferData();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// Digital Pattern
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void setMask(int idx, bool value)
{
   int stage = pOsciloscope->window.trigger.stage;
   pOsciloscope->window.trigger.mask[stage][idx] = value;
   pOsciloscope->control.setDigitalMask((DigitalStage)stage, (DigitalBit)idx, (DigitalPattern)pOsciloscope->window.trigger.mask[stage][idx]);
   pOsciloscope->control.transferData();
}

void setMaskNoTr(int idx, bool value)
{
   int stage = pOsciloscope->window.trigger.stage;
   pOsciloscope->window.trigger.mask[stage][idx] = value;
   pOsciloscope->control.setDigitalMask((DigitalStage)stage, (DigitalBit)idx, (DigitalPattern)pOsciloscope->window.trigger.mask[stage][idx]);
}

void setPattern( int idx, wxChoice* comboBox )
{
   if (comboBox->GetSelection() == 4)
   {
      setMask(idx, false);
   }
   else
   {
      setMaskNoTr(idx, true);
      int stage = pOsciloscope->window.trigger.stage;
      pOsciloscope->window.trigger.pattern[stage][idx] = comboBox->GetSelection();
      pOsciloscope->control.setDigitalPattern( (DigitalStage)stage, (DigitalBit)idx, (DigitalPattern)pOsciloscope->window.trigger.pattern[stage][idx] );
      pOsciloscope->control.transferData();
   }
}

void setPatternNoTr(int idx, wxChoice* comboBox)
{
   if (comboBox->GetSelection() == 4)
   {
      setMaskNoTr(idx, false);
   }
   else
   {
      setMaskNoTr(idx, true);
      int stage = pOsciloscope->window.trigger.stage;
      pOsciloscope->window.trigger.pattern[stage][idx] = comboBox->GetSelection();
      pOsciloscope->control.setDigitalPattern((DigitalStage)stage, (DigitalBit)idx, (DigitalPattern)pOsciloscope->window.trigger.pattern[stage][idx]);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// 0-7
//
/////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_comboBoxBit0OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit0OnCombobox
   setPattern(0,m_comboBoxBit0);
}

void OsciloskopOsciloskop::m_comboBoxBit1OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit1OnCombobox
   setPattern(1,m_comboBoxBit1);
}

void OsciloskopOsciloskop::m_comboBoxBit2OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit2OnCombobox
   setPattern(2,m_comboBoxBit2);
}

void OsciloskopOsciloskop::m_comboBoxBit3OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit3OnCombobox
   setPattern(3,m_comboBoxBit3);
}

void OsciloskopOsciloskop::m_comboBoxBit4OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit4OnCombobox
   setPattern(4,m_comboBoxBit4);
}

void OsciloskopOsciloskop::m_comboBoxBit5OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit5OnCombobox
   setPattern(5,m_comboBoxBit5);
}

void OsciloskopOsciloskop::m_comboBoxBit6OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit6OnCombobox
   setPattern(6,m_comboBoxBit6);
}

void OsciloskopOsciloskop::m_comboBoxBit7OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit7OnCombobox
   setPattern(7,m_comboBoxBit7);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// 8-15
//
/////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_comboBoxBit8OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit8OnCombobox
   setPattern(8,m_comboBoxBit8);
}

void OsciloskopOsciloskop::m_comboBoxBit9OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit9OnCombobox
   setPattern(9,m_comboBoxBit9);
}

void OsciloskopOsciloskop::m_comboBoxBit10OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit10OnCombobox
   setPattern(10,m_comboBoxBit10);
}

void OsciloskopOsciloskop::m_comboBoxBit11OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit11OnCombobox
   setPattern(11,m_comboBoxBit11);
}

void OsciloskopOsciloskop::m_comboBoxBit12OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit12OnCombobox
   setPattern(12,m_comboBoxBit12);
}

void OsciloskopOsciloskop::m_comboBoxBit13OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit13OnCombobox
   setPattern(13,m_comboBoxBit13);
}

void OsciloskopOsciloskop::m_comboBoxBit14OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit14OnCombobox
   setPattern(14,m_comboBoxBit14);
}

void OsciloskopOsciloskop::m_comboBoxBit15OnCombobox( wxCommandEvent& event )
{
// TODO: Implement m_comboBoxBit15OnCombobox
   setPattern(15,m_comboBoxBit15);
}

//////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////

void OsciloskopOsciloskop::m_menuItemReadEEPROMOnMenuSelection(wxCommandEvent& event)
{
   wxCommandEvent evt;
   m_radioBoxTimeMode->SetSelection(SIGNAL_MODE_PAUSE);
   m_radioBoxTimeModeOnRadioBox( evt );

   UsbEEPROM eeprom;
   memset(eeprom.bytes, 0xdd, sizeof(UsbEEPROM) );
   pOsciloscope->ReadEEPROM(eeprom);
   wxCommandEvent et;
   m_menuItemDebugOnMenuSelection(et);
   if (pDebug && pDebug->IsShown())
   {
      pDebug->m_textCtrl41->Clear();

      FORMAT_BUFFER();
      for (int i = 0; i < EEPROM_BYTE_COUNT; i++)
      {
         byte byteToPrint = eeprom.bytes[i];
         FORMAT("%02x ", byteToPrint);
         pDebug->m_textCtrl41->AppendText(wxString::FromAscii(formatBuffer));
      }
   }
}

void OsciloskopOsciloskop::m_menuItemWriteEEPROMOnMenuSelection(wxCommandEvent& event)
{
   pOsciloscope->WriteEEPROM();
}

void OsciloskopOsciloskop::m_menuItemResetEEPROMOnMenuSelection(wxCommandEvent& event)
{
   pOsciloscope->EraseEEPROM();
}

void OsciloskopOsciloskop::m_buttonConnectOnButtonClick(wxCommandEvent& event)
{
   pOsciloscope->uploadFirmware();
}

void OsciloskopOsciloskop::m_choiceSpeedOnChoice(wxCommandEvent& event)
{
    pOsciloscope->window.speed = (UsbSpeed)m_choiceSpeed->GetSelection();
}

void OsciloskopOsciloskop::m_textCtrlDigitalVoltageOnTextEnter(wxCommandEvent& event)
{
   pOsciloscope->window.digitalSetup.voltage = stringToDouble( m_textCtrlDigitalVoltage->GetValue().ToAscii().data() );
   pOsciloscope->control.setDigitalVoltage( pOsciloscope->window.digitalSetup.voltage );
   pOsciloscope->control.transferData();

   m_textCtrlDigitalVoltage->SetValue(doubleToString( pOsciloscope->control.getDigitalVoltage()) );
}

void OsciloskopOsciloskop::m_textCtrlFreqDividerOnTextEnter(wxCommandEvent& event)
{
   uint  divider = stringToInteger(m_textCtrlFreqDivider->GetValue().ToAscii().data());
   pOsciloscope->window.digitalSetup.divider = divider;
   double   freq = 100000000.0 / (double(divider) + 1.0);
   m_staticTextMhz->SetLabel( wxString(doubleToString(freq)).append(wxT(" Hz")));
   pOsciloscope->control.setDigitalClockDivide( divider );
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_choiceInputOutputOnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digitalSetup.inputOutput15 = m_choiceInputOutput->GetSelection();
   pOsciloscope->control.setDigitalInputOutput(pOsciloscope->window.digitalSetup.inputOutput15, pOsciloscope->window.digitalSetup.inputOutput7);
   pOsciloscope->control.transferData();
   pOsciloscope->window.hardwareGenerator.uploadDigital();

   if (m_choiceInputOutput->GetSelection() == 1)
   {
      m_choiceBit8->Disable();
      m_choiceBit9->Disable();
      m_choiceBit10->Disable();
      m_choiceBit11->Disable();
      m_choiceBit12->Disable();
      m_choiceBit13->Disable();
      m_choiceBit14->Disable();
      m_choiceBit15->Disable();
   }
   else
   {
      m_choiceBit8->Enable();
      m_choiceBit9->Enable();
      m_choiceBit10->Enable();
      m_choiceBit11->Enable();
      m_choiceBit12->Enable();
      m_choiceBit13->Enable();
      m_choiceBit14->Enable();
      m_choiceBit15->Enable();

   }
}

void OsciloskopOsciloskop::m_choiceInputOutput1OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digitalSetup.inputOutput7 = m_choiceInputOutput1->GetSelection();
   pOsciloscope->control.setDigitalInputOutput(pOsciloscope->window.digitalSetup.inputOutput15, pOsciloscope->window.digitalSetup.inputOutput7 );
   pOsciloscope->control.transferData();
   pOsciloscope->window.hardwareGenerator.uploadDigital();

   if (m_choiceInputOutput1->GetSelection() == 1)
   {
      m_choiceBit0->Disable();
      m_choiceBit1->Disable();
      m_choiceBit2->Disable();
      m_choiceBit3->Disable();
      m_choiceBit4->Disable();
      m_choiceBit5->Disable();
      m_choiceBit6->Disable();
      m_choiceBit7->Disable();
   }
   else
   {
      m_choiceBit0->Enable();
      m_choiceBit1->Enable();
      m_choiceBit2->Enable();
      m_choiceBit3->Enable();
      m_choiceBit4->Enable();
      m_choiceBit5->Enable();
      m_choiceBit6->Enable();
      m_choiceBit7->Enable();
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// output bits
//////////////////////////////////////////////////////////////////////////////////////////////////

void OsciloskopOsciloskop::m_choiceBit0OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[0] = m_choiceBit0->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(0, m_choiceBit0->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit1OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[1] = m_choiceBit1->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(1, m_choiceBit1->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit2OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[2] = m_choiceBit2->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(2, m_choiceBit2->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit3OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[3] = m_choiceBit3->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(3, m_choiceBit3->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit4OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[4] = m_choiceBit4->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(4, m_choiceBit4->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit5OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[5] = m_choiceBit5->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(5, m_choiceBit5->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit6OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[6] = m_choiceBit6->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(6, m_choiceBit6->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit7OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[7] = m_choiceBit7->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(7, m_choiceBit7->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit8OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[8] = m_choiceBit8->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(8, m_choiceBit8->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit9OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[9] = m_choiceBit9->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(9, m_choiceBit9->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit10OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[10] = m_choiceBit10->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(10, m_choiceBit10->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit11OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[11] = m_choiceBit11->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(11, m_choiceBit11->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit12OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[12] = m_choiceBit12->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(12, m_choiceBit12->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit13OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[13] = m_choiceBit13->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(13, m_choiceBit13->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit14OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[14] = m_choiceBit14->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(14, m_choiceBit14->GetSelection());
   pOsciloscope->control.transferData();
}
void OsciloskopOsciloskop::m_choiceBit15OnChoice(wxCommandEvent& event)
{
   pOsciloscope->window.digital.output[15] = m_choiceBit15->GetSelection();
   pOsciloscope->control.setDigitalOutputBit(15, m_choiceBit15->GetSelection());
   pOsciloscope->control.transferData();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Custon / All Zero/ All One
//////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_choiceBit158OnChoice(wxCommandEvent& event)
{
   switch (m_choiceBit158->GetSelection()) {
   case 0:
      break;
   case 1:
      m_comboBoxBit15->SetSelection(0);
      m_comboBoxBit14->SetSelection(0);
      m_comboBoxBit13->SetSelection(0);
      m_comboBoxBit12->SetSelection(0);
      m_comboBoxBit11->SetSelection(0);
      m_comboBoxBit10->SetSelection(0);
      m_comboBoxBit9->SetSelection(0);
      m_comboBoxBit8->SetSelection(0);
      break;
   case 2:
      m_comboBoxBit15->SetSelection(1);
      m_comboBoxBit14->SetSelection(1);
      m_comboBoxBit13->SetSelection(1);
      m_comboBoxBit12->SetSelection(1);
      m_comboBoxBit11->SetSelection(1);
      m_comboBoxBit10->SetSelection(1);
      m_comboBoxBit9->SetSelection(1);
      m_comboBoxBit8->SetSelection(1);
      break;
   case 3:
      m_comboBoxBit15->SetSelection(4);
      m_comboBoxBit14->SetSelection(4);
      m_comboBoxBit13->SetSelection(4);
      m_comboBoxBit12->SetSelection(4);
      m_comboBoxBit11->SetSelection(4);
      m_comboBoxBit10->SetSelection(4);
      m_comboBoxBit9->SetSelection(4);
      m_comboBoxBit8->SetSelection(4);
      break;
   };

   setPatternNoTr(15, m_comboBoxBit15);
   setPatternNoTr(14, m_comboBoxBit14);
   setPatternNoTr(13, m_comboBoxBit13);
   setPatternNoTr(12, m_comboBoxBit12);
   setPatternNoTr(11, m_comboBoxBit11);
   setPatternNoTr(10, m_comboBoxBit10);
   setPatternNoTr(9, m_comboBoxBit9);
   setPatternNoTr(8, m_comboBoxBit8);

   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_choiceBit70OnChoice(wxCommandEvent& event)
{
   switch (m_choiceBit70->GetSelection()) {
   case 0:
      break;
   case 1:
      m_comboBoxBit7->SetSelection(0);
      m_comboBoxBit6->SetSelection(0);
      m_comboBoxBit5->SetSelection(0);
      m_comboBoxBit4->SetSelection(0);
      m_comboBoxBit3->SetSelection(0);
      m_comboBoxBit2->SetSelection(0);
      m_comboBoxBit1->SetSelection(0);
      m_comboBoxBit0->SetSelection(0);
      break;
   case 2:
      m_comboBoxBit7->SetSelection(1);
      m_comboBoxBit6->SetSelection(1);
      m_comboBoxBit5->SetSelection(1);
      m_comboBoxBit4->SetSelection(1);
      m_comboBoxBit3->SetSelection(1);
      m_comboBoxBit2->SetSelection(1);
      m_comboBoxBit1->SetSelection(1);
      m_comboBoxBit0->SetSelection(1);
      break;
   case 3:
      m_comboBoxBit7->SetSelection(4);
      m_comboBoxBit6->SetSelection(4);
      m_comboBoxBit5->SetSelection(4);
      m_comboBoxBit4->SetSelection(4);
      m_comboBoxBit3->SetSelection(4);
      m_comboBoxBit2->SetSelection(4);
      m_comboBoxBit1->SetSelection(4);
      m_comboBoxBit0->SetSelection(4);
      break;
   };

   setPatternNoTr(7, m_comboBoxBit7);
   setPatternNoTr(6, m_comboBoxBit6);
   setPatternNoTr(5, m_comboBoxBit5);
   setPatternNoTr(4, m_comboBoxBit4);
   setPatternNoTr(3, m_comboBoxBit3);
   setPatternNoTr(2, m_comboBoxBit2);
   setPatternNoTr(1, m_comboBoxBit1);
   setPatternNoTr(0, m_comboBoxBit0);

   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_choiceDS815OnChoice(wxCommandEvent& event)
{
   switch (m_choiceDS815->GetSelection()) {
   case 0:
      break;
   case 1:
      m_choiceBit8->SetSelection(0);
      m_choiceBit9->SetSelection(0);
      m_choiceBit10->SetSelection(0);
      m_choiceBit11->SetSelection(0);
      m_choiceBit12->SetSelection(0);
      m_choiceBit13->SetSelection(0);
      m_choiceBit14->SetSelection(0);
      m_choiceBit15->SetSelection(0);
      break;
   case 2:
      m_choiceBit8->SetSelection(1);
      m_choiceBit9->SetSelection(1);
      m_choiceBit10->SetSelection(1);
      m_choiceBit11->SetSelection(1);
      m_choiceBit12->SetSelection(1);
      m_choiceBit13->SetSelection(1);
      m_choiceBit14->SetSelection(1);
      m_choiceBit15->SetSelection(1);
      break;
   };

   pOsciloscope->control.setDigitalOutputBit(8,  m_choiceBit8->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(9,  m_choiceBit9->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(10, m_choiceBit10->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(11, m_choiceBit11->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(12, m_choiceBit12->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(13, m_choiceBit13->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(14, m_choiceBit14->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(15, m_choiceBit15->GetSelection());

   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_choiceDS70OnChoice(wxCommandEvent& event)
{
   switch (m_choiceDS70->GetSelection()) {
   case 0:
      break;
   case 1:
      m_choiceBit9->SetSelection(0);
      m_choiceBit1->SetSelection(0);
      m_choiceBit2->SetSelection(0);
      m_choiceBit3->SetSelection(0);
      m_choiceBit4->SetSelection(0);
      m_choiceBit5->SetSelection(0);
      m_choiceBit6->SetSelection(0);
      m_choiceBit7->SetSelection(0);
      break;
   case 2:
      m_choiceBit0->SetSelection(1);
      m_choiceBit1->SetSelection(1);
      m_choiceBit2->SetSelection(1);
      m_choiceBit3->SetSelection(1);
      m_choiceBit4->SetSelection(1);
      m_choiceBit5->SetSelection(1);
      m_choiceBit6->SetSelection(1);
      m_choiceBit7->SetSelection(1);
      break;
   };

   pOsciloscope->control.setDigitalOutputBit(0, m_choiceBit0->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(1, m_choiceBit1->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(2, m_choiceBit2->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(3, m_choiceBit3->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(4, m_choiceBit4->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(5, m_choiceBit5->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(6, m_choiceBit6->GetSelection());
   pOsciloscope->control.setDigitalOutputBit(7, m_choiceBit7->GetSelection());

   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_choiceDOnOff815OnChoice(wxCommandEvent& event)
{
   switch (m_choiceDOnOff815->GetSelection()) {
   case 0:
      break;
   case 1:
      m_checkBox9->SetValue(false);
      m_checkBox10->SetValue(false);
      m_checkBox11->SetValue(false);
      m_checkBox12->SetValue(false);
      m_checkBox13->SetValue(false);
      m_checkBox14->SetValue(false);
      m_checkBox15->SetValue(false);
      m_checkBox16->SetValue(false);
      break;
   case 2:
      m_checkBox9->SetValue(true);
      m_checkBox10->SetValue(true);
      m_checkBox11->SetValue(true);
      m_checkBox12->SetValue(true);
      m_checkBox13->SetValue(true);
      m_checkBox14->SetValue(true);
      m_checkBox15->SetValue(true);
      m_checkBox16->SetValue(true);
      break;
   };

   pOsciloscope->window.digital.digital[8]  = m_checkBox9->GetValue();
   pOsciloscope->window.digital.digital[9]  = m_checkBox10->GetValue();
   pOsciloscope->window.digital.digital[10] = m_checkBox11->GetValue();
   pOsciloscope->window.digital.digital[11] = m_checkBox12->GetValue();
   pOsciloscope->window.digital.digital[12] = m_checkBox13->GetValue();
   pOsciloscope->window.digital.digital[13] = m_checkBox14->GetValue();
   pOsciloscope->window.digital.digital[14] = m_checkBox15->GetValue();
   pOsciloscope->window.digital.digital[15] = m_checkBox16->GetValue();
}

void OsciloskopOsciloskop::m_choiceDOnOff70OnChoice(wxCommandEvent& event)
{
   switch (m_choiceDOnOff70->GetSelection()) {
   case 0:
      break;
   case 1:
      m_checkBox1->SetValue(false);
      m_checkBox2->SetValue(false);
      m_checkBox3->SetValue(false);
      m_checkBox4->SetValue(false);
      m_checkBox5->SetValue(false);
      m_checkBox6->SetValue(false);
      m_checkBox7->SetValue(false);
      m_checkBox8->SetValue(false);
      break;
   case 2:
      m_checkBox1->SetValue(true);
      m_checkBox2->SetValue(true);
      m_checkBox3->SetValue(true);
      m_checkBox4->SetValue(true);
      m_checkBox5->SetValue(true);
      m_checkBox6->SetValue(true);
      m_checkBox7->SetValue(true);
      m_checkBox8->SetValue(true);
      break;
   };

   pOsciloscope->window.digital.digital[0] = m_checkBox1->GetValue();
   pOsciloscope->window.digital.digital[1] = m_checkBox2->GetValue();
   pOsciloscope->window.digital.digital[2] = m_checkBox3->GetValue();
   pOsciloscope->window.digital.digital[3] = m_checkBox4->GetValue();
   pOsciloscope->window.digital.digital[4] = m_checkBox5->GetValue();
   pOsciloscope->window.digital.digital[5] = m_checkBox6->GetValue();
   pOsciloscope->window.digital.digital[6] = m_checkBox7->GetValue();
   pOsciloscope->window.digital.digital[7] = m_checkBox8->GetValue();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// custom output bits
//////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_filePicker1OnFileChanged(wxFileDirPickerEvent& event)
{
   pOsciloscope->window.hardwareGenerator.loadCustomDigital( m_filePicker1->GetFileName().GetFullPath().ToAscii().data());
}

void OsciloskopOsciloskop::m_buttonCustomFileOnButtonClick(wxCommandEvent& event)
{
   pOsciloscope->window.hardwareGenerator.loadCustomDigital(m_filePicker1->GetFileName().GetFullPath().ToAscii().data());
   pOsciloscope->control.transferData();
   pOsciloscope->window.hardwareGenerator.uploadDigital();
}

void OsciloskopOsciloskop::SaveOldSlotLoadNewSlot(int newSlot)
{
   int oldSlot = pOsciloscope->windowSlot;

   pOsciloscope->windowState[oldSlot] = pOsciloscope->window;

   pOsciloscope->window = pOsciloscope->windowState[newSlot];
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// slots
//////////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopOsciloskop::m_menuItemSlot1OnMenuSelection(wxCommandEvent& event)
{
   SaveOldSlotLoadNewSlot(0);

   GetMenuBar()->GetMenu(6)->FindItemByPosition(0)->Check(1);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(1)->Check(0);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(2)->Check(0);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(3)->Check(0);

   loadWindow(0);
}
void OsciloskopOsciloskop::m_menuItemSlot2OnMenuSelection(wxCommandEvent& event)
{

   SaveOldSlotLoadNewSlot(1);

   GetMenuBar()->GetMenu(6)->FindItemByPosition(0)->Check(0);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(1)->Check(1);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(2)->Check(0);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(3)->Check(0);

   loadWindow(1);
}
void OsciloskopOsciloskop::m_menuItemSlot3OnMenuSelection(wxCommandEvent& event)
{
   SaveOldSlotLoadNewSlot(2);

   GetMenuBar()->GetMenu(6)->FindItemByPosition(0)->Check(0);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(1)->Check(0);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(2)->Check(1);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(3)->Check(0);

   loadWindow(2);
}
void OsciloskopOsciloskop::m_menuItemSlot4OnMenuSelection(wxCommandEvent& event)
{
   SaveOldSlotLoadNewSlot(3);

   GetMenuBar()->GetMenu(6)->FindItemByPosition(0)->Check(0);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(1)->Check(0);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(2)->Check(0);
   GetMenuBar()->GetMenu(6)->FindItemByPosition(3)->Check(1);

   loadWindow(3);
}

void OsciloskopOsciloskop::loadWindow(int slot)
{
   slot = clamp(slot, 0, 3);
   pOsciloscope->window     = pOsciloscope->windowState[slot];
   pOsciloscope->windowSlot = slot;

   String slotName = pOsciloscope->windowName[slot];
   if( slotName.getLength() > 32 )
   {
      slotName.remove(0,slotName.getLength()-32);
      slotName.insert(0,"...");
   }
   GetMenuBar()->GetMenu(6)->FindItemByPosition(slot)->SetItemLabel( slotName.asChar() );

   loadSlot(pOsciloscope->window);
}

void OsciloskopOsciloskop::loadSlot(WndMain& slot)
{
   pOsciloscope->oscCameraSetup(pOsciloscope->window.fftDigital.is(VIEW_SELECT_OSC_3D));
   pOsciloscope->fftCameraSetup(pOsciloscope->window.fftDigital.is(VIEW_SELECT_FFT_3D));

   setupUI(slot);
   setupControl(slot);

   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::setupControl(WndMain window)
{
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_A_GROUND, window.channel01.Ground );
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_B_GROUND, window.channel02.Ground );
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_A_ACDC,   window.channel01.AcDc   );
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_B_ACDC,   window.channel02.AcDc   );
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_ETS,      window.horizontal.ETS   );
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_ATTR_A, pOsciloscope->control.getAttr(captureVoltFromValue(window.channel01.Capture) ) );
   pOsciloscope->control.setAnalogSwitchBit( CHANNEL_ATTR_B, pOsciloscope->control.getAttr(captureVoltFromValue(window.channel02.Capture) ) );

   pOsciloscope->control.setEts( window.horizontal.ETS );
   pOsciloscope->control.setControl( window.horizontal.Control );
   pOsciloscope->control.setYRangeScaleA( captureVoltFromValue(window.channel01.Capture), window.channel01.Scale );
   pOsciloscope->control.setYPositionA( window.channel01.YPosition + pOsciloscope->settings.analogChannelOffsets[0][captureVoltFromValue(window.channel01.Capture)] );
   pOsciloscope->control.setYRangeScaleB( captureVoltFromValue(window.channel02.Capture), window.channel02.Scale );
   pOsciloscope->control.setYPositionB( window.channel02.YPosition + pOsciloscope->settings.analogChannelOffsets[1][captureVoltFromValue(window.channel02.Capture)] );

   pOsciloscope->control.setTriggerSource(window.trigger.Source);
   pOsciloscope->control.setTriggerMode(window.trigger.Mode);
   pOsciloscope->control.setTriggerSlope(window.trigger.Slope);

   pOsciloscope->control.setTriggerPre(   window.trigger.Percent );
   pOsciloscope->control.setTriggerHis(   window.trigger.His     );
   pOsciloscope->control.setTriggerLevel( window.trigger.Level   );

   pOsciloscope->control.setXRange( captureTimeFromValue(window.horizontal.Capture) );
   pOsciloscope->control.setFrameSize( window.horizontal.FrameSize );
   pOsciloscope->control.setHoldoff( window.trigger.Holdoff );

   pOsciloscope->control.setDigitalStart( window.trigger.stageStart );
   pOsciloscope->control.setDigitalMode( window.trigger.stageMode );
   pOsciloscope->control.setDigitalChannel( window.trigger.stageChannel );

   for (int stage = 0; stage < 4; stage++)
   {
      pOsciloscope->control.setDigitalDelay((DigitalStage)stage, window.trigger.delay[stage] );
   }
   for (int stage = 0; stage < 4; stage++)
   {
      for (int bit = 0; bit < 16; bit++)
      {
         pOsciloscope->control.setDigitalMask((DigitalStage)stage, (DigitalBit)bit, window.trigger.mask[stage][bit]);
      }
   }
   for (int stage = 0; stage < 4;stage++)
   {
      for (int bit = 0; bit < 16; bit++)
      {
         pOsciloscope->control.setDigitalPattern((DigitalStage)stage, (DigitalBit) bit, (DigitalPattern)window.trigger.pattern[stage][bit]);
      }
   }

   pOsciloscope->control.setGeneratorType0( (GeneratorType)window.hardwareGenerator.type0 );
   pOsciloscope->control.setGeneratorOn0( window.hardwareGenerator.onOff0 );
   pOsciloscope->control.setGeneratorSlope0( window.hardwareGenerator.sawSlopePositive0 );
   pOsciloscope->control.setGeneratorVoltage0( window.hardwareGenerator.voltage0 );
   pOsciloscope->control.setGeneratorOffset0( window.hardwareGenerator.offset0 + pOsciloscope->settings.generatorOffset[0]);
   pOsciloscope->control.setGeneratorFrequency0(window.hardwareGenerator.frequency0, pOsciloscope->settings.generatorFs );
   pOsciloscope->control.setGeneratorSquareDuty0(window.hardwareGenerator.squareDuty0);

   pOsciloscope->control.setGeneratorType1((GeneratorType)window.hardwareGenerator.type1 );
   pOsciloscope->control.setGeneratorOn1( window.hardwareGenerator.onOff1 );
   pOsciloscope->control.setGeneratorSlope1( window.hardwareGenerator.sawSlopePositive1 );
   pOsciloscope->control.setGeneratorVoltage1( window.hardwareGenerator.voltage1 );
   pOsciloscope->control.setGeneratorOffset1( window.hardwareGenerator.offset1 + pOsciloscope->settings.generatorOffset[1] );
   pOsciloscope->control.setGeneratorFrequency1( window.hardwareGenerator.frequency1, pOsciloscope->settings.generatorFs );
   pOsciloscope->control.setGeneratorSquareDuty1( window.hardwareGenerator.squareDuty1 );

   pOsciloscope->control.setDigitalVoltage( window.digitalSetup.voltage );
   pOsciloscope->control.setDigitalInputOutput(window.digitalSetup.inputOutput15, window.digitalSetup.inputOutput7 );
   for (int i = 0; i < 16;i++)
   {
      pOsciloscope->control.setDigitalOutputBit( i, window.digital.output[i] );
   }
   pOsciloscope->control.setDigitalClockDivide(window.digitalSetup.divider);
}

void OsciloskopOsciloskop::setupUI(WndMain window)
{
   // speed
   m_choiceSpeed->SetSelection( pOsciloscope->window.speed );

   ////////////////////////////////////////////////////////////////////////////////////////
   // horizontal
   ////////////////////////////////////////////////////////////////////////////////////////

   // capture
   m_comboBoxTimeCapture->SetSelection(captureTimeFromValue(window.horizontal.Capture));

   // display
   uint  multiEnum = multiplyerFromValue(window.horizontal.Display);
   float multiFloat = multiplyerFromEnum(multiEnum);
   m_textCtrlTimeDisplay->SetValue(wxString::FromAscii(floatToString(window.horizontal.Display / multiFloat)));
   m_comboBoxTimeDisplay->SetSelection(multiEnum);

   // position
   m_textCtrlTimePosition->SetValue(wxString::FromAscii(floatToString(window.horizontal.Position)));
   m_sliderTimePosition->SetValue(window.horizontal.Position);


   // mode
   m_radioBoxTimeMode->SetSelection(window.horizontal.Mode);


   // control
   m_comboBoxTimeControl->SetSelection(window.horizontal.Control);


   //  FrameSize
   const char* str = integerToString((int)window.horizontal.FrameSize);
   m_textCtrlTimeFrameSize->SetValue(wxString::FromAscii(str));

   // Frame
   m_textCtrlTimeFrame->SetValue(wxString::FromAscii(floatToString(window.horizontal.Frame)));
   m_sliderTimeFrame->SetValue(window.horizontal.Frame);
   m_sliderTimeFrame->SetMax(pOsciloscope->settings.historyFrameCount);

   // FFTSize
   m_textCtrlTimeFFTSize->SetValue(wxString::FromAscii(integerToString(window.horizontal.FFTSize)));

   // ETS
   m_checkBoxETS->SetValue(window.horizontal.ETS );

   ////////////////////////////////////////////////////////////////////////////////////////
   // channel 0
   ////////////////////////////////////////////////////////////////////////////////////////

   // Capture
   m_comboBoxCh0Capture->SetSelection(captureVoltFromValue(window.channel01.Capture));

   // Scale
   m_textCtrlCh0Scale->SetValue(wxString::FromAscii(floatToString(window.channel01.Scale)));

   // Display
   {
      float value = window.channel01.Display / multiplyerFromEnum(multiplyerFromValue(window.channel01.Display));
      m_textCtrlCh0Display->SetValue(wxString::FromAscii(floatToString(value)));
      m_comboBoxCh0Display->SetSelection(multiplyerFromValue(window.channel01.Display));
   }

   // YPosition
   m_sliderCh0Position->SetValue(window.channel01.YPosition);
   m_textCtrlCh0Position->SetValue(wxString::FromAscii(floatToString(window.channel01.YPosition)));

   // OscOnOff
   m_checkBoxSignal1->SetValue(window.channel01.OscOnOff);

   // FFTOnOff
   m_checkBoxFFT1->SetValue(window.channel01.FFTOnOff);

   // Invert
   m_checkBoxCh0Invert->SetValue(window.channel01.Invert);

   // Ground
   m_checkBoxCh0Ground->SetValue(window.channel01.Ground);

   // AcDc
   m_choiceCh0ACDC->SetSelection(window.channel01.AcDc);


   ////////////////////////////////////////////////////////////////////////////////////////
   // channel 1
   ////////////////////////////////////////////////////////////////////////////////////////

   // Capture
   m_comboBoxCh1Capture->SetSelection(captureVoltFromValue(window.channel02.Capture));

   // Scale
   m_textCtrlCh1Scale->SetValue(wxString::FromAscii(floatToString(window.channel02.Scale)));

   // Display
   {
      float value = window.channel02.Display / multiplyerFromEnum(multiplyerFromValue(window.channel02.Display));
      m_textCtrlCh1Display->SetValue(wxString::FromAscii(floatToString(value)));
      m_comboBoxCh1Display->SetSelection(multiplyerFromValue(window.channel02.Display));
   }

   // YPosition
   m_sliderCh1Position->SetValue(window.channel02.YPosition);
   m_textCtrlCh1Position->SetValue(wxString::FromAscii(floatToString(window.channel02.YPosition)));

   // OscOnOff
   m_checkBoxSignal2->SetValue(window.channel02.OscOnOff);

   // FFTOnOff;
   m_checkBoxFFT2->SetValue(window.channel02.FFTOnOff);

   // Invert
   m_checkBoxCh1Invert->SetValue(window.channel02.Invert);

   // Ground
   m_checkBoxCh1Ground->SetValue(window.channel02.Ground);

   // AcDc
   m_choiceCh1ACDC->SetSelection(window.channel02.AcDc);

   ////////////////////////////////////////////////////////////////////////////////////////
   // digital
   ////////////////////////////////////////////////////////////////////////////////////////

   // on/off
   m_checkBox1->SetValue(window.digital.digital[0]);
   m_checkBox2->SetValue(window.digital.digital[1]);
   m_checkBox3->SetValue(window.digital.digital[2]);
   m_checkBox4->SetValue(window.digital.digital[3]);
   m_checkBox5->SetValue(window.digital.digital[4]);
   m_checkBox6->SetValue(window.digital.digital[5]);
   m_checkBox7->SetValue(window.digital.digital[6]);
   m_checkBox8->SetValue(window.digital.digital[7]);
   m_checkBox9->SetValue(window.digital.digital[8]);
   m_checkBox10->SetValue(window.digital.digital[9]);
   m_checkBox11->SetValue(window.digital.digital[10]);
   m_checkBox12->SetValue(window.digital.digital[11]);
   m_checkBox13->SetValue(window.digital.digital[12]);
   m_checkBox14->SetValue(window.digital.digital[13]);
   m_checkBox15->SetValue(window.digital.digital[14]);
   m_checkBox16->SetValue(window.digital.digital[15]);

   // output
   m_choiceBit0->SetSelection(window.digital.output[0]);
   m_choiceBit1->SetSelection(window.digital.output[1]);
   m_choiceBit2->SetSelection(window.digital.output[2]);
   m_choiceBit3->SetSelection(window.digital.output[3]);
   m_choiceBit4->SetSelection(window.digital.output[4]);
   m_choiceBit5->SetSelection(window.digital.output[5]);
   m_choiceBit6->SetSelection(window.digital.output[6]);
   m_choiceBit7->SetSelection(window.digital.output[7]);
   m_choiceBit8->SetSelection(window.digital.output[8]);
   m_choiceBit9->SetSelection(window.digital.output[9]);
   m_choiceBit10->SetSelection(window.digital.output[10]);
   m_choiceBit11->SetSelection(window.digital.output[11]);
   m_choiceBit12->SetSelection(window.digital.output[12]);
   m_choiceBit13->SetSelection(window.digital.output[13]);
   m_choiceBit14->SetSelection(window.digital.output[14]);
   m_choiceBit15->SetSelection(window.digital.output[15]);

   ////////////////////////////////////////////////////////////////////////////////////////
   // digital setup
   ////////////////////////////////////////////////////////////////////////////////////////

   // voltage
   m_textCtrlDigitalVoltage->SetValue(wxString::FromAscii(floatToString(window.digitalSetup.voltage)));

   // divider
   m_textCtrlFreqDivider->SetValue(wxString::FromAscii(integerToString(window.digitalSetup.divider)));
   double freq = 100000000.0 / (double(window.digitalSetup.divider) + 1.0);
   m_staticTextMhz->SetLabel(wxString::FromAscii(doubleToString(freq)).append(wxT(" Hz")));

   // inputOutput15
   m_choiceInputOutput->SetSelection(window.digitalSetup.inputOutput15);
   if (m_choiceInputOutput->GetSelection() == 1)
   {
      m_choiceBit8->Disable();
      m_choiceBit9->Disable();
      m_choiceBit10->Disable();
      m_choiceBit11->Disable();
      m_choiceBit12->Disable();
      m_choiceBit13->Disable();
      m_choiceBit14->Disable();
      m_choiceBit15->Disable();
   }
   else
   {
      m_choiceBit8->Enable();
      m_choiceBit9->Enable();
      m_choiceBit10->Enable();
      m_choiceBit11->Enable();
      m_choiceBit12->Enable();
      m_choiceBit13->Enable();
      m_choiceBit14->Enable();
      m_choiceBit15->Enable();

   }

   // inputOutput7
   m_choiceInputOutput1->SetSelection(window.digitalSetup.inputOutput7);
   if (m_choiceInputOutput1->GetSelection() == 1)
   {
      m_choiceBit0->Disable();
      m_choiceBit1->Disable();
      m_choiceBit2->Disable();
      m_choiceBit3->Disable();
      m_choiceBit4->Disable();
      m_choiceBit5->Disable();
      m_choiceBit6->Disable();
      m_choiceBit7->Disable();
   }
   else
   {
      m_choiceBit0->Enable();
      m_choiceBit1->Enable();
      m_choiceBit2->Enable();
      m_choiceBit3->Enable();
      m_choiceBit4->Enable();
      m_choiceBit5->Enable();
      m_choiceBit6->Enable();
      m_choiceBit7->Enable();
   }


   ////////////////////////////////////////////////////////////////////////////////////////
   // function
   ////////////////////////////////////////////////////////////////////////////////////////

   // xyGraph
   m_checkBoxXYVoltageGraph->SetValue( window.function.xyGraph );

   // Type
   m_comboBoxFunction->SetSelection(window.function.Type);

   // OscOnOff
   m_checkBoxSignalF->SetValue(window.function.OscOnOff );

   // FFTOnOff;
   m_checkBoxFFTF->SetValue(window.function.FFTOnOff);

   ////////////////////////////////////////////////////////////////////////////////////////
   // trigger
   ////////////////////////////////////////////////////////////////////////////////////////

   // Source
   m_comboBoxTriggerSource->SetSelection(window.trigger.Source);

   // Slope
   m_comboBoxTriggerSlope->SetSelection(window.trigger.Slope);

   // Mode
   m_comboBoxTrigger->SetSelection(window.trigger.Mode);

   // Level
   m_textCtrlTriggerLevel->SetValue(wxString::FromAscii(floatToString(window.trigger.Level)));
   m_sliderTriggerLevel->SetValue(window.trigger.Level);

   // His
   m_textCtrlTriggerHisteresis->SetValue(wxString::FromAscii(floatToString(window.trigger.His)));
   m_sliderTriggerHisteresis->SetValue(window.trigger.His);

   // Percent
   m_textCtrlTriggerPre->SetValue(wxString::FromAscii(floatToString(window.trigger.Percent)));
   m_sliderTriggerPre->SetValue(window.trigger.Percent);

   // Holdoff
   m_textCtrlTriggerHoldoff->SetValue(wxString::FromAscii(floatToString(window.trigger.Holdoff)));
   m_sliderTriggerHoldoff->SetValue(window.trigger.Holdoff);

   // stage
   m_comboBoxDigitalStage->SetSelection(window.trigger.stage);

   // stageStart
   m_comboBoxDigitalStageStart->SetSelection( window.trigger.stageStart);

   // stageMode
   m_comboBoxDigitalMode->SetSelection( window.trigger.stageMode);

   // stageChannel
   m_comboBoxDigitalSerialChannel->SetSelection( window.trigger.stageChannel);

   // stage
   int istage = window.trigger.stage;

   // delay[4]
   ushort delay = window.trigger.delay[istage];
   m_textCtrDigitallDelay->SetValue(wxString::FromAscii(integerToString(delay)));

   // pattern[4]
   m_comboBoxBit0->SetSelection(window.trigger.pattern[istage][0]);
   m_comboBoxBit1->SetSelection(window.trigger.pattern[istage][1]);
   m_comboBoxBit2->SetSelection(window.trigger.pattern[istage][2]);
   m_comboBoxBit3->SetSelection(window.trigger.pattern[istage][3]);
   m_comboBoxBit4->SetSelection(window.trigger.pattern[istage][4]);
   m_comboBoxBit5->SetSelection(window.trigger.pattern[istage][5]);
   m_comboBoxBit6->SetSelection(window.trigger.pattern[istage][6]);
   m_comboBoxBit7->SetSelection(window.trigger.pattern[istage][7]);
   m_comboBoxBit8->SetSelection(window.trigger.pattern[istage][8]);
   m_comboBoxBit9->SetSelection(window.trigger.pattern[istage][9]);
   m_comboBoxBit10->SetSelection(window.trigger.pattern[istage][10]);
   m_comboBoxBit11->SetSelection(window.trigger.pattern[istage][11]);
   m_comboBoxBit12->SetSelection(window.trigger.pattern[istage][12]);
   m_comboBoxBit13->SetSelection(window.trigger.pattern[istage][13]);
   m_comboBoxBit14->SetSelection(window.trigger.pattern[istage][14]);
   m_comboBoxBit15->SetSelection(window.trigger.pattern[istage][15]);

   // mask[4]
   if (!window.trigger.mask[istage][0])m_comboBoxBit0->SetSelection(4);
   if (!window.trigger.mask[istage][1])m_comboBoxBit1->SetSelection(4);
   if (!window.trigger.mask[istage][2])m_comboBoxBit2->SetSelection(4);
   if (!window.trigger.mask[istage][3])m_comboBoxBit3->SetSelection(4);
   if (!window.trigger.mask[istage][4])m_comboBoxBit4->SetSelection(4);
   if (!window.trigger.mask[istage][5])m_comboBoxBit5->SetSelection(4);
   if (!window.trigger.mask[istage][6])m_comboBoxBit6->SetSelection(4);
   if (!window.trigger.mask[istage][7])m_comboBoxBit7->SetSelection(4);
   if (!window.trigger.mask[istage][8])m_comboBoxBit8->SetSelection(4);
   if (!window.trigger.mask[istage][9])m_comboBoxBit9->SetSelection(4);
   if (!window.trigger.mask[istage][10])m_comboBoxBit10->SetSelection(4);
   if (!window.trigger.mask[istage][11])m_comboBoxBit11->SetSelection(4);
   if (!window.trigger.mask[istage][12])m_comboBoxBit12->SetSelection(4);
   if (!window.trigger.mask[istage][13])m_comboBoxBit13->SetSelection(4);
   if (!window.trigger.mask[istage][14])m_comboBoxBit14->SetSelection(4);
   if (!window.trigger.mask[istage][15])m_comboBoxBit15->SetSelection(4);
}

int OsciloskopOsciloskop::getCurrentSlot()
{
   int index = 0;
   if (GetMenuBar()->GetMenu(6)->FindItemByPosition(0)->IsChecked()) index = 0;
   if (GetMenuBar()->GetMenu(6)->FindItemByPosition(1)->IsChecked()) index = 1;
   if (GetMenuBar()->GetMenu(6)->FindItemByPosition(2)->IsChecked()) index = 2;
   if (GetMenuBar()->GetMenu(6)->FindItemByPosition(3)->IsChecked()) index = 3;
   return index;
}

void OsciloskopOsciloskop::m_menuItemLoadOnMenuSelection(wxCommandEvent& event)
{
   wxFileName fn = wxStandardPaths::Get().GetExecutablePath();
   wxFileDialog *LoadDialog = new wxFileDialog(this, _("Load File As _?"), fn.GetPath().append(_("/data/state")), wxEmptyString, _("*.slot"), wxFD_OPEN, wxDefaultPosition);

   // Creates a Save Dialog with 4 file types
   if (LoadDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
   {
      String file = LoadDialog->GetPath().ToAscii().data();

      LoadSlot(getCurrentSlot(), file.asChar());

      wxString empty;
      wxString temp = file.asChar();
      temp.Replace(fn.GetPath(), empty );
      pOsciloscope->windowName[getCurrentSlot()] = temp.ToAscii().data();
   }

   // Clean up after ourselves
   LoadDialog->Destroy();

   // update window
   loadWindow(getCurrentSlot());
}

void OsciloskopOsciloskop::m_menuItemSaveFileOnMenuSelection(wxCommandEvent& event)
{
   wxFileName fn = wxStandardPaths::Get().GetExecutablePath();
   wxFileDialog *SaveDialog = new wxFileDialog(this, _("Save File As _?"), fn.GetPath().Append(_("/data/state")), wxEmptyString, _("*.slot"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition );

   // Creates a Save Dialog with 4 file types
   if (SaveDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
   {
      String file = SaveDialog->GetPath().ToAscii().data();

      SaveSlot(getCurrentSlot(), file.asChar());
   }

   // Clean up after ourselves
   // delete 	SaveDialog;
   SaveDialog->Destroy();
}

void OsciloskopOsciloskop::m_menuItemResetToDefaultOnMenuSelection(wxCommandEvent& event)
{
   pOsciloscope->windowState[getCurrentSlot()].Default();
   loadWindow(getCurrentSlot());
}

void OsciloskopOsciloskop::m_buttonCurrentFrameToClipboardOnButtonClick(wxCommandEvent& event)
{
   if (wxTheClipboard->Open())
   {
      SDL_AtomicLock(&pOsciloscope->displayLock);

      FORMAT_BUFFER();
      wxString text;
      text.Append("ScopeFun Oscilloscope Current Frame Data\n");

      char buffer[FORMAT_BUFFER_SIZE] = { 0 };
      pOsciloscope->display.getTime(buffer, FORMAT_BUFFER_SIZE);
      FORMAT("Date-Time-Offset: %s \n", buffer);
      text.append(formatBuffer);

      for (int c = 0; c < 2; c++)
      {
         FORMAT("Channel %d \n", c+1 );
         text.Append( formatBuffer );
         FORMAT("    Time/Div: %f ms \n", pOsciloscope->window.horizontal.Capture*1000.f);
         text.append(formatBuffer);
         if( c==0)
            FORMAT("    Volt/Div: %f V \n", pOsciloscope->window.channel01.Capture)
         else
            FORMAT("    Volt/Div: %f V \n", pOsciloscope->window.channel02.Capture);
         text.append(formatBuffer);

         FORMAT("    Values[%d,%d] : |", -int(MAXOSCVALUE), int(MAXOSCVALUE) );
         text.append(formatBuffer);

         for (int i = 0; i < pOsciloscope->display.analog[c].getCount(); i++)
         {
            ishort analog = pOsciloscope->display.getAnalogShort(c, i);
            wxVariant temp(analog);
            wxString  output = temp.GetString();
            while (output.length() < 8)
            {
               size_t len = output.length();
               if (len % 2) output.Prepend(" ");
               else        output.Append(" ");
            }

            text.Append(output);
            text.Append(_T("|"));
         }
         text.Append(_T("\n"));
      }
      text.append("Digital Channels     : |");
      for (int i = 0; i < pOsciloscope->display.digital.getCount(); i++)
      {
         ushort digital = pOsciloscope->display.getDigitalChannels(i);

         FORMAT("0x%x", digital);
         wxString  output(formatBuffer);
         while (output.length() < 8)
         {
            size_t len = output.length();
            if (len % 2) output.Prepend(" ");
            else        output.Append(" ");
         }
         text.Append(output);
         text.Append(_T("|"));
      }
      wxTheClipboard->SetData(new wxTextDataObject(text));
      wxTheClipboard->Close();

      SDL_AtomicUnlock(&pOsciloscope->displayLock);
   }
}

void OsciloskopOsciloskop::m_buttonAllFramesToClipboardOnButtonClick(wxCommandEvent& event)
{
   if (wxTheClipboard->Open())
   {
      SDL_AtomicLock(&pOsciloscope->displayLock);

      FORMAT_BUFFER();
      wxString text;
      text.Append("ScopeFun Oscilloscope Current Frame Data\n");

      Ring<OsciloscopeFrame> His = pOsciloscope->threadHistory;
      His.offset(pOsciloscope->window.horizontal.Frame);
      His.offset( His.getSize() - pOsciloscope->settings.historyFrameClipboard );

      int f = 0;
      OsciloscopeFrame frame;
      uint frameCount = pOsciloscope->settings.historyFrameClipboard;
      for (uint i = 0; i < frameCount;i++)
      {
         His.read(frame);
         FORMAT("Frame : %d \n", f);
         text.append(formatBuffer);
         f++;

         char buffer[FORMAT_BUFFER_SIZE] = { 0 };
         frame.getTime(buffer, FORMAT_BUFFER_SIZE);
         FORMAT("   Date-Time-Offset: %s \n", buffer);
         text.append(formatBuffer);

         for (int c = 0; c < 2; c++)
         {
            FORMAT("   Channel %d \n", c+1);
            text.Append(formatBuffer);
            FORMAT("       Time/Div: %f ms \n", pOsciloscope->window.horizontal.Capture*1000.f);
            text.append(formatBuffer);
            if (c == 0)
               FORMAT("       Volt/Div: %f V \n", pOsciloscope->window.channel01.Capture)
            else
               FORMAT("       Volt/Div: %f V \n", pOsciloscope->window.channel02.Capture);
            text.append(formatBuffer);

            FORMAT("       Values[%d,%d] : |", -int(MAXOSCVALUE), int(MAXOSCVALUE));
            text.append(formatBuffer);

            for (int i = 0; i < frame.analog[c].getCount(); i++)
            {
               ishort analog = frame.getAnalogShort(c, i);
               wxVariant temp(analog);
               wxString  output = temp.GetString();
               while (output.length() < 8)
               {
                  size_t len = output.length();
                  if (len % 2) output.Prepend(" ");
                  else        output.Append(" ");
               }

               text.Append(output);
               text.Append(_T("|"));
            }
            text.Append(_T("\n"));
         }
         text.append("   Digital Channels     : |");
         for (int i = 0; i < frame.digital.getCount(); i++)
         {
            ushort digital = frame.getDigitalChannels(i);

            FORMAT("0x%x", digital);
            wxString  output(formatBuffer);
            while (output.length() < 8)
            {
               size_t len = output.length();
               if (len % 2) output.Prepend(" ");
               else        output.Append(" ");
            }
            text.Append(output);
            text.Append(_T("|"));
         }
         text.Append(_T("\n"));
      }
      wxTheClipboard->SetData(new wxTextDataObject(text));
      wxTheClipboard->Close();

      SDL_AtomicUnlock(&pOsciloscope->displayLock);
   }
}

void OsciloskopOsciloskop::m_buttonReArmOnButtonClick(wxCommandEvent& event)
{
   pOsciloscope->control.setTriggerReArm(true);
   pOsciloscope->control.transferData();
   pOsciloscope->control.setTriggerReArm(false);
}

void OsciloskopOsciloskop::m_buttonAutoCallibrateOnButtonClick(wxCommandEvent& event)
{
   wxFileName fn = GetOscDataFolder();
   wxString fileName = fn.GetPath().append(_("/data/callibrate/callibrate.slot"));
   wxString saveName = fn.GetPath().append(_("/data/callibrate/start.slot"));

   pOsciloscope->windowState[getCurrentSlot()] = pOsciloscope->window;
   SaveSlot(getCurrentSlot(), saveName.ToAscii().data());

   LoadSlot(getCurrentSlot(), fileName.ToAscii().data());
   loadWindow(getCurrentSlot());

   pOsciloscope->AutoCallibrate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// spin
//////////////////////////////////////////////////////////////////////////////////////////////////

void OsciloskopOsciloskop::m_spinBtnXPosOnSpinUp(wxSpinEvent& event)
{
   m_sliderTimePosition->SetValue(m_sliderTimePosition->GetValue() + 1);
   wxScrollEvent e;
   m_sliderTimePositionOnScroll(e);
}

void OsciloskopOsciloskop::m_spinBtnXPosOnSpinDown(wxSpinEvent& event)
{
   m_sliderTimePosition->SetValue(m_sliderTimePosition->GetValue() - 1);
   wxScrollEvent e;
   m_sliderTimePositionOnScroll(e);
}

void OsciloskopOsciloskop::m_spinBtnFrameHistoryOnSpinUp(wxSpinEvent& event)
{
   m_sliderTimeFrame->SetValue(m_sliderTimeFrame->GetValue() + 1);
   wxScrollEvent e;
   m_sliderTimeFrameOnScroll(e);
}

void OsciloskopOsciloskop::m_spinBtnFrameHistoryOnSpinDown(wxSpinEvent& event)
{
   m_sliderTimeFrame->SetValue(m_sliderTimeFrame->GetValue() - 1);
   wxScrollEvent e;
   m_sliderTimeFrameOnScroll(e);
}

void OsciloskopOsciloskop::m_spinBtnCh0YPosOnSpinUp(wxSpinEvent& event)
{
   VoltageCapture  capture = (VoltageCapture)captureVoltFromValue(pOsciloscope->window.channel01.Capture);
   double   value = stringToDouble(m_textCtrlCh0Position->GetValue()) + pOsciloscope->settings.getAnalogChannelYVoltageStep(0,capture);
   m_textCtrlCh0Position->SetValue( doubleToString( value ) );
   wxCommandEvent e;
   m_textCtrlCh0PositionOnTextEnter(e);
}

void OsciloskopOsciloskop::m_spinBtnCh0YPosOnSpinDown(wxSpinEvent& event)
{
   VoltageCapture   capture = (VoltageCapture)captureVoltFromValue(pOsciloscope->window.channel01.Capture);
   double   value = stringToDouble(m_textCtrlCh0Position->GetValue()) - pOsciloscope->settings.getAnalogChannelYVoltageStep(0,capture);
   m_textCtrlCh0Position->SetValue(doubleToString(value));
   wxCommandEvent e;
   m_textCtrlCh0PositionOnTextEnter(e);
}

void OsciloskopOsciloskop::m_spinBtnCh1YPosOnSpinUp(wxSpinEvent& event)
{
   VoltageCapture   capture = (VoltageCapture)captureVoltFromValue(pOsciloscope->window.channel02.Capture);
   double   value = stringToDouble(m_textCtrlCh1Position->GetValue()) + pOsciloscope->settings.getAnalogChannelYVoltageStep(1,capture);
   m_textCtrlCh1Position->SetValue(doubleToString(value));
   wxCommandEvent e;
   m_textCtrlCh1PositionOnTextEnter(e);
}

void OsciloskopOsciloskop::m_spinBtnCh1YPosOnSpinDown(wxSpinEvent& event)
{
   VoltageCapture   capture = (VoltageCapture)captureVoltFromValue(pOsciloscope->window.channel02.Capture);
   double   value = stringToDouble(m_textCtrlCh1Position->GetValue()) - pOsciloscope->settings.getAnalogChannelYVoltageStep(1,capture);
   m_textCtrlCh1Position->SetValue(doubleToString(value));
   wxCommandEvent e;
   m_textCtrlCh1PositionOnTextEnter(e);
}

void OsciloskopOsciloskop::m_spinBtnDigVoltageOnSpinUp(wxSpinEvent& event)
{
   double kDigital = pOsciloscope->settings.digitalVoltageCoeficient;
   double Vstep = 1.25 * ((1.0 / kDigital) + 1.0) - 1.25;
   double Vmin  = 1.25 * ((0.0 / kDigital) + 1.0);
   double Vmax  = 1.25 * ((255.0 / kDigital) + 1.0);
   double value = stringToDouble( m_textCtrlDigitalVoltage->GetValue() ) + Vstep;
          value = clamp(value, Vmin, Vmax);
   m_textCtrlDigitalVoltage->SetValue(doubleToString(value));
   pOsciloscope->window.digitalSetup.voltage = value;
   pOsciloscope->control.setDigitalVoltage(value);
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_spinBtnDigVoltageOnSpinDown(wxSpinEvent& event)
{
   double kDigital = pOsciloscope->settings.digitalVoltageCoeficient;
   double Vstep = 1.25 * ((1.0 / kDigital) + 1.0) - 1.25;
   double Vmin = 1.25 * ((0.0 / kDigital) + 1.0);
   double Vmax = 1.25 * ((255.0 / kDigital) + 1.0);
   double value = stringToDouble(m_textCtrlDigitalVoltage->GetValue()) - Vstep;
   value = clamp(value, Vmin, Vmax);
   m_textCtrlDigitalVoltage->SetValue(doubleToString(value));
   pOsciloscope->window.digitalSetup.voltage = value;
   pOsciloscope->control.setDigitalVoltage(value);
   pOsciloscope->control.transferData();
}

void OsciloskopOsciloskop::m_spinBtnTrigLevelOnSpinUp(wxSpinEvent& event)
{
   double  triggerLevel = stringToDouble(m_textCtrlTriggerLevel->GetValue().ToAscii().data());
           triggerLevel = triggerLevel + pOsciloscope->getTriggerVoltagePerStep();
           m_textCtrlTriggerLevel->SetValue(doubleToString(triggerLevel));

   int    numberOfSteps = int(triggerLevel / pOsciloscope->getTriggerVoltagePerStep());
   pOsciloscope->window.trigger.Level = numberOfSteps;
   pOsciloscope->control.setTriggerLevel(numberOfSteps);
   pOsciloscope->control.transferData();
   m_sliderTriggerLevel->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_spinBtnTrigLevelOnSpinDown(wxSpinEvent& event)
{
   double  triggerLevel = stringToDouble(m_textCtrlTriggerLevel->GetValue().ToAscii().data());
   triggerLevel = triggerLevel - pOsciloscope->getTriggerVoltagePerStep();
   m_textCtrlTriggerLevel->SetValue(doubleToString(triggerLevel));

   int    numberOfSteps = int(triggerLevel / pOsciloscope->getTriggerVoltagePerStep());
   pOsciloscope->window.trigger.Level = numberOfSteps;
   pOsciloscope->control.setTriggerLevel(numberOfSteps);
   pOsciloscope->control.transferData();
   m_sliderTriggerLevel->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_spinBtnTrigHisOnSpinUp(wxSpinEvent& event)
{
   double  hisLevel = stringToDouble(m_textCtrlTriggerHisteresis->GetValue().ToAscii().data());
           hisLevel = hisLevel + pOsciloscope->getTriggerVoltagePerStep();
           m_textCtrlTriggerHisteresis->SetValue(doubleToString(hisLevel));

   int    numberOfSteps = int(hisLevel / pOsciloscope->getTriggerVoltagePerStep());
   pOsciloscope->window.trigger.His = numberOfSteps;
   pOsciloscope->control.setTriggerHis(numberOfSteps);
   pOsciloscope->control.transferData();
   //
   m_sliderTriggerHisteresis->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_spinBtnTrigHisOnSpinDown(wxSpinEvent& event)
{
   double  hisLevel = stringToDouble(m_textCtrlTriggerHisteresis->GetValue().ToAscii().data());
           hisLevel = hisLevel - pOsciloscope->getTriggerVoltagePerStep();
           m_textCtrlTriggerHisteresis->SetValue(doubleToString(hisLevel));

   int    numberOfSteps = int(hisLevel / pOsciloscope->getTriggerVoltagePerStep());
   pOsciloscope->window.trigger.His = numberOfSteps;
   pOsciloscope->control.setTriggerHis(numberOfSteps);
   pOsciloscope->control.transferData();
   //
   m_sliderTriggerHisteresis->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_spinBtnTrigPreOnSpinUp(wxSpinEvent& event)
{
   pOsciloscope->window.trigger.Percent = max( stringToFloat( m_textCtrlTriggerPre->GetValue().ToAscii().data() ) + 1.0f, 0.f);
   pOsciloscope->control.setTriggerPre( stringToFloat( m_textCtrlTriggerPre->GetValue().ToAscii().data() ) );
   pOsciloscope->control.transferData();

   m_sliderTriggerPre->SetValue( pOsciloscope->window.trigger.Percent );
   m_textCtrlTriggerPre->SetValue(floatToString(pOsciloscope->window.trigger.Percent));
}

void OsciloskopOsciloskop::m_spinBtnTrigPreOnSpinDown(wxSpinEvent& event)
{
   pOsciloscope->window.trigger.Percent = max( stringToFloat(m_textCtrlTriggerPre->GetValue().ToAscii().data()) - 1.0f, 0.f );
   pOsciloscope->control.setTriggerPre(stringToFloat(m_textCtrlTriggerPre->GetValue().ToAscii().data()));
   pOsciloscope->control.transferData();

   m_sliderTriggerPre->SetValue(pOsciloscope->window.trigger.Percent);
   m_textCtrlTriggerPre->SetValue(floatToString(pOsciloscope->window.trigger.Percent));
}

void OsciloskopOsciloskop::m_spinBtnTrigHoldoffOnSpinUp(wxSpinEvent& event)
{
   double Hstep = 1000 * double(NANO);
   pOsciloscope->window.trigger.Holdoff = stringToDouble(m_textCtrlTriggerHoldoff->GetValue().ToAscii().data()) + Hstep;
   pOsciloscope->control.setHoldoff(stringToDouble(m_textCtrlTriggerHoldoff->GetValue().ToAscii().data()));
   pOsciloscope->control.transferData();

   m_sliderTriggerHoldoff->SetValue(pOsciloscope->window.trigger.Holdoff);
   m_textCtrlTriggerHoldoff->SetValue(doubleToString(pOsciloscope->window.trigger.Holdoff));
}

void OsciloskopOsciloskop::m_spinBtnTrigHoldoffOnSpinDown(wxSpinEvent& event)
{
   double Hstep = 1000 * double(NANO);
   pOsciloscope->window.trigger.Holdoff = stringToDouble(m_textCtrlTriggerHoldoff->GetValue().ToAscii().data()) - Hstep;
   pOsciloscope->control.setHoldoff(stringToDouble(m_textCtrlTriggerHoldoff->GetValue().ToAscii().data()));
   pOsciloscope->control.transferData();

   m_sliderTriggerHoldoff->SetValue(pOsciloscope->window.trigger.Holdoff);
   m_textCtrlTriggerHoldoff->SetValue(doubleToString(pOsciloscope->window.trigger.Holdoff));
}

void OsciloskopOsciloskop::RecalculateTriggerPosition(double oldTriggerVoltagePerStep,double newTriggerVoltagePerStep)
{
   // trigger level
   // pOsciloscope->window.trigger.Level = double(pOsciloscope->window.trigger.Level) * oldTriggerVoltagePerStep / newTriggerVoltagePerStep;
   int    newTriggerSteps = pOsciloscope->window.trigger.Level;
   double newTriggerLevel = double(pOsciloscope->window.trigger.Level) * newTriggerVoltagePerStep;
   m_sliderTriggerLevel->SetValue(newTriggerSteps);
   m_textCtrlTriggerLevel->SetValue(doubleToString(newTriggerLevel));
   pOsciloscope->window.trigger.Level = newTriggerSteps;
   pOsciloscope->control.setTriggerLevel(newTriggerSteps);


   // histeresis level
   // pOsciloscope->window.trigger.His = double(pOsciloscope->window.trigger.His) * oldTriggerVoltagePerStep / newTriggerVoltagePerStep;
   int     hisSteps = pOsciloscope->window.trigger.His;
   double  hisLevel = double(pOsciloscope->window.trigger.His) * newTriggerVoltagePerStep;
   m_sliderTriggerHisteresis->SetValue(hisSteps);
   m_textCtrlTriggerHisteresis->SetValue(doubleToString(hisLevel));
   pOsciloscope->window.trigger.His = hisSteps;
   pOsciloscope->control.setTriggerHis(hisSteps);
}
