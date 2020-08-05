#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include "bitmap.h"

using namespace std;

#ifndef _EVENT_HPP
#define _EVENT_HPP

struct collision
{
	float* pos;
	int* color;
};

class Event {
	public:
		Event(Bitmap *b, int start, int duration) : _b(b), _start(start), _duration(duration) {}

		virtual void Activate(int frame_num) = 0;

	protected:
		Bitmap* _b; 
		int _start;
		int _duration;
};

class Circle
{
	public:
		Circle()
		{
			_pos = new float[2];
			_vel = new float[2];
			_rgb = new int[3];

			_pos[0] = ((float) (rand() % 400000)) / 1000;
			_pos[1] = ((float) (rand() % 400000)) / 1000;

			_vel[0] = ((float) (rand() % 1000)) / 2500 - 0.20;
			_vel[1] = ((float) (rand() % 1000)) / 2500 - 0.20;

			_rad = ((float) (rand() % 180) + 10);
			
			
			if 	(rand() % 1) 
			{
				_rgb[2] = 80; // (2, 40), (0, 40)
				_rgb[1] = 40; // (1, 20), (1, 20)
			}
			else 
			{
				_rgb[2] = 40; // (2, 20), (0, 20)
				_rgb[1] = 80; // (1, 40), (1, 40)
			}


			_rgb[0] = 20;
			

			/*
		
			if (_pos[0] > 300)
				_rgb[0] = 196;
			else if (_pos[0] > 200)
				_rgb[0] = 128;
			else if (_pos[0] > 100)
				_rgb[0] = 64;
			else
				_rgb[0] = 20;

			if (_pos[1] > 300)
				_rgb[2] = 196;
			else if (_pos[1] > 200)
				_rgb[2] = 128;
			else if (_pos[1] > 100)
				_rgb[2] = 64;
			else
				_rgb[2] = 20;

			_rgb[1] = 0;
			*/

			//cout << "Circle at (" << _pos[0] << ", " << _pos[1] << "), moving at <" << _vel[0] << ", " << _vel[1] << ">" <<
			//	    " with color " << _rgb[0] << ", " << _rgb[1] << ", " << _rgb[2] << endl;
		}

		Circle(float* pos, float* vel, float rad, int* rgb) : 
			_pos(pos), _vel(vel), _rad(rad), _rgb(rgb) {}

		~Circle()
		{
			delete[] _pos;
			delete[] _vel;
			delete[] _rgb;
		}

		int	   Update(int width, int height);
		float* Intersect(Circle* c);
		int*   get_color(Circle* c);

		float* get_pos() {return _pos;}
		float  get_rad() {return _rad;}
		int*   get_rgb() {return _rgb;}

	private:

		//Position of center of circle (x, y)
		float* _pos;

		//Velocity of circle
		float* _vel;

		//Radius of circle
		float _rad;

		//Color of circle
		int* _rgb;
};

int Circle::Update(int width, int height)
{
	_pos[0] += _vel[0];
	_pos[1] += _vel[1];

#ifdef SPARSE
	if ((_pos[0] >= width) || (_pos[0] < 0) || (_pos[1] >= height) || (_pos[1] < 0))
		return 0; 
	else
		return 1;
#else
	if ((_pos[0] >= width) || (_pos[0] < 0) || (_pos[1] >= height) || (_pos[1] < 0))
		return 1; 
	else
		return 0; 
#endif
}

//Checks intersection with given circle
float* Circle::Intersect(Circle* c)
{
	float distance = sqrt(pow(c->get_pos()[0] - _pos[0], 2) + pow(c->get_pos()[1] - _pos[1], 2));

	if (c->get_rad() + _rad < distance)
		return NULL;

	float* point = new float[2];

	point[0] = (c->get_rad() * c->get_pos()[0] + _rad * _pos[0]) / (c->get_rad() + _rad);
	point[1] = (c->get_rad() * c->get_pos()[1] + _rad * _pos[1]) / (c->get_rad() + _rad);

	//cout << "	Intersection Found" << endl;
	//cout << "		distance = " << distance << endl;
	//cout << "		combined radii = " << c->get_rad() + _rad << endl;
	//cout << "		midway point = (" << point[0] << ", " << point[1] << ")" << endl;

	return point;
}

//Averages color
int* Circle::get_color(Circle* c)
{
	int* ret_rgb = new int[3];
	int* rgb = c->get_rgb();

	for (int i = 0; i < 3; i++) 
		ret_rgb[i] = (int) ((float)rgb[i] + _rgb[i]) / 2;

	return ret_rgb;
}


#endif