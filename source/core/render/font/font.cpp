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
#include<core/core.h>

////////////////////////////////////////////////////////////////////////////////
// globals - manager
///////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER(Font);

////////////////////////////////////////////////////////////////////////////////
//
// FontManager
//
////////////////////////////////////////////////////////////////////////////////
FontManager::FontManager()
{
   pTexture = 0;
   for (int i = 0; i < MAX_THREAD;i++)
      sizeThread[i] = 0.75f;
   kerningOnOff = 0;
}

void FontManager::threadStart(uint id)
{
   sizeThread[id] = 0.75f;
}

void FontManager::setSize(uint threadId,float fsize)
{
   sizeThread[threadId] = fsize;
}

float FontManager::getLineHeight(uint threadId) const
{
   return ( float(CharSet.LineHeight)/float(CharSet.Height) )  * sizeThread[threadId];
}

int FontManager::getWidth(uint threadid,String &text) const
{
   float size = sizeThread[threadid];
   float fx = 0;
   for(int c=0;c<text.getLength();c++)
   {
      ushort                ch = text[c];
      CharacterDescriptor Desc = Chars[ch];

      if( kerningOnOff )
      {
         int kerning = 0;
         if(c<text.getLength()-2)
         {
            ushort next = text[c+1];

            FontKerningPair Pair;
            Pair.first   = ch;
            Pair.second  = next;
            Pair.ammount = 0;
            int idx = aKerningPair.find(Pair);
            if( idx >= 0 )
            {
               Pair = aKerningPair.index(idx);
               kerning = Pair.ammount;
            }
         }
         fx += ( (float)Desc.XAdvance*size ) + kerning*size;
      }
      else
      {
         fx += ( (float)Desc.XAdvance*size );
      }
   }
   return (int)fx;
}

void FontManager::writeText3d(uint threadId,Matrix4x4 &final,float x,float y,float z,Vector4 right,Vector4 up,char* text,int color,float scaleX,float scaleY)
{
   float size = sizeThread[threadId];

   Vector4 pos(x,y,z,1.f);

   ilarge count = strlen(text);
   if( !count ) return;

   pCanvas3d->beginBatch(threadId,CANVAS3D_BATCH_TRIANGLEUV, 2*count );
   for(int c=0;c<count;c++)
   {
      ushort                ch = (ushort)text[c];
      CharacterDescriptor Desc = Chars[ch];

      float xoffset = (float(Desc.XOffset)/float(CharSet.Width)  )*size*scaleX;
      float yoffset = (float(Desc.YOffset)/float(CharSet.Height) )*size*scaleY;
      float  width  = (float(Desc.Width)/float(CharSet.Width) )*size*scaleX;
      float  height = (float(Desc.Height)/float(CharSet.Height) )*size*scaleY;

      Vector4 vOffsetX(xoffset);
      Vector4 vOffsetY(yoffset);

      Vector4 upperleft;
      upperleft = pos       + right*vOffsetX;
      upperleft = upperleft - up*vOffsetY;

      Vector4 vWidth(width);
      Vector4 vHeight(height);

      Vector4 vec0 = upperleft;
      Vector4 vec1 = upperleft - up*vHeight;
      Vector4 vec2 = upperleft - up*vHeight;
              vec2 = vec2      + right*vWidth;
      Vector4 vec3 = upperleft + right*vWidth;

      float srcleft   = float(Desc.x)/float(CharSet.Width);
      float srctop    = float(Desc.y)/float(CharSet.Height);
      float srcright  = float(Desc.x + Desc.Width) /float(CharSet.Width);
      float srcbottom = float(Desc.y + Desc.Height)/float(CharSet.Height);

      float u0 = srcleft;
      float v0 = srctop;
      float u1 = srcleft;
      float v1 = srcbottom;
      float u2 = srcright;
      float v2 = srcbottom;
      float u3 = srcright;
      float v3 = srctop;

      PosUv3d pos0( vec0.x, vec0.y, vec0.z, 1.f, u0, v0 );
      PosUv3d pos1( vec1.x, vec1.y, vec1.z, 1.f, u1, v1 );
      PosUv3d pos2( vec2.x, vec2.y, vec2.z, 1.f, u2, v2 );
      PosUv3d pos3( vec3.x, vec3.y, vec3.z, 1.f, u3, v3 );

      pCanvas3d->bTriangleUV( threadId, pos1, pos0, pos2 );
      pCanvas3d->bTriangleUV( threadId, pos0, pos2, pos3 );

      if( kerningOnOff )
      {
         int kerning = 0;
         if(c<count-2)
         {
            ushort next = text[c+1];

            FontKerningPair Pair;
            Pair.first   = ch;
            Pair.second  = next;
            Pair.ammount = 0;
            int idx = aKerningPair.find(Pair);
            if( idx >= 0 )
            {
               Pair = aKerningPair.index(idx);
               kerning = Pair.ammount;
            }
         }
         Vector4 vFactor(((float)(Desc.XAdvance)/float(CharSet.Width))*size*scaleX);
         Vector4 vKerning(kerning*size*scaleX);
         pos = (pos + right*vFactor) + vKerning;
      }
      else
      {
         Vector4 vFactor = Vector4( ((float)(Desc.XAdvance)/float(CharSet.Width))*size*scaleX );
         pos = pos + right*vFactor;
      }
   }

   pCanvas3d->endBatch(threadId, final, color, pTexture, BLEND_MODE_ALPHA, CANVAS3D_SHADER_TEXTURE );
}

void FontManager::writeText(uint threadId, int x,int y,String text,int color)
{
   float size = sizeThread[threadId];

   if( !text.getLength() ) return;

   pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_RECTANGLEUV,text.getLength());



   for(int c=0;c<text.getLength();c++)
   {
      ushort                  ch = text[c];
      CharacterDescriptor Desc = Chars[ch];

      Vector4 min = Vector4( float(x + Desc.XOffset*size),
                               float(y + Desc.YOffset*size),
                               float(Desc.x)/float(CharSet.Width),
                               float(Desc.y)/float(CharSet.Height) );

      Vector4 max = Vector4( float(x + Desc.XOffset*size + Desc.Width*size),
                               float(y + Desc.YOffset*size + Desc.Height*size),
                               float(Desc.x + Desc.Width)/float(CharSet.Width),
                               float(Desc.y + Desc.Height)/float(CharSet.Height) );

      pCanvas2d->bRectangleUV(threadId, min, max );

      if( kerningOnOff )
      {
         int kerning = 0;
         if(c<text.getLength()-2)
         {
            ushort next = text[c+1];

            FontKerningPair Pair;
            Pair.first   = ch;
            Pair.second  = next;
            Pair.ammount = 0;
            int idx = aKerningPair.find(Pair);
            if( idx >= 0 )
            {
               Pair = aKerningPair.index(idx);
               kerning = Pair.ammount;
            }
         }
         x = x + (Desc.XAdvance)*size + kerning*size;
      }
      else
      {
         x = x + (Desc.XAdvance)*size;
      }
   }
   pCanvas2d->endBatch(threadId, color, pTexture, BLEND_MODE_ALPHA);
}

int FontManager::start()
{
   FORMAT_BUFFER();
   FORMAT_PATH("data/font/test_0.dds");
   pTexture = grCreateTexture( formatBuffer );

   SDL_RWops* ctx = 0;
   FORMAT_PATH("data/font/test.fnt");
   ctx = SDL_RWFromFile( formatBuffer, "rb" );

   char     buffer[4096];
   char     magic[4];
   BlockID  blockID1;
   BlockID  blockID2;
   BlockID  blockID3;
   BlockID  blockID4;
   BlockID  blockID5;
   SDL_RWread(ctx,(char*)magic,1,4);
   SDL_RWread(ctx,&blockID1,1,5);
   SDL_RWread(ctx,buffer,1,blockID1.size);
   SDL_RWread(ctx,&blockID2,1,sizeof(BlockID));

   ////////////////////////////////////////////////////////////////////////////////
   // character set
   ////////////////////////////////////////////////////////////////////////////////
   SDL_RWread(ctx,&CharSet,1,sizeof(CharSet));
   SDL_RWread(ctx,&blockID3,1,sizeof(BlockID));
   SDL_RWread(ctx,buffer,1,blockID3.size);
   SDL_RWread(ctx,&blockID4,1,sizeof(BlockID));

   ////////////////////////////////////////////////////////////////////////////////
   // characters
   ////////////////////////////////////////////////////////////////////////////////
   CharacterDescriptor     CharsRead[256];
   SDL_RWread( ctx, CharsRead, 1, blockID4.size );
   int numchars = blockID4.size/sizeof(CharacterDescriptor);
   for(int a=0;a<numchars;a++)
   {
      int id = CharsRead[a].id;
      Chars[ id ] = CharsRead[a];
   }

   ////////////////////////////////////////////////////////////////////////////////
   // kerning
   ////////////////////////////////////////////////////////////////////////////////
   SDL_RWread(ctx,&blockID5,1,sizeof(BlockID));
   int kernings = blockID5.size/sizeof(FontKerningPair);
   for(int a=0;a<kernings;a++)
   {
      FontKerningPair Pair;
      SDL_RWread( ctx, &Pair, 1, sizeof(FontKerningPair) );
      aKerningPair.pushBack(Pair);
   }
   SDL_RWclose(ctx);
   return 0;
}
int FontManager::stop()
{
   return 0;
}
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
