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

SDL_SpinLock memoryLock = 0;

ularge memoryMaximum(ularge min, ularge max)
{
    SDL_AtomicLock(&memoryLock);

    ularge allocated = max;

    void *p = 0;
    while ( !p && allocated >= min )
    {
        p = SDL_malloc(allocated);
        if (p)
        {
            SDL_free(p);
            SDL_AtomicUnlock(&memoryLock);
            return allocated;
        }

        allocated = allocated >> 1;
    }
    SDL_AtomicUnlock(&memoryLock);
    return allocated;
}

void* memoryAllocate(ularge size,int align)
{
   SDL_AtomicLock(&memoryLock);
   void* ptr = SDL_malloc(size);
   if (!ptr)
      CORE_ABORT("memory allocation failed", 0);
   SDL_AtomicUnlock(&memoryLock);
   return ptr;
}

void* memoryReallocate(void* ptr,ularge newsize)
{
   SDL_AtomicLock(&memoryLock);
   void* newptr = SDL_realloc(ptr,newsize);
   if( !newptr)
      CORE_ABORT("memory reallocation failed",0);
   SDL_AtomicUnlock(&memoryLock);
   return newptr;
}

void memoryFree(void* ptr)
{
    SDL_AtomicLock(&memoryLock);
    SDL_free(ptr);
    SDL_AtomicUnlock(&memoryLock);
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
