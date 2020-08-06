/* 
**  Layer represents a single layer of animated content
**
**      Holds frame data for a certain number of frames
**      Enables masking operations to be performed over frame data
*/

#include <vector>

#ifndef LAYER_H
#define LAYER_H

class Layer {

    public:
        Layer(int start, int frame_num, int width, int height)
            : _start(start), _frame_num(frame_num), _end(start + frame_num), _width(width), _height(height) 
            {}

        Layer(Layer* l1, Layer* l2, int op)
        {
            //Declare variables to later store color data
            int r1, r2, g1, g2, b1, b2, a1, a2, ao;

            //Declare variables to track active layers
            int l1_active, l2_active;

            cout << "layer 1: " << l1->_start << ", " << l1->_end << endl;
            cout << "layer 2: " << l2->_start << ", " << l2->_end << endl;

            //determine bounds of new layer given those of composite layers
            _start = (l1->_start < l2->_start) ? l1->_start : l2->_start;
            _end = (l1->_end > l2->_end) ? l1->_end : l2->_end;
            _frame_num = _end - _start;
            _width = l1->_width;
            _height = l1->_height;

            cout << "Compositing layers, with start " << _start << ", duration " << _frame_num << ", and end " << _end << endl;

            //compose frame data together
            for (int i = 0; i < _frame_num; i++)
            {
                std::vector<uint8_t> frame;

                //Check if first layer is active
                if ((i >= (l1->_start - _start)) && (i < (l1->_end - _start)))
                    l1_active = 1;
                else
                    l1_active = 0;
                
                //Check if second layer is active
                if ((i >= (l2->_start - _start)) && (i < (l2->_end - _start)))
                    l2_active = 1;
                else
                    l2_active = 0;

                for (int j = 0; j < _width * _height * 4; j = j + 4)
                {

                    if (l1_active && l2_active)
                    {
                        r1 = l1->_frame_data[i - l1->_start + _start][j + 0];
                        r2 = l2->_frame_data[i - l2->_start + _start][j + 0];
                        g1 = l1->_frame_data[i - l1->_start + _start][j + 1];
                        g2 = l2->_frame_data[i - l2->_start + _start][j + 1];
                        b1 = l1->_frame_data[i - l1->_start + _start][j + 2];
                        b2 = l2->_frame_data[i - l2->_start + _start][j + 2];
                        a1 = l1->_frame_data[i - l1->_start + _start][j + 3];
                        a2 = l2->_frame_data[i - l2->_start + _start][j + 3];
                    }
                    if (l1_active && !l2_active)
                    {
                        r1 = l1->_frame_data[i - l1->_start + _start][j + 0];
                        r2 = 0;
                        g1 = l1->_frame_data[i - l1->_start + _start][j + 1];
                        g2 = 0;
                        b1 = l1->_frame_data[i - l1->_start + _start][j + 2];
                        b2 = 0;
                        a1 = l1->_frame_data[i - l1->_start + _start][j + 3];
                        a2 = 0;
                    }
                    if (!l1_active && l2_active)
                    {
                        r1 = l2->_frame_data[i - l2->_start + _start][j + 0];
                        r2 = 0;
                        g1 = l2->_frame_data[i - l2->_start + _start][j + 1];
                        g2 = 0;
                        b1 = l2->_frame_data[i - l2->_start + _start][j + 2];
                        b2 = 0;
                        a1 = l2->_frame_data[i - l2->_start + _start][j + 3];
                        a2 = 0;
                    }
                    if (!l1_active && !l2_active)
                    {
                        r1 = 0;
                        r2 = 0;
                        g1 = 0;
                        g2 = 0;
                        b1 = 0;
                        b2 = 0;
                        a1 = 0;
                        a2 = 0;
                    }

                    ao = a1 + (int) (a2 * ((float)(255 - a1) / 255));

                    frame.push_back((ao != 0) ? (uint8_t)((r1*a1 + r2*a2*((float)(255 - a1) / 255)) / ao) : 0);
                    frame.push_back((ao != 0) ? (uint8_t)((g1*a1 + g2*a2*((float)(255 - a1) / 255)) / ao) : 0);
                    frame.push_back((ao != 0) ? (uint8_t)((b1*a1 + b2*a2*((float)(255 - a1) / 255)) / ao) : 0);
                    frame.push_back((uint8_t)ao);
                }

                _frame_data.push_back(frame);

                if (l1_active && l2_active)
                    cout << "   Frame " << i << " Composited, with l1 & l2" << endl;
                if (!l1_active && l2_active)
                    cout << "   Frame " << i << " Composited, with l2" << endl;
                if (l1_active && !l2_active)
                    cout << "   Frame " << i << " Composited, with l1" << endl;
                if (!l1_active && !l2_active)
                    cout << "   Frame " << i << " Composited, with neither layer" << endl;
            }
        }

        int* get_pixel(int frame, int* pos);
        void set_pixel(int frame, int* pos, int* val);

        void set_mask(int val);
        void mask_inactive();

        int _start;
        int _frame_num;
        int _end;
        int _width;
        int _height;

        std::vector<std::vector<uint8_t>> _frame_data;
};

int* Layer::get_pixel(int frame, int* pos)
{
    int* val = new int[4];

    for (int i = 0; i < 4; i++)
        val[i] = (int)_frame_data[frame][(pos[0] * _width * 4) + pos[1] * 4 + i];

    return val;
}

void Layer::set_pixel(int frame, int* pos, int* val)
{
    for (int i = 0; i < 4; i++)
        _frame_data[frame][(pos[0] * _width * 4) + pos[1] * 4 + i] = (uint8_t)val[i];
}

void Layer::set_mask(int val)
{
    for (int i = 0; i < _frame_num; i++)
        for (int j = 0; j < _width * _height * 4; j = j + 4)
            _frame_data[i][j + 3] = (uint8_t)val;
}

#endif