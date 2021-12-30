#ifndef STROKES_H
#define STROKES_H

#include "frame.h"

class Strokes
{
    public:
        Strokes(Frame* f) : _f(f) {}
        Strokes() {}

        Frame* _f;

        virtual void Activate() = 0;
        virtual void Activate(Frame* f) = 0;
};



class pen_dot : public Strokes{ // Data structure for holding painterly strokes.

	public:
		pen_dot(Frame* f, int radius, int x, int y, int r, int g, int b, int a) 
			: Strokes(f), _radius(radius), _x(x), _y(y), _r(r), _g(g), _b(b), _a(a) 
            {
                Activate();
            }
            
		pen_dot(int radius, int x, int y, int r, int g, int b, int a) 
			: Strokes(), _radius(radius), _x(x), _y(y), _r(r), _g(g), _b(b), _a(a) {}
   
        // data
        int _radius, _x, _y;	// Location for the stroke
        int _r, _g, _b, _a;		// Color

        void Activate() 
        {      
            int radius_squared = _radius * _radius;
            
            for (int x_off = -(_radius); x_off <= _radius; x_off++) {
                for (int y_off = -(_radius); y_off <= _radius; y_off++) {
                    int x_loc = _x + x_off;
                    int y_loc = _y + y_off;
                    // are we inside the circle, and inside the image?
                    if ((x_loc >= 0 && x_loc < _f->_height && y_loc >= 0 && y_loc < _f->_width)) {
                        int dist_squared = x_off * x_off + y_off * y_off;
                        if (dist_squared <= radius_squared) {

                            int rgb[] = {0, 0, 0, 0};

                            _f->get(y_loc, x_loc, rgb);

                            rgb[0] = (int)((float)rgb[0] * _a + _r * (255 - _a)) / (2 * 255);
                            rgb[1] = (int)((float)rgb[1] * _a + _g * (255 - _a)) / (2 * 255);
                            rgb[2] = (int)((float)rgb[2] * _a + _b * (255 - _a)) / (2 * 255);
                            rgb[3] = _a;
                            
                            _f->set(y_loc, x_loc, rgb);
                        }
                    }
                }
            }
        }

        void Activate(Frame* f) 
        {      
            _f = f;
            Activate();
        }
};



class pen_line : public Strokes{ // Data structure for holding painterly strokes.

	public:
        //constructor that takes two points (x, y) and (x_t, y_t) and draws line between them with t strokes
		pen_line(Frame* f, double radius, double x, double y, int r, int g, int b, int a, double x_t, double y_t, int t) 
			: Strokes(f), _radius(radius), _x(x), _y(y), _r(r), _g(g), _b(b), _a(a), _t(t) 
        {
            _dx = (x_t - x) / ((double)t);
            _dy = (y_t - y) / ((double)t);
            Activate();
        }

        /*
        //constructor that takes a point (x, y) and draws t strokes each stroke changing point by dx and dy
        //  and activates immediately upon creation onto given frame
		pen_line(Frame* f, double radius, int x, int y, int r, int g, int b, int a, double dx, double dy, int t) 
			: Stroke(f), _radius(radius), _x(x), _y(y), _r(r), _g(g), _b(b), _a(a), _dx(dx), _dy(dy), _t(t) 
        {
            Activate();
        }
        */

		pen_line(double radius, int x, int y, int r, int g, int b, int a, double dx, double dy, int t) 
			: Strokes(), _radius(radius), _x(x), _y(y), _r(r), _g(g), _b(b), _a(a), _dx(dx), _dy(dy), _t(t) {}
   
        // data
        double _radius;         // Radius of stroke
        double _dx, _dy;        // vector giving stroke direction
        int _t;                 // Parameterized distance of stroke
        double _x, _y;	        // Location for the start of stroke
        int _r, _g, _b, _a;		// Color

        void Activate() 
        {   
            int radius_squared = _radius * _radius;

            for (int t = 0; t < _t; t++)
            {
                for (int x_off = -(_radius); x_off <= _radius; x_off++) {
                    for (int y_off = -(_radius); y_off <= _radius; y_off++) {
                        double x_loc = _x + x_off;
                        double y_loc = _y + y_off;
                        // are we inside the circle, and inside the image?
                        if ((x_loc >= 0 && x_loc < _f->_width && y_loc >= 0 && y_loc < _f->_height)) {
                            int dist_squared = x_off * x_off + y_off * y_off;
                            if (dist_squared <= radius_squared) {

                                int rgb[] = {0, 0, 0, 255};

                                _f->get((int) x_loc, (int) y_loc, rgb);
                                
                                rgb[0] = (int)((float)rgb[0] * (1 - ((float)_a) / 255) + _r * ((float)_a / 255)) % 255;
                                rgb[1] = (int)((float)rgb[1] * (1 - ((float)_a) / 255) + _g * ((float)_a / 255)) % 255;
                                rgb[2] = (int)((float)rgb[2] * (1 - ((float)_a) / 255) + _b * ((float)_a / 255)) % 255;
                                rgb[3] = 255;

                                /*
                                rgb[0] = (int)((float)rgb[0] * (1 - _a / 255) + _r * ((float)_a / 255)) % 255;
                                rgb[1] = (int)((float)rgb[1] * (1 - _g / 255) + _g * ((float)_a / 255)) % 255;
                                rgb[2] = (int)((float)rgb[2] * (1 - _b / 255) + _b * ((float)_a / 255)) % 255;
                                rgb[3] = 255;
                                */

                                /*  glitch stroke
                                rgb[0] = (int)((float)rgb[0] * _a + _r * (255 - _a)) / (2 * 255);
                                rgb[1] = (int)((float)rgb[1] * _a + _g * (255 - _a)) / (2 * 255);
                                rgb[2] = (int)((float)rgb[2] * _a + _b * (255 - _a)) / (2 * 255);
                                */
                                
                                _f->set((int) x_loc, (int) y_loc, rgb);
                            }
                        }
                    }
                }

                _x = _x + _dx;
                _y = _y + _dy;
            }
        }

        void Activate(Frame* f) 
        { 
            _f = f;
            Activate();
        }
};


#endif