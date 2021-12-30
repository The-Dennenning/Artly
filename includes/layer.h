/* 
**  Layer represents a single layer of animated content
**
**      Holds frame data for a certain number of frames
**      Enables masking operations to be performed over frame data
*/
#ifndef LAYER_H
#define LAYER_H

#include <vector>

#include "frame.h"


class Layer {

    public:
        Layer(int start, int frame_num, int width, int height) 
            : _start(start), _frame_num(frame_num), _end(start + frame_num), _width(width), _height(height), _id(next_id) {next_id++;}

        ~Layer() {for (auto f : _frames) delete f;}

        Layer(Layer* l)
            : _start(l->_start), _frame_num(l->_frame_num), _end(l->_start + l->_frame_num), _width(l->_width), _height(l->_height), _id(next_id)
        {
            for (auto f : l->_frames)
                _frames.push_back(new Frame(*f));

            next_id++;
        }

        Layer(Layer* l1, Layer* l2, int op)
        {
            //Declare variables to later store color data
            int r1, r2, g1, g2, b1, b2, a1, a2, ao;

            //Declare variables to track active layers
            int l1_active, l2_active;

#ifdef DEBUG
            cout << "layer " << l1->_id << ": " << l1->_start << ", " << l1->_end << endl;
            cout << "layer " << l2->_id << ": " << l2->_start << ", " << l2->_end << endl;
#endif

            //determine bounds of new layer given those of composite layers
            _start = (l1->_start < l2->_start) ? l1->_start : l2->_start;
            _end = (l1->_end > l2->_end) ? l1->_end : l2->_end;
            _frame_num = _end - _start;
            _width = l1->_width;
            _height = l1->_height;
            _id = next_id;
            next_id++;

#ifdef DEBUG
            cout << "Compositing layers, with start " << _start << ", duration " << _frame_num << ", and end " << _end << ", with operation " << op << endl;
#endif

            if ((_start == l1->_start) && ((_start + (l1->_end - l1->_start)) == l2->_start))
            {
                for (auto l : l1->_frames)
                    _frames.push_back(new Frame(*l));
                for (auto l : l2->_frames)
                    _frames.push_back(new Frame(*l));
            }

            else if ((_start == l2->_start) && ((_start + (l2->_end - l2->_start)) == l1->_start))
            {
                for (auto l : l2->_frames)
                    _frames.push_back(new Frame(*l));
                for (auto l : l1->_frames)
                    _frames.push_back(new Frame(*l));
            }

            else
            {
                //compose frame data together
                for (int i = 0; i < _frame_num; i++)
                {
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


                    if (l1_active && l2_active)
                    {
                        std::vector<uint8_t> frame_data;

                        for (int j = 0; j < _width * _height * 4; j = j + 4)
                        {
                            r1 = l1->_frames[i - l1->_start + _start]->_frame_data[j + 0];
                            r2 = l2->_frames[i - l2->_start + _start]->_frame_data[j + 0];
                            g1 = l1->_frames[i - l1->_start + _start]->_frame_data[j + 1];
                            g2 = l2->_frames[i - l2->_start + _start]->_frame_data[j + 1];
                            b1 = l1->_frames[i - l1->_start + _start]->_frame_data[j + 2];
                            b2 = l2->_frames[i - l2->_start + _start]->_frame_data[j + 2];
                            a1 = l1->_frames[i - l1->_start + _start]->_frame_data[j + 3];
                            a2 = l2->_frames[i - l2->_start + _start]->_frame_data[j + 3];

                            if (op == 0)
                            {
                                ao = a1 + (int) ((float)a2 * ((float)(255 - a1) / 255));
                                
                                /*
                                if (rand() % 10000 == 0)
                                {
                                    if (a1 != 0 || a2 != 0)
                                        cout << "ao, a1, a2 = (" << ao << ", " << a1 << ", " << a2 << ")" << endl;
                                }
                                */

                                if (ao != 0)
                                {
                                    frame_data.push_back((uint8_t)(((float)r1*a1 + r2*a2*((float)(255 - a1) / 255)) / ao));
                                    frame_data.push_back((uint8_t)(((float)g1*a1 + g2*a2*((float)(255 - a1) / 255)) / ao));
                                    frame_data.push_back((uint8_t)(((float)b1*a1 + b2*a2*((float)(255 - a1) / 255)) / ao));
                                    frame_data.push_back((uint8_t)ao);
                                }
                                else
                                {
                                    frame_data.push_back(r1);
                                    frame_data.push_back(g1);
                                    frame_data.push_back(b1);
                                    frame_data.push_back(a1);
                                }
                            }
                            else if (op == 1)
                            {
                                if (a1 >= a2)
                                {
                                    frame_data.push_back((uint8_t) r1);
                                    frame_data.push_back((uint8_t) g1);
                                    frame_data.push_back((uint8_t) b1);
                                    frame_data.push_back((uint8_t) a1);
                                }
                                else
                                {
                                    frame_data.push_back((uint8_t) r2);
                                    frame_data.push_back((uint8_t) g2);
                                    frame_data.push_back((uint8_t) b2);
                                    frame_data.push_back((uint8_t) a2);
                                }
                            }
                            else if (op == 2)
                            {
                                if ((j % (_width * 4)) >= (_width * 2))
                                {
                                    frame_data.push_back((uint8_t) r1);
                                    frame_data.push_back((uint8_t) g1);
                                    frame_data.push_back((uint8_t) b1);
                                    frame_data.push_back((uint8_t) a1);
                                }
                                else
                                {
                                    frame_data.push_back((uint8_t) r2);
                                    frame_data.push_back((uint8_t) g2);
                                    frame_data.push_back((uint8_t) b2);
                                    frame_data.push_back((uint8_t) a2);
                                }
                            }
                            
                        }
                        
                        _frames.push_back(new Frame(frame_data, _width, _height));
                    }

                    if (l1_active && !l2_active)
                        _frames.push_back(new Frame(*l1->_frames[i - l1->_start + _start]));
                    
                    if (!l1_active && l2_active)
                        _frames.push_back(new Frame(*l2->_frames[i - l2->_start + _start]));

                    if (!l1_active && !l2_active)
                        _frames.push_back(new Frame(_width, _height));

    #ifdef DEBUG
                    if (i % 10 == 0)
                    {
                        if (l1_active && l2_active)
                            cout << "   Frame " << i << " Composited, with l" << l1->_id << " & l" << l2->_id << endl;
                        if (!l1_active && l2_active)
                            cout << "   Frame " << i << " Composited, with l" << l2->_id << endl;
                        if (l1_active && !l2_active)
                            cout << "   Frame " << i << " Composited, with l" << l1->_id << endl;
                        if (!l1_active && !l2_active)
                            cout << "   Frame " << i << " Composited, with neither layer" << endl;
                    }
    #endif
                }
            }
        }

        void set_mask(int val);

        void fade_in(int start, int duration);
        void fade_in(int start, int duration, double scale);

        void fade_out(int start, int duration);

        void scan(int start, int dur, double dx, double dy);
        
        void glitch(int start, int dur, int degree);

        void shift_color(int start, int dur, double degree);

        int _start;
        int _frame_num;
        int _end;
        int _width;
        int _height;

        int _id;
        static int next_id;
        
        vector<Frame*> _frames;
};

int Layer::next_id = 0;

void Layer::set_mask(int val)
{
    for (int i = 0; i < _frame_num; i++)
        for (int j = 0; j < _width * _height * 4; j = j + 4)
            _frames[i]->_frame_data[j + 3] = (uint8_t)val;
}

void Layer::fade_in(int start, int duration)
{
    double step = ((double) 255 / duration);
    double sum = 0;

    for (int i = start; i < start + duration; i++)
    {
        if (i >= _end) return;

        for (int j = 0; j < _width * _height * 4; j = j + 4)
            _frames[i]->_frame_data[j + 3] = (uint8_t) sum;

        sum += step;

        if (sum > 255) sum = 255;
    }
}

void Layer::fade_in(int start, int duration, double scale)
{
    double step = ((double) 255 / duration) * scale;
    double sum = 0;

    int ub;

    if (start + duration >= _frame_num)
        ub = _frame_num;
    else
        ub = start + duration;

    for (int i = start; i < ub; i++)
    {
        for (int j = 0; j < _width * _height * 4; j = j + 4)
            _frames[i]->_frame_data[j + 3] = (uint8_t) sum;

        sum += step;

        if (sum > 255) sum = 255;
    }
}

void Layer::fade_out(int start, int duration)
{
    double step = ((double) 255 / duration);
    double sum = 255;

    int ub;

    if (start + duration >= _frame_num)
        ub = _frame_num;
    else
        ub = start + duration;

    for (int i = start; i < ub; i++)
    {        
        for (int j = 0; j < _width * _height * 4; j = j + 4)
            _frames[i]->_frame_data[j + 3] = (uint8_t) sum;

        sum -= step;
        
        if (sum < 0) sum = 0;
    }
}

void Layer::scan(int start, int dur, double dx, double dy)
{
    int ub;

    if (start + dur >= _frame_num)
        ub = _frame_num;
    else
        ub = start + dur;

    for (int n = start; n < ub; n++)
    {
        int _dx = (n - start) * dx;
        int _dy = (n - start) * dy;

        _frames[n]->_offset(_dx, _dy);
    }
}

void Layer::glitch(int start, int dur, int degree)
{
    int ub;

    if (start + dur >= _frame_num)
        ub = _frame_num;
    else
        ub = start + dur;

    for (int n = start; n < ub; n++)
    {
        _frames[n]->_glitch((n - start) * degree);
    }
}

void Layer::shift_color(int start, int dur, double degree)
{
    int ub;

    if (start + dur >= _frame_num)
        ub = _frame_num;
    else
        ub = start + dur;

    for (int n = start; n < ub; n++)
    {
        _frames[n]->_color_shift(((double) n) * degree);
    }
}

#endif