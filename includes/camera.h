#ifndef CAMERA_H
#define CAMERA_H

#include "frame.h"
#include "space.h"
#include "events.h"

#define PI 3.1415926


class Camera : public Event
{
    public:
        Camera(int start, int dur, int width, int height, int type) :
            Event(start, dur, width, height), _type(type) 
            {
                _delta_c[0] = 0;
                _delta_c[1] = 0;

                _angle[0] = 0;
                _angle[1] = 0;
                _angle[2] = -1;
                _angle[3] = 0;
                _angle[4] = 1;
                _angle[5] = 0;
                _angle[6] = 1;
                _angle[7] = 0;
                _angle[8] = 0;
            }

        vector<Face*> _fs;

        //Space that this camera samples from when activated
        Space* _s;

        //different types to select different movement scripts
        int _type;

        //position of BOTTOM LEFT corner of camera in 3-space
        double _coord[3], _angle[9];
        //  _coord[0, 1, 2] = x, y, z
        //      is vector from origin to camera location
        //  
        //  _angle[0, 1, 2]
        //      is unit vector f, describing which way the front of camera is looking
        //
        //  _angle[3, 4, 5]
        //      is unit vector t, describing which way the top of the camera is facing
        //
        //  _angle[6, 7, 8]
        //      is unit vector r, describing which way the right of the camera is facing
        //
        //      camera 'lense' is face created by t and r
        //
        //  _angle = 0, 1, 0 | 0, 0, 1 | 1, 0, 0
        //      DEFAULT VIEW --> camera looking out along +y axis, top facing up along +z axis
        //
        //  _angle = 0, 0, -1| 0, 1, 0 | 1, 0, 0
        //      LOOKING DOWN --> camera looking down along -z axis, top facing out along +y axis

        //change in position/direction of camera in 3-space
        double _delta_c[3], _delta_a[9];

        //height and width of rectangular camera lense in 3-space
        double _real_height, _real_width;
    

        //event activation function
        //  samples into layer in renderer
        void Activate(Frame* f, Layer* l)
        {
            for (int n = 0; n < l->_frame_num; n++)
            {
                cout << "Camera Sampling frame " << n << endl;
                l->_frames.push_back(sample());
                update(n);
            }
        }

        //updates camera coordinates and angle with given delta
        void update(double n)
        {
            _s->_t = n;
            
            if (n == 100)
            {
                _s->_objects.clear();
                _s->_objects.push_back(_fs[2]);
                _s->_objects.push_back(_fs[3]);
                _s->_objects.push_back(_fs[4]);
            }
            else if (n == 260)
            {
                _s->_objects.clear();
                _s->_objects.push_back(_fs[0]);
                _s->_objects.push_back(_fs[1]);
                _s->_objects.push_back(_fs[2]);
                _s->_objects.push_back(_fs[3]);
                _s->_objects.push_back(_fs[4]);
            }

            /*
            _angle[1] += sin(n * 3 / 180);
            _angle[2] += sin(n * 3 / 180);

            _angle[1] = pow(pow(_angle[1], 2) + pow(_angle[2], 2), 0.5);
            _angle[2] = pow(pow(_angle[1], 2) + pow(_angle[2], 2), 0.5);

            _angle[4] -= sin(n * 3 / 180);
            _angle[5] += sin(n * 3 / 180);
            
            _angle[4] = pow(pow(_angle[4], 2) + pow(_angle[5], 2), 0.5);
            _angle[5] = pow(pow(_angle[4], 2) + pow(_angle[5], 2), 0.5);
            */
            /*
            _coord[0] += sin(n / 180) * 5;
            _coord[1] += cos(n / 180) * 5;
            */

            /*
            _coord[1] -= 0.5;
            _coord[0] -= 0.2;

            _real_height += 0.1;
            _real_width += 0.1;
            */

            /*
            */
/*
            if (_type == 1)
            {
                if (((int) n) % 100 == 0)
                {
                    _delta_c[0] = ((double) (rand() % 20) / 10) - 1;
                    _delta_c[1] = ((double) (rand() % 20) / 10) - 1;

                    int z = rand() % 200 + 50;
                    _real_height = z;
                    _real_width = z;

                    int d = rand() % 360;
                    
                    _angle[3] = cos((d + 90) * PI / 180);
                    _angle[4] = sin((d + 90) * PI / 180);

                    _angle[6] = cos(d * PI / 180);
                    _angle[7] = sin(d * PI / 180);
                
                    _coord[0] = rand() % 1000 - 500;
                    _coord[1] = rand() % 1000 - 500;
                }

                _coord[0] += _delta_c[0];
                _coord[1] += _delta_c[1];

                if (((int) n) % 2 == 0)
                    _s->_t++;
            }

            if (_type == 2)
            {
                if (((int) n) % 250 == 0)
                {
                    _delta_c[0] = ((double) (rand() % 20) / 10) - 1;
                    _delta_c[1] = ((double) (rand() % 20) / 10) - 1;

                    int z = rand() % 200 + 200;
                    _real_height = z;
                    _real_width = z;

                    int d = rand() % 360;
                    
                    _angle[3] = cos((d + 90) * PI / 180);
                    _angle[4] = sin((d + 90) * PI / 180);

                    _angle[6] = cos(d * PI / 180);
                    _angle[7] = sin(d * PI / 180);
                
                    _coord[0] = rand() % 500 - 250;
                    _coord[1] = rand() % 500 - 333;
                }

                _coord[0] += _delta_c[0];
                _coord[1] += _delta_c[1];

                _s->_t++;
            }
            */

           _real_height = ((double) 200) / pow(10, n / 150);
           _real_width = ((double) 200) / pow(10, n / 150);

           _coord[0] = 100 + ((double) -100) / pow(10, n / 150);
           _coord[1] = 100 + ((double) -100) / pow(10, n / 150);
        }
        
        //Samples the given space for a frame of given dimension
        Frame* sample()
        {
            Frame* f = new Frame(_width, _height);
            Ray* r = new Ray();
                r->_width = _real_width / _width;
                r->_height = _real_height / _height;
                //sets ray angle to camera angle
                for (int k = 0; k < 3; k++)
                    r->_angle[k] = _angle[k];

            //pixel coords(0, 0) = real coords (0, 0)
            //  both are at bottom left of image
            //  pixel coord corresponds to bottom left of pixel
            for (int i = 0; i < _width; i++) 
            {
                for (int j = 0; j < _height; j++)
                {
                    //calculate pixel vector 
                    //  i.e. real distance from camera coord to (i, j) coord
                    for (int k = 0; k < 3; k++)
                    {
                        //initializes ray coords with camera coords
                        r->_coord[k] = _coord[k];

                        //add r vector scaled by j
                        //  i.e. adds vector to get from camera coords to jth row
                        r->_coord[k] += j * _angle[k + 6] * r->_width;

                        //add t vector scaled by i
                        //  i.e. adds vector to get from jth row to (i, j) pixel
                        r->_coord[k] += i * _angle[k + 3] * r->_height;
                    }

#ifdef DEBUG
                    cout << "Intersecting from pixel (" << i << ", " << j << ")" << endl;
#endif
                    //gives ray to space, where the actual intersection and sampling happens
                    //  intersect returns RGB value, which is then set in frame
                    r->rgb.clear();
                    _s->intersect(r);
                    f->set(i, j, r->rgb);

                    /*
                    if (r->rgb[0] != 0)
                        cout << "   pixel set to (" << r->rgb[0] << ", " << r->rgb[1] << ", " << r->rgb[2] << ")" << endl;
                    */

                }
            }

            return f;
        }
};

#endif