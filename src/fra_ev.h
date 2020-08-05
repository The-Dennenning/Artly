#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string.h>
#include "bitmap.h"

using namespace std;

#ifndef _EVENT_HPP
#define _EVENT_HPP


// Computes n choose s, efficiently
double Binomial(int n, int s)
{
    double        res;

    res = 1;
    for (int i = 1 ; i <= s ; i++)
        res = (n - i + 1) * res / i ;

    return res;
}// Binomial


class Event {
	public:
		Event(Bitmap *b, int start, int duration) : _b(b), _start(start), _duration(duration) {}

		virtual void Activate(int frame_num) = 0;

	protected:
		Bitmap* _b; 
		int _start;
		int _duration;
};


class Fractal : public Event
{
	public:

		Fractal();
		Fractal(Bitmap *b, int start, int duration, int width, int height, int scaleFactor) :
			Event(b, start, duration), _width(width), _height(height), _scaleFactor(scaleFactor) {}

		~Fractal();

	private:

		int _width;
		int _height;
		int _scaleFactor;

		void Activate(int frame_num);
		int* getColor(int i, int j, int n);
		int evalFractal(float re, float im, int n);
};

void Fractal::Activate(int frame_num)
{
	//Activate if frame number is after start but before end of duration
	if (frame_num < _start || frame_num >= _start + _duration)
		return;

	for (int i = 0; i < _width; i++)
		for (int j = 0; j < _height; j++)
			_b -> getPixel(i, j).setRGB(getColor(i, j, frame_num));
}

//
int* Fractal::getColor(int i, int j, int n)
{
	int* ret_color = new int[3];
	int* div_color = new int[3];
	int* con_color = new int[3];

	//Sets base convergent color
	con_color[0] = 32;
	con_color[1] = 64;
	con_color[2] = 0;

	//Sets base divergent color
	div_color[0] = 200;
	div_color[1] = 64;
	div_color[2] = 0;

	//Transform row and col coordinates
	//	to float coordiates over box [-2, -1] x [1, 1]
	float re = ((float) i / _width) * 4 - 2;
	float im = ((float) j / _height) * 2.5 - 1.25;

	//Returns how many iterations it takes to converge
	//	Or returns 0 if diverges
	int eval = evalFractal(re, im, n);

	//Assigns color value given eval
	//	Divergent Case
	if (eval > 1) {	
		ret_color[0] = div_color[0] / (eval / 2);
		ret_color[1] = div_color[1] / (eval / 2);
	//	Convergent Case
	} else {			
		ret_color[0] = con_color[0];
		ret_color[1] = con_color[1];
	}

	ret_color[2] = 0;
	
	//cout << "return color (" << i << ", " << j << ") = " << ret_color[0] 
	//     << ", " << ret_color[1] << " – Eval = " << eval << endl;

	delete[] div_color;
	delete[] con_color;

	return ret_color;
}

//Evaluates whether mandlebrot set converges or diverges
//	And returns number of iterations taken
int Fractal::evalFractal(float c_re, float c_im, int n)
{
	float z_re = 0, z_im = 0;
	int i = 0;
	bool diverge = false;
	bool converge = false;

	while (!diverge && !converge)
	{
		/* Multiplying Complex Numbers...
			(a + bi) * (c + di) + (e + fi)
			real = a*c - b*d + e
			imag = a*d + b*c + f 
		*/

		z_re = (z_re * z_re) - (z_im * z_im) + c_re;
		z_im = (z_re * z_im) + (z_re * z_im) + c_im;
		i++;

		if (z_re <= -2 || z_re >= 2 || z_im <= -2 || z_im >= 2)
			diverge = true;

		if (i >= n)
			converge = true;
	}

	if (diverge) return i;

	return 0;
}

#endif