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
        ~Render() {}

        //Generates layer
        //  using seed frame and event
        int Add_Layer(Frame* f, Event* e)
        {
            //Create layer 
            //  at start of event, and for duration of event,
            //  with width and height of provided frame
            Layer* l = new Layer(e->_start, e->_duration, e->_width, e->_height);

            cout << "Layer created, with start " << e->_start << " and duration " << e->_duration << endl;

            //Activates event
            //  using seed frame f to generate frame data in layer l
            e->Activate(f, l);

            delete e;

            //Stores new layer
            //  in vector _layers,
            //  with index _layers.size() - 1
            _layers.push_back(l);

            //Returns index of new layer
            return _layers.size() - 1;
        }

        //Generates Layer
        //  by compositing two existing layers together
        int Add_Layer(int l1, int l2, int op)
        {
            //Create layer
            //  generates frame data in layer constructor
            //  by compositing layers l1 and l2 together
            Layer* l = new Layer(_layers[l1], _layers[l2], op);

            //Stores new layer
            //  in vector _layers
            //  with index _layers.size() - 1
            _layers.push_back(l);

            //Collects Garbage
            //  deletes old layers from memory
            //  erases old layers from vector
            delete _layers[l1];
            delete _layers[l2];
            _layers.erase(_layers.begin() + l1);
            _layers.erase(_layers.begin() + l2);

            //Returns index of new layer
            return _layers.size() - 1;
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
            Composite_All();

            Layer *l = _layers.back();

            for (int i = 0; i < l->_frame_num; i++)
            {
                GifWriteFrame(&gifw, flip(l->_frames[i]).data(), l->_width, l->_height, 10);

                cout << "   Frame " << i << " Written" << endl;
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