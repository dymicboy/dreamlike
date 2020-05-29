#pragma once
#ifndef __CGUT2_H__
#define __CGUT2_H__

#include "cgut.h"

struct Texture
{
	std::string path;
	GLuint id = 0;

	~Texture(void){ glDeleteTextures(1, &id); }
};

struct material
{
	std::string name;

	vec4	ambient;
	vec4	diffuse;
	vec4	specular;
	vec4	emissive;

	float	shininess;			// the power of specular in phong.
	float	opacity;			// alpha OR opacity
	float	refractive_index;		// index of refraction (IOR)

	struct maps {
		Texture* ambient	= nullptr;
		Texture* diffuse	= nullptr;
		Texture* specular	= nullptr;
		Texture* bump		= nullptr;
	} textures;
};

struct geometry
{
	material*	mat = nullptr;		// material of this geometry
	size_t		index_start = 0;	// the starting index of index buffer.
	size_t		index_count = 0;	// the number of indicies of index buffer. 

	geometry(void) {}
	geometry(material* mat, size_t index_start, size_t index_count): mat(mat), index_start(index_start), index_count(index_count) {}
	size_t get_num_faces(void) { return index_count / 3; }
};

struct mesh2
{
	std::vector<vertex>	vertex_list;
	std::vector<uint>	index_list;
	GLuint				vertex_buffer = 0;
	GLuint				index_buffer = 0;
	GLuint				vertex_array = 0;
	std::vector<material*>	material_list;
	std::vector<geometry>	geometry_list;

	~mesh2(void)
	{
		const size_t k_end = material_list.size();
		for (size_t k = 0; k < k_end; k++) {
			delete material_list[k];
		}

		if(vertex_buffer) glDeleteBuffers(1, &vertex_buffer);
		if(index_buffer) glDeleteBuffers(1, &index_buffer);
		if(vertex_array) glDeleteVertexArrays(1,&vertex_array);
	}
};

#endif // __CGUT2_H__
