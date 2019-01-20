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
	vec4 gVSRenderTarget;
	vec4 gVSColor;
	mat4 gVSFinalMatrix;
	vec4 gVSDirectionalLight;
};

#ifdef TEXTURE
	in  vec4 inPos;
	in  vec2 inUV;
	out vec2 Tex;
#else
	in  vec4 inPos;
	#ifdef NORMAL
		in  vec4 inNormal;
		out vec4 Normal;
	#endif

	#ifdef COLOR
		out vec4 WorldPosition;
	#endif
#endif

void main()
{
	gl_Position   = gVSFinalMatrix*inPos;

	#ifdef TEXTURE
		Tex = inUV;
	#else
		#ifdef NORMAL
			Normal 		  = inNormal;
		#endif

		#ifdef COLOR
			WorldPosition = inPos;
		#endif
	#endif
}