#pragma once
#ifndef __block_H__
#define __block_H__

struct block_t
{
	vec3	center = vec3(0);	// 3D position for translation
	float	size = 1.0f;		// size
	mat4	model_matrix;		// modeling transformation
	float	current_theta = 0;
	float	target_theta = 0;
	float	facing_theta = 0;
	vec3	rotate_center = vec3(0);

	// public functions
	void	update(float t);
};

inline block_t init_block( vec3 _center, vec3 _rotate_center, bool _rotate_flag = false)
{
	block_t temp;
	float dist = distance(_center, _rotate_center);

	temp.center = _center;
	temp.rotate_center = _rotate_center;
	if (dist == 0.0f)	temp.current_theta = 0.0f;
	else				temp.current_theta = atan((_center - _rotate_center).y / (_center - _rotate_center).x);
	if (_rotate_flag)	temp.target_theta = temp.current_theta + PI;
	else				temp.target_theta = temp.current_theta;

	return temp;
}

inline std::vector<block_t> create_blocks()
{
	std::vector<block_t> blocks;

	blocks.push_back( init_block( vec3(0,0,0), vec3(0) ) );
	blocks.push_back( init_block( vec3(1,0,0), vec3(0) ) );
	blocks.push_back( init_block( vec3(2,0,0), vec3(0) ) );
	blocks.push_back( init_block( vec3(3,0,0), vec3(0) ) );
	blocks.push_back( init_block( vec3(3,1,0), vec3(0) ) );
	blocks.push_back( init_block( vec3(3,2,0), vec3(0) ) );
	blocks.push_back( init_block( vec3(3,3,0), vec3(0) ) );
	blocks.push_back( init_block( vec3(3,3,1), vec3(0) ) );
	blocks.push_back( init_block( vec3(3,3,2), vec3(0) ) );
	blocks.push_back( init_block( vec3(3,3,3), vec3(0) ) );
	
	return blocks;
}

inline void block_t::update(float t)
{
	//현재 도는 중일때
	if (abs(current_theta - target_theta)>t){
		float dis = distance(vec2(center.x, center.y), vec2(rotate_center.x,rotate_center.y));
		current_theta = current_theta + t;
		facing_theta = facing_theta + t;
		center = vec3(dis * cos(current_theta), dis * sin(current_theta), center.z);
	}
	model_matrix = mat4::translate(center)	// rotation around sun
		* mat4::rotate(vec3(0, 0, 1), facing_theta)	// self rotation
		* mat4::scale(size, size, size);	// size
}

#endif
