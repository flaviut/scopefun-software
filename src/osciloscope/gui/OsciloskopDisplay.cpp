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
#include "OsciloskopDisplay.h"

OsciloskopDisplay::OsciloskopDisplay( wxWindow* parent )
:
Display( parent )
{
}

void OsciloskopDisplay::DisplayOnInitDialog( wxInitDialogEvent& event )
{
// TODO: Implement DisplayOnInitDialog
    m_checkBoxUnits->SetValue( pOsciloscope->window.display.oscUnits );
    m_checkBoxAxis->SetValue( pOsciloscope->window.display.oscAxis );
    m_checkBoxGrid->SetValue( pOsciloscope->window.display.oscGrid );

    m_checkBoxFFTUnit->SetValue( pOsciloscope->window.display.fftUnits );
    m_checkBoxFFTAxis->SetValue( pOsciloscope->window.display.fftAxis );
    m_checkBoxFFTGrid->SetValue( pOsciloscope->window.display.fftGrid );

    m_checkBoxDigitalUnit->SetValue( pOsciloscope->window.display.digitalUnits );
    m_checkBoxDigitalAxis->SetValue( pOsciloscope->window.display.digitalAxis );
    m_checkBoxDigitalGrid->SetValue( pOsciloscope->window.display.digitalGrid );

    m_choiceSignal->SetSelection(pOsciloscope->window.display.signalType);
    m_choiceFFT->SetSelection(pOsciloscope->window.display.fftType);

    m_textCtrlSignal->SetValue( wxString::FromAscii( floatToString(pOsciloscope->window.display.signalWidth) ) );
    m_textCtrlFFT->SetValue( wxString::FromAscii( floatToString(pOsciloscope->window.display.fftWidth) ) );
    m_sliderSignal->SetValue((pOsciloscope->window.display.signalWidth / MAX_TRIANGLE_SIZE) * 100);
    m_sliderFFT->SetValue((pOsciloscope->window.display.fftWidth / MAX_TRIANGLE_SIZE) * 100);

    m_choiceFFTY->SetSelection( pOsciloscope->window.display.fftDecibel  );
    m_choiceFFTX->SetSelection( pOsciloscope->window.display.fftLogFreq  );

    m_checkBox3dSolid->SetValue( pOsciloscope->window.display.solid3d > 0 ? true : false );
    m_checkBox3dLight->SetValue(pOsciloscope->window.display.light3d> 0 ? true : false);
    m_checkBox3dDepthTest->SetValue(pOsciloscope->window.display.depthTest3d > 0 ? true : false);
    m_textCtrl3dTessalation->SetValue( wxString( integerToString(pOsciloscope->window.display.tessalation3d) ) );
    m_textCtrl3dTessalation->SetValue( wxString( integerToString(pOsciloscope->window.display.tessalation3d) ) );
    m_textCtrl3dAlphaCh0->SetValue( wxString(integerToString(pOsciloscope->window.display.alpha3dCh0)) );
    m_textCtrl3dAlphaCh1->SetValue(wxString(integerToString(pOsciloscope->window.display.alpha3dCh1)));
    m_textCtrl3dAlphaFun->SetValue(wxString(integerToString(pOsciloscope->window.display.alpha3dFun)));
    m_slider3dTessalation->SetValue(pOsciloscope->window.display.tessalation3d);
    m_slider2dTessalation->SetValue(pOsciloscope->window.display.tessalation2d);
    m_slider3dAlphaCh0->SetValue(pOsciloscope->window.display.alpha3dCh0);
    m_slider3dAlphaCh1->SetValue(pOsciloscope->window.display.alpha3dCh1);
    m_slider3dAlphaFun->SetValue(pOsciloscope->window.display.alpha3dFun);
}

void OsciloskopDisplay::m_checkBoxUnitsOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxUnitsOnCheckBox
    pOsciloscope->window.display.oscUnits = m_checkBoxUnits->GetValue();
}

void OsciloskopDisplay::m_checkBoxAxisOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxAxisOnCheckBox
    pOsciloscope->window.display.oscAxis = m_checkBoxAxis->GetValue();
}

void OsciloskopDisplay::m_checkBoxGridOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxGridOnCheckBox
    pOsciloscope->window.display.oscGrid = m_checkBoxGrid->GetValue();
}

void OsciloskopDisplay::m_checkBoxFFTUnitOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxFFTUnitOnCheckBox
    pOsciloscope->window.display.fftUnits = m_checkBoxFFTUnit->GetValue();
}

void OsciloskopDisplay::m_checkBoxFFTAxisOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxFFTAxisOnCheckBox
    pOsciloscope->window.display.fftAxis = m_checkBoxFFTAxis->GetValue();
}

void OsciloskopDisplay::m_checkBoxFFTGridOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxFFTGridOnCheckBox
    pOsciloscope->window.display.fftGrid = m_checkBoxFFTGrid->GetValue();
}

void OsciloskopDisplay::m_checkBoxDigitalUnitOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxDigitalUnitOnCheckBox
    pOsciloscope->window.display.digitalUnits = m_checkBoxDigitalUnit->GetValue();
}

void OsciloskopDisplay::m_checkBoxDigitalAxisOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxDigitalAxisOnCheckBox
    pOsciloscope->window.display.digitalAxis = m_checkBoxDigitalAxis->GetValue();
}

void OsciloskopDisplay::m_checkBoxDigitalGridOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxDigitalGridOnCheckBox
    pOsciloscope->window.display.digitalGrid = m_checkBoxDigitalGrid->GetValue();
}

void OsciloskopDisplay::m_choiceSignalOnChoice( wxCommandEvent& event )
{
// TODO: Implement m_choiceSignalOnChoice
   pOsciloscope->window.display.signalType = (SignalType)m_choiceSignal->GetSelection();
}

void OsciloskopDisplay::m_textCtrlSignalOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlSignalOnTextEnter
    pOsciloscope->window.display.signalWidth = stringToFloat( m_textCtrlSignal->GetValue().ToAscii().data() );
    m_sliderSignal->SetValue((pOsciloscope->window.display.signalWidth / MAX_TRIANGLE_SIZE) * 100);
}

void OsciloskopDisplay::m_sliderSignalOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderSignalOnScroll
   int   ivalue = m_sliderSignal->GetValue();
   float fvalue = (float(ivalue) / 100.f)*MAX_TRIANGLE_SIZE;
   pOsciloscope->window.display.signalWidth = fvalue;
   m_textCtrlSignal->SetValue(floatToString(fvalue));
}

void OsciloskopDisplay::m_choiceFFTOnChoice( wxCommandEvent& event )
{
// TODO: Implement m_choiceFFTOnChoice
   pOsciloscope->window.display.fftType = (SignalType)m_choiceFFT->GetSelection();
}

void OsciloskopDisplay::m_textCtrlFFTOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlFFTOnTextEnter
    pOsciloscope->window.display.fftWidth = stringToFloat( m_textCtrlFFT->GetValue().ToAscii().data() );
    m_sliderFFT->SetValue( (pOsciloscope->window.display.fftWidth/MAX_TRIANGLE_SIZE)*100 );
}

void OsciloskopDisplay::m_sliderFFTOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderFFTOnScroll
   int   ivalue = m_sliderFFT->GetValue();
   float fvalue = (float(ivalue) / 100.f)*MAX_TRIANGLE_SIZE;
   pOsciloscope->window.display.fftWidth = fvalue;
   m_textCtrlFFT->SetValue(floatToString(fvalue));
}

void OsciloskopDisplay::m_choiceFFTYOnChoice( wxCommandEvent& event )
{
// TODO: Implement m_choiceFFTYOnChoice
   pOsciloscope->window.display.fftDecibel = m_choiceFFTY->GetSelection();
}

void OsciloskopDisplay::m_choiceFFTXOnChoice( wxCommandEvent& event )
{
// TODO: Implement m_choiceFFTXOnChoice
    pOsciloscope->window.display.fftLogFreq = m_choiceFFTX->GetSelection();
}

void OsciloskopDisplay::m_checkBox3dSolidOnCheckBox(wxCommandEvent& event)
{
   pOsciloscope->window.display.solid3d = m_checkBox3dSolid->IsChecked() ? 1 : 0;
}
void OsciloskopDisplay::m_checkBox3dLightOnCheckBox(wxCommandEvent& event)
{
   pOsciloscope->window.display.light3d = m_checkBox3dLight->IsChecked() ? 1 : 0;
}
void OsciloskopDisplay::m_checkBox3dDepthTestOnCheckBox(wxCommandEvent& event)
{
   pOsciloscope->window.display.depthTest3d = m_checkBox3dDepthTest->IsChecked() ? 1 : 0;
}

void OsciloskopDisplay::m_textCtrl3dTessalationOnTextEnter(wxCommandEvent& event)
{
   uint tesselation3d = stringToInteger(m_textCtrl3dTessalation->GetValue().ToAscii().data());

   tesselation3d = pOsciloscope->max3dTesselation(tesselation3d, pOsciloscope->window);

   pOsciloscope->window.display.tessalation3d = tesselation3d;
   m_slider3dTessalation->SetValue(tesselation3d);
   m_textCtrl3dTessalation->SetValue(wxString(integerToString(tesselation3d)));
}
void OsciloskopDisplay::m_slider3dTessalationOnScroll(wxScrollEvent& event)
{
   uint tesselation3d = m_slider3dTessalation->GetValue();

   tesselation3d = pOsciloscope->max3dTesselation(tesselation3d, pOsciloscope->window);

   pOsciloscope->window.display.tessalation3d = tesselation3d;
   m_slider3dTessalation->SetValue(tesselation3d);
   m_textCtrl3dTessalation->SetValue(wxString(integerToString(tesselation3d)));
}

void OsciloskopDisplay::m_textCtrl2dTessalationOnTextEnter(wxCommandEvent& event)
{
   pOsciloscope->window.display.tessalation2d = stringToInteger(m_textCtrl2dTessalation->GetValue().ToAscii().data());
   m_slider2dTessalation->SetValue(pOsciloscope->window.display.tessalation2d);

}
void OsciloskopDisplay::m_slider2dTessalationOnScroll(wxScrollEvent& event)
{
   pOsciloscope->window.display.tessalation2d = m_slider2dTessalation->GetValue();
   m_textCtrl2dTessalation->SetValue(wxString(integerToString(pOsciloscope->window.display.tessalation2d)));
}

void OsciloskopDisplay::m_slider3dAlphaCh0OnScroll(wxScrollEvent& event)
{
   pOsciloscope->window.display.alpha3dCh0 = m_slider3dAlphaCh0->GetValue();
   m_textCtrl3dAlphaCh0->SetValue(wxString(integerToString(pOsciloscope->window.display.alpha3dCh0)));
}

void OsciloskopDisplay::m_textCtrl3dAlphaCh0OnTextEnter(wxCommandEvent& event)
{
   pOsciloscope->window.display.alpha3dCh0 = stringToInteger(m_textCtrl3dAlphaCh0->GetValue().ToAscii().data());
   m_slider3dAlphaCh0->SetValue(pOsciloscope->window.display.alpha3dCh0);
}

void OsciloskopDisplay::m_slider3dAlphaCh1OnScroll(wxScrollEvent& event)
{
   pOsciloscope->window.display.alpha3dCh1 = m_slider3dAlphaCh1->GetValue();
   m_textCtrl3dAlphaCh1->SetValue(wxString(integerToString(pOsciloscope->window.display.alpha3dCh1)));
}

void OsciloskopDisplay::m_textCtrl3dAlphaCh1OnTextEnter(wxCommandEvent& event)
{
   pOsciloscope->window.display.alpha3dCh1 = stringToInteger(m_textCtrl3dAlphaCh1->GetValue().ToAscii().data());
   m_slider3dAlphaCh1->SetValue(pOsciloscope->window.display.alpha3dCh1);
}

void OsciloskopDisplay::m_slider3dAlphaFunOnScroll(wxScrollEvent& event)
{
   pOsciloscope->window.display.alpha3dFun = m_slider3dAlphaFun->GetValue();
   m_textCtrl3dAlphaFun->SetValue(wxString(integerToString(pOsciloscope->window.display.alpha3dFun)));
}

void OsciloskopDisplay::m_textCtrl3dAlphaFunOnTextEnter(wxCommandEvent& event)
{
   pOsciloscope->window.display.alpha3dFun = stringToInteger(m_textCtrl3dAlphaFun->GetValue().ToAscii().data());
   m_slider3dAlphaFun->SetValue(pOsciloscope->window.display.alpha3dFun);
}

void OsciloskopDisplay::m_buttonOkOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement m_buttonOkOnButtonClick
   Hide();
}

void OsciloskopDisplay::m_buttonDefaultOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement m_buttonDefaultOnButtonClick
   pOsciloscope->window.display.Default();
   wxInitDialogEvent e;
   DisplayOnInitDialog( e );
}

void OsciloskopDisplay::m_buttonCancelOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement m_buttonCancelOnButtonClick
   Hide();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////
