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
#ifndef __CORE__FLAG__
#define __CORE__FLAG__

////////////////////////////////////////////////////////////////////////////////
//
// Flag
//
////////////////////////////////////////////////////////////////////////////////
template<class T>  class Flag {
private:
   T attr;
public:
   Flag() : attr(0) {}
public:
   void raise(T bits)               { attr |=  bits; }
   void lower(T bits)               { attr &= ~bits; }
   void togle(T bits)               { attr ^=  bits; }
   T    is(T bits)                  { return attr & bits; }
   void bit(T bits, T value)        { attr ^= ((~value +1)^attr) & bits; }
public:
   void set(T bits)   { attr  =  bits; };
   T    get()         { return attr;   };
   void clear()       { attr = 0; };
};

class Flag8  : public Flag<byte> {};
class Flag16 : public Flag<ushort> {};
class Flag32 : public Flag<uint> {};
class Flag64 : public Flag<ularge> {};

INLINE ushort raiseFlag16(ushort attr,ushort bits) { return attr |  bits; }
INLINE ushort lowerFlag16(ushort attr,ushort bits) { return attr & ~bits; }

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
