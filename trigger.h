#pragma once
#ifndef __trigger_H__
#define __trigger_H__


float trigger_size = 0.5f;
struct trigger_t
{
	vec3	center			= vec3(0);			// 3D position for translation
	float	size			= trigger_size;		// size
	mat4	model_matrix;						// modeling transformation
	int		status_num		= 0;
	int		status			= 0;
	int		floor			= 0;			// 0 = xy floor with z>0, 1 = yz floor with x>0, 2= xz floor with y>0
	int		shape			= 0;
	// public functions
	void	trigger_activate();
	void	update(float t);
};

inline trigger_t init_trigger(vec3 _center, int _status_num = 0, int _floor = 0, int _shape = 0, int _status = 0)
{
	trigger_t temp;
	temp.center = _center;
	temp.status_num = _status_num;
	temp.status = _status;
	temp.floor	= _floor;
	temp.shape = _shape;
	if (_floor == 0) temp.center.z += trigger_size / 2 + 0.1f;
	if (_floor == 1) temp.center.x += trigger_size / 2 + 0.1f;
	if (_floor == 2) temp.center.y += trigger_size / 2 + 0.1f;
	return temp;
}

inline std::vector<trigger_t> create_triggers0_0()
{
	std::vector<trigger_t> triggers;
	//z-axis test 
	//triggers.push_back(init_trigger(vec3(+2 * trigger_size, -2 * trigger_size, -2 * trigger_size), 4, 0));
	//x-axis test 
	//triggers.push_back(init_trigger(vec3(+2 * block_size, +3 * block_size, +0 * block_size), 4, 1));
	//y-axis test 
	//triggers.push_back(init_trigger(vec3(-6 * block_size, -2 * block_size, -1 * block_size), 4, 2));
	triggers.push_back(init_trigger(vec3(-2 * block_size, -2 * block_size, +0 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers0_1()
{
	std::vector<trigger_t> triggers;
	triggers.push_back(init_trigger(vec3(-5 * block_size, -2 * block_size, -2 * block_size), 4, 2));
	triggers.push_back(init_trigger(vec3(-5 * block_size, -2 * block_size, -2 * block_size), 4, 2, 5));
	return triggers;
}

inline std::vector<trigger_t> create_triggers1_0()
{
	std::vector<trigger_t> triggers;
	triggers.push_back(init_trigger(vec3(-2 * block_size, -2 * block_size, +3 * block_size), 4, 2));
	triggers.push_back(init_trigger(vec3(+3 * block_size, -2 * block_size, -2 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers1_1()
{
	std::vector<trigger_t> triggers;
	triggers.push_back(init_trigger(vec3(-4 * block_size, -2 * block_size, -7 * block_size), 4, 2));
	triggers.push_back(init_trigger(vec3(-4 * block_size, -2 * block_size, -7 * block_size), 4, 2, 5));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_0()
{
	std::vector<trigger_t> triggers;
	// First Pillar.
	triggers.push_back(init_trigger(vec3(+2 * block_size, -3 * block_size, +1 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_1()
{
	std::vector<trigger_t> triggers;
	// First Pillar.
	triggers.push_back(init_trigger(vec3(+2 * block_size, -3 * block_size, -2 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_2()
{
	std::vector<trigger_t> triggers;
	// Second Pillar.
	triggers.push_back(init_trigger(vec3(+1 * block_size, -3 * block_size, -3 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_3()
{
	std::vector<trigger_t> triggers;
	// Second Pillar.
	triggers.push_back(init_trigger(vec3(-2 * block_size, -3 * block_size, -3 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_4()
{
	std::vector<trigger_t> triggers;
	// Third Pillar.
	triggers.push_back(init_trigger(vec3(-3 * block_size, -3 * block_size, -2 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_5()
{
	std::vector<trigger_t> triggers;
	// Third Pillar.
	triggers.push_back(init_trigger(vec3(-3 * block_size, -3 * block_size, +1 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_6()
{
	std::vector<trigger_t> triggers;
	// Fourth Pillar.
	triggers.push_back(init_trigger(vec3(-2 * block_size, -3 * block_size, +2 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_7()
{
	std::vector<trigger_t> triggers;
	// Fourth Pillar.
	triggers.push_back(init_trigger(vec3(+1 * block_size, -3 * block_size, +2 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_8()
{
	std::vector<trigger_t> triggers;
	// Top.
	triggers.push_back(init_trigger(vec3(+2 * block_size, +2 * block_size, +2 * block_size), 4, 1));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_9()
{
	std::vector<trigger_t> triggers;
	// Top.
	triggers.push_back(init_trigger(vec3(-3 * block_size, +2 * block_size, +2 * block_size), 4, 0));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_10()
{
	std::vector<trigger_t> triggers;
	// Top.
	triggers.push_back(init_trigger(vec3(-3 * block_size, +2 * block_size, +2 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers2_11()
{
	std::vector<trigger_t> triggers;
	// Top.
	triggers.push_back(init_trigger(vec3(+2 * block_size, +2 * block_size, -3 * block_size), 4, 2));
	triggers.push_back(init_trigger(vec3(+2 * block_size, +2 * block_size, -3 * block_size), 4, 2, 5));
	return triggers;
}

inline std::vector<trigger_t> create_triggers3_0()
{
	std::vector<trigger_t> triggers;
	triggers.push_back(init_trigger(vec3(-3 * block_size, +0 * block_size, -3 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers3_1()
{
	std::vector<trigger_t> triggers;
	triggers.push_back(init_trigger(vec3(+2 * block_size, +3 * block_size, -3 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers3_2()
{
	std::vector<trigger_t> triggers;
	triggers.push_back(init_trigger(vec3(-3 * block_size, +3 * block_size, -3 * block_size), 4, 2));
	return triggers;
}

inline std::vector<trigger_t> create_triggers3_3()
{
	std::vector<trigger_t> triggers;
	triggers.push_back(init_trigger(vec3(-3 * block_size, +3 * block_size, +2 * block_size), 4, 2));
	return triggers;
}

inline void trigger_t::trigger_activate()
{
	status = (status+1) % status_num;
}

inline void trigger_t::update(float t)
{
	model_matrix = mat4::translate(center)	// rotation around sun
		//* mat4::rotate(axis, facing_theta)	// self rotation
		* mat4::scale(size, size, size);	// size
}

#endif
