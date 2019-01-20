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
//
// Format
//
////////////////////////////////////////////////////////////////////////////////
int formatPath(char* buffer,int size,const char* path)
{
   #ifdef WINDOWS
      memset(buffer,0,size);
      strcpy(buffer,path);
      int len = strlen(buffer);

      for(int i=0;i<len;i++)
      {
         if( buffer[i] == '/' ) buffer[i] = '\\';
      }
   #else
        #ifdef PLATFORM_MAC
            char tmp[1024] = {0};
            formatString(tmp,1024,"%s%s","./Oscilloscope.app/Contents/Resources/",path);
            strcpy(buffer,tmp);
        #else
            strcpy(buffer,path);
        #endif
   #endif
   return 0;
}

const char* fixPath(const char* path)
{
    static char tmp[1024] = {0};
    SDL_zero(tmp);
    formatPath(tmp,1024,path);
    return tmp;
}


int formatString(char* buffer,int size,const char* str,...)
{
   va_list argptr;
   va_start(argptr, str);
   vsprintf( buffer, str, argptr );
   va_end(argptr);
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
