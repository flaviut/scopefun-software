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
#ifndef __SET__
#define __SET__

////////////////////////////////////////////////////////////////////////////////
//
// static set
//
////////////////////////////////////////////////////////////////////////////////
template<class T,int size> class Set {
private:
	T								data[size];
	int							count;
public:
									Set()
                           {
                              count = 0;
                           }
public:
   int setCount(int count)
   {
      if( count>=0 && count <= size )
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
      int low  = 0;
      int high = count-1;
      while( low < high || low == high )
      {
         // mid = (low + high)/2 written as this prevents overflow
         int mid = low + ((high - low) / 2);
         if( data[mid] > el )
         {
            // element must be somewhere in the lower half
            high = mid - 1;
         }
         else if (data[mid] < el)
         {
            // element must be somewhere in the upper half
            low = mid + 1;
         }
         else
         {
            // exact match found, return result
            return mid;
         }
      }

      // not found, return insert point as negative value
      return -1-low;
   }

   int findRemove(const T &el)
   {
      int idx = find(el);
      if( idx>=0 )
      {
         remove(idx);
         return 0;
      }
      return 1;
   }

   int insert(const T &el)
   {
      int index = find(el);
      if( index < 0 )
      {
         index  = -index;
         index -= 1;

         setCount( count + 1 );

         for(int idx=count-1;idx>index;idx--)
         {
            data[idx] = data[idx-1];
         }
         data[index]=el;
      }
      return index;
   }
   void remove(int index)
   {
      for(int idx=index;idx<count;idx++)
      {
         data[idx] = data[idx+1];
      }
      setCount( count - 1 );
   }

   void remove(T &el)
   {
      int index = find(el);
      if(index == -1) return;

      remove(index);
   }

   T& add()
   {
      T element;
      int idx = insert(element);
      if(idx<0)
         CORE_ABORT("Can't insert element into set",0);

      return data[idx];
   }

   operator int() const
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
