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
#ifndef __ARRAY__
#define __ARRAY__

////////////////////////////////////////////////////////////////////////////////
//
// static array
//
////////////////////////////////////////////////////////////////////////////////
template<class T,int size> class Array {
private:
	T	 data[size];
	int count;
public:
   Array()
   {
      count = 0;
   }
public:
   int setCount(int count)
   {
      if( count >= 0 && count <= size )
      {
         this->count = count;
         return 0;
      }
      return 1;
   }

   int getCount() const
   {
      return count;
   }

   int getSize() const
   {
      return size;
   }
public:
   T &index(int idx)
   {
      if( idx < 0 || idx > count ) CORE_ABORT("idx out of range",0);
      return data[idx];
   }

   const T &index(int idx) const
   {
      if( idx < 0 || idx > count ) CORE_ABORT("idx out of range",0);
      return data[idx];
   }

   int find(const T &el) const
   {
      for(int idx=0;idx<count;idx++)
      {
         if(data[idx]==el) return idx;
      }
      return -1;
   }

   void insert(int index,const T &el)
   {
      setCount( count + 1 );

      for(int idx=count-1;idx>index;idx--)
      {
         data[idx] = data[idx-1];
      }
      data[index]=el;
   }

   void remove(int index)
   {
      for(int idx=index;idx<count;idx++)
      {
         data[idx] = data[idx+1];
      }
      setCount( count - 1 );
   }

   void clear()
   {
      count = 0;
   }

   int findRemove(const T& el)
   {
      int idx = find(el);
      if( idx>=0 )
      {
         remove(idx);
         return 0;
      }
      return 1;
   }

   T &first()
   {
      return index(0);
   }

   const T &first() const
   {
      return index(0);
   }

   T &last()
   {
      return index( getCount() - 1 );
   }

   const T &last() const
   {
      return index( getCount() - 1 );
   }

   T& add()
   {
      T element;
      pushBack(element);
      return last();
   }

   void pushBack(const T &el)
   {
      insert(count,el);
   }

   void pushFront(const T &el)
   {
      insert(0,el);
   }

   void popBack()
   {
      remove( count-1 );
   }

   void popFront()
   {
      remove(0);
   }
public:
   operator int () const
   {
      return getCount();
   }

   const T &operator[](int idx) const
   {
      return index(idx);
   }

   T &operator[](int idx)
   {
      return index(idx);
   }
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
