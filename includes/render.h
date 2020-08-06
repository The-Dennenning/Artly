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
**/

#include "events.h"
#include "bitmap.h"
#include "layer.h"
#include "gif.h"

#ifndef RENDER_H
#define RENDER_H

class Render {

    public:

        //Generates layer
        //  using seed bitmap and event
        int Add_Layer(Bitmap* b, Event* e)
        {
            //Create layer 
            //  at start of event, and for duration of event,
            //  with width and height of provided bitmap
            Layer* l = new Layer(e->_start, e->_duration, b->getSize(0), b->getSize(1));

            cout << "Layer created, with start " << e->_start << " and duration " << e->_duration << endl;

            //Activates event
            //  using seed bitmap b to generate frame data in layer l
            e->Activate(b, l);

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
                GifWriteFrame(&gifw, l->_frame_data[i].data(), l->_width, l->_height, 10);

                cout << "   Frame " << i << " Written" << endl;
            }
        }

        void layer_SetMask(int layer_id, int val)
        {
            _layers[layer_id]->set_mask(val);
        }

    private:

        std::vector<Layer*> _layers;

};

#endif