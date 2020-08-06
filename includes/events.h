#include "bitmap.h"
#include "layer.h"
#include <vector>
#include <algorithm>

#ifndef EVENTS_H
#define EVENTS_H

class Event {
	public:
		Event(int start, int duration, int width, int height) 
			: _start(start), _duration(duration), _width(width), _height(height) {}

		virtual void Activate(Bitmap* b, Layer* l) = 0;

		int _start;
		int _duration;
		int _width;
		int _height;
};

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