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
#ifndef __ENDIAN__
#define __ENDIAN__

FORCE_INLINE ushort endianSwap16(ushort &value)
{
   ushort low  = (value&0x00FF);
   ushort high = (value&0xFF00);
   return (low<<8)|(high>>8);
}

FORCE_INLINE uint endianSwap32(uint &value)
{
   uint v0 = (value&0x000000FF);
   uint v1 = (value&0x0000FF00);
   uint v2 = (value&0x00FF0000);
   uint v3 = (value&0xFF000000);
   return (v0>>24)|(v1>>4)|(v2<<4)|(v3<<24);
}

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
