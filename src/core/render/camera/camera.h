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
#ifndef __CORE__CAMERA__
#define __CORE__CAMERA__

////////////////////////////////////////////////////////////////////////////////
//
// Camera
//
////////////////////////////////////////////////////////////////////////////////
class Camera {
public:
   Matrix4x4 View;
   Matrix4x4 Perspective;
   Matrix4x4 Final;
public:
   float nearz;
   float farz;
   float width;
   float height;
   float aspect;
   float fov;
public:
   Camera();
public:
   void activate();
public:
   void setPerspective(float fov,float aspect,float znear,float zfar);
   void setOrthographic(float width,float height,float znear,float zfar);
   void lookAt(Vector4 &pos,Vector4 &lookat,Vector4 &up);
   void lookTo(Vector4 &pos,Vector4 &dir,Vector4 &up);
public:
   void update(float dt);
};

////////////////////////////////////////////////////////////////////////////////
//
// CameraManager
//
////////////////////////////////////////////////////////////////////////////////
class CameraManager : public Manager {
   friend class Camera;
private:
   Set<Camera*,16> sCamera;
   Camera*          pActiveCamera;
public:
   void registerCamera(Camera *cam);
public:
   void nextCamera();
   void previusCamera();
public:
   Camera* getActiveCamera();
public:
   int update(float dt);
};

MANAGER_POINTER( Camera );

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
