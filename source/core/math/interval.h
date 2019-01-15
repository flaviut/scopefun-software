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
#ifndef __MATH__INTERVAL__
#define __MATH__INTERVAL__

template<class T> T min( T a, T b )
{
   if( a < b ) return a;
   else return b;
}

template<class T> T max( T a, T b )
{
   if( a > b ) return a;
   else return b;
}

template<class T> T clamp( T a, T min, T max )
{
   if( a < min ) return min;
   if( a > max ) return max;
   return a;
}

template<class T> T saturate( T a )
{
   if( a < 0 ) return 0;
   if( a > 1 ) return 1;
   return a;
}

FORCE_INLINE float lerp(float a, float b, float t)
{
   return (1 - t)*a + t*b;
}

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
