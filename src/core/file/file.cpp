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
// helpers
//
////////////////////////////////////////////////////////////////////////////////

int fileLoadString(const char* fileName, char** buffer, ilarge* bufferSize)
{
	SDL_RWops* ctx = SDL_RWFromFile(fileName, "rb");
   if (!ctx) return 1;
	*bufferSize = SDL_RWsize(ctx);
	*buffer = (char*)memoryAllocate(*bufferSize + 1);
   SDL_RWread(ctx, *buffer, 1, *bufferSize);
   (*buffer)[(*bufferSize)] = 0;
	SDL_RWclose(ctx);
	(*bufferSize) += 1;
   return 0;
}

int fileSaveString(const char* fileName, char* string)
{
   SDL_RWops* ctx = SDL_RWFromFile( fileName, "wb+");
   if (!ctx) return 1;
   ilarge len = strlen(string);
   SDL_RWwrite(ctx, string, 1, len);
   SDL_RWclose(ctx);
   return 0;
}

int fileLoad(const char* fileName,char** buffer,ilarge* bufferSize)
{
	SDL_RWops* ctx = SDL_RWFromFile(fileName, "rb");
   if (!ctx) return 1;
	*bufferSize = SDL_RWsize(ctx);
	*buffer = (char*)memoryAllocate( *bufferSize );
   SDL_RWread(ctx, *buffer, 1, *bufferSize);
	SDL_RWclose(ctx);
   return 0;
}

int fileSave(const char* fileName, char* buffer, ilarge bufferSize)
{
   SDL_RWops* ctx = SDL_RWFromFile(fileName, "wb+");
   if (!ctx) return 1;
   SDL_RWwrite(ctx, buffer, 1, bufferSize);
   SDL_RWclose(ctx);
   return 0;
}

int fileFree(char *memory)
{
	memoryFree(memory);
   return 0;
}
