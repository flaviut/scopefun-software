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





uniform vec4 gVSAdjust;
uniform vec4 gVSColor;

void main()
{
	gl_Position = vec4( gl_Vertex.x*gVSAdjust.x + gVSAdjust.z, gl_Vertex.y*gVSAdjust.y + gVSAdjust.w, 0.5, 1.0 );
	#ifdef TEXTURE
	   gl_TexCoord[0] = gl_MultiTexCoord0;
	#endif
}
