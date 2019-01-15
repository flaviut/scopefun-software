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
#ifndef __CORE_PLATFORM__
#define __CORE_PLATFORM__

////////////////////////////////////////////////////////////////////////////////
// gcc
////////////////////////////////////////////////////////////////////////////////
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC) || defined(PLATFORM_MINGW)

	#define INLINE					__inline__
	#define FORCE_INLINE			__inline__
	#define CLASS_CDECL				__attribute__ ((__cdecl__))
	#define CLASS_ALIGN(size)		__attribute__ ((aligned(size)))

#endif

////////////////////////////////////////////////////////////////////////////////
// visual studio c++
////////////////////////////////////////////////////////////////////////////////
#ifdef PLATFORM_WIN

	#define INLINE					__inline__
	#define FORCE_INLINE			__forceinline
	#define CLASS_CDECL				 __cdecl
	#define CLASS_ALIGN(size)		__declspec(align(size))

	#pragma warning( disable : 4200 )
	#pragma warning( disable : 4244 )
	#pragma warning( disable : 4800 )

#endif

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
