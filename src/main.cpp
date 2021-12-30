#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>
#include <chrono>

#include "..\includes\space.h"
#include "..\includes\render.h"
#include "..\includes\events.h"
#include "..\includes\bitmap.h"
#include "..\includes\gif.h"
#include "..\includes\image_gen.h"
#include "..\includes\zine.h"

#include "..\includes\Events\fractal.h"
#include "..\includes\Events\plotter.h"
#include "..\includes\Events\painterly.h"
#include "..\includes\Events\cellular.h"
#include "..\includes\Events\kmeans.h"
#include "..\includes\Events\pixel_sort.h"
#include "..\includes\Events\field.h"
#include "..\includes\Events\camera.h"
#include "..\includes\Events\dither.h"
#include "..\includes\Events\morph.h"
#include "..\includes\Events\mathly.h"

using namespace std;
using namespace chrono;

/*



*/





void run_tidecastle();

void run_lineage();
void run_invocation();
void run_creaturely();
void run_stone();

void run_cellular(vector<int> params, int n);
void run_zine();
void run_kmeans_collager();
void run_painterly();

void run_doodles();

void run_bodhi();


int main()
{
    srand(time(NULL));

    run_bodhi();

}

void run_bodhi()
{
    vector<Frame*> inits;
    vector<Bitmap*> images;
    ofstream out;
    ifstream in;
    Frame* f;

    vector<Kdata*> data;

    int width[4] = {1080, 1080, 1172, 896};
    int height[4] = {1324, 866, 1412, 1232};
    
    int k_array[6] = {1, 2, 3, 4, 8, 16};
    
    int _t = 20;
    int _k = 4;
    int _n = 4;

    cout << "   Loading Images..." << endl;

    for (int i = 0; i <= 3; i++)
    {
        images.push_back(new Bitmap());
        string instring("input/bozposter/" + to_string(i) + ".bmp");

        in.open(instring);
        in >> *(images.back());
        in.close();
        
        f = new Frame(*(images.back()));

        inits.push_back(f);

        cout << "       ...image set " << i << " loaded" << endl;
    }

    cout << "   Making the rectangles happen..." << endl;

    for (int i = 0; i <= 3; i++)
    {
        for (int j = 0; j < _k; j++)
        {
            Kmeans *km = new Kmeans(0, 1, inits[i], images[i], k_array[j], i);

            data.push_back(km->get_data());
        }
    }

    cout << "   Making the collages happen..." << endl;


    for (int _i = 0; _i <= 3; _i++)
    {
        Frame *result = new Frame(width[_i], height[_i]);

        for (int t = 0; t < _t; t++)
        {
            result->_clear_w();

            cout << "Assembling image " << t << "..." << endl;

            for (int k = 0; k < _k; k++)
            {
                cout << "   " << k << endl;
            
                random_shuffle(data.begin(), data.end());

                for (int n = 0; n < _n; n++)
                {
                    for (int i = 0; i < width[_i]; i++)
                    {
                        if (i >= data[n]->_width)
                            break;

                        for (int j = 0; j < height[_i]; j++)
                        {
                            if (j >= data[n]->_height)
                                break;

                            if (data[n]->_clusters[i * data[n]->_height + j] == k)
                            {
                                result->set(i, j, data[n]->_data[i * data[n]->_height + j]);
                            }
                        }
                    }
                }
            }

            images[_i]->settoFrame(result->flip());

            out.open("output/bodhi_collage_out " + to_string(t) + ".bmp");
            out << *(images[_i]);
            out.close();

        }
    }
}

void run_doodles()
{
    vector<Frame*> inits;
    Bitmap *image = new Bitmap();
    ofstream out;
    ifstream in;
    Frame* f;

    int height = 2400;
    int width = 2400;

    int scale = 1;
    int delay = 5;

    cout << "   Loading Images..." << endl;

    for (int i = 0; i <= 2; i++)
    {
        image = new Bitmap();
        string instring("input/doodles/" + to_string(i) + ".bmp");

        in.open(instring);
        in >> *image;
        in.close();
        
        f = new Frame(*image);

        inits.push_back(f);

        delete image;
        cout << "       ...image set " << i + 1 << " loaded" << endl;
    }

    for (int i = 0; i < 3; i++)
    {
        Render *r = new Render(scale, delay);

        r->Add_Layer(inits[i], new Morph(0, 200, width, height, inits[(i + 1 % 3)], 200));

        r->Export("doodle_" + to_string(i));

        delete r;

        cout << "   ...render " << i << " done" << endl;
    }
}

void run_tidecastle()
{
    vector<Frame*> inits;
    ofstream out;
    ifstream in;
    Bitmap *image;
    Frame *f;

    int height = 1080;
    int width = 1440;

    int length = 4950;

    int nper[5] = {13, 11, 13, 12, 1};

    int sum = 0;

    cout << "   Loading Images..." << endl;

    for (int i = 0; i <= 4; i++)
    {
        sum += nper[i];

        for (int j = 1; j <= nper[i]; j++)
        {
            image = new Bitmap();
            string instring("input/tidecastle/" + to_string(i + 1) + "-" + to_string(j) + ".bmp");

            in.open(instring);
            in >> *image;
            in.close();
            
            f = new Frame(*image);

            inits.push_back(f);

            delete image;
        }
        cout << "       ...image set " << i + 1 << " loaded" << endl;
    }

    int frame_track = 0;
    int frame_dur = (length / sum) / 2;
    int scale = 1;
    int delay = 5;

    cout << "   total image number: " << inits.size() << endl;
    cout << "   total frame count: " << frame_dur * (inits.size()) * 2 << endl;

    cout << "   beginning render..." << endl;

    Render *r = new Render(scale, delay);
    
    r->Add_Layer(inits[25], new Plotter(frame_track, 1, width, height, 0, 0, 0));

    frame_track += 1;

    for (int i = 25; i < inits.size(); i++)
    {
        //get last frame of render
        delete f;
        f = new Frame(*r->layer_getLastFrame());

        //morph from last frame into current frame
        r->Add_Layer(f, new Morph(frame_track, frame_dur, width, height, inits[i], frame_dur));
        frame_track += frame_dur;

        //hold current frame
        r->Add_Layer(inits[i], new Plotter(frame_track, frame_dur, width, height, 0, 0, 0));
        frame_track += frame_dur;

        delete inits[i];
        cout << "       ...rendered morph " << i + 1 << " of " << inits.size() << endl;
    }

    cout << frame_track << endl;

    r->Composite_All();

    //get last frame of render
    //delete f;
    f = new Frame(*r->layer_getLastFrame());
    
    //hold current frame
    r->Add_Layer(f, new Plotter(frame_track, frame_dur, width, height, 0, 0, 0));
    frame_track += frame_dur;

    Frame *f2 = new Frame(width, height);
    f2->_clear();


    //morph from last frame into current frame
    r->Add_Layer(f, new Morph(frame_track, 50, width, height, f2, 50));
    frame_track += 50;
    
    cout << "       ...render complete" << endl;

    r->Export("tidecastle_2");

    delete r;
}

void run_painterly()
{
    Bitmap *image = new Bitmap();
    ifstream in;
    ofstream out;

    int width = 2400;
    int height = 3000;

    in.open("input/annie.bmp");
    in >> *image;
    in.close();

    Frame* out_f = new Frame(width, height);
    Frame* ref_f = new Frame(*image);

    Render *r = new Render(1, 5);

    cout << "   applying painterly layer 0..." << endl;

    r->Add_Layer(out_f, new Painter(0, 1, out_f, ref_f, 50000, 15));
    
    delete out_f;
    delete ref_f;
    out_f = new Frame(*r->layer_getLastFrame());
    ref_f = new Frame(*image);

    cout << "   applying painterly layer 1..." << endl;

    r->Add_Layer(out_f, new Painter(0, 1, out_f, ref_f, 75000, 10));
    
    delete out_f;
    delete ref_f;
    out_f = new Frame(*r->layer_getLastFrame());
    ref_f = new Frame(*image);

    cout << "   applying painterly layer 2..." << endl;

    r->Add_Layer(out_f, new Painter(1, 1, out_f, ref_f, 100000, 7));
    
    
    delete out_f;
    delete ref_f;
    out_f = new Frame(*r->layer_getLastFrame());
    ref_f = new Frame(*image);
    
    cout << "   applying painterly layer 3..." << endl;

    r->Add_Layer(out_f, new Painter(2, 1, out_f, ref_f, 25000, 3));
    

    delete out_f;
    delete ref_f;
    out_f = new Frame(*r->layer_getLastFrame());

    cout << "   printing..." << endl;

    image->settoFrame(out_f->flip());

    out.open("output/annie.bmp");
    out << *image;
    out.close();

    delete r;
    delete image;
}

void run_kmeans_collager()
{
    vector<Kdata*> data;
    ofstream out;
    ifstream in;
    Bitmap *image;
    Frame *f;

    //number of kmeans clusters to calculate
    int _k = 3;
    //number of images in image in folder
    int _m = 6;
    //number of images to grab for this run
    int _n = 6;
    //number of images to generate
    int _t = 20;

    int height = 1080;
    int width = 1080;

    for (int i = 0; i <= _n; i++)
    {
        int m = i;

        cout << "KMeansing image " << i << ": " << m << "..." << endl;
        image = new Bitmap();
        string instring("input/stone/" + to_string(m) + ".bmp");
        in.open(instring);
        in >> *image;
        in.close();
        
        f = new Frame(*image);
        
        Kmeans *K = new Kmeans(0, 0, f, image, _k);

        data.push_back(K->get_data());

        delete image;
    }
    

    image = new Bitmap();
    Frame *result = new Frame(width, height);

    in.open("input/stone/1.bmp");
    in >> *image;
    in.close();

    for (int t = 0; t < _t; t++)
    {
        result->_clear_w();

        cout << "Assembling image " << t << "..." << endl;

        for (int k = 0; k < _k; k++)
        {
            cout << "   " << k << endl;
        
            random_shuffle(data.begin(), data.end());

            for (int n = 0; n < _n; n++)
            {
                for (int i = 0; i < width; i++)
                {
                    if (i >= data[n]->_width)
                        break;

                    for (int j = 0; j < height; j++)
                    {
                        if (j >= data[n]->_height)
                            break;

                        if (data[n]->_clusters[i * data[n]->_height + j] == k)
                        {
                            result->set(i, j, data[n]->_data[i * data[n]->_height + j]);
                        }
                    }
                }
            }
        }

        image->settoFrame(result->flip());

        out.open("output/stone_collage_out " + to_string(t) + ".bmp");
        out << *image;
        out.close();

    }

    for (auto d : data) delete d;
    delete image;
}

void run_zine()
{
    int width = 3000;
    int height = 2400;

    int i_width = 648;
    int i_height = 648;
    
    ifstream in;
    ofstream out;
    Bitmap *image;

    vector<Frame*> inits;

    cout << "   Loading images..." << endl;

    for (int n = 1; n <= 8; n++)
    {
        image = new Bitmap();

        in.open("input/poetry/Anima/" + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);
        Frame* f2 = new Frame(f, i_width, i_height);

        inits.push_back(f2);

        delete image;
    }

    vector<Frame*> inputs;

    //orders images for eight-fold zine
    inputs.push_back(inits[4]);
    inputs.push_back(inits[3]);
    inputs.push_back(inits[2]);
    inputs.push_back(inits[1]);
    inputs.push_back(inits[5]);
    inputs.push_back(inits[6]);
    inputs.push_back(inits[7]);
    inputs.push_back(inits[0]);

    cout << "   Compiling Zine..." << endl;

    Zine* z = new Zine(width, height, inputs);

    image = new Bitmap();

    in.open("input/in 5.bmp");
    in >> *image;
    in.close();

    image->settoFrame(z->_pages[0]->flip());

    out.open("output/anima_zine.bmp");
    out << *image;
    out.close();

    delete image;
}


/* lineage

    Animation Idea:
        branching trees
            TODO build branching trees module (consult lightning module)
        camera scanning over branching trees
            MADO refine camera module (one plane parallel to camera shortcut?)

    Music Idea:
        TODO collect tree field recordings
        very percussive

*/
void run_lineage()
{
    int width = 540;
    int height = 540;
    int scale = 2;
    int delay = 3;

    Frame* f = new Frame(width, height);
    f->_clear_w();

    Render* r = new Render(scale, delay);

    int frame_track = 0;
    int frame_dur = 500;

    r->Add_Layer(f, new Field(frame_track, frame_dur, width, height, 3000, 100));

    r->Export("Field");

}

/* invocation

    Animation Idea:
        colors, blacks and whites, abstract
        waves? think echolocation

    Music Idea:
        voices, chanting, 
*/
void run_invocation()
{
    int width = 1080;
    int height = 1080;
    int delay = 5;
    int scale = 1;

    Bitmap *image;
    ifstream in;
    ofstream out;
    vector<Frame*> inits;

    cout << "   Loading images..." << endl;

    for (int n = 0; n <= 2; n++)
    {
        image = new Bitmap();

        in.open("input/invoc/" + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    Render *r = new Render(scale, delay);

    r->Add_Layer(inits[0], new Morph(0, 400, width, height, inits[1], 400));

/*
    double x = 540;
    double t = 400;

    vector<double> params;
        params.push_back(x);
        params.push_back(x);
        params.push_back(1);
        params.push_back(5);
        params.push_back(3);
        
    vector<double> params2;
        params2.push_back(x);
        params2.push_back(x);
        params2.push_back(750);
        params2.push_back(5);
        params2.push_back(-3);
        
    vector<double> params3;
        params3.push_back((3 * x) / 2);
        params3.push_back(x);
        params3.push_back(1);
        params3.push_back(5);
        params3.push_back(2);    


    cout << "   circle 1" << endl;
    r->Add_Layer(f, new Mathly(0, t, width, height, 1, params));

    cout << "   circle 1" << endl;
    r->Add_Layer(f, new Mathly(t / 10, t, width, height, 1, params));

    cout << "   circle 2" << endl;
    r->Add_Layer(f, new Mathly(t / 5, t, width, height, 1, params2));

    cout << "   circle 3" << endl;
    r->Add_Layer(f, new Mathly((3 * t) / 10, t, width, height, 1, params));
    
    cout << "   circle 3" << endl;
    r->Add_Layer(f, new Mathly((3 * t) / 10, t, width, height, 1, params3));
    
    cout << "   circle 4" << endl;
    r->Add_Layer(f, new Mathly((4 * t) / 10, 150, width, height, 1, params2));

    cout << "   circle 1" << endl;
    r->Add_Layer(f, new Mathly((4 * t) / 10, 150, width, height, 1, params));
    
    cout << "   circle 4" << endl;
    r->Add_Layer(f, new Mathly((5 * t) / 10, 150, width, height, 1, params2));

    cout << "   circle 3" << endl;
    r->Add_Layer(f, new Mathly((5 * t) / 10, 175, width, height, 1, params3));
    
    cout << "   circle 5" << endl;
    r->Add_Layer(f, new Mathly((6 * t) / 10, 150, width, height, 1, params));

    r->Composite_All();
    //r->_layers.back()->shift_color(0, 300, 1);
    */

    r->Export("invocation");
    
    delete r;
}

/* creaturely

    Animation Idea:
        cellular automata at various levels

    Music Idea
        def a section thats like the flight of the bumblebee

*/
void run_creaturely()
{
    Space* s = new Space();
        s->_t = 0;

    Camera* c = new Camera(0, 1200, 540, 540, 1);
        c->_s = s;

        c->_real_height = 100;
        c->_real_width = 100;

        c->_coord[0] = -50;
        c->_coord[1] = -50;
        c->_coord[2] = 5;
        
    Face* f1 = new Face(s, 1, 0);
        s->_objects.push_back(f1);

        Vertex* v1 = new Vertex(-2000, -2000, 0);
        f1->_verts.push_back(v1);

        Vertex* v2 = new Vertex(-2000, 2000, 0);
        f1->_verts.push_back(v2);

        Vertex* v3 = new Vertex(2000, 2000, 0);
        f1->_verts.push_back(v3);

        Vertex* v4 = new Vertex(2000, -2000, 0);
        f1->_verts.push_back(v4);

        v1->_conto.push_back(make_pair(v4, v2));
        v2->_conto.push_back(make_pair(v1, v3));
        v3->_conto.push_back(make_pair(v2, v4));
        v4->_conto.push_back(make_pair(v3, v1));

        f1->_normal[0] = 0;
        f1->_normal[1] = 0;
        f1->_normal[2] = 1;

        f1->_faces.push_back(f1);
        
        Render fr;

            int fwidth = 500;
            int fheight = 500;

            Frame* init_1 = new Frame(fwidth, fheight);
            vector<Frame*> inits;

            vector<vector<int>> args; 
            vector<int> args_2;
                args_2.push_back(2);
                args_2.push_back(3);
                args_2.push_back(3);
                args_2.push_back(3);
                args_2.push_back(1);
            args.push_back(args_2);

            cout << "   Running Events..." << endl;


            for (int i = 0; i < 4; i++)
            {
                if (i % 2 == 0)
                    fr.Add_Layer(init_1, new Plotter(0 + 300 * i, 1, fwidth, fheight, 1, 8, 4));
                else
                    fr.Add_Layer(init_1, new Plotter(0 + 300 * i, 1, fwidth, fheight, 2, 10, 0));

                inits.push_back(new Frame(*fr.layer_getLastFrame()));

                fr.Add_Layer(inits.back(), new Cellular(1 + 300 * i, 300, fwidth, fheight, 2, args[0]));

                //fr._layers.back()->shift_color(0, 200, 1);   
            }
            
            /*
                fr.Add_Layer(init_1, new Plotter(0, 1, fwidth, fheight, 1, 5));

                init_2 = new Frame(*fr.layer_getLastFrame());

                fr.Add_Layer(init_2, new Cellular(1, 400, fwidth, fheight, 1, args[0]));
            */

        f1->_content = new Layer(fr.get_content());


    //write gif

    int width = 1080;
    int height = 1080;

    int delay = 5;

    Frame* init = new Frame(width, height);

    Render* r = new Render(2, delay);

    cout << "   Running Camera..." << endl;

    r->Add_Layer(init, c);

    Layer* l = r->pop_content();

    Render* pr = new Render(1, delay);

    Frame* ref = new Frame(width, height);
    Frame* out_f = new Frame(width, height);

    cout << "   Running Painterly..." << endl;

    for (int i = 0; i < l->_frame_num; i++)
    {
        delete ref;
        ref = new Frame(*l->_frames[i]);

        if ((i + 1) < l->_frame_num)
            delete l->_frames[i];

        if (i % 100 == 0)
            cout << "       painting frame " << i << endl;

        delete out_f;

        if (i == 0)
        {
            out_f = new Frame(width, height);
            out_f->_clear();
        }
        else
            out_f = new Frame(*pr->layer_getLastFrame());

        pr->Add_Layer(out_f, new Painter(i, 1, out_f, ref, 6000, 2));
    }
    
    cout << "   Compositing and Exporting Gif..." << endl;

    pr->Export("creaturely");
}

/* of the stone

    Animation Idea:
        pics of stones morphing in and out 
        also AI generated aesthetic, but slower 

    Music Idea
        soundscape, dripping water, patient
*/
void run_stone()
{
    int width = 1080;
    int height = 1080;
    int delay = 5;
    int scale = 1;

    ifstream in;
    ofstream out;
    Bitmap *image;

    vector<Frame*> inits;

    cout << "   Loading images..." << endl;

    for (int n = 0; n <= 11; n++)
    {
        image = new Bitmap();

        in.open("input/stone/" + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    Render* r = new Render(scale, delay);

    cout << "   Executing painterly..." << endl;

    Frame* reference = new Frame(width, height);
    Frame* output = new Frame(width, height);
    output->_clear();

    int frame_track = 0;
    int frame_dur = 100;

    for (int i = 0; i <= 11; i++)
    {
        delete reference;
        reference = new Frame(*inits[i]);

        if (i != 0)
        {
            delete output;
            output = new Frame(*r->layer_getLastFrame());
        }
        
        r->Add_Layer(output, new Painter(frame_track, frame_dur, output, reference, 500, 10));

        frame_track += frame_dur;

        delete reference;
        reference = new Frame(*inits[i]);
        delete output;
        output = new Frame(*r->layer_getLastFrame());
        
        r->Add_Layer(output, new Painter(frame_track, frame_dur, output, reference, 1000, 3));

        frame_track += frame_dur;

        delete reference;
        reference = new Frame(*inits[i]);
        delete output;
        output = new Frame(*r->layer_getLastFrame());
        
        r->Add_Layer(output, new Painter(frame_track, frame_dur, output, reference, 2000, 1));

        frame_track += frame_dur;
        
    }

    r->Export("stone");
}



void run_cellular(vector<int> params, int n)
{
    cout << "   Initializing..." << endl;

    ifstream in;
    ofstream out;
    Bitmap *image;

    image = new Bitmap();

    in.open("input/susansmol.bmp");
    in >> *image;
    in.close();

    Frame* f = new Frame(*image);

    int width = 400;
    int height = 400;
    int scale = 5;
    int delay = 4;

    Frame* init_1 = new Frame(width, height);

    Render r(scale);

    vector<int> args_1;
        args_1.push_back(params[0]);
        args_1.push_back(params[1]);
        args_1.push_back(params[2]);
        args_1.push_back(params[3]);
        args_1.push_back(1);

    cout << "   Running Events..." << endl;

    r.Add_Layer(f, new Plotter(0, 1, width, height, 0, 0, 0));

    Frame* init_2 = new Frame(*r.layer_getLastFrame());

    r.Add_Layer(init_2, new Cellular(1, 1000, width, height, 1, args_1));
    
    cout << "   Compositing and Exporting Gif..." << endl;

    r.Export("susan_cells");

    cout << "   ...Gif Written" << endl;
}



void run_morph()
{
    
    int width = 1000;
    int height = 1000;
    int delay = 5;
    int scale = 1;

    ifstream in;
    ofstream out;
    Bitmap *image;

    vector<Frame*> inits;

    cout << "   Loading images..." << endl;

    for (int n = 1; n <= 2; n++)
    {
        image = new Bitmap();

        in.open("input/morph" + to_string(n + 7) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    Render* r = new Render(scale, delay);

    cout << "   Executing morph..." << endl;

    r->Add_Layer(inits[0], new Morph(0, 400, width, height, inits[1], 100));

    r->Export("morph");
}