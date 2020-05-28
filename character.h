#pragma once
#ifndef __character_H__
#define __character_H__

float character_size = 0.2f;
float jump_power = 0.01f;
float gravity = 0.03f;

struct character_t
{
	vec3	location			= vec3(0);		// position
	vec3	ori_location		= vec3(0);		// position
	float	facing_theta		= 0;			// facing direction
	float	falling				= 0;
	int		floor = 0;			// 0 = xy floor with z>0, 1 = yz floor with x>0, 2= xz floor with y>0
	bool	front_arrow_button	= false;		// front arrow button
	bool	back_arrow_button	= false;		// back arrow button
	bool	left_arrow_button	= false;		// left arrow button
	bool	right_arrow_button	= false;		// right arrow button

	mat4	model_matrix;						// modeling transformation

	// public functions
	void	front_button(bool status);
	void	back_button(bool status);
	void	left_button(bool status);
	void	right_button(bool status);
	void	spacebar_button();
	void	update(float t, std::vector<std::vector<block_t>*>& obstcle);
};

inline character_t init_character( vec3 _center, float _facing=0, int _floor=0)
{
	character_t temp;
	temp.ori_location	= _center;
	temp.location		= _center;
	temp.facing_theta	= _facing;
	temp.floor			= _floor;

	return temp;
}

inline std::vector<character_t> create_characters()
{
	std::vector<character_t> characters;
	characters.push_back(init_character(vec3(1, 0.5, 0.5),PI/2));
	return characters;
}

inline void character_t::front_button(bool status){ front_arrow_button = status; }
inline void character_t::back_button(bool status) { back_arrow_button = status; }
inline void character_t::left_button(bool status) { left_arrow_button = status; }
inline void character_t::right_button(bool status) { right_arrow_button = status; }
inline void character_t::spacebar_button() { 
	
	if (falling == 0) {
		falling = -sqrt(jump_power / gravity);
	}
}

inline void character_t::update(float t, std::vector<std::vector<block_t>*>& obstacles)
{
	int moving = 0;
	int rotating = 0;
	if (front_arrow_button) moving += 1;
	if (back_arrow_button) moving -= 1;
	if (right_arrow_button) rotating -= 1;
	if (left_arrow_button) rotating += 1;

	facing_theta += 2*t*rotating;

	vec3 next_location = vec3(0);
	vec3 axis = vec3(0, 0, 1);

	if (floor == 0) {
		vec3 axis = vec3(0, 0, 1);
		next_location = vec3(
			location.x + cos(facing_theta) * t * moving, 
			location.y + sin(facing_theta) * t * moving, 
			location.z);
	}
	else if (floor == 1) {
		vec3 axis = vec3(1, 0, 0);
		next_location = vec3(
			location.x , 
			location.y + sin(facing_theta) * t * moving, 
			location.z + cos(facing_theta) * t * moving);
	}
	else {
		vec3 axis = vec3(0, 1, 0);
		next_location = vec3(
			location.x + sin(facing_theta) * t * moving, 
			location.y, 
			location.z + cos(facing_theta) * t * moving);
	}

	bool onfloor = false;
	bool blocked = false;

	std::vector<std::vector<block_t>*>::iterator iter;
	for (iter = obstacles.begin(); iter != obstacles.end(); iter++) {
		std::vector<block_t> blocks = **iter;
		for (auto& s : blocks)
		{
			if (floor == 0) {
				//block on next_location
				//x������ ���� �� �����̶� ������ ��
				if (abs(next_location.x - s.center.x) < character_size + s.size / 2 &&
					next_location.y >= s.center.y - s.size / 2 && next_location.y <= s.center.y + s.size / 2 &&
					next_location.z > s.center.z - s.size / 2 && next_location.z < s.center.z + s.size / 2) {
					next_location.x = location.x;
				}
				//y������ ���� �� �����̶� ������ ��
				if (abs(next_location.y - s.center.y) < character_size + s.size / 2 &&
					next_location.x >= s.center.x - s.size / 2 && next_location.x <= s.center.x + s.size / 2 &&
					next_location.z > s.center.z - s.size / 2 && next_location.z < s.center.z + s.size / 2) {
					next_location.y = location.y;
				}
				//���� ��ġ���� �����ִ� �����̸�
				if (next_location.x >= s.center.x - s.size / 2 && next_location.x <= s.center.x + s.size / 2 &&
					next_location.y >= s.center.y - s.size / 2 && next_location.y <= s.center.y + s.size / 2 &&
					next_location.z - s.center.z <= s.size / 2 + character_size &&
					s.center.z - next_location.z <= s.size / 2 + character_size) {
					falling = min(0, falling);
					onfloor = true;
					next_location.z = s.size / 2 + character_size + s.center.z;
				}
			}
			else if (floor == 1) {

			}
			else if (floor == 2) {

			}
		}
	}
	if ((!onfloor) || falling < 0) {
		if (floor == 0) {
			falling += t;
			next_location.z = location.z - (falling>=0?1:-1)*falling * falling * gravity;
		}
		else if (floor == 1) {
			falling += t;
			next_location.x = location.x - falling * falling * gravity;
		}
	}
	else {
		falling = 0;
	}
	if (next_location.z < -10.0f) next_location = ori_location;
	location = next_location;



	
	model_matrix = mat4::translate(location)	// rotation around sun
		* mat4::rotate(axis, facing_theta)	// self rotation
		* mat4::scale(character_size, character_size, character_size);	// size
}

#endif