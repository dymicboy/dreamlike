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
uniform vec4	light_top_position, top_Ia, top_Id, top_Is;	// top light
uniform vec4	light_left_position, left_Ia, left_Id, left_Is;	// top light
uniform vec4	Ka, Kd, Ks;					// material properties

uniform sampler2D TEX;
uniform bool use_texture;

vec3 CalcDirLight(vec3 position, vec3 Ia, vec3 Id, vec3 Is, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-position);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	// combine results

	vec3 ambient = Ia * Kd.xyz;
	vec3 diffuse = Id * diff * Kd.xyz;
	vec3 specular = Is * spec * Ks.xyz;
	return (ambient + diffuse + specular);
}

vec4 phong(vec3 l, vec3 n, vec3 h, vec4 Kd, vec4 Ia, vec4 Id, vec4 Is)
{
	vec4 Ira = Ka * Ia;											// ambient reflection
	vec4 Ird = max(Kd * dot(l, n) * Id, 0.0);						// diffuse reflection
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
	vec4 lpos = view_matrix * light_top_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l + v);	// the halfway vector
	vec4 iKd = texture(TEX, tc);	// Kd from image

	lpos = view_matrix * light_left_position;

	n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	p = epos.xyz;			// 3D position of this fragment
	l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));	// lpos.a==0 means directional light
	v = normalize(-p);		// eye-epos = vec3(0)-epos
	h = normalize(l + v);	// the halfway vector
	iKd = texture(TEX, tc);	// Kd from image

	//vec4 temp = phong(l, n, h, iKd, top_Ia, top_Id, top_Is) + phong(l, n, h, iKd, left_Ia, left_Id, left_Is);
	vec3 temp = CalcDirLight(l, top_Ia.xyz, top_Id.xyz, top_Is.xyz, n, v) + CalcDirLight(l, left_Ia.xyz, left_Id.xyz, left_Is.xyz, n, v);
	fragColor = use_texture ? texture( TEX, tc ) : vec4(temp.x * norm.x, temp.y * norm.y, temp.z * norm.z, 1);
}