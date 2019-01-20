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
#ifndef __CORE__MATRIX4X4__
#define __CORE__MATRIX4X4__

////////////////////////////////////////////////////////////////////////////////
//
// Matrix4x4
//
////////////////////////////////////////////////////////////////////////////////
class Matrix4x4 {
public:
    float m11, m12, m13, m14;
    float m21, m22, m23, m24;
    float m31, m32, m33, m34;
    float m41, m42, m43, m44;
public:
   Matrix4x4()
   {
      m11 = 1.f;m12 = 0.f;m13 = 0.f;m14 = 0.f;
      m21 = 0.f;m22 = 1.f;m23 = 0.f;m24 = 0.f;
      m31 = 0.f;m32 = 0.f;m33 = 1.f;m34 = 0.f;
      m41 = 0.f;m42 = 0.f;m43 = 0.f;m44 = 1.f;
   }
public:
    Vector4& RowX() { return (Vector4&)m11; }
    Vector4& RowY() { return (Vector4&)m21; }
    Vector4& RowZ() { return (Vector4&)m31; }
    Vector4& Pos()  { return (Vector4&)m41; }
public:
    const Vector4& RowX() const { return (Vector4&)m11; }
    const Vector4& RowY() const { return (Vector4&)m21; }
    const Vector4& RowZ() const { return (Vector4&)m31; }
    const Vector4& Pos()  const { return (Vector4&)m41; }
};


////////////////////////////////////////////////////////////////////////////////
// identity
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Matrix4x4 matIdentity()
{
   Matrix4x4   ret;
   ret.m11 = 1.f;
   ret.m12 = 0.f;
   ret.m13 = 0.f;
   ret.m14 = 0.f;
   ret.m21 = 0.f;
   ret.m22 = 1.f;
   ret.m23 = 0.f;
   ret.m24 = 0.f;
   ret.m31 = 0.f;
   ret.m32 = 0.f;
   ret.m33 = 1.f;
   ret.m34 = 0.f;
   ret.m41 = 0.f;
   ret.m42 = 0.f;
   ret.m43 = 0.f;
   ret.m44 = 1.f;
   return ret;
}

////////////////////////////////////////////////////////////////////////////////
// scale
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Matrix4x4 matScale(const Vector4 &scale)
{
   Matrix4x4 ret;
   ret.m11 = scale.x;
   ret.m12 = 0.f;
   ret.m13 = 0.f;
   ret.m14 = 0.f;

   ret.m21 = 0.f;
   ret.m22 = scale.y;
   ret.m23 = 0.f;
   ret.m24 = 0.f;

   ret.m31 = 0.f;
   ret.m32 = 0.f;
   ret.m33 = scale.z;
   ret.m34 = 0.f;

   ret.m41 = 0.f;
   ret.m42 = 0.f;
   ret.m43 = 0.f;
   ret.m44 = scale.w;
   return ret;
}

////////////////////////////////////////////////////////////////////////////////
// rotation
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Matrix4x4 matRotationX(float angle)
{
   Matrix4x4 ret;

   float sin = sinf(angle);
   float cos = cosf(angle);

   ret.m11 = 1.f;
   ret.m12 = 0.f;
   ret.m13 = 0.f;
   ret.m14 = 0.f;

   ret.m21 = 0.f;
   ret.m22 = cos;
   ret.m23 = sin;
   ret.m24 = 0.f;

   ret.m31 = 0.f;
   ret.m32 = -sin;
   ret.m33 =  cos;
   ret.m34 = 0.f;

   ret.m41 = 0.f;
   ret.m42 = 0.f;
   ret.m43 = 0.f;
   ret.m44 = 1.f;
   return ret;
}

FORCE_INLINE Matrix4x4 matRotationY(float angle)
{
   Matrix4x4 ret;

   float sin = sinf(angle);
   float cos = cosf(angle);

   ret.m11 = cos;
   ret.m12 = 0.f;
   ret.m13 = -sin;
   ret.m14 = 0.f;

   ret.m21 = 0.f;
   ret.m22 = 1.f;
   ret.m23 = 0.f;
   ret.m24 = 0.f;

   ret.m31 = sin;
   ret.m32 = 0.f;
   ret.m33 = cos;
   ret.m34 = 0.0f;

   ret.m41 = 0.f;
   ret.m42 = 0.f;
   ret.m43 = 0.f;
   ret.m44 = 1.f;
   return ret;
}

FORCE_INLINE Matrix4x4 matRotationZ(float angle)
{
   Matrix4x4 ret;

   float sin = sinf(angle);
   float cos = cosf(angle);

   ret.m11 = cos;
   ret.m12 = sin;
   ret.m13 = 0.f;
   ret.m14 = 0.f;

   ret.m21 = -sin;
   ret.m22 = cos;
   ret.m23 = 0.f;
   ret.m24 = 0.f;

   ret.m31 = 0.f;
   ret.m32 = 0.f;
   ret.m33 = 1.f;
   ret.m34 = 0.0f;

   ret.m41 = 0.f;
   ret.m42 = 0.f;
   ret.m43 = 0.f;
   ret.m44 = 1.f;
   return ret;
}

FORCE_INLINE Matrix4x4 matRotationZYX(float z,float y,float x)
{
   Matrix4x4 ret;

   float sinz = sinf(z);
   float cosz = cosf(z);
   float siny = sinf(y);
   float cosy = cosf(y);
   float sinx = sinf(x);
   float cosx = cosf(x);

   ret.m11 = cosy*cosz;
   ret.m12 = cosx*sinz+cosz*sinx*siny;
   ret.m13 = sinx*sinz-cosx*cosz*siny;
   ret.m14 = 0.f;

   ret.m21 = -cosy*sinz;
   ret.m22 = cosx*cosz-sinx*siny*sinz;
   ret.m23 = cosx*siny*sinz+cosz*sinx;
   ret.m24 = 0.f;

   ret.m31 = siny;
   ret.m32 = -cosy*sinx;
   ret.m33 = cosx*cosy;
   ret.m34 = 0.0f;

   ret.m41 = 0.f;
   ret.m42 = 0.f;
   ret.m43 = 0.f;
   ret.m44 = 1.f;
   return ret;
}

FORCE_INLINE Matrix4x4 matRotationAxisAngle(const Vector4 &axis,float angle)
{
   Matrix4x4 ret;

   Vector4 u  = vecNormalize3d( axis );

   float sin = sinf(angle);
   float cos = cosf(angle);

   ret.m11 = cos + u.x*u.x*(1.f-cos);
   ret.m12 = u.x*u.y*(1.f-cos) - u.z*sin;
   ret.m13 = u.x*u.z*(1.f-cos) + u.y*sin;
   ret.m14 = 0.f;

   ret.m21 = u.y*u.x*(1.f-cos) + u.z*sin;
   ret.m22 = cos + u.y*u.y*(1.f-cos);
   ret.m23 = u.y*u.z*(1.f-cos) - u.x*sin;
   ret.m24 = 0.f;

   ret.m31 = u.z*u.x*(1.f-cos) - u.y*sin;
   ret.m32 = u.z*u.y*(1.f-cos) + u.x*sin;
   ret.m33 = cos + u.z*u.z*(1.f-cos);
   ret.m34 = 0.f;

   ret.m41 = 0.f;
   ret.m42 = 0.f;
   ret.m43 = 0.f;
   ret.m44 = 1.f;
   return ret;
}

////////////////////////////////////////////////////////////////////////////////
// multiply
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Matrix4x4 matMultiply(const Matrix4x4 &a, const Matrix4x4 &b)
{
   Matrix4x4 ret;
   ret.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31 + a.m14*b.m41;
   ret.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32 + a.m14*b.m42;
   ret.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33 + a.m14*b.m43;
   ret.m14 = a.m11*b.m14 + a.m12*b.m24 + a.m13*b.m34 + a.m14*b.m44;

   ret.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31 + a.m24*b.m41;
   ret.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32 + a.m24*b.m42;
   ret.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33 + a.m24*b.m43;
   ret.m24 = a.m21*b.m14 + a.m22*b.m24 + a.m23*b.m34 + a.m24*b.m44;

   ret.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31 + a.m34*b.m41;
   ret.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32 + a.m34*b.m42;
   ret.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33 + a.m34*b.m43;
   ret.m34 = a.m31*b.m14 + a.m32*b.m24 + a.m33*b.m34 + a.m34*b.m44;

   ret.m41 = a.m41*b.m11 + a.m42*b.m21 + a.m43*b.m31 + a.m44*b.m41;
   ret.m42 = a.m41*b.m12 + a.m42*b.m22 + a.m43*b.m32 + a.m44*b.m42;
   ret.m43 = a.m41*b.m13 + a.m42*b.m23 + a.m43*b.m33 + a.m44*b.m43;
   ret.m44 = a.m41*b.m14 + a.m42*b.m24 + a.m43*b.m34 + a.m44*b.m44;
   return ret;
}

FORCE_INLINE Matrix4x4 matMultiply(const Matrix4x4 &a, const float value)
{
   Matrix4x4 ret = a;
   ret.m11 *= value;
   ret.m12 *= value;
   ret.m13 *= value;
   ret.m14 *= value;

   ret.m21 *= value;
   ret.m22 *= value;
   ret.m23 *= value;
   ret.m24 *= value;

   ret.m31 *= value;
   ret.m32 *= value;
   ret.m33 *= value;
   ret.m34 *= value;

   ret.m41 *= value;
   ret.m42 *= value;
   ret.m43 *= value;
   ret.m44 *= value;
   return ret;
}

////////////////////////////////////////////////////////////////////////////////
// add
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Matrix4x4 matAdd(const Matrix4x4 &a, const Matrix4x4 &b)
{
   Matrix4x4 ret;
   ret.m11 = a.m11 + b.m11;
   ret.m12 = a.m12 + b.m12;
   ret.m13 = a.m13 + b.m13;
   ret.m14 = a.m14 + b.m14;

   ret.m21 = a.m21 + b.m21;
   ret.m22 = a.m22 + b.m22;
   ret.m23 = a.m23 + b.m23;
   ret.m24 = a.m24 + b.m24;

   ret.m31 = a.m31 + b.m31;
   ret.m32 = a.m32 + b.m32;
   ret.m33 = a.m33 + b.m33;
   ret.m34 = a.m34 + b.m34;

   ret.m41 = a.m41 + b.m41;
   ret.m42 = a.m42 + b.m42;
   ret.m43 = a.m43 + b.m43;
   ret.m44 = a.m44 + b.m44;
   return ret;
}

FORCE_INLINE Matrix4x4 matAdd(const Matrix4x4 &a, const float value)
{
   Matrix4x4 ret = a;
   ret.m11 += value;
   ret.m12 += value;
   ret.m13 += value;
   ret.m14 += value;

   ret.m21 += value;
   ret.m22 += value;
   ret.m23 += value;
   ret.m24 += value;

   ret.m31 += value;
   ret.m32 += value;
   ret.m33 += value;
   ret.m34 += value;

   ret.m41 += value;
   ret.m42 += value;
   ret.m43 += value;
   ret.m44 += value;
   return ret;
}


////////////////////////////////////////////////////////////////////////////////
// transform
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Vector4 matTransform3x3(const Vector4 &v, const Matrix4x4 &m)
{
   Vector4 ret;
   ret.x = v.x*m.m11 + v.y*m.m21 + v.z*m.m31;
   ret.y = v.x*m.m12 + v.y*m.m22 + v.z*m.m32;
   ret.z = v.x*m.m13 + v.y*m.m23 + v.z*m.m33;
   ret.w = v.w;
   return ret;
}

FORCE_INLINE Vector4 matTransform4x4(const Vector4 &v, const Matrix4x4 &m)
{
   Vector4 ret;
   ret.x = v.x*m.m11 + v.y*m.m21 + v.z*m.m31 + v.w*m.m41;
   ret.y = v.x*m.m12 + v.y*m.m22 + v.z*m.m32 + v.w*m.m42;
   ret.z = v.x*m.m13 + v.y*m.m23 + v.z*m.m33 + v.w*m.m43;
   ret.w = v.x*m.m14 + v.y*m.m24 + v.z*m.m34 + v.w*m.m44;
   return ret;
}

////////////////////////////////////////////////////////////////////////////////
// transpose
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Matrix4x4 matTranspose(const Matrix4x4 &a)
{
   Matrix4x4 ret;
   ret.m11 = a.m11;
   ret.m12 = a.m21;
   ret.m13 = a.m31;
   ret.m14 = a.m41;
   ret.m21 = a.m12;
   ret.m22 = a.m22;
   ret.m23 = a.m32;
   ret.m24 = a.m42;
   ret.m31 = a.m13;
   ret.m32 = a.m23;
   ret.m33 = a.m33;
   ret.m34 = a.m43;
   ret.m41 = a.m14;
   ret.m42 = a.m24;
   ret.m43 = a.m34;
   ret.m44 = a.m44;
   return ret;
}

////////////////////////////////////////////////////////////////////////////////
// determinante
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE float matDeterminante(const Matrix4x4 &m)
{
   float c11 = +( m.m22*m.m33*m.m44 + m.m23*m.m34*m.m42 + m.m24*m.m32*m.m43 - m.m42*m.m33*m.m24 - m.m43*m.m34*m.m22 - m.m44*m.m32*m.m23 );
   float c12 = -( m.m21*m.m33*m.m44 + m.m23*m.m34*m.m41 + m.m24*m.m31*m.m43 - m.m41*m.m33*m.m24 - m.m43*m.m34*m.m21 - m.m44*m.m31*m.m23 );
   float c13 = +( m.m21*m.m32*m.m44 + m.m22*m.m34*m.m41 + m.m24*m.m31*m.m42 - m.m41*m.m32*m.m24 - m.m42*m.m34*m.m21 - m.m44*m.m31*m.m22 );
   float c14 = -( m.m21*m.m32*m.m43 + m.m22*m.m33*m.m41 + m.m23*m.m31*m.m42 - m.m41*m.m32*m.m23 - m.m42*m.m33*m.m21 - m.m43*m.m31*m.m22 );

   return  m.m11*c11 + m.m12*c12 + m.m13*c13 + m.m14*c14;
}

////////////////////////////////////////////////////////////////////////////////
// inverse
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Matrix4x4 matInverse(const Matrix4x4 &m)
{
   Matrix4x4 adj;

   float det = matDeterminante(m);
   if( det == 0.f )
   {
      return matIdentity();
   }

   adj.m11 =  m.m22*(m.m33*m.m44-m.m34*m.m43)-m.m23*(m.m32*m.m44-m.m34*m.m42)+m.m24*(m.m32*m.m43-m.m33*m.m42);
   adj.m12 = -m.m12*(m.m33*m.m44-m.m34*m.m43)+m.m13*(m.m32*m.m44-m.m34*m.m42)-m.m14*(m.m32*m.m43-m.m33*m.m42);
   adj.m13 =  m.m12*(m.m23*m.m44-m.m24*m.m43)-m.m13*(m.m22*m.m44-m.m24*m.m42)+m.m14*(m.m22*m.m43-m.m23*m.m42);
   adj.m14 = -m.m12*(m.m23*m.m34-m.m24*m.m33)+m.m13*(m.m22*m.m34-m.m24*m.m32)-m.m14*(m.m22*m.m33-m.m23*m.m32);

   adj.m21 = -m.m21*(m.m33*m.m44-m.m34*m.m43)+m.m23*(m.m31*m.m44-m.m34*m.m41)-m.m24*(m.m31*m.m43-m.m33*m.m41);
   adj.m22 =  m.m11*(m.m33*m.m44-m.m34*m.m43)-m.m13*(m.m31*m.m44-m.m34*m.m41)+m.m14*(m.m31*m.m43-m.m33*m.m41);
   adj.m23 = -m.m11*(m.m23*m.m44-m.m24*m.m43)+m.m13*(m.m21*m.m44-m.m24*m.m41)-m.m14*(m.m21*m.m43-m.m23*m.m41);
   adj.m24 =  m.m11*(m.m23*m.m34-m.m24*m.m33)-m.m13*(m.m21*m.m34-m.m24*m.m31)+m.m14*(m.m21*m.m33-m.m23*m.m31);

   adj.m31 =  m.m21*(m.m32*m.m44-m.m34*m.m42)-m.m22*(m.m31*m.m44-m.m34*m.m41)+m.m24*(m.m31*m.m42-m.m32*m.m41);
   adj.m32 = -m.m11*(m.m32*m.m44-m.m34*m.m42)+m.m12*(m.m31*m.m44-m.m34*m.m41)-m.m14*(m.m31*m.m42-m.m32*m.m41);
   adj.m33 =  m.m11*(m.m22*m.m44-m.m24*m.m42)-m.m12*(m.m21*m.m44-m.m24*m.m41)+m.m14*(m.m21*m.m42-m.m22*m.m41);
   adj.m34 = -m.m11*(m.m22*m.m34-m.m24*m.m32)+m.m12*(m.m21*m.m34-m.m24*m.m31)-m.m14*(m.m21*m.m32-m.m22*m.m31);

   adj.m41 = -m.m21*(m.m32*m.m43-m.m33*m.m42)+m.m22*(m.m31*m.m43-m.m33*m.m41)-m.m23*(m.m31*m.m42-m.m32*m.m41);
   adj.m42 =  m.m11*(m.m32*m.m43-m.m33*m.m42)-m.m12*(m.m31*m.m43-m.m33*m.m41)+m.m13*(m.m31*m.m42-m.m32*m.m41);
   adj.m43 = -m.m11*(m.m22*m.m43-m.m23*m.m42)+m.m12*(m.m21*m.m43-m.m23*m.m41)-m.m13*(m.m21*m.m42-m.m22*m.m41);
   adj.m44 =  m.m11*(m.m22*m.m33-m.m23*m.m32)-m.m12*(m.m21*m.m33-m.m23*m.m31)+m.m13*(m.m21*m.m32-m.m22*m.m31);

   return matMultiply( adj, 1.f/det );
}

////////////////////////////////////////////////////////////////////////////////
// projection
////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE Vector4 matUnprojectScreenSpace(int x, int y, int w, int h, Matrix4x4 inverse, float fz)
{
   float fx =  2.f*(float(x) / float(w) - 0.5f);
   float fy =  2.f*(float(y) / float(h) - 0.5f);
   Vector4 point = Vector4(fx, fy, fz, 1.f);
   point = matTransform4x4(point, inverse);
   point.w = 1.f/point.w;
   point.x *= point.w;
   point.y *= point.w;
   point.z *= point.w;
   return point;
}

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
