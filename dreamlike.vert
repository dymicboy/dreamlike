// vertex attributes
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

// matrices
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform bool screen;
uniform float aspect;

out vec4 epos;
out vec3 norm;
out vec2 tc;

void main()
{
	if(screen){
		gl_Position = vec4(position.x / aspect, position.y, position.z ,1);
	}
	else{
		vec4 wpos = model_matrix * vec4(position,1);
		epos = view_matrix * wpos;
		gl_Position = projection_matrix * epos;
	}

	norm = normal;
	tc = texcoord;
}
