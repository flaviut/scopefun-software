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
#include "OsciloskopInfo.h"

#include <wx/config.h>
#include <wx/app.h>

#define VERSION_INFO "2016 1.00, rc4"

OsciloskopInfo::OsciloskopInfo( wxWindow* parent )
:
Info( parent )
{

}

int checkAgree()
{
   wxConfig config(wxApp::GetInstance()->GetAppName());
   int agree = 0;
   int ret = config.Read(wxT("License_Agree"), &agree, 0);
   return agree;
}

void Agree()
{
   wxConfig config(wxApp::GetInstance()->GetAppName());
   config.Write( wxT("License_Agree"), 1 );
   config.Flush();
}

void OsciloskopInfo::ThermalOnActivate( wxActivateEvent& event )
{
// TODO: Implement ThermalOnActivate
   wxFile fileVersion;
   wxString stringVersion;
   fileVersion.Open(fixPath("data/about/version.txt"));
   fileVersion.ReadAll(&stringVersion);
   fileVersion.Close();
   stringVersion = stringVersion.Trim();

   m_textCtrlVersion->Clear();
   m_textCtrlVersion->AppendText( wxT("Version:  ") );
   m_textCtrlVersion->AppendText( wxT(VERSION_INFO) );

   m_textCtrlDateTime->Clear();
   m_textCtrlDateTime->AppendText(wxT("Timestamp: "));
   m_textCtrlDateTime->AppendText(stringVersion);

   wxBitmap bm;
   bm.LoadFile(fixPath("data/icon/icon512.bmp"),wxBITMAP_TYPE_BMP);
   m_bitmapLogo->SetBitmap(bm);
   m_bitmapLogo->Center(wxHORIZONTAL);

   wxBitmap gplbm;
   gplbm.LoadFile(fixPath("data/logo/gpl.bmp"),wxBITMAP_TYPE_BMP);
   m_gplLogo->SetBitmap(gplbm);
   m_gplLogo->Center(wxHORIZONTAL);

   m_textCtrlInfo->LoadFile(fixPath("data/about/info.txt"));
   m_textCtrlLicense->LoadFile(fixPath( "data/about/license.txt") );
   m_textCtrlGlew->LoadFile( fixPath("data/about/glew.txt") );
   m_textCtrlKissFFT->LoadFile( fixPath("data/about/kissfft.txt") );
   m_textCtrlwxWidgets->LoadFile( fixPath("data/about/wxWidgets.txt") );
   m_textCtrlSDL2->LoadFile( fixPath("data/about/sdl2.txt") );
   m_textCtrlLibUSB->LoadFile( fixPath("data/about/libusb.txt") );
   m_textCtrlCJSON->LoadFile( fixPath("data/about/cjson.txt"));
   m_textCtrlPThreads->LoadFile( fixPath("data/about/winpthreads.txt"));


   #if !defined(PLATFORM_MINGW) && !defined(PLATFORM_WIN)
      if( m_notebook7->GetPageCount() == 7 )
        m_notebook7->RemovePage(6);
   #endif

   int agree = checkAgree();
   if( !agree )
   {
      m_buttonOk->SetLabel("I agree");
   }
   else
   {
      m_buttonOk->SetLabel("Ok");
   }
}

void OsciloskopInfo::InfoOnClose( wxCloseEvent& event )
{
// TODO: Implement InfoOnClose

   int agree = checkAgree();
   if( !agree )
   {
      GetParent()->Close(true);
   }
   else
   {
      Hide();
   }
}

void OsciloskopInfo::m_textCtrlLicenseOnTextURL( wxTextUrlEvent& event )
{
// TODO: Implement m_textCtrlLicenseOnTextURL
   if( event.GetMouseEvent().ButtonDown() )
   {
      wxString string = m_textCtrlLicense->GetValue().SubString(event.GetURLStart(),event.GetURLEnd());
      string.RemoveLast();
      wxLaunchDefaultBrowser(  string );
   }
}

void OsciloskopInfo::m_textCtrlGlewOnTextURL( wxTextUrlEvent& event )
{
// TODO: Implement m_textCtrlGlewOnTextURL
   if( event.GetMouseEvent().ButtonDown() )
   {
      wxLaunchDefaultBrowser(  m_textCtrlGlew->GetValue().SubString(event.GetURLStart(),event.GetURLEnd()) );
   }
}

void OsciloskopInfo::m_textCtrlInfoOnTextURL( wxTextUrlEvent& event )
{
// TODO: Implement m_textCtrlInfoOnTextURL
   if( event.GetMouseEvent().ButtonDown() )
   {
      wxLaunchDefaultBrowser(  m_textCtrlInfo->GetValue().SubString(event.GetURLStart(),event.GetURLEnd()) );
   }
}

void OsciloskopInfo::m_buttonOkOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement m_buttonOkOnButtonClick
   int agree = checkAgree();
   if( !agree )
   {
      Agree();
   }

   Hide();
}
