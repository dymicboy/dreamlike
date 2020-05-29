#pragma once
#ifndef __trigger_H__
#define __trigger_H__


float trigger_size = 0.5f;
struct trigger_t
{
	vec3	center			= vec3(0);		// 3D position for translation
	float	size			= trigger_size;	// size
	mat4	model_matrix;					// modeling transformation
	int		status_num		= 0;
	int		status			= 0;
	int		floor			= 0;			// 0 = xy floor with z>0, 1 = yz floor with x>0, 2= xz floor with y>0
	
	// public functions
	void	trigger_activate();
	void	update(float t);
};

inline trigger_t init_trigger(vec3 _center, int _status_num = 0, int _floor = 0, int _status = 0)
{
	trigger_t temp;
	temp.center = _center;
	temp.status_num = _status_num;
	temp.status = _status;
	temp.floor	= _floor;
	if (_floor == 0) temp.center.z += trigger_size / 2 + 0.1f;
	if (_floor == 1) temp.center.x += trigger_size / 2 + 0.1f;
	if (_floor == 2) temp.center.y += trigger_size / 2 + 0.1f;
	return temp;
}

inline std::vector<trigger_t> create_triggers1()
{
	std::vector<trigger_t> triggers;
	triggers.push_back(init_trigger(vec3(+2 * trigger_size, -2 * trigger_size, -2 * trigger_size), 4, 0));
	triggers.push_back(init_trigger(vec3(+2 * trigger_size, +3 * trigger_size, +0 * trigger_size), 4, 1));
	triggers.push_back(init_trigger(vec3(+2 * trigger_size, +3 * trigger_size, -1 * trigger_size), 4, 2));
	
	return triggers;
}

inline void trigger_t::trigger_activate()
{
	status = (status+1)%status_num;
}

inline void trigger_t::update(float t)
{
	model_matrix = mat4::translate(center)	// rotation around sun
		//* mat4::rotate(axis, facing_theta)	// self rotation
		* mat4::scale(size, size, size);	// size
}

#endif
