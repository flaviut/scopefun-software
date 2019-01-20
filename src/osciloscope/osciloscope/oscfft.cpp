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

INLINE float complexMultiplyReal(float aRe, float aIm, float bRe, float bIm)
{
   return aRe*bRe - aIm*bIm;
}

INLINE float complexMultiplyImag(float aRe, float aIm, float bRe, float bIm)
{
   return aIm*bRe + aRe*bIm;
}

void OsciloscopeThreadRenderer::dftCalculate(iint n, double* inRe, double* inIm, double* outRe, double* outIm)
{
   for(int i=0;i<n;i++)
   {
      for(int j=0;j<n;j++)
      {
         double angle = 2.f*PI*double(i)*double(j)/double(n);
         double cosRe =  cos( angle );
         double sinIm = -sin( angle );
         outRe[i] += complexMultiplyReal( cosRe, sinIm, inRe[j], inIm[j] );
         outIm[i] += complexMultiplyImag( cosRe, sinIm, inRe[j], inIm[j] );
      }
   }
}

void OsciloscopeThreadRenderer::fftCalculate( uint threadId, uint backward, uint n, double* real, double* imag)
{
   if( n <= NUM_FFT )
   {
      size_t          size = (size_t)cfgSize;
      kiss_fft_cfg     cfg = kiss_fft_alloc(n,0,cfgMem,&size);
      kiss_fft_cpx* fftIn  = in;
      kiss_fft_cpx* fftOut = out;

      for(uint i=0;i<n;i++)
      {
         fftIn[i].r = real[i];
         fftIn[i].i = imag[i];
      }
      kiss_fft(cfg,fftIn,fftOut);
      for(uint i = 0; i < n; i++)
      {
         fftOut[i].r = fftOut[i].r / double(n/2);
         fftOut[i].i = fftOut[i].i / double(n/2);
      }
      for(uint i=0;i<n;i++)
      {
         real[i] = fftOut[i].r;
         imag[i] = fftOut[i].i;
      }
   }
}

void OsciloscopeThreadRenderer::fftAmplitude(iint points,iint n, double *gAmp, double *gRe, double *gIm)
{
  for(iint x = 0; x < n; x++)
  {
     gAmp[x] = gRe[x] * gRe[x] + gIm[x] * gIm[x];
  }
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
