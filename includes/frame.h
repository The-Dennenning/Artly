#include "bitmap.h"

#ifndef FRAME_H
#define FRAME_H

class Frame
{
    public:
        Frame();
        Frame(const Frame &f) 
            : _frame_data(f._frame_data), _meta_data(f._meta_data), _width(f._width), _height(f._height) {}

        Frame(vector<uint8_t> frame_data, int width, int height)
            : _frame_data(frame_data), _width(width), _height(height) {}
            
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
                    _frame_data.push_back(temp_data[j * _width * 4 + i * 4 + 2]);
                    _frame_data.push_back(temp_data[j * _width * 4 + i * 4 + 1]);
                    _frame_data.push_back(temp_data[j * _width * 4 + i * 4 + 0]);
                    _frame_data.push_back(temp_data[j * _width * 4 + i * 4 + 3]);
                }
            }

        }
        ~Frame() {}

        //Stores pixel RGBA data as array
        //  indexed by [i * _height + j]
        //  i, j iterated by _width, _height respectively
        vector<uint8_t> _frame_data;

        //Stores sets of pixel meta data
        //  indexed by meta data set,
        //  then indexed by pixel
        vector<vector<vector<int>>> _meta_data;

        int _width;
        int _height;
};

#endif