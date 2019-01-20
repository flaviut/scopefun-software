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
#ifndef __CORE__FONT__
#define __CORE__FONT__

#pragma pack(push)
#pragma pack(1)

////////////////////////////////////////////////////////////////////////////////
//
// BlockID
//
////////////////////////////////////////////////////////////////////////////////
class BlockID {
public:
   char  id;
   unsigned int   size;
};


////////////////////////////////////////////////////////////////////////////////
//
// CharSet
//
////////////////////////////////////////////////////////////////////////////////
class CharacterSet
{
public:
	unsigned short LineHeight;
	unsigned short Base;
	unsigned short Width, Height;
	unsigned short Pages;
   char           bits;
   char           a;
   char           r;
   char           g;
   char           b;
public:
    CharacterSet()
    {
      LineHeight = 0;
	   Base = 0;
	   Width = 0;
      Height = 0;
	   Pages = 0;
      bits = 0;
      a = 0;
      r = 0;
      g = 0;
      b = 0;
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// CharacterDescriptor
//
////////////////////////////////////////////////////////////////////////////////
class CharacterDescriptor
{
public:
   int     id;
	short   x, y;
	short   Width, Height;
	short   XOffset, YOffset;
	short   XAdvance;
	char    Page;
   char    chanel;
public:
   CharacterDescriptor()
   {
     id       = 0;
     x        = 0;
     y        = 0;
     Width    = 0;
     Height   = 0;
     XOffset  = 0;
     YOffset  = 0;
     XAdvance = 0;
     Page     = 0;
     chanel   = 0;
   }
};

////////////////////////////////////////////////////////////////////////////////
//
// FontKerningPair
//
////////////////////////////////////////////////////////////////////////////////
class FontKerningPair {
public:
   int    first;
   int    second;
   short  ammount;
public:
   int operator==(const FontKerningPair &pair) const
   {
      if( first  == pair.first &&
          second == pair.second ) return 1;
      return 0;
   }
};

#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
//
// FontRect
//
////////////////////////////////////////////////////////////////////////////////
class FontRect{
public:
   int                  rminx;
   int                  rminy;
   int                  rmaxx;
   int                  rmaxy;
};


////////////////////////////////////////////////////////////////////////////////
//
// FontManager
//
////////////////////////////////////////////////////////////////////////////////
class FontManager : public Manager {
public:
   GrTexture                   *pTexture;
   CharacterSet                CharSet;
   CharacterDescriptor         Chars[256];
   Array<FontKerningPair,256> aKerningPair;
   float                       sizeThread[MAX_THREAD];
   int                         kerningOnOff;
public:
   FontManager();
public:
   void  setSize(uint threadId,float size);
   float getSize(uint threadId) const { return sizeThread[threadId]; };
public:
   void  setKerning(int enable) {kerningOnOff=enable;};
public:
   float getLineHeight(uint threadid) const;
   int   getWidth(uint threadId,String &text) const;
public:
   void writeText3d(uint threadId,Matrix4x4 &final,float x,float y,float z,Vector4 right,Vector4 up,char* text,int color = 0xffffffff,float scaleX=1.f,float scaleY=1.f);
   void writeText(uint threadId,int x,int y,String text,int color = 0xffffffff);
public:
   void threadStart(uint threadId);
public:
   int start();
   int stop();
};

MANAGER_POINTER(Font);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
