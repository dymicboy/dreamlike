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
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties

vec4 phong(vec3 l, vec3 n, vec3 h)
{
	vec4 Ira = Ka * Ia;									// ambient reflection
	vec4 Ird = max(dot(l, n) * Id, 0.0);					// diffuse reflection
	vec4 Irs = max(Ks * pow(dot(h, n), shininess) * Is, 0.0);	// specular reflection

	float dln = dot(l, n);
	vec4 i = Id;
	if (dln > 0.7) return i * 1.0;
	else if (dln > 0.3) return i * 0.7;
	else return i * 0.3;

	return Ira + Ird + Irs;
}

void main()
{
	//fragColor = vec4(norm,1);
	// light position in the eye space
	vec4 lpos = view_matrix * light_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l + v);	// the halfway vector

	//vec4 iKd = texture(TEX, tc);	// Kd from image
	//if (mode == 0)			fragColor = phong(l, n, h, iKd);
	//else if (mode == 1)	fragColor = phong(l, n, h, Kd);
	//else if (mode == 2)	fragColor = iKd;
	//else				fragColor = vec4(tc, 0, 1);
	vec4 temp = phong(l, n, h);
	fragColor = vec4(temp.x * norm.x, temp.y * norm.y, temp.z * norm.z, 1);
}