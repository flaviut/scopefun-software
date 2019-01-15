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
#include "OsciloskopThermal.h"

OsciloskopThermal::OsciloskopThermal( wxWindow* parent )
:
Thermal( parent )
{

}

void OsciloskopThermal::ThermalOnActivate( wxActivateEvent& event )
{
// TODO: Implement ThermalOnActivate
   m_textCtrlHeating->SetValue( wxString::FromAscii( floatToString( pOsciloscope->window.thermal.heating ) ) );
   m_textCtrlCooling->SetValue( wxString::FromAscii( floatToString( pOsciloscope->window.thermal.cooling ) ) );
   m_sliderHeat->SetValue( (pOsciloscope->window.thermal.heating / MAX_HEAT) * 100 );
   m_sliderCool->SetValue( (pOsciloscope->window.thermal.cooling / MAX_COOL) * 100 );
   m_checkBoxEnabled->SetValue(  pOsciloscope->window.thermal.enabled );
}

void OsciloskopThermal::m_textCtrlHeatingOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlHeatingOnTextEnter
   pOsciloscope->window.thermal.heating = stringToFloat( m_textCtrlHeating->GetValue().ToAscii().data() );
   m_sliderHeat->SetValue( (pOsciloscope->window.thermal.heating / MAX_HEAT)*100 );
}

void OsciloskopThermal::m_sliderHeatOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderHeatOnScroll
   int   ivalue = m_sliderHeat->GetValue();
   float fvalue = (float(ivalue) / 100.f)*MAX_HEAT;
   pOsciloscope->window.thermal.heating = fvalue;
   m_textCtrlHeating->SetValue(floatToString(fvalue));   
}

void OsciloskopThermal::m_textCtrlCoolingOnTextEnter( wxCommandEvent& event )
{
// TODO: Implement m_textCtrlCoolingOnTextEnter
   pOsciloscope->window.thermal.cooling = stringToFloat( m_textCtrlCooling->GetValue().ToAscii().data() );
   m_sliderCool->SetValue((pOsciloscope->window.thermal.cooling/ MAX_COOL) * 100);
}

void OsciloskopThermal::m_sliderCoolOnScroll( wxScrollEvent& event )
{
// TODO: Implement m_sliderCoolOnScroll
   int   ivalue = m_sliderCool->GetValue();
   float fvalue = (float(ivalue) / 100.f)*MAX_COOL;
   pOsciloscope->window.thermal.cooling = fvalue;
   m_textCtrlCooling->SetValue(floatToString(fvalue));
}

void OsciloskopThermal::m_checkBoxEnabledOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_checkBoxEnabledOnCheckBox
   pOsciloscope->window.thermal.enabled = m_checkBoxEnabled->GetValue();
}

void OsciloskopThermal::m_spinBtnHeatingOnSpinUp(wxSpinEvent& event)
{
   m_sliderHeat->SetValue(m_sliderHeat->GetValue() + 1);
   wxScrollEvent e;
   m_sliderHeatOnScroll(e);
}
void OsciloskopThermal::m_spinBtnHeatingOnSpinDown(wxSpinEvent& event)
{
   m_sliderHeat->SetValue(m_sliderHeat->GetValue() - 1);
   wxScrollEvent e;
   m_sliderHeatOnScroll(e);
}
void OsciloskopThermal::m_spinBtnCoolingOnSpinUp(wxSpinEvent& event)
{
   m_sliderCool->SetValue(m_sliderCool->GetValue() + 1);
   wxScrollEvent e;
   m_sliderCoolOnScroll(e);
}
void OsciloskopThermal::m_spinBtnCoolingOnSpinDown(wxSpinEvent& event)
{
   m_sliderCool->SetValue(m_sliderCool->GetValue() - 1);
   wxScrollEvent e;
   m_sliderCoolOnScroll(e);
}

void OsciloskopThermal::m_buttonOkOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement m_buttonOkOnButtonClick
   Hide();
}

void OsciloskopThermal::m_buttonDefaultOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement m_buttonDefaultOnButtonClick
   pOsciloscope->window.thermal.Default();
   wxActivateEvent e;
   ThermalOnActivate( e );
}

void OsciloskopThermal::m_buttonCancelOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement m_buttonCancelOnButtonClick
   Hide();
}
