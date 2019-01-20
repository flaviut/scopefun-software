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
#include "wx/wx.h"
#include <osciloscope/gui/OsciloskopOsciloskop.h>

#include <core/core.h>

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <limits>

#include <wx/config.h>
#include <wx/app.h>

wxLocale* pLocalization = 0;

int loadLanguageFromConfig()
{
   wxConfig config( wxApp::GetInstance()->GetAppName() );
   long language = wxLANGUAGE_UNKNOWN;
   if (!config.Read(wxT("wxTranslation_Language"), &language, wxLANGUAGE_UNKNOWN) )
   {
      language = wxLANGUAGE_ENGLISH;
   }
   return language;
}

void saveLanguageToConfig(int id)
{
   wxConfig config(wxApp::GetInstance()->GetAppName());
   config.Write(wxT("wxTranslation_Language"), id);
   config.Flush();
}

wxFileName GetOscDataFolder();
wxFileName GetOscLangFolder();

void setLocalization(int id)
{
   if (pLocalization)
   {
      wxDELETE(pLocalization);
   }
   pLocalization = new wxLocale;
   pLocalization->Init(id, wxLOCALE_DONT_LOAD_DEFAULT);

   wxFileName fn = GetOscLangFolder();
   wxLocale::AddCatalogLookupPathPrefix( fn.GetPath() );
   bool ret = pLocalization->AddCatalog(_T("oscilloscope"));
}

class wxLanguageUserData : public wxObject
{
   wxDECLARE_ABSTRACT_CLASS(wxLanguageUserData);
public: wxLanguage data;
};
wxIMPLEMENT_ABSTRACT_CLASS(wxLanguageUserData, wxObject);

void recreateGUI(int initial = 0)
{
   // destroy current top level window
   wxApp* pApp = (wxApp*)wxApp::GetInstance();
   wxWindow * topwindow = pApp->GetTopWindow();
   if (topwindow)
   {
      wxArrayString list = wxTranslations::Get()->GetAvailableTranslations(_T("oscilloscope"));
      for (int i = 0; i<list.Count(); i++)
      {
         wxLanguageInfo * info = (wxLanguageInfo*)wxLocale::FindLanguageInfo(list.Item(i));
         if (!info)continue;

         wxMenuItem* item = ((OsciloskopOsciloskop*)topwindow)->m_menu6->FindItemByPosition(i);
         ((OsciloskopOsciloskop*)topwindow)->Disconnect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OsciloskopOsciloskop::MenuLanguageSelection ), 0, topwindow );
      }

      pApp->SetTopWindow(NULL);
      topwindow->Destroy();
   }

   // create new top level frame
   OsciloskopOsciloskop *frame = new OsciloskopOsciloskop(0);
   pApp->SetTopWindow(frame);

   // localization
   frame->m_menu6->GetMenuItems().empty();
   wxArrayString list = wxTranslations::Get()->GetAvailableTranslations(_T("oscilloscope"));
   for(int i=0;i<list.Count();i++)
   {
      wxLanguageInfo * info = (wxLanguageInfo*)wxLocale::FindLanguageInfo( list.Item(i) );
      if (!info)continue;

      wxMenuItem* menuItem = new wxMenuItem(frame->m_menu6, wxID_ANY, info->Description, wxEmptyString, wxITEM_NORMAL);
      frame->m_menu6->Append(menuItem);
      wxLanguageUserData * userData = new wxLanguageUserData();
      userData->data = (wxLanguage)info->Language;
      frame->Connect(menuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OsciloskopOsciloskop::MenuLanguageSelection), (wxObject*)userData, frame );
   }

   // set clinet size for proper border calculation
   frame->SetClientSize(pOsciloscope->settings.controlWidth, pOsciloscope->settings.controlHeight);

   // border
   wxRect rect3 = frame->GetClientRect();
   wxRect rect4 = frame->GetScreenRect();
   int  borderX = (rect4.width - rect3.width) / 2;
   int  borderY = (rect4.height - rect3.height) / 2;

   static wxPoint pt(pOsciloscope->sdlX + pRender->width, pOsciloscope->sdlY);
   if (initial)
   {
      pt.x += borderX;
      #ifdef MAC
            pt.y -= 2 * borderY;
      #endif
      #ifdef _WINDOWS
            pt.y -= borderY;
      #endif
   }

   // proper client size
   #ifdef _WINDOWS
      frame->SetClientSize(pOsciloscope->settings.controlWidth, pOsciloscope->settings.controlHeight - borderY + borderX);
   #endif

   // position
   frame->SetPosition(pt);

   // show window
   frame->Show(true);
   frame->Layout();
}

class MyApp: public wxApp
{
    bool OnInit()
    {
        // init
        if( !wxApp::OnInit() )
            return false;

        // used by wxConfig
        SetAppName("Oscilloscope");

        //// localization
        int language = loadLanguageFromConfig();
        setLocalization(language);
        saveLanguageToConfig(language);

        // mac
        #ifdef MAC
           void create();
           create();

           void setup();
           setup();

           pManager->start();
        #endif

        // gui
        recreateGUI(1);

        return true;
    }

    int OnExit()
    {
       pManager->stop();
       return 0;
    }
};

#ifdef MAC
  IMPLEMENT_APP(MyApp);
#else
   #if defined(LINUX)
      int main(int argc,char** argv)
   #endif
   #ifdef _WINDOWS
      int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
   #endif
   {
       void create();
       create();

       void setup();
       setup();

       pManager->start();

       wxApp* pApp = new MyApp();
       wxApp::SetInstance(pApp);

       int cc = 0;
       wxEntry( cc, (char**)0 );

       pManager->stop();

       return 0;
   }
#endif
