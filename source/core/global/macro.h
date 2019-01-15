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
#ifndef __MACRO__
#define __MACRO__

////////////////////////////////////////////////////////////////////////////////
// color
////////////////////////////////////////////////////////////////////////////////
#define COLOR_ARGB(a,r,g,b) ((unsigned int)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define COLOR_RGBA(r,g,b,a) ((unsigned int)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))

////////////////////////////////////////////////////////////////////////////////
// bit
////////////////////////////////////////////////////////////////////////////////
#define BIT(index) (1<<index)

////////////////////////////////////////////////////////////////////////////////
// format
////////////////////////////////////////////////////////////////////////////////
#define FORMAT_BUFFER_SIZE 1024
#define FORMAT_BUFFER()   char formatBuffer[1024] = {0}
#define FORMAT(msg,...)   formatString( formatBuffer, 1024, msg, __VA_ARGS__ );
#define FORMAT_PATH(path) formatPath( formatBuffer, 1024, path );

////////////////////////////////////////////////////////////////////////////////
// abort,message,break
////////////////////////////////////////////////////////////////////////////////
#define CORE_MESSAGE(msg,...) { char buffer[1024]={0}; char bufferMsg[1024]={0}; formatString( bufferMsg, 1024, msg, __VA_ARGS__); formatString( buffer, 1024, "message: %s \n file: %s \n function: %s \n line: %d", bufferMsg,__FILE__,__FUNCTION__,__LINE__); coreMessage(buffer); }
#define CORE_ERROR(msg,...)   { char buffer[1024]={0}; char bufferMsg[1024]={0}; formatString( bufferMsg, 1024, msg, __VA_ARGS__); formatString( buffer, 1024, "message: %s \n file: %s \n function: %s \n line: %d", bufferMsg,__FILE__,__FUNCTION__,__LINE__); coreMessage(buffer); coreDebugBreak(); }
#define CORE_ABORT(msg,...)   { char buffer[1024]={0}; char bufferMsg[1024]={0}; formatString( bufferMsg, 1024, msg, __VA_ARGS__); formatString( buffer, 1024, "message: %s \n file: %s \n function: %s \n line: %d", bufferMsg,__FILE__,__FUNCTION__,__LINE__); coreMessage(buffer); coreDebugBreak(); coreTerminate(); }
#define CORE_BREAK_IF(cond)   { cond ? coreDebugBreak() : {} }

////////////////////////////////////////////////////////////////////////////////
// safe release
////////////////////////////////////////////////////////////////////////////////
#define SAFE_RELEASE(p) p ? p->Release() : 0; p = 0

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
