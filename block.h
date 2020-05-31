#pragma once
#ifndef __block_H__
#define __block_H__

#define	TOTAL_STAGE	(5)
int		stage = 0;
int		eye_change = 0;
int		eye_state[TOTAL_STAGE] = { 0, 0, 4, 4, 0 };
int		lever_change = 0;
int		lever_state[TOTAL_STAGE] = { 4, 8, 0, 4, 0 };
int		rotate_blocks_cnt[TOTAL_STAGE] = { 1, 1, 1, 4, 0 };
int		triggers_cnt[TOTAL_STAGE] = { 1, 1, 1, 4, 0 };
float	block_size = 0.5f;

enum Block_Type : int
{
	Stop_Block,
	Rotate_Block,
	Move_Block,
	Elevate_Block	// #TODO# Not yet implemented.
};

struct block_t
{
	// Common Variables.
	vec3	center		= vec3(0);		// 3D position for translation
	float	size		= block_size;	// size
	mat4	model_matrix;					// modeling transformation
	Block_Type	type	= Stop_Block;
	// Change Block's Common Variables. (Change Block = Rotate_Block | Move_Block | Elevate_Block)
	int		sw			= 0;
	vec3	destination = vec3(0);
	vec3	diff		= vec3(0);
	float	dis			= 0;
	// "Rotate_Block" Variables.
	vec3	rotate_center	= vec3(0);
	vec3	axis			= vec3(0);
	float	current_theta	= 0;
	float	target_theta	= 0;
	float	facing_theta	= 0;
	bool	rotate_flag		= false;
	// "Elevate_Block" Variables.
	// #TODO#

	// public functions
	void	block_rotation(float rotate_angle);
	void	update(float t);
};

inline block_t init_block( vec3 _center, Block_Type _type = Stop_Block, int _sw = 0, vec3 _destination = vec3(0),
							vec3 _rotate_center = vec3(0), vec3 _axis = vec3(0), float _current_theta = 0 )
{
	block_t temp;
	// Common Variables.
	temp.center	= _center;
	temp.type	= _type;
	// Change Block's Common Variables. (Change Block = Rotate_Block | Move_Block | Elevate_Block)
	temp.sw				= _sw;
	temp.destination	= _destination;
	temp.diff			= _destination - _center;
	if (_type == Rotate_Block)
		temp.dis = distance(_center, _rotate_center);
	else if (_type == Move_Block)
		temp.dis = distance(_center, _destination);
	else
		temp.dis = 0;
	// "Rotate_Block" Variables.
	temp.rotate_center	= _rotate_center;
	temp.axis			= _axis;
	temp.current_theta	= _current_theta;
	temp.target_theta	= _current_theta;
	temp.rotate_flag	= false;
	// "Elevate_Block" Variables.
	// #TODO#
	return temp;
}

inline std::vector<block_t> create_blocks0()
{
	std::vector<block_t> blocks;
	// Connected to 'Rotate Block'.
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -1 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +0 * block_size, +2 * block_size)));
	// Starting Main Road.
	blocks.push_back(init_block(vec3(+1 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+0 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-2 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, +1 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, +0 * block_size)));
	// Main Road to the Goal.
	blocks.push_back(init_block(vec3(-6 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-6 * block_size, -2 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(-6 * block_size, -2 * block_size, -1 * block_size)));
	return blocks;
}

inline std::vector<block_t> create_rotate_blocks0()
{
	std::vector<block_t> blocks;
	// Rotate_Blocks to the Goal.
	blocks.push_back(init_block(
		vec3(+2 * block_size, +1 * block_size, +2 * block_size), Rotate_Block, 3,
		vec3(-3 * block_size, -4 * block_size, -3 * block_size),
		vec3(+2 * block_size, +3 * block_size, +2 * block_size),
		vec3(1, 0, 0), PI));
	blocks.push_back(init_block(
		vec3(+2 * block_size, +2 * block_size, +2 * block_size), Rotate_Block, 3,
		vec3(-3 * block_size, -3 * block_size, -3 * block_size),
		vec3(+2 * block_size, +3 * block_size, +2 * block_size),
		vec3(1, 0, 0), PI));
	// Center.
	blocks.push_back(init_block(
		vec3(+2 * block_size, +3 * block_size, +2 * block_size), Rotate_Block, 3,
		vec3(-3 * block_size, -2 * block_size, -3 * block_size),
		vec3(+2 * block_size, +3 * block_size, +2 * block_size),
		vec3(1, 0, 0), PI));
	blocks.push_back(init_block(
		vec3(+1 * block_size, +3 * block_size, +2 * block_size), Rotate_Block, 3,
		vec3(-4 * block_size, -2 * block_size, -3 * block_size),
		vec3(+1 * block_size, +3 * block_size, +2 * block_size),
		vec3(1, 0, 0), PI));
	blocks.push_back(init_block(
		vec3(+0 * block_size, +3 * block_size, +2 * block_size), Rotate_Block, 3,
		vec3(-5 * block_size, -2 * block_size, -3 * block_size),
		vec3(+0 * block_size, +3 * block_size, +2 * block_size),
		vec3(1, 0, 0), PI));
	return blocks;
}

inline std::vector<block_t> create_blocks1()
{
	std::vector<block_t> blocks;
	// Starting Main Road.
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+1 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+0 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, +1 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, +0 * block_size)));
	// Right-Side Road.
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+3 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+4 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+4 * block_size, -2 * block_size, -4 * block_size)));
	blocks.push_back(init_block(vec3(+4 * block_size, -2 * block_size, -5 * block_size)));
	// Left-Side Road.
	blocks.push_back(init_block(vec3(-4 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-5 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-6 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-6 * block_size, -2 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(-6 * block_size, -2 * block_size, -1 * block_size)));
	// Main Road to the Goal.
	blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, -6 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, -7 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, -8 * block_size)));
	blocks.push_back(init_block(vec3(-2 * block_size, -2 * block_size, -8 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, -8 * block_size)));
	return blocks;
}

inline std::vector<block_t> create_rotate_blocks1()
{
	std::vector<block_t> blocks;
	blocks.push_back(init_block(
		vec3(-1 * block_size, -2 * block_size, -1 * block_size), Rotate_Block, 4,
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(0, 1, 0), 0));
	blocks.push_back(init_block(
		vec3(-1 * block_size, -2 * block_size, -2 * block_size), Rotate_Block, 4,
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(0, 1, 0), 0));
	// Center.
	blocks.push_back(init_block(
		vec3(-1 * block_size, -2 * block_size, -3 * block_size), Rotate_Block, 4,
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(0, 1, 0), 0));
	blocks.push_back(init_block(
		vec3(-2 * block_size, -2 * block_size, -3 * block_size), Rotate_Block, 4,
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(0, 1, 0), - PI / 2));
	blocks.push_back(init_block(
		vec3(-3 * block_size, -2 * block_size, -3 * block_size), Rotate_Block, 4,
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(-1 * block_size, -2 * block_size, -3 * block_size),
		vec3(0, 1, 0), - PI / 2));
	return blocks;
}

inline std::vector<block_t> create_blocks2()
{
	std::vector<block_t> blocks;
	// First Pillar.
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, -1 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, +0 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, +1 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +1 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +0 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -1 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, +1 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, +0 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, -1 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, -2 * block_size)));
	// Second Pillar.
	blocks.push_back(init_block(vec3(-2 * block_size, +3 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, +3 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+0 * block_size, +3 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+1 * block_size, +3 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +3 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +1 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, +0 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -1 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+2 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+1 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(+0 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-2 * block_size, -2 * block_size, -3 * block_size)));
	// Third Pillar.
	blocks.push_back(init_block(vec3(-3 * block_size, +3 * block_size, +1 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +3 * block_size, +0 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +3 * block_size, -1 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +3 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +3 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +1 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +0 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -1 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, -3 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, -2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, -1 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, +0 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, +1 * block_size)));
	// Fourth Pillar.
	blocks.push_back(init_block(vec3(+1 * block_size, +3 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+0 * block_size, +3 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, +3 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-2 * block_size, +3 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +3 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +1 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, +0 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -1 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-3 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-2 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(-1 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+0 * block_size, -2 * block_size, +2 * block_size)));
	blocks.push_back(init_block(vec3(+1 * block_size, -2 * block_size, +2 * block_size)));
	// Main Road to the Goal.
	return blocks;
}

inline std::vector<block_t> create_rotate_blocks2()
{
	std::vector<block_t> blocks;
	return blocks;
}

inline std::vector<block_t> create_blocks3()
{
	std::vector<block_t> blocks;
	// Four Sides.
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			blocks.push_back(init_block(vec3(+2 * block_size, (+2 - i) * block_size, (+2 - j) * block_size)));
			blocks.push_back(init_block(vec3((+2 - j) * block_size, (+2 - i) * block_size, -3 * block_size)));
			blocks.push_back(init_block(vec3(-3 * block_size, (+2 - i) * block_size, (-3 + j) * block_size)));
			blocks.push_back(init_block(vec3((-3 + j) * block_size, (+2 - i) * block_size, +2 * block_size)));
		}
	}
	// Main Road to the Goal.
	return blocks;
}

inline std::vector<block_t> create_rotate_blocks3_0()
{
	std::vector<block_t> blocks;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			blocks.push_back(init_block(
				vec3((-3 + i) * block_size, +3 * block_size, (-3 + j) * block_size), Rotate_Block, 0,
				vec3((-3 + i) * block_size, (+3 + j)* block_size, -3 * block_size),
				vec3((-3 + i) * block_size, +3 * block_size, -3 * block_size),
				vec3(1, 0, 0), - PI / 2));
		}
	}
	return blocks;
}

inline std::vector<block_t> create_rotate_blocks3_1()
{
	std::vector<block_t> blocks;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			blocks.push_back(init_block(
				vec3((-3 + j) * block_size, +3 * block_size, (+2 - i) * block_size), Rotate_Block, 1,
				vec3(-3 * block_size, (+3 + j) * block_size, (+2 - i) * block_size),
				vec3(-3 * block_size, +3 * block_size, (+2 - i) * block_size),
				vec3(0, 0, 1), 0));
		}
	}
	return blocks;
}

inline std::vector<block_t> create_rotate_blocks3_2()
{
	std::vector<block_t> blocks;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			blocks.push_back(init_block(
				vec3((-3 + i) * block_size, +3 * block_size, (+2 - j) * block_size), Rotate_Block, 2,
				vec3((-3 + i) * block_size, (+3 + j) * block_size, +2 * block_size),
				vec3((-3 + i) * block_size, +3 * block_size, +2 * block_size),
				vec3(1, 0, 0), - PI / 2));
		}
	}
	return blocks;
}

inline std::vector<block_t> create_rotate_blocks3_3()
{
	std::vector<block_t> blocks;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			blocks.push_back(init_block(
				vec3((+2 - j) * block_size, +3 * block_size, (+2 - i) * block_size), Rotate_Block, 3,
				vec3(+2 * block_size, (+3 + j) * block_size, (+2 - i) * block_size),
				vec3(+2 * block_size, +3 * block_size, (+2 - i) * block_size),
				vec3(0, 0, 1), - PI));
		}
	}
	return blocks;
}

inline void block_t::block_rotation(float rotate_angle=0)
{
	if (type == Rotate_Block)
	{
		target_theta = current_theta + rotate_angle;
	}
	rotate_flag = true;
}

inline void block_t::update(float t)
{
	if (type == Stop_Block)
	{
		model_matrix = mat4::translate(center)	// rotation around sun
			* mat4::rotate(axis, facing_theta)	// self rotation
			* mat4::scale(size, size, size);	// size
	}
	else if (type == Rotate_Block)
	{
		//if it's rotating
		if (abs(target_theta - current_theta) > 0) {
			if (target_theta > current_theta) {
				if (target_theta < current_theta + 10*t) t = (target_theta - current_theta) / 10;
			}
			else {
				t = -t;
				if (target_theta > current_theta + 10*t) t = (target_theta - current_theta) / 10;
			}
			current_theta = current_theta + 10*t;
			center = rotate_center;
			if (stage == 0)
			{
				facing_theta = facing_theta + 10 * t;
				if (axis.x > 0.0f)		center = rotate_center + vec3(0, dis * cos(current_theta), dis * sin(current_theta));// +(diff * sin(facing_theta));
				else if (axis.y > 0.0f)	center = rotate_center + vec3(dis * sin(current_theta), 0, dis * cos(current_theta));// +(diff * sin(facing_theta));
				else if (axis.z > 0.0f)	center = rotate_center + vec3(dis * cos(current_theta), dis * sin(current_theta), 0);// +(diff * sin(facing_theta));
				if (sw == lever_change || (sw + 1) % lever_state[stage] == lever_change) center += diff * sin(current_theta);
			}
			else if (stage == 1)
			{
				facing_theta = facing_theta + 10 * t;
				if (axis.x > 0.0f)		center = rotate_center + vec3(0, dis * cos(current_theta), dis * sin(current_theta));// +(diff * sin(facing_theta));
				else if (axis.y > 0.0f)	center = rotate_center + vec3(dis * sin(current_theta), 0, dis * cos(current_theta));// +(diff * sin(facing_theta));
				else if (axis.z > 0.0f)	center = rotate_center + vec3(dis * cos(current_theta), dis * sin(current_theta), 0);// +(diff * sin(facing_theta));
			}
			else if (stage == 3)
			{
				if (sw == 0 || sw == 3)
					facing_theta = facing_theta - 10 * t;
				else
					facing_theta = facing_theta + 10 * t;
				if (axis.x > 0.0f)
				{
					if (sw == 0)	center = rotate_center + vec3(0, dis * cos(-current_theta), dis * sin(-current_theta));
					else			center = rotate_center + vec3(0, dis * cos(current_theta - PI / 2), dis * sin(current_theta - PI / 2));
				}
				else if (axis.z > 0.0f)
				{
					if (sw == 1)	center = rotate_center + vec3(dis * cos(current_theta), dis * sin(current_theta), 0);
					else			center = rotate_center + vec3(dis * cos(-current_theta), dis * sin(-current_theta), 0);
				}
			}
		}
		//if it finished rotating
		else if (rotate_flag == true)
		{
			rotate_flag = false;
			if (stage == 3)
				sw = (sw + 2) % 4;
		}
		else {
			rotate_flag = false;
		}
		model_matrix = mat4::translate(center)	// rotation around sun
			* mat4::rotate(axis, facing_theta)	// self rotation
			* mat4::scale(size, size, size);	// size
	}
	else if (type == Move_Block)
	{
		model_matrix = mat4::translate(center)
			* mat4::scale(size, size, size);
	}
}

#endif
