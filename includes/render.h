/*
**  Render combines events, bitmaps, and layers into a single package
**  representing the final 'render' from a usage of Artly.
**
**  How many ways can one combine these things?
**
**      Bitmap -> Event -> Layer (void Generate_Layers)
**      
**          This represents layer generation from a seed image. A bitmap
**          is fed to an event, which generates a layer. The bitmap
**          represents a single frame, whereas a layer represents n frames.
**          
**          this flow is used when an event can generate content from a seed
**          image. 
**
**      Layer + Layer -> Layer (void Composit_Layers)
**
**          This represents composing two layers together via masking operations.
**          It is in this way that output from two different events can be
**          superimposed in various ways.
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
        Render() {}
        ~Render() {for (auto l : _layers) delete l;}

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

                Layer* l = new Layer(_layers[idx1], _layers[idx2], 0);
                
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

                _layers.push_back(new Layer(l1, l2, 0));

                delete l1;
                delete l2;
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
                GifWriteFrame(&gifw, flip(l->_frames[i]).data(), l->_width, l->_height, 10);

#ifdef DEBUG
                cout << "   Frame " << i << " Written" << endl;
#endif
            }
        }

        void layer_SetMask(int layer_id, int val)
        {
            _layers[layer_id]->set_mask(val);
        }

        vector<uint8_t> flip(Frame* f)
        {
            vector<uint8_t> flip;

            for (int j = 0; j < f->_height; j++)
            {
                for (int i = 0; i < f->_width; i++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        flip.push_back(f->_frame_data[i * f->_height * 4 + (j * 4) + k]);
                    }
                }
            }

            return flip;
        }

        Frame* layer_getLastFrame()
        {
            return _layers.back()->_frames.back();
        }

    private:

        std::vector<Layer*> _layers;

};

#endif