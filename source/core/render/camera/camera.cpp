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
// Globals
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER( Camera );

////////////////////////////////////////////////////////////////////////////////
//
// Camera
//
////////////////////////////////////////////////////////////////////////////////
Camera::Camera()
{
   View        = matIdentity();
   Perspective = matIdentity();
   Final       = matIdentity();
}
void Camera::activate()
{
   pCamera->pActiveCamera = this;
}

void Camera::setPerspective(float ffov,float faspect,float znear,float zfar)
{
   nearz  = znear;
   farz   = zfar;
   fov    = ffov;
   aspect = faspect;

   float sinFov = sinf(0.5f*fov);
   float cosFov = cosf(0.5f*fov);

   height = cosFov / sinFov;
   width  = height / aspect;

   float q = zfar / (zfar - znear);

   Perspective.RowX() = Vector4(width, 0.0f, 0.0f, 0.0f);
   Perspective.RowY() = Vector4(0.0f, height, 0.0f, 0.0f);
   Perspective.RowZ() = Vector4(0.0f, 0.0f, q, 1.0f);
   Perspective.Pos()  = Vector4(0.0f, 0.0f, -q * znear, 0.0f);
}

void Camera::setOrthographic(float fwidth,float fheight,float znear,float zfar)
{
   nearz  = znear;
   farz   = zfar;
   width  = fwidth;
   height = fheight;
   aspect = width/height;
   fov    = 90.f;

   float   fRange = 1.0f / (zfar-znear);
   Perspective.RowX() = Vector4(2.0f / width, 0.0f, 0.0f, 0.0f);
   Perspective.RowY() = Vector4(0.0f, 2.0f / height, 0.0f, 0.0f);
   Perspective.RowZ() = Vector4(0.0f, 0.0f, fRange, 0.0f);
   Perspective.Pos()  = Vector4(0.0f, 0.0f, -fRange * znear, 1.0f);
}

void Camera::lookAt(Vector4 &pos,Vector4 &lookat,Vector4 &up)
{
   Vector4 EyeDirection = lookat-pos;
   lookTo(pos, EyeDirection, up);
}

void Camera::lookTo(Vector4 &pos,Vector4 &dir,Vector4 &up)
{
   Vector4 look  = vecNormalize3d(dir);
   Vector4 right = vecNormalize3d(vecCross3d(up, look));
           up    = vecNormalize3d(vecCross3d(look, right));

   View.RowX() = Vector4(right.x, right.y, right.z, 0);
   View.RowY() = Vector4(   up.x,    up.y,    up.z, 0);
   View.RowZ() = Vector4( look.x,  look.y,  look.z, 0);
   View.Pos()  = Vector4(  pos.x,   pos.y,   pos.z, 1);
}

void Camera::update(float dt)
{
   Matrix4x4 ViewInv = matInverse( View );
   Final = matMultiply( ViewInv, Perspective );
}

////////////////////////////////////////////////////////////////////////////////
//
// CameraManager
//
////////////////////////////////////////////////////////////////////////////////
void CameraManager::registerCamera(Camera *cam)
{
   sCamera.insert(cam);
}

void CameraManager::nextCamera()
{
   int idx = sCamera.find(pActiveCamera);
   if( idx != -1 )
   {
      idx++;
      idx = clamp( idx, 0, sCamera.getCount()-1 );

      pActiveCamera = sCamera.index(idx);
   }
}

void CameraManager::previusCamera()
{
   int idx = sCamera.find(pActiveCamera);
   if( idx != -1 )
   {
      idx--;
      idx = clamp( idx, 0, sCamera.getCount()-1 );

      pActiveCamera = sCamera.index(idx);
   }
}

Camera *CameraManager::getActiveCamera()
{
   return pActiveCamera;
}

int CameraManager::update(float dt)
{
   for(int c=0;c<sCamera.getCount();c++)
   {
      Camera *cam = sCamera.index(c);
      cam->update(dt);
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
