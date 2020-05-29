#pragma once
#ifndef __trigger_H__
#define __trigger_H__


float trigger_size = 0.5f;
struct trigger_t
{
	vec3	center			= vec3(0);		// 3D position for translation
	float	size			= trigger_size;	// size
	mat4	model_matrix;					// modeling transformation
	int		status_num = 0;
	int		status = 0;
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
	return temp;
}

inline std::vector<trigger_t> create_trigger1()
{
	std::vector<trigger_t> triggers;

	
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