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


inline std::vector<block_t> create_blocks()
{
	std::vector<block_t> blocks;
	block_t temp;

	temp.center = vec3(0, 0, 0);
	temp.rotate_center = vec3(0);
	temp.current_theta = 0;
	temp.target_theta = temp.current_theta + PI;
	blocks.push_back(temp);

	temp.center = vec3(1, 0, 0);
	temp.rotate_center = vec3(0);
	temp.current_theta = atan((temp.center - temp.rotate_center).y / (temp.center - temp.rotate_center).x);
	temp.target_theta = temp.current_theta + PI;
	blocks.push_back(temp);

	temp.center = vec3(2, 0, 0);
	temp.rotate_center = vec3(0);
	temp.current_theta = atan((temp.center - temp.rotate_center).y / (temp.center - temp.rotate_center).x);
	temp.target_theta = temp.current_theta + PI;
	blocks.push_back(temp);

	temp.center = vec3(3, 0, 0);
	temp.rotate_center = vec3(0);
	temp.current_theta = atan((temp.center - temp.rotate_center).y / (temp.center - temp.rotate_center).x);
	temp.target_theta = temp.current_theta + PI;
	blocks.push_back(temp);

	temp.center = vec3(3, 1, 0);
	temp.rotate_center = vec3(0);
	temp.current_theta = atan((temp.center - temp.rotate_center).y / (temp.center - temp.rotate_center).x);
	temp.target_theta = temp.current_theta + PI;
	blocks.push_back(temp);

	temp.center = vec3(3, 2, 0);
	temp.rotate_center = vec3(0);
	temp.current_theta = atan((temp.center - temp.rotate_center).y / (temp.center - temp.rotate_center).x);
	temp.target_theta = temp.current_theta + PI;
	blocks.push_back(temp);

	temp.center = vec3(3, 3, 0);
	temp.rotate_center = vec3(0);
	temp.current_theta = atan((temp.center - temp.rotate_center).y / (temp.center - temp.rotate_center).x);
	temp.target_theta = temp.current_theta + PI;
	blocks.push_back(temp);

	temp.center = vec3(3, 3, 1);
	temp.rotate_center = vec3(0);
	temp.current_theta = atan((temp.center - temp.rotate_center).y / (temp.center - temp.rotate_center).x);
	temp.target_theta = temp.current_theta + PI;
	blocks.push_back(temp);

	temp.center = vec3(3, 3, 2);
	temp.rotate_center = vec3(0);
	temp.current_theta = atan((temp.center - temp.rotate_center).y / (temp.center - temp.rotate_center).x);
	temp.target_theta = temp.current_theta + PI;
	blocks.push_back(temp);
//	temp.center = vec3(3, 3, 3);
//	blocks.push_back(temp);
	
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
