#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

uniform mat4	view_matrix;

uniform sampler2D TEX;
uniform bool use_texture;

void main()
{
	fragColor = use_texture ? texture( TEX, tc ) : vec4(norm, 1);
}