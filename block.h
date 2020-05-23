#pragma once
#ifndef __block_H__
#define __block_H__

struct block_t
{
	vec3	center = vec3(0);	// 3D position for translation
	float	size = 1.0f;		// size
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float t);
};


inline std::vector<block_t> create_blocks()
{
	std::vector<block_t> blocks;
	block_t temp;

	temp.center = vec3(0, 0, 0);
	blocks.push_back(temp);
	temp.center = vec3(1, 0, 0);
	blocks.push_back(temp);
	temp.center = vec3(2, 0, 0);
	blocks.push_back(temp);
	temp.center = vec3(3, 0, 0);
	blocks.push_back(temp);

	temp.center = vec3(3, 1, 0);
	blocks.push_back(temp);
	temp.center = vec3(3, 2, 0);
	blocks.push_back(temp);
	temp.center = vec3(3, 3, 0);
	blocks.push_back(temp);

	temp.center = vec3(3, 3, 1);
	blocks.push_back(temp);
	temp.center = vec3(3, 3, 2);
	blocks.push_back(temp);
//	temp.center = vec3(3, 3, 3);
//	blocks.push_back(temp);
	
	return blocks;
}

inline void block_t::update(float t)
{
	
	model_matrix = mat4::translate(center)	// rotation around sun
		* mat4::rotate(vec3(0, 0, 0), 0)	// self rotation
		* mat4::scale(size, size, size);	// size
}

#endif
