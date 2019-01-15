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




layout (std140) uniform VSCBuffer0 {
	vec4 gVSAdjust;
	vec4 gVSColor;
};

#ifdef TEXTURE
	in  vec2 inPos;
	in  vec2 inUV;
	out vec4 Tex;
#else
	in  vec2 inPos;
#endif

void main()
{
	gl_Position = vec4( inPos.x*gVSAdjust.x + gVSAdjust.z, inPos.y*gVSAdjust.y + gVSAdjust.w, 0.5f, 1.f );
	#ifdef TEXTURE
		Tex.x  = inUV.x;
		Tex.y  = inUV.y;
		Tex.z  = 0.0;
		Tex.w = 0.0;
	#endif
}
