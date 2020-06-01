#ifndef __PARTICLE_H__
#define __PARTICLE_H__
#pragma once

#include "cgmath.h"
#include "cgut.h"

inline float random_range( float min, float max ){ return mix( min, max, rand()/float(RAND_MAX) ); }

struct particle_t
{
	static constexpr int MAX_PARTICLES = 40;

	vec3 origin;
	vec3 pos;
	vec4 color;
	vec3 velocity;
	float scale;
	float life;
	mat4 model_matrix;

	//optional
	float elapsed_time;
	float time_interval;

	particle_t() { reset(); }
	void reset();
	void update(vec3 _origin, float t);
};

inline void particle_t::reset()
{
	pos = vec3(0,0,0);
	color = vec4(random_range(0.8f, 1.0f), random_range(0.8f, 1.0f), 0, 1);
	scale = random_range(0.001f, 0.008f);
	life = random_range(0.01f, 1.0f);
	velocity = vec3(random_range(-1.0f, 1.0f), random_range(-1.0f, 1.0f), random_range(-1.0f, 1.0f))*0.1f;
	elapsed_time = 0.0f;
}

inline void particle_t::update(vec3 _origin, float t)
{
	origin = _origin;
	t = t * 1.5f;
	pos += velocity*t;
	life -= t;
	// disappear
	if (life < 0.0f) 
		color.a -=  t;
	// dead
	if (color.a < 0.0f) reset();

	model_matrix = mat4::translate(pos+origin)*
					mat4::scale(scale);
}

#endif