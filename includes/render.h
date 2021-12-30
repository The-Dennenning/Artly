/*
**  Render combines events, bitmaps, and layers into a single package
**  representing the final 'render' from a usage of Artly.
**
**  
**
*/

#include "events.h"
#include "layer.h"
#include "frame.h"
#include "gif.h"

#ifndef RENDER_H
#define RENDER_H

class Render {

    public:
        Render() : _scale(1) {}
        Render(int scale) : _scale(scale), _delay(10), _op(0) {}
        Render(int scale, int delay) : _scale(scale), _delay(delay), _op(0) {}
        Render(int scale, int delay, int op) : _scale(scale), _delay(delay), _op(op) {}

        ~Render() {for (auto l : _layers) delete l;}

        int _scale;
        int _delay;
        int _op;

        //Generates layer
        //  using seed frame and event
        int Add_Layer(Frame* f, Event* e)
        { 
            //Create layer 
            //  at start of event, and for duration of event,
            //  with width and height of provided frame
            Layer* l = new Layer(e->_start, e->_duration, e->_width, e->_height);

#ifdef DEBUG
            cout << "Layer " << l->_id << " created, with start " << e->_start << " and duration " << e->_duration << endl;
#endif

            //Activates event
            //  using seed frame f to generate frame data in layer l
            e->Activate(f, l);

            delete e;

            //Stores new layer
            //  in vector _layers,
            //  with index _layers.size() - 1
            _layers.push_back(l);

            //Returns index of new layer
            return l->_id;
        }

        int get_index(int id)
        {
            for (int i = 0; i < _layers.size(); i++)
            {
                if (_layers[i]->_id == id)
                    return i;
            }

            return -1;
        }

        //Generates Layer
        //  by compositing existing layers together
        int Composite_Layers(vector<int> layer_ids)
        {
#ifdef DEBUG
            cout << "Before composite, _layers contains: ";

            for (auto l : _layers) cout << l->_id << ", ";

            cout << endl;
#endif

            while (layer_ids.size() > 1)
            {
                //Create layer
                //  generates frame data in layer constructor
                //  by compositing layers l1 and l2 together
                int idx1 = get_index(layer_ids.back());
                layer_ids.pop_back();
                int idx2 = get_index(layer_ids.back());
                layer_ids.pop_back();

                Layer* l = new Layer(_layers[idx1], _layers[idx2], _op);
                
                //Collects Garbage
                //  deletes old layers from memory
                //  erases old layers from vector
                delete _layers[idx1];
                delete _layers[idx2];
                _layers.erase(_layers.begin() + idx1);
                _layers.erase(_layers.begin() + idx2);

                //Stores new layer
                //  in vector _layers
                //  with index _layers.size() - 1
                _layers.push_back(l);
                layer_ids.push_back(l->_id);
            }

#ifdef DEBUG
            cout << "After composite, _layers contains: ";

            for (auto l : _layers) cout << l->_id << ", ";

            cout << endl;
#endif

            //Returns id of new layer
            return layer_ids.back();
        }

        void Composite_All()
        {
            while (_layers.size() > 1)
            {
                Layer* l1 = _layers.back();
                _layers.pop_back();
                Layer* l2 = _layers.back();
                _layers.pop_back();
                
                //determine bounds of new layer given those of composite layers
                int _start = (l1->_start < l2->_start) ? l1->_start : l2->_start;
                int _end = (l1->_end > l2->_end) ? l1->_end : l2->_end;

                if ((_start == l1->_start) && ((_start + (l1->_end - l1->_start)) == l2->_start))
                {
                    if(l1->_frames.size() >= l2->_frames.size())
                    {
                        l1->_frames.insert(l1->_frames.end(), l2->_frames.begin(), l2->_frames.end());
                        l1->_end = _end;
                        l1->_frame_num = _end - _start;
                        _layers.push_back(l1);
                    }
                    else
                    {
                        l2->_frames.insert(l2->_frames.begin(), l1->_frames.begin(), l1->_frames.end());
                        l2->_start = _start;
                        l2->_frame_num = _end - _start;
                        _layers.push_back(l2);
                    }
#ifdef DEBUG  
                    cout << "Layer Composited by vector operation in Render" << endl;
#endif
                }

                else if ((_start == l2->_start) && ((_start + (l2->_end - l2->_start)) == l1->_start))
                {
                    if(l1->_frames.size() >= l2->_frames.size())
                    {
                        l1->_frames.insert(l1->_frames.begin(), l2->_frames.begin(), l2->_frames.end());
                        l1->_start = _start;
                        l1->_frame_num = _end - _start;
                        _layers.push_back(l1);
                    }
                    else
                    {
                        l2->_frames.insert(l2->_frames.end(), l1->_frames.begin(), l1->_frames.end());
                        l2->_end = _end;
                        l2->_frame_num = _end - _start;
                        _layers.push_back(l2);
                    }
#ifdef DEBUG
                    cout << "Layer Composited by vector operation in Render" << endl;
#endif
                }

                else
                {
                    _layers.push_back(new Layer(l1, l2, _op));

                    delete l1;
                    delete l2;

#ifdef DEBUG
                    cout << "Layer Composited by copy constructor in Render" << endl;
#endif
                }

#ifdef DEBUG
                cout << "   Now, _layers contains: ";

                for (auto l : _layers) cout << l->_id << ", ";

                cout << endl;
#endif
            }
        }

        void Export(string fileName)
        {
            int step = 500;

            Composite_All();

            GifWriter gifw;
            ofstream out;
            Layer *l = _layers.back();
            
            int n = 0;

            for (int i = 0; i < l->_frame_num; i++)
            {
                if (i % step == 0)
                {
                    cout << "   beginning gif at " << i << endl;
                    GifBegin(&gifw, ("output/" + fileName + "_" + to_string(n) + ".gif").c_str(), l->_width * _scale, l->_height * _scale, _delay);
                    n++;
                }

                GifWriteFrame(&gifw, flip(l->_frames[i]).data(), l->_width * _scale, l->_height * _scale, _delay);

                if (i % 100 == 0)
                    cout << "       writing frame " << i << endl;

                if ((i % step == (step - 1)) || (i == l->_frame_num - 1))
                {
                    cout << "   ending gif at " << i << endl;
                    GifEnd(&gifw);

                    if ((n + 1) * step > l->_frame_num) break;
                }
            }

            for (int i = n * step; i < l->_frame_num; i++)
            {
                if (i == n * step)
                {
                    cout << "   beginning gif at " << i << endl;
                    GifBegin(&gifw, ("output/" + fileName + "_" + to_string(n) + ".gif").c_str(), l->_width * _scale, l->_height * _scale, _delay);
                    n++;
                }

                GifWriteFrame(&gifw, flip(l->_frames[i]).data(), l->_width * _scale, l->_height * _scale, _delay);

                if (i % 100 == 0)
                    cout << "       writing frame " << i << endl;

                if (i == l->_frame_num - 1)
                {
                    cout << "   ending gif at " << i << endl;
                    GifEnd(&gifw);
                }
            }
        }

        void Export(GifWriter &gifw)
        {
#ifdef DEBUG
            cout << "Before export, _layers contains: ";

            for (auto l : _layers) cout << l->_id << ", ";

            cout << endl;
#endif

            Composite_All();

            Layer *l = _layers.back();

            for (int i = 0; i < l->_frame_num; i++)
            {
                GifWriteFrame(&gifw, flip(l->_frames[i]).data(), l->_width * _scale, l->_height * _scale, _delay);

#ifdef DEBUG
                cout << "   Frame " << i << " Written" << endl;
#endif
            }
        }

        void layer_SetMask(int layer_id, int val)
        {
            _layers[layer_id]->set_mask(val);
        }

        vector<uint8_t> scale(Frame* f)
        {
            vector<uint8_t> scale;

            for (int i = 0; i < f->_width; i++)
            {
                for (int m1 = 0; m1 < _scale; m1++)
                {
                    for (int j = 0; j < f->_height; j++)
                    {
                        for (int m2 = 0; m2 < _scale; m2++)
                        {
                            for (int k = 0; k < 4; k++)
                            {
                                scale.push_back(f->_frame_data[i * f->_height * 4 + (j * 4) + k]);
                            }
                        }
                    }
                }
            }

            return scale;
        }

        vector<uint8_t> flip(Frame* f)
        {
            vector<uint8_t> flip;

            for (int j = 0; j < f->_height; j++)
            {
                for (int m1 = 0; m1 < _scale; m1++)
                {
                    for (int i = 0; i < f->_width; i++)
                    {
                        for (int m2 = 0; m2 < _scale; m2++)
                        {
                            for (int k = 0; k < 4; k++)
                            {
                                flip.push_back(f->_frame_data[i * f->_height * 4 + (j * 4) + k]);
                            }
                        }
                    }
                }
            }

            return flip;
        }

        Frame* layer_getLastFrame()
        {
            return _layers.back()->_frames.back();
        }

        Layer* get_content()
        {
            Composite_All();
            return _layers.back();
        }

        Layer* Scale()
        {
            Layer* l_s = new Layer(0, _layers.back()->_frame_num, _layers.back()->_width * _scale, _layers.back()->_height * _scale);

            for (int n = 0; n < _layers.back()->_frame_num; n++)
            {
                l_s->_frames.push_back(new Frame(scale(_layers.back()->_frames[n]), _layers.back()->_width * _scale, _layers.back()->_height * _scale));
            }

            delete _layers.back();
            _layers.pop_back();

            return l_s;
        }

        Layer* pop_content()
        {
            Composite_All();

            return Scale();
        }

        std::vector<Layer*> _layers;

};

#endif