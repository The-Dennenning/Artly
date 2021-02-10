#ifndef FRAME_H
#define FRAME_H

#include "bitmap.h"


class Frame
{
    public:
        Frame();
        Frame(const Frame &f) 
            : _frame_data(f._frame_data), _width(f._width), _height(f._height) {}

        Frame(vector<uint8_t> frame_data, int width, int height)
            : _frame_data(frame_data), _width(width), _height(height) {}

        Frame(int width, int height)
            : _frame_data(width*height*4, 0), _width(width), _height(height) {}

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
        ~Frame() {}

        void get(int i, int j, int* rgba)
        {
            rgba[0] = _frame_data[i * _height * 4 + j * 4 + 0];
            rgba[1] = _frame_data[i * _height * 4 + j * 4 + 1];
            rgba[2] = _frame_data[i * _height * 4 + j * 4 + 2];
            rgba[3] = _frame_data[i * _height * 4 + j * 4 + 3];
        }

        void set(int i, int j, int* rgba)
        {
            _frame_data[i * _height * 4 + j * 4 + 0] = (uint8_t)rgba[0];
            _frame_data[i * _height * 4 + j * 4 + 1] = (uint8_t)rgba[1];
            _frame_data[i * _height * 4 + j * 4 + 2] = (uint8_t)rgba[2];
            _frame_data[i * _height * 4 + j * 4 + 3] = (uint8_t)rgba[3];
        }

        void set_mask(int val)
        {
            for (int i = 0; i < _width * _height * 4; i = i + 4)
                _frame_data[i + 3] = (uint8_t)val;
        }

        void _clear()
        {
            for (int i = 0; i < _width * _height * 4; i = i + 4)
            {
                _frame_data[i + 0] = 0;
                _frame_data[i + 1] = 0;
                _frame_data[i + 2] = 0;
            }
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

#endif