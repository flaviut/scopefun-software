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
#ifndef __RING__
#define __RING__

////////////////////////////////////////////////////////////////////////////////
//
// Ring
//
///////////////////////////////////////////////////////////////////////////////
template<class T> class Ring {
private:
   uint  size;
   uint  start;
   uint  count;
   T*    data;
public:
   Ring() : size(0), start(0), count(0), data(0) {};
public:
   void init(T* buffer, uint size)
   {
       this->size  = size;
       this->start = 0;
       this->count = 0;
       this->data  = buffer;
   }

   void clear()
   {
      start = 0;
      count = 0;
   }

   int isFull()
   {
      return count == size;
   }

   int isEmpty()
   {
       return count == 0;
   }

   void offset(uint amount)
   {
      start = (start + amount) % size;
   }

   uint getCount()
   {
      return count;
   }

   void setCount(uint cnt)
   {
      count = cnt;
   }

   uint getSize()
   {
      return size;
   }

   void setStart(uint istart)
   {
      start = istart;
   }

   uint getStart()
   {
      return start;
   }

   uint getEnd()
   {
      return (start + count) % size;
   }

   void write(T& elem)
   {
       uint end = (start + count) % size;
       data[end] = elem;
       if ( count < size ) count++;
       else
       {
          start = (start + 1) % size;
       }
   }

   void read(T& elem)
   {
       elem  = data[start];
       start = (start + 1) % size;
       --count;
   }
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
