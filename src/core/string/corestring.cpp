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
// globals
//
////////////////////////////////////////////////////////////////////////////////
const String       String::null("");
const String       String::space(" ");

////////////////////////////////////////////////////////////////////////////////
// floatToString
////////////////////////////////////////////////////////////////////////////////
const char*  doubleToString(double value)
{
   static char buffer[1024] = { 0 };
   memset(buffer, 0, sizeof(buffer));
   sprintf(buffer, "%f", value);
   return buffer;
}

double stringToDouble(const char* str)
{
   double d = strtod(str,0);
   return d;
}

const char* floatToString(float value)
{
    static char buffer[1024] = {0};
    memset( buffer, 0, sizeof(buffer) );
    sprintf( buffer, "%f", value );
    return buffer;
}

float stringToFloat(const char* str)
{
    float f = atof( str );
    return f;
}

const char* integerToString(int value)
{
    static char buffer[1024] = {0};
    memset( buffer, 0, sizeof(buffer) );
    sprintf( buffer, "%d", value );
    return buffer;
}

int stringToInteger(const char* str)
{
   int i = atoi( str  );
   return i;
}

////////////////////////////////////////////////////////////////////////////////
//
// String
//
////////////////////////////////////////////////////////////////////////////////
String::String()
{
   memset(data,0,MAX_STRING);
   length = 0;
}

String::String(const char* str)
{
   memset(data,0,MAX_STRING);
   length = min( (int)strlen(str), MAX_STRING-1 );
   memcpy( data, str, length );
}

String::String(const char c)
{
   memset(data,0,MAX_STRING);
   length = 1;
   data[0] = c;
}

String::~String()
{
}

////////////////////////////////////////////////////////////////////////////////
//
// public
//
////////////////////////////////////////////////////////////////////////////////
void String::setLength(int newlength)
{
   length = min( newlength, MAX_STRING-1 );
   data[length] = 0;
}

int String::getLength() const
{
   return length;
}

char &String::index(int idx)
{
   if( idx < 0 || idx >= length ) CORE_ABORT("idx out of range",0);

   return data[idx];
}
const char &String::index(int idx) const
{
   if( idx < 0 || idx >= length ) CORE_ABORT("idx out of range",0);

   return data[idx];
}
int String::pos(const String &substr,int start) const
{
   if( substr.getLength() == 0 ) return -1;
   if( start + substr.getLength() > length  ) return -1;

   for(int a=start;a<length-substr.getLength()+1;a++)
   {
      if( memcmp( &data[a], (void*)substr.data, substr.getLength() )==0 ) return a;
   }
   return -1;
}
int String::posReverse(const String &substr,int end) const
{
   if( substr.getLength() == 0 ) return -1;
   if( end + substr.getLength() > length  ) return -1;

   for(int a=length-1;a>=end;a--)
   {
      if( memcmp( &data[a], (void*)substr.data, substr.getLength() )==0 ) return a;
   }
   return -1;
}
void String::insert(int pos,const String &str)
{
   if( pos < 0 || pos + str.getLength() > MAX_STRING - 1 ) CORE_ABORT("pos out of range",0);

   char  tmp[MAX_STRING] = {0};
   memcpy( &tmp[pos], &data[pos], (length - pos) );
   memcpy( &data[pos+str.getLength()], &tmp[pos], (length - pos) );
   memcpy( &data[pos], (void*)str.data, str.getLength() );

   length += str.getLength();
   data[length] = 0;
}

void String::remove(int pos,int cnt)
{
   if( pos < 0 || pos > length ) CORE_ABORT("pos out of range",0);
   if( pos + cnt > length ) CORE_ABORT("pos + cnt out of range",0);

   memcpy( &data[pos], &data[pos+cnt], (length - (pos + cnt - 1)) );
   length -= cnt;
}

void String::toUpperCase()
{
   const int Conversion = 'a' - 'A';
   for(int a=0;a<length;a++)
   {
      if( data[a] >= 'a' && data[a] <= 'z' )
         data[a] -= Conversion;
   }
}
void String::toLowerCase()
{
   const int Conversion = 'a' - 'A';
   for(int a=0;a<length;a++)
   {
      if( data[a] >= 'A' && data[a] <= 'Z' )
         data[a] += Conversion;
   }
}

char* String::asChar() const
{
   return (char*)data;
}

float String::asFloat() const
{
   return (float)strtod( data, 0 );
}

int String::asInteger() const
{
   return strtol( data, 0, 10 );
}

void String::copyTo(String &out,int start,int cnt) const
{
   out.setLength(cnt);

   memcpy( (void*)out.data, &data[start], cnt );
   out[cnt] = 0;
}

void String::replace(const String thisstr,const String withthis,int start)
{
   if( start<0 || start>length ) CORE_ABORT( "start out of range",0);

   while( start < length )
   {
      int position = pos( thisstr , start );
      if( position < 0 ) break;

      start = position + withthis.getLength();

      remove( position , thisstr.getLength() );
      insert( position , withthis );
   }
}
void String::tokenize(const String delimitors,Array<String,16> &aToken) const
{
   int length = getLength();
   int pos   = 0;
   int   a   = 0;
   for(;a<length;a++)
   {
      int delpos = delimitors.pos( String( data[a] ) );
      if( delpos >= 0 )
      {
         if( a-pos )
         {
            String out;
            copyTo( out, pos, a-pos );
            aToken.pushBack( out );
         }
         pos=a+1;
      }
   }
   if( a-pos )
   {
      String out;
      copyTo( out, pos , a-pos );
      aToken.pushBack( out );
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// operators
//
////////////////////////////////////////////////////////////////////////////////
String::operator int() const
{
   return length;
}

const char& String::operator[](int idx) const
{
   return index(idx);
}

char& String::operator[](int idx)
{
   return index(idx);
}
String& String::operator = (const String &str)
{
   setLength( str.getLength() );
   memcpy( data, str.asChar(), min( length, MAX_STRING - 1 ) );
   data[length] = 0;
   return *this;
}
int String::operator == (const String &str) const
{
   return memcmp( data, str.asChar(), length ) == 0;
}
int String::operator != (const String &str) const
{
   return memcmp( data, str.asChar(), length ) != 0;
}

int String::operator < (const String &str) const
{
   return memcmp( data, str.asChar(), length ) < 0;
}

int String::operator > (const String &str) const
{
   return memcmp(data, str.asChar(), length) > 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
