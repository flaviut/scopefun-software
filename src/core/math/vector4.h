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
#ifndef __CORE__VECTOR4__
#define __CORE__VECTOR4__

////////////////////////////////////////////////////////////////////////////////
//
// Vector4
//
////////////////////////////////////////////////////////////////////////////////
class Vector4 {
public:
   float x,y,z,w;
public:
   Vector4()
   {
      x = 0.f;
      y = 0.f;
      z = 0.f;
      w = 0.f;
   }
   Vector4(float fx)
   {
      x = fx;
      y = fx;
      z = fx;
      w = fx;
   }
   Vector4(float fx,float fy)
   {
      x = fx;
      y = fy;
      z = 0.f;
      w = 1.f;
   }
   Vector4(float fx,float fy,float fz)
   {
      x = fx;
      y = fy;
      z = fz;
      w = 1.f;
   }
   Vector4(float fx,float fy,float fz,float fw)
   {
      x = fx;
      y = fy;
      z = fz;
      w = fw;
   }
public:
   Vector4 operator-() const
   {
      Vector4 ret;
      ret.x = -x;
      ret.y = -y;
      ret.z = -z;
      ret.w = -w;
      return ret;
   }
   Vector4 operator+(const Vector4 &b) const
   {
      Vector4 ret;
      ret.x = x + b.x;
      ret.y = y + b.y;
      ret.z = z + b.z;
      ret.w = w + b.w;
      return ret;
   }
   Vector4 operator-(const Vector4 &b) const
   {
      Vector4 ret;
      ret.x = x - b.x;
      ret.y = y - b.y;
      ret.z = z - b.z;
      ret.w = w - b.w;
      return ret;
   }
   Vector4 operator*(const Vector4 &b) const
   {
      Vector4 ret;
      ret.x = x * b.x;
      ret.y = y * b.y;
      ret.z = z * b.z;
      ret.w = w * b.w;
      return ret;
   }
   Vector4 operator/(const Vector4 &b) const
   {
      Vector4 ret;
      ret.x = x / b.x;
      ret.y = y / b.y;
      ret.z = z / b.z;
      ret.w = w / b.w;
      return ret;
   }
public:
   void operator+=(const Vector4 &b)
   {
      x = x + b.x;
      y = y + b.y;
      z = z + b.z;
      w = w + b.w;
   }
   void operator-=(const Vector4 &b)
   {
      x = x - b.x;
      y = y - b.y;
      z = z - b.z;
      w = w - b.w;
   }
   void operator*=(const Vector4 &b)
   {
      x = x * b.x;
      y = y * b.y;
      z = z * b.z;
      w = w * b.w;
   }
   void operator/=(const Vector4 &b)
   {
      x = x / b.x;
      y = y / b.y;
      z = z / b.z;
      w = w / b.w;
   }
public:
   Vector4 operator*(const float b) const
   {
      Vector4 ret;
      ret.x = x * b;
      ret.y = y * b;
      ret.z = z * b;
      ret.w = w * b;
      return ret;
   }
   Vector4 operator/(const float b) const
   {
      Vector4 ret;
      ret.x = x / b;
      ret.y = y / b;
      ret.z = z / b;
      ret.w = w / b;
      return ret;
   }
   Vector4 operator+(const float b) const
   {
      Vector4 ret;
      ret.x = x + b;
      ret.y = y + b;
      ret.z = z + b;
      ret.w = w + b;
      return ret;
   }
   Vector4 operator-(const float b) const
   {
      Vector4 ret;
      ret.x = x - b;
      ret.y = y - b;
      ret.z = z - b;
      ret.w = w - b;
      return ret;
   }
public:
   void operator*=(const float b)
   {
      x = x * b;
      y = y * b;
      z = z * b;
      w = w * b;
   }
   void operator/=(const float b)
   {
      x = x / b;
      y = y / b;
      z = z / b;
      w = w / b;
   }
   void operator+=(const float b)
   {
      x = x + b;
      y = y + b;
      z = z + b;
      w = w + b;
   }
   void operator-=(const float b)
   {
      x = x - b;
      y = y - b;
      z = z - b;
      w = w - b;
   }
};

////////////////////////////////////////////////////////////////////////////////
// color
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Vector4 vecFromColor(uint color)
{
   float A = ((color >> 24) & 0xFF)/255.f;
   float R = ((color >> 16) & 0xFF)/255.f;
   float G = ((color >>  8) & 0xFF)/255.f;
   float B = ((color >>  0) & 0xFF)/255.f;
   return Vector4( R, G, B, A );
}
FORCE_INLINE uint vecToColor(const Vector4 &v)
{
   uint R = uint( v.x*255.f );
   uint G = uint( v.y*255.f );
   uint B = uint( v.z*255.f );
   uint A = uint( v.w*255.f );
   return COLOR_ARGB( A, B, G, R );
}

////////////////////////////////////////////////////////////////////////////////
// geometric 3d
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE float vecDot3d(const Vector4 &a,const Vector4 &b)
{
   return a.x*b.x + a.y*b.y + a.z*b.z;
}
FORCE_INLINE float vecLength3d(const Vector4 &a)
{
   float dot = vecDot3d(a,a);
   return sqrt( dot );
}
FORCE_INLINE Vector4 vecNormalize3d(const Vector4 &a)
{
   float length = vecLength3d(a);

   // prevent divide by zero
   if( length > 0.f )
      length = 1.f / length;

   Vector4 ret;
   ret.x = a.x * length;
   ret.y = a.y * length;
   ret.z = a.z * length;
   ret.w = 1.f;
   return ret;
}
FORCE_INLINE Vector4 vecCross3d(const Vector4 &a,const Vector4 &b)
{
   Vector4 ret;
   ret.x = ( a.y * b.z ) - ( a.z * b.y ),
   ret.y = ( a.z * b.x ) - ( a.x * b.z ),
   ret.z = ( a.x * b.y ) - ( a.y * b.x ),
   ret.w = 1.f;
   return ret;
}

////////////////////////////////////////////////////////////////////////////////
// geometric 4d
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE float vecDot4d(const Vector4 &a, const Vector4 &b)
{
   return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}
FORCE_INLINE float vecLength4d(const Vector4 &a)
{
   float dot = vecDot4d(a,a);
   return sqrt( dot );
}
FORCE_INLINE Vector4 vecNormalize4d(const Vector4 &a)
{
   float length = vecLength4d(a);

   // prevent divide by zero
   if( length > 0.f )
      length = 1.f / length;

   Vector4 ret;
   ret.x = a.x * length;
   ret.y = a.y * length;
   ret.z = a.z * length;
   ret.w = a.w * length;
   return ret;
}
FORCE_INLINE Vector4 vecCross4d(const Vector4 &a,const Vector4 &b,const Vector4 &c)
{
   Vector4 ret;
   ret.x = (((b.z*c.w)-(b.w*c.z))*a.y)-(((b.y*c.w)-(b.w*c.y))*a.z)+(((b.y*c.z)-(b.z*c.y))*a.w);
   ret.y = (((b.w*c.z)-(b.z*c.w))*a.x)-(((b.w*c.x)-(b.x*c.w))*a.z)+(((b.z*c.x)-(b.x*c.z))*a.w);
   ret.z = (((b.y*c.w)-(b.w*c.y))*a.x)-(((b.x*c.w)-(b.w*c.x))*a.y)+(((b.x*c.y)-(b.y*c.x))*a.w);
   ret.w = (((b.z*c.y)-(b.y*c.z))*a.x)-(((b.z*c.x)-(b.x*c.z))*a.y)+(((b.y*c.x)-(b.x*c.y))*a.z);
   return ret;
}

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
