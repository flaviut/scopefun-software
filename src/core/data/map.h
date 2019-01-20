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
#ifndef __MAP__
#define __MAP__

////////////////////////////////////////////////////////////////////////////////
//
// map key
//
////////////////////////////////////////////////////////////////////////////////
template<class K,class R> class MapKey {
public:
   K Key;
   R Result;
public:
   int operator < (MapKey<K,R> mk) const
   {
      if( Key < mk.Key ) return 1;
      return 0;
   }
   int operator > (MapKey<K, R> mk) const
   {
      if (Key > mk.Key) return 1;
      return 0;
   }
   int operator == (MapKey<K,R> mk) const
   {
      if( Key == mk.Key ) return 1;
      return 0;
   }
};

////////////////////////////////////////////////////////////////////////////////
//
// static map
//
////////////////////////////////////////////////////////////////////////////////
template<class K,class R,int size> class Map {
private:
   Set< MapKey<K,R>, size >	data;
public:
   Map()
   {
   }
public:
   void setCount(int count)
   {
      data.setCount(count);
   }
   int getCount() const
   {
      return data.getCount();
   }
public:
   R &index(int index)
   {
      return data.index(index).Result;
   }

   const R &index(int index) const
   {
      return data.index(index).Result;
   }

   const K &indexKey(int index) const
   {
      return data.index(index).Key;
   }

   int find(const K &key) const
   {
      MapKey<K,R> TempKey;
      TempKey.Key = key;
      return data.find( TempKey );
   }

   int findRemove(const K &key)
   {
      int idx = find(key);
      if( idx>=0 )
      {
         remove(idx);
         return 0;
      }
      return 1;
   }

   int insert(const K &key,const R &result)
   {
      MapKey<K,R> TempKey;
      TempKey.Key    = key;
      TempKey.Result = result;
      return data.insert( TempKey );
   }

   void remove(int index)
   {
      return data.remove(index);
   }

   R& add(const K &key)
   {
      R result;
      int idx = insert(key,result);
      if(idx<0)
         CORE_ABORT("Can't insert element into set",0);

      return data[idx];
   }
public:
   operator int() const
   {
      return data.getCount();
   }

   const R &operator[](int idx) const
   {
      return data.index(idx).Result;
   }

   R &operator[](int idx)
   {
      return data.index(idx).Result;
   }
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
