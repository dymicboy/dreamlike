#pragma once
#ifndef __block_H__
#define __block_H__


float block_size = 0.5f;
struct block_t
{
	vec3	center			= vec3(0);		// 3D position for translation
	float	size			= block_size;	// size
	mat4	model_matrix;					// modeling transformation
	float	current_theta	= 0;
	float	target_theta	= 0;
	float	facing_theta	= 0;
	vec3	rotate_center	= vec3(0);
	float	dis				= 0;
	bool	rotate_flag		= false;
	vec3	lever			= vec3(0);
	vec3	axis			= vec3(0);

	// public functions
	void	block_rotation(float rotate_angle);
	void	update(float t);
};

inline block_t init_block( vec3 _center, vec3 _rotate_center=vec3(0), vec3 _lever = vec3(0), vec3 _axis = vec3(0), float _current_theta = 0)
{
	block_t temp;
	temp.center = _center;
	temp.rotate_center = _rotate_center;
	temp.dis = distance(_center, _rotate_center);
	temp.current_theta = _current_theta;
	temp.target_theta = temp.current_theta;
	temp.lever = _lever;
	temp.axis = _axis;
	temp.rotate_flag = false;

	return temp;
}

inline std::vector<block_t> create_blocks1()
{
	std::vector<block_t> blocks;

	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(+1 * block_size, -2 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(+0 * block_size, -2 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -1 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +0 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +1 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +2 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, -1 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, +0 * block_size)));
	
	return blocks;
}

inline std::vector<block_t> create_rotate_blocks1()
{
	std::vector<block_t> blocks;
	/*
	if (_axis.x > 0.0f)	temp.current_theta = atan((_center - _rotate_center).z / (_center - _rotate_center).y);
	if (_axis.y > 0.0f)	temp.current_theta = atan((_rotate_center - _center).z / (_rotate_center - _center).x);
	if (_axis.z > 0.0f)	temp.current_theta = atan((_center - _rotate_center).y / (_center - _rotate_center).x);
	printf("%f %f %f %f\n", _axis.x, _axis.y, _axis.z, atan((_center - _rotate_center).x / (_center - _rotate_center).z));
	*/
	vec3 rotating_center = vec3(+2 * block_size, +3 * block_size, +3 * block_size);
	vec3 ex = vec3(+2 * block_size, +3 * block_size, +1 * block_size);
	float current = atan((rotating_center - ex).z / (rotating_center - ex).x);

	blocks.push_back(init_block(
		vec3(+2 * block_size, +3 * block_size, +1 * block_size),
		vec3(+2 * block_size, +3 * block_size, +3 * block_size),
		vec3(-5 * block_size, -5 * block_size, -5 * block_size),
		vec3(0, 1, 0), current));
	blocks.push_back(init_block(
		vec3(+2 * block_size, +3 * block_size, +2 * block_size),
		vec3(+2 * block_size, +3 * block_size, +3 * block_size),
		vec3(-5 * block_size, -5 * block_size, -5 * block_size),
		vec3(0, 1, 0), current));
	blocks.push_back(init_block(
		vec3(+2 * block_size, +3 * block_size, +3 * block_size),
		vec3(+2 * block_size, +3 * block_size, +3 * block_size),
		vec3(-5 * block_size, -5 * block_size, -5 * block_size),
		vec3(0, 1, 0), current));
	return blocks;
}

inline void block_t::block_rotation(float rotate_angle=0)
{
	target_theta = current_theta + rotate_angle;
	rotate_flag = true;
}

inline void block_t::update(float t)
{
	//if it's rotating
	if (abs(target_theta-current_theta)>0) {
		if (target_theta > current_theta) {
			if (target_theta < current_theta + t) t = target_theta - current_theta;
		}
		else {
			t = -t;
			if (target_theta > current_theta + t) t = target_theta - current_theta;
		}
		current_theta = current_theta + t;
		facing_theta = facing_theta + t;
		if (axis.x > 0.0f)		center = rotate_center + vec3(0, dis * cos(current_theta), dis * sin(current_theta)) + (lever * cos(current_theta));
		else if (axis.y > 0.0f)	center = rotate_center + vec3(dis * cos(-current_theta), 0, dis * sin(-current_theta)) + (lever * cos(current_theta));
		else if (axis.z > 0.0f)	center = rotate_center + vec3(dis * sin(current_theta), dis * cos(current_theta), 0) + (lever * cos(current_theta));
	}
	//if it finished rotating
	else {
		rotate_flag = false;
	}
	model_matrix = mat4::translate(center)	// rotation around sun
		* mat4::rotate(axis, facing_theta)	// self rotation
		* mat4::scale(size, size, size);	// size
}

#endif
