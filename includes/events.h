
#include "layer.h"
#include "frame.h"
#include "strokes.h"

#ifndef EVENTS_H
#define EVENTS_H


#include <vector>
#include <algorithm>


class Event {
	public:
		Event(int start, int duration, int width, int height) 
			: _start(start), _duration(duration), _width(width), _height(height) {}

		virtual void Activate(Frame* f, Layer* l) = 0;

		int _start;
		int _duration;
		int _width;
		int _height;
};

	#ifndef BIN
	#define BIN
	// Computes n choose s, efficiently
	double Binomial(int n, int s)
	{
		double res;

		res = 1;
		for (int i = 1 ; i <= s ; i++)
			res = (n - i + 1) * res / i ;

		return res;
	}// Binomial
	#endif

#endif