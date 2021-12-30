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

#include "bitmap.h"

#ifndef FRAME_H
#define FRAME_H


class Frame
{
    public:
        Frame();
        
        Frame(const Frame &f) 
            : _frame_data(f._frame_data), _width(f._width), _height(f._height) {}

        Frame(vector<uint8_t> frame_data, int width, int height)
            : _frame_data(frame_data), _width(width), _height(height) {}

        Frame(int width, int height)
            : _frame_data(width*height*4, 255), _width(width), _height(height) {}

        Frame(Bitmap &b)
            : _width(b.getSize(0)), _height(b.getSize(1))
        {
            vector<uint8_t> temp_data;

            for (int i = 0; i < _width; i++) 
            {
                for (int j = 0; j < _height; j++)
                {
                    int* rgb = b.getPixel(i, j).getRGB();

                    for (int k = 0; k < 4; k++)
                        temp_data.push_back((uint8_t) rgb[k]);
                }		
            }
                
            for (int i = 0; i < _width; i++)
            {
                for (int j = 0; j < _height; j++)
                {
                    //Color is flipped around on purpose
                    //  to account for bug in bitmap reading library
                    _frame_data.push_back(temp_data[j * _width * 4 + i * 4 + 2]);
                    _frame_data.push_back(temp_data[j * _width * 4 + i * 4 + 1]);
                    _frame_data.push_back(temp_data[j * _width * 4 + i * 4 + 0]);
                    _frame_data.push_back(temp_data[j * _width * 4 + i * 4 + 3]);
                }
            }

        }

        Frame(Frame* f, int width, int height)
            : _width(width), _height(height)
        {
            double ratio_w = ((double) f->_width) / ((double) _width);
            double ratio_h = ((double) f->_height) / ((double) _height);

            for (int i = 0; i < _width; i++)
            {
                for (int j = 0; j < _height; j++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        _frame_data.push_back(f->_frame_data[((int)(i * ratio_w)) * f->_height * 4 + ((int)(j * ratio_h)) * 4 + k]);
                    }
                }
            }
        }

        Frame(Frame* f, int width, int height, int w_off, int h_off)
            : _width(width), _height(height)
        {
            for (int i = 0; i < _width; i++)
            {
                for (int j = 0; j < _height; j++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        _frame_data.push_back(f->_frame_data[((i + w_off) % f->_width) * f->_height * 4 + ((j + h_off) % f->_height) * 4 + k]);
                    }
                }
            }
        }

        Frame(Frame* f, int h_off)
            : _width(f->_width), _height(f->_height)
        {
            for (int i = 0; i < _width; i++)
            {
                for (int j = 0; j < _height; j++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        if (i <= h_off)
                            _frame_data.push_back(f->_frame_data[i * _height * 4 + j * 4 + k]);
                        else
                            _frame_data.push_back(0);
                    }
                }
            }
        }

        ~Frame() {}

        void get(int i, int j, int* rgba)
        {
            rgba[0] = _frame_data[i * _height * 4 + j * 4 + 0];
            rgba[1] = _frame_data[i * _height * 4 + j * 4 + 1];
            rgba[2] = _frame_data[i * _height * 4 + j * 4 + 2];
            rgba[3] = _frame_data[i * _height * 4 + j * 4 + 3];
        }

        vector<int> get(int i, int j)
        {
            vector<int> rgb;
            rgb.push_back(_frame_data[i * _height * 4 + j * 4 + 0]);
            rgb.push_back(_frame_data[i * _height * 4 + j * 4 + 1]);
            rgb.push_back(_frame_data[i * _height * 4 + j * 4 + 2]);
            return rgb;
        }

        void set(int i, int j, int* rgba)
        {
            _frame_data[i * _height * 4 + j * 4 + 0] = (uint8_t)rgba[0];
            _frame_data[i * _height * 4 + j * 4 + 1] = (uint8_t)rgba[1];
            _frame_data[i * _height * 4 + j * 4 + 2] = (uint8_t)rgba[2];
            _frame_data[i * _height * 4 + j * 4 + 3] = (uint8_t)rgba[3];
        }

        void set(int i, int j, vector<int> rgb)
        {
            _frame_data[i * _height * 4 + j * 4 + 0] = (uint8_t)rgb[0];
            _frame_data[i * _height * 4 + j * 4 + 1] = (uint8_t)rgb[1];
            _frame_data[i * _height * 4 + j * 4 + 2] = (uint8_t)rgb[2];
            _frame_data[i * _height * 4 + j * 4 + 3] = 255;
        }

        void set_mask(int val)
        {
            for (int i = 0; i < _width * _height * 4; i = i + 4)
                _frame_data[i + 3] = (uint8_t)val;
        }

        void set_image(Frame* f, int b_width, int b_height)
        {
            for (int i = b_width; i < b_width + f->_width; i++)
            {
                for (int j = b_height; j < b_height + f->_height; j++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        _frame_data[i * _height * 4 + j * 4 + k] = f->_frame_data[(i - b_width) * f->_height * 4 + (j - b_height) * 4 + k];
                    }
                }
            }
        }

        void _flip()
        {
            vector<uint8_t> flip_data;

            for (int i = _width - 1; i >= 0; i--)
            {
                for (int j = _height - 1; j >= 0; j--)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        flip_data.push_back(_frame_data[i * _height * 4 + j * 4 + k]);
                    }
                }
            }

            _frame_data.clear();
            _frame_data.assign(flip_data.begin(), flip_data.end());
        }

        void _clear()
        {
            for (int i = 0; i < _width * _height * 4; i = i + 4)
            {
                _frame_data[i + 0] = 0;
                _frame_data[i + 1] = 0;
                _frame_data[i + 2] = 0;
                _frame_data[i + 2] = 0;
            }
        }

        void _clear_w()
        {
            for (int i = 0; i < _width * _height * 4; i = i + 4)
            {
                _frame_data[i + 0] = 255;
                _frame_data[i + 1] = 255;
                _frame_data[i + 2] = 255;
                _frame_data[i + 3] = 0;
            }
        }

        void _greyscale()
        {
            for (int i = 0; i < _width * _height * 4; i = i + 4)
            {
                double sum = 0;
                sum += ( (double) _frame_data[i + 0]) * 0.2126;
                sum += ( (double) _frame_data[i + 1]) * 0.7152;
                sum += ( (double) _frame_data[i + 2]) * 0.0722;
                
                _frame_data[i + 0] = sum;
                _frame_data[i + 1] = sum;
                _frame_data[i + 2] = sum;
                _frame_data[i + 3] = 0;
            }
        }

        void _negate()
        {
            for (int i = 0; i < _width * _height * 4; i = i + 4)
            {
                _frame_data[i + 0] = 255 - _frame_data[i + 0];
                _frame_data[i + 1] = 255 - _frame_data[i + 1];
                _frame_data[i + 2] = 255 - _frame_data[i + 2];
            }
        }

        void _noise_color(int degree)
        {
            for (int i = 0; i < _width * _height * 4; i = i + 4)
            {
                if((rand() % degree) == 0)
                {
                    int r = 
                    _frame_data[i + 0] = rand() % 255;
                    _frame_data[i + 1] = rand() % 255;
                    _frame_data[i + 2] = rand() % 255;
                }
            }   
        }

        void _offset(int dx, int dy)
        {
            vector<uint8_t> offset;

            for (int i = 0; i < _width; i++)
            {
                for (int j = 0; j < _height; j++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        offset.push_back(_frame_data[abs((i + dx) % _width) * _height * 4 + abs((j + dy) % _height) * 4 + k]);
                    }
                }
            }

            _frame_data.clear();
            _frame_data.assign(offset.begin(), offset.end());
        }
        
        void _glitch(int degree)
        {
            vector<uint8_t> glitch;

            for (int i = 0; i < _width; i++) 
            {
                for (int j = 0; j < _height; j++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        glitch.push_back(_frame_data[((i + degree * j) % _width) * _height * 4 + j * 4 + k]);
                    }
                }
            }

            _frame_data.clear();
            _frame_data.assign(glitch.begin(), glitch.end());
        }

        void _color_vaporwave()
        {
            for (int i = 0; i < _width; i++) 
            {
                for (int j = 0; j < _height; j++)
                {
                    int temp[3] = {0,0,0};

                    temp[0] = _frame_data[(i * _height + j) * 4 + 0];
                    temp[1] = _frame_data[(i * _height + j) * 4 + 1];
                    temp[2] = _frame_data[(i * _height + j) * 4 + 2];

                    _frame_data[(i * _height + j) * 4 + 0] = 255 - temp[2];
                    _frame_data[(i * _height + j) * 4 + 1] = 255 - temp[1];
                    _frame_data[(i * _height + j) * 4 + 2] = 255 - temp[0];
                }
            }
        }

        void _color_shift(double c)
        {
            for (int i = 0; i < _width; i++) 
            {
                for (int j = 0; j < _height; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        _frame_data[(i * _height + j) * 4 + k] += (int) c;
                        _frame_data[(i * _height + j) * 4 + k] %= 255;
                    }
                }
            }
        }

        void _blur(int _radius)
        {
            int N = 2 * _radius + 1;
            double *Gaussian = new double[N];
            double sum[3];
            double total;

            int*** temp = new int**[_width];
            for (int i = 0; i < _width; i++)
            {
                temp[i] = new int*[_height];
                for (int j = 0; j < _height; j++)
                {
                    temp[i][j] = new int[3];
                }
            }

            //Calculates 1D Gaussian
            for (int k = 0; k < N; k++)
                Gaussian[k] = Binomial(N, k);

            //Loop through image
            for (int i = 0; i < _width; i++)
            {
                for (int j = 0; j < _height; j++)
                {
                    //Zero out summing variable
                    for (int k = 0; k < 3; k++)
                        sum[k] = 0;

                    total = 0;

                    int lb = -(N / 2);
                    int ub = (N / 2) + (N % 2);

                    //Computes value of filter over pixel (i, j) given sized filter N
                    for (int g_i = lb; g_i < ub; g_i++)
                    {
                        for (int g_j = lb; g_j < ub; g_j++)
                        {
                            //Skips values if out of bounds
                            if (i + g_i >= 0 && i + g_i < _width)
                            {
                                if (j + g_j >= 0 && j + g_j < _height)
                                {
                                    //Multiplies two 1-D gaussian filter terms together
                                    //	Simulating 2-D gaussian filter
                                    for (int k = 0; k < 3; k++)
                                        sum[k] += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)] * ((int) _frame_data[(i * _height + j) * 4 + k]);

                                    total += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)];
                                }
                            }
                        }
                    }

                    //Assigns value to temporary holding
                    //	So as to not mess with other filter calculations
                    for (int k = 0; k < 3; k++)
                        temp[i][j][k] = (int) (sum[k] / total); //weighted average
                }
            }

            //Empties and Deletes temporary holding
            
            for (int i = 0; i < _width; i++)
            {
                for (int j = 0; j < _height; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        _frame_data[(i * _height + j) * 4 + k] = (uint8_t) temp[i][j][k];
                    }

                    delete[] temp[i][j];
                }
                delete[] temp[i];
            }

            delete[] temp;
        }
        
        vector<uint8_t> flip()
        {
            vector<uint8_t> flip;

            for (int j = 0; j < _height; j++)
            {
                for (int i = 0; i < _width; i++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        flip.push_back(_frame_data[i * _height * 4 + (j * 4) + k]);
                    }
                    
                }

            }
            
            return flip;
        }

        //Stores pixel RGBA data as array
        //  indexed by [i * _height + j]
        //  i, j iterated by _width, _height respectively
        vector<uint8_t> _frame_data;

        int _width;
        int _height;
};

Frame* image_flip(Frame* f)
{
    Frame* temp = new Frame(*f);

    for (int i = 0; i < f->_width; i++)
    {
        for (int j = 0; j < f->_height; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                f->_frame_data[i * f->_height * 4 + (((j + (f->_height / 2)) % f->_height) * 4) + k] = temp->_frame_data[i * f->_height * 4 + (j * 4) + k];
            }
        }
    }

    delete temp;
    return f;
}

/*
class KFrame : public Frame
{   
        KFrame(const Frame &f, int k) 
            : Frame(f), _k(k) 
        {
            Kmeans *km = new Kmeans(this, k);
            _kdata = km->get_data();
        }

    //number of kmeans clusters
    int _k;

    //kmean cluster data
    Kdata* _kdata;
};
*/



#endif