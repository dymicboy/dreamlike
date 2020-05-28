#pragma once
#ifndef __block_H__
#define __block_H__


float block_size = 0.5f;
struct block_t
{
	vec3	center = vec3(0);	// 3D position for translation
	float	size = block_size;		// size
	mat4	model_matrix;		// modeling transformation
	float	current_theta = 0;
	float	target_theta = 0;
	float	facing_theta = 0;
	vec3	rotate_center = vec3(0);
	float	dis = 0;
	bool	rotate_flag = false;
	vec3	lever = vec3(0);
	vec3	axis = vec3(0);

	// public functions
	void	update(float t);
};

inline block_t init_block( vec3 _center, vec3 _rotate_center, vec3 _lever = vec3(0), vec3 _axis = vec3(0), bool _rotate_flag = false)
{
	block_t temp;

	temp.center = _center;
	temp.rotate_center = _rotate_center;
	temp.dis = distance(_center, _rotate_center);
	if (temp.dis == 0.0f)	temp.current_theta = 0;
	else
	{
		if (_axis.x > 0.0f)	temp.current_theta = atan((_center - _rotate_center).z / (_center - _rotate_center).y);
		if (_axis.y > 0.0f)	temp.current_theta = atan((_center - _rotate_center).x / (_center - _rotate_center).z);
		if (_axis.z > 0.0f)	temp.current_theta = atan((_center - _rotate_center).y / (_center - _rotate_center).x);
	}
	if (_rotate_flag)	temp.target_theta = temp.current_theta + PI;
	else				temp.target_theta = temp.current_theta;
	temp.lever = _lever;
	temp.axis = _axis;
	temp.rotate_flag = _rotate_flag;

	return temp;
}

inline std::vector<block_t> create_blocks()
{
	std::vector<block_t> blocks;

	//blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, -2 * block_size), vec3(0)));
	//blocks.push_back(init_block(vec3(-2 * block_size, -2 * block_size, -2 * block_size), vec3(0)));
	//blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, -2 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, -2 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+1 * block_size, -2 * block_size, -2 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+0 * block_size, -2 * block_size, -2 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+2 * block_size, -1 * block_size, -2 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+2 * block_size, +0 * block_size, -2 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+2 * block_size, +1 * block_size, -2 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+2 * block_size, +2 * block_size, -2 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, -2 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, -1 * block_size), vec3(0)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, +0 * block_size), vec3(0)));

	blocks.push_back(
		init_block(
			vec3(+2 * block_size, +3 * block_size, +1 * block_size), 
			vec3(+2 * block_size, +3 * block_size, +3 * block_size), 
			vec3(-5 * block_size, -5 * block_size, -5 * block_size), 
			vec3(0,1,0), true));
	blocks.push_back(
		init_block(
			vec3(+2 * block_size, +3 * block_size, +2 * block_size), 
			vec3(+2 * block_size, +3 * block_size, +3 * block_size), 
			vec3(-5 * block_size, -5 * block_size, -5 * block_size), 
			vec3(0,1,0), true));
	blocks.push_back(
		init_block(
			vec3(+2 * block_size, +3 * block_size, +3 * block_size), 
			vec3(+2 * block_size, +3 * block_size, +3 * block_size), 
			vec3(-5 * block_size, -5 * block_size, -5 * block_size), 
			vec3(0,1,0), true));
	
	return blocks;
}

inline void block_t::update(float t)
{
	//현재 도는 중일때
	if (abs(current_theta-target_theta)>t) {
		float c = cos(current_theta), s = sin(current_theta);
		current_theta = current_theta + t;
		facing_theta = facing_theta + t;
		if (axis.x > 0.0f)		center = rotate_center + vec3(0, dis * cos(current_theta), dis * sin(current_theta)) + (lever * cos(current_theta));
		else if (axis.y > 0.0f)	center = rotate_center + vec3(dis * cos(-current_theta), 0, dis * sin(-current_theta)) + (lever * cos(current_theta));
		else if (axis.z > 0.0f)	center = rotate_center + vec3(dis * sin(current_theta), dis * cos(current_theta), 0) + (lever * cos(current_theta));
	}
	model_matrix = mat4::translate(center)	// rotation around sun
		* mat4::rotate(axis, facing_theta)	// self rotation
		* mat4::scale(size, size, size);	// size
}

#endif
