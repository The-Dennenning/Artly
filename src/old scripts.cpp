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

#include "..\includes\Events\fractal.h"
#include "..\includes\Events\plotter.h"
#include "..\includes\Events\painterly.h"
#include "..\includes\Events\cellular.h"
#include "..\includes\Events\kmeans.h"
#include "..\includes\Events\pixel_sort.h"
#include "..\includes\Events\field.h"
#include "..\includes\Events\camera.h"
#include "..\includes\Events\dither.h"

using namespace std;
using namespace chrono;


void Run_image_gen()
{
    int height = 8;
    int depth = 8;
    int k = 1;

    Image_Gen ig(height, depth, k, "image_gen/bored_wnbs.csv");
}

void Run_Field()
{
    int width = 108;
    int height = 108;
    int scale = 10;
    int delay = 3;

    Frame* f = new Frame(width, height);
    f->_clear_w();

    Render* r = new Render(scale, delay);

    int frame_track = 0;
    int frame_dur = 500;

    r->Add_Layer(f, new Field(frame_track, frame_dur, width, height, 1000, 100));

    r->Export("Field");
}

void Run_Pixelsorter()
{
    ifstream in;
    ofstream out;
    Bitmap *image;

    image = new Bitmap();
    in.open("input/in.bmp");
    in >> *image;
    in.close();

    int width = image->getSize(0);
    int height = image->getSize(1);

    int k_array[6] = {1, 2, 3, 4, 8, 16};

    Frame *f = new Frame(*image);
    
    for (int j = 0; j < 6; j++)
        Kmeans *K = new Kmeans(0, 0, f, image, k_array[j], 1, BLUE);

    delete image;
    delete f;
}

void Run_NewtonFractal()
{
    ifstream in;
    ofstream out;
    Bitmap *image;
    Frame *f;

    image = new Bitmap();
    in.open("input/in 0.bmp");
    in >> *image;
    in.close();

    int width = image->getSize(0);
    int height = image->getSize(1);

    Render *r = new Render(1, 5);

/*
    vector<double> coefs;
        coefs.push_back(1);
        coefs.push_back(-5);
        coefs.push_back(-19);
        coefs.push_back(125);
        coefs.push_back(-150);
*/
/*
    vector<double> coefs;
        coefs.push_back(2);
        coefs.push_back(4);
        coefs.push_back(18);
        coefs.push_back(-100);
        coefs.push_back(50);
*/



/*
    egg shapes at scale = 50
    vector<double> coefs;
        coefs.push_back(5);
        coefs.push_back(10);
        coefs.push_back(-3);
        coefs.push_back(200);
        coefs.push_back(14);
*/

/*
    vector<double> coefs;
         coefs.push_back(-1);
        coefs.push_back(-2);
        coefs.push_back(3);
        coefs.push_back(2);
        coefs.push_back(1);
*/

/*
    vector<double> coefs;
        coefs.push_back(1);
        coefs.push_back(11);
        coefs.push_back(4);
        coefs.push_back(3);
        coefs.push_back(-54);
        coefs.push_back(20);
        coefs.push_back(1);
*/
/*
    vector<double> coefs;
        coefs.push_back(1);
        coefs.push_back(2);
        coefs.push_back(3);
        coefs.push_back(4);
        coefs.push_back(5);
        coefs.push_back(6);
        coefs.push_back(7);
    */

    //really cool balls at scale = 50, (-0.5, -0.5)
    vector<double> coefs;
        coefs.push_back(1);
        coefs.push_back(100);
        coefs.push_back(-100);
        coefs.push_back(300);
        coefs.push_back(-200);
        coefs.push_back(1);

    r->Add_Layer(f, new NFractal(0, 1, width, height, coefs));

    f = new Frame(*r->layer_getLastFrame());

    r->Add_Layer(f, new Plotter(1, 200, width, height, 0, 0, 0));
    r->Composite_All();

    r->_layers.back()->shift_color(0, 200, 2);

    image->settoFrame(f->flip());

    int rn = rand() % 100000;

    cout << "Saving as: " << rn << endl;

    out.open("output/" + to_string(rn) + ".bmp");
    out << *image;
    out.close();
    
    r->Export("nfrac gif");

    delete image;
    delete r;

}

void Run_Kmeans_Collager()
{
    vector<Kdata*> data;
    ofstream out;
    ifstream in;
    Bitmap *image;
    Frame *f;

    //number of kmeans clusters to calculate
    int _k = 4;
    //number of images in image in folder
    int _m = 18;
    //number of images to grab for this run
    int _n = 6;
    //number of images to generate
    int _t = 40;

    int height;
    int width;

    for (int i = 0; i <= _n; i++)
    {
        int m;

        if (i > 3)
            m = rand() % (_m - 4) + 4;
        else
            m = i;

        cout << "KMeansing image " << i << ": " << m << "..." << endl;
        image = new Bitmap();
        string instring("input/energy/" + to_string(m) + ".bmp");
        in.open(instring);
        in >> *image;
        in.close();
        
        f = new Frame(*image);
        
        Kmeans *K = new Kmeans(0, 0, f, image, _k);

        data.push_back(K->get_data());

        delete image;
    }
    
    width = 960;
    height = 1280;

    image = new Bitmap();
    Frame *result = new Frame(width, height);

    in.open("input/energy/1.bmp");
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

        result->_greyscale();
        image->settoFrame(result->flip());

        out.open("output/energy_collage_out " + to_string(t) + ".bmp");
        out << *image;
        out.close();

    }

    for (auto d : data) delete d;
    delete image;
}

void Dreams()
{
    
    
    srand(time(NULL));

    /*
        start                   0           0       0
        thin beat comes in      13.71       274     274
        thic beat comes in      41.14       822     548             0
        beat out                1:08.57     1372    550             550
        main beat in            1:37.71     1956    584     0       1134            
                                                            274                     
        main beat continues     2:05.14     2504    548     548
                                                            274                     
        main beat continues     2:32.57     3052    550     1096                    
        hats out                2:46.28     3326    274     1370
        flute picks back up     3:00.00     3600    274     1644    0
        flute outro             3:39.42     4388    788             788
        end                     4:00.00     4800    412             1200
    */

    int width = 1080;
    int height = 1080;
    int scale = 1;
    int delay = 5;

    vector<Frame*> inits;
    Bitmap* image;
    ifstream in;
    ofstream out;

    for (int i = 3; i < 6; i++)
    {
        image = new Bitmap();

        in.open("input/Dreams/in " + to_string(i) + ".bmp");
        in >> *image;
        in.close();

        inits.push_back(new Frame(*image));

        if (i != 5)
            delete image;
    }

    int frame_track = 0;

    Frame* f = new Frame(width, height);
    Render* r = new Render(scale, delay);
    Frame* cinit = new Frame(width, height);

    /*  Pt 1 

    cout << "   Executing Camera Pan..." << endl;
    //Execute camera pan
    Space* s1 = new Space();
        s1->_t = 0;

    Camera* c1 = new Camera(274, 548, 1080, 1080, 1); //548
        c1->_s = s1;

        c1->_real_height = 1080;
        c1->_real_width = 1080;

        c1->_coord[0] = 0;
        c1->_coord[1] = 0;
        c1->_coord[2] = 10;

    Render *r_cc = new Render(scale, delay, 1);

    int fwidth = 1080;
    int fheight = 5400;

    inits[0]->set_mask(255);
    r_cc->Add_Layer(inits[0], new Plotter(0, 1, fwidth, fheight, 0, 0, 0));
    
    Face* f1;

    vector<double> cs1;

    cs1.push_back(0);
    cs1.push_back(0);
    cs1.push_back(fheight);
    cs1.push_back(fwidth);

    f1 = new Face(s1, 1, 0, 0, cs1, 0);

    f1->_content = new Layer(r_cc->get_content());

    s1->_objects.push_back(f1);
    
    delete r_cc;

    cinit->set_mask(255);

    cout << "   Running Camera 1..." << endl;
    r->Add_Layer(cinit, c1);

    cout << "       executing layer effects..." << endl;
    
    r->_layers.back()->set_mask(255);
    r->_layers.back()->shift_color(0, 548, 1);

    cinit = new Frame(*inits[1]);

    cout << "   Running plotter..." << endl;
    r->Add_Layer(cinit, new Plotter(0, 274, width, height, 0, 0, 0));

    cout << "       executing layer effects..." << endl;
    r->_layers.back()->scan(0, 274, -10, -3);
    r->_layers.back()->glitch(264, 10, 10);
    r->_layers.back()->shift_color(0, 274, 1);
    r->_layers.back()->fade_in(0, 100);

    cinit = new Frame(width, height);
    cinit->_clear();

    cout << "   Running plotter..." << endl;
    r->Add_Layer(cinit, new Plotter(0, 100, width, height, 0, 0, 0));

    cout << "       executing layer effects..." << endl;
    r->_layers.back()->fade_out(0, 100);

    int length = 250;

    f->_clear();

    */

    /* pt 2
    cout << "   Running Plotter..." << endl;

    r->Add_Layer(inits[0], new Plotter(0, 125, width, height, 0, 0, 0));    
    r->_layers.back()->fade_out(0, 125);

    cout << "   Running huegene layer 1..." << endl;

    int l1 = r->Add_Layer(f, new Cellular_Hue(frame_track, length, width, height, 5));
    
    cout << "       executing layer effects..." << endl;
    r->_layers[r->get_index(l1)]->fade_in(0, 125);
    r->_layers[r->get_index(l1)]->fade_out(125, 125);
    
    cout << "   executing huegene layer 2..." << endl;
    l1 = r->Add_Layer(f, new Cellular_Hue(frame_track + 125, length, width, height, 10));       //ends 375
    r->_layers[r->get_index(l1)]->fade_in(0, 125);
    r->_layers[r->get_index(l1)]->fade_out(125, 125);
    
    cout << "   executing huegene layer 3..." << endl;
    l1 = r->Add_Layer(f, new Cellular_Hue(frame_track + 250, length, width, height, 20));       //ends 500
    r->_layers[r->get_index(l1)]->fade_in(0, 125);
    r->_layers[r->get_index(l1)]->fade_out(125, 125);
    
    cout << "   executing huegene layer 4..." << endl;
    l1 = r->Add_Layer(f, new Cellular_Hue(frame_track + 375, length, width, height, 25));       //ends 625
    r->_layers[r->get_index(l1)]->fade_in(0, 125);
    r->_layers[r->get_index(l1)]->fade_out(125, 125);
    
    cout << "   executing huegene layer 5..." << endl;
    l1 = r->Add_Layer(f, new Cellular_Hue(frame_track + 500, length, width, height, 30));       //ends 750
    r->_layers[r->get_index(l1)]->fade_in(0, 125);
    r->_layers[r->get_index(l1)]->fade_out(125, 125);

     cout << "   executing huegene layer 6..." << endl;
    l1 = r->Add_Layer(f, new Cellular_Hue(frame_track + 625, length, width, height, 25));       //ends 875
    r->_layers[r->get_index(l1)]->fade_in(0, 125);
    r->_layers[r->get_index(l1)]->fade_out(125, 125);
    
    cout << "   executing huegene layer 7..." << endl;
    l1 = r->Add_Layer(f, new Cellular_Hue(frame_track + 750, length, width, height, 15));       //ends 1000
    r->_layers[r->get_index(l1)]->fade_in(0, 125);
    r->_layers[r->get_index(l1)]->fade_out(125, 125);

    cout << "   executing huegene layer 8..." << endl;
    l1 = r->Add_Layer(f, new Cellular_Hue(frame_track + 875, length + 9, width, height, 10));   //ends 1134
    r->_layers[r->get_index(l1)]->fade_in(0, 125);
    r->_layers[r->get_index(l1)]->fade_out(125, 125 + 9);
    
    cout << "   executing huegene layer 9..." << endl;
    l1 = r->Add_Layer(f, new Cellular_Hue(frame_track + 1000, 134, width, height, 5));          //ends 1134
    r->_layers[r->get_index(l1)]->fade_in(0, 75);
    r->_layers[r->get_index(l1)]->fade_in(75, 134 - 75);

    r->Composite_All();

    cout << "   executing color shifting..." << endl;
    r->_layers.back()->shift_color(550, 584, 1);
    */

   /*
    int length = 274;
    int params[6] = {10, 10, 10, 10, 5, 5};
    int l1;

    f->_clear();
    
    for (int i = 1; i < 7; i++)
    {
        cout << "   executing huegene layer " << i << "..." << endl;
        l1 = r->Add_Layer(f, new Cellular_Hue(frame_track, length, width, height, params[i - 1]));  

        if (i > 2)    
            r->_layers[r->get_index(l1)]->fade_in(0, (length / 2));
        if (i > 1)
            r->_layers[r->get_index(l1)]->fade_out((length / 2), (length / 2));
        frame_track += length;
    }

    r->Composite_All();

    cout << "   executing layer effects..." << endl;
    r->_layers.back()->shift_color(0, length * 2, 0.85);
    r->_layers.back()->shift_color(length * 2, length * 2, -2);
    r->_layers.back()->shift_color(length * 4, length * 2, 2);

    r->_layers.back()->glitch(length - 10, 10, 4);
    r->_layers.back()->glitch((length * 2) - 10, 10, 4);

    r->_layers.back()->scan(0, length, -0.5, 0.5);
    r->_layers.back()->scan(length, length, 0.5, -1.5);
    r->_layers.back()->scan(length * 2, length, -1, -1);
    r->_layers.back()->scan(length * 3, length, 1.5, -0.5);
    r->_layers.back()->scan(length * 4, length, -1, 0.5);
    r->_layers.back()->scan(length * 5, length, 0, -1);

    */

   int length = 788;

   cout << "   Executing Camera Pan..." << endl;
    //Execute camera pan
    Space* s1 = new Space();
        s1->_t = 0;

    Camera* c1 = new Camera(0, length, 1080, 1080, 1); //548
        c1->_s = s1;

        c1->_real_height = 1080;
        c1->_real_width = 1080;

        c1->_coord[0] = 4320;
        c1->_coord[1] = 0;
        c1->_coord[2] = 10;

    Render *r_cc = new Render(scale, delay, 1);

    int fwidth = 1080;
    int fheight = 5400;

    inits[0]->set_mask(255);
    r_cc->Add_Layer(inits[0], new Plotter(0, 1, fwidth, fheight, 0, 0, 0));
    
    Face* f1;

    vector<double> cs1;

    cs1.push_back(0);
    cs1.push_back(0);
    cs1.push_back(fheight);
    cs1.push_back(fwidth);

    f1 = new Face(s1, 1, 0, 0, cs1, 0);

    f1->_content = new Layer(r_cc->get_content());

    s1->_objects.push_back(f1);
    
    delete r_cc;

    cinit = new Frame(width, height);
    cinit->set_mask(255);

    cout << "   Running Camera 1..." << endl;
    r->Add_Layer(cinit, c1);

    cout << "   Running plotter..." << endl;
    cinit = new Frame(*r->layer_getLastFrame());
    r->Add_Layer(cinit, new Plotter(length, 200, width, height, 0, 0, 0));

    r->Composite_All();

    cout << "       executing layer effects..." << endl;
    r->_layers.back()->fade_in(0, length);
    r->_layers.back()->fade_out(length, 100);
    r->_layers.back()->shift_color(0, length + 200, 1);

    cout << "   Running plotter..." << endl;
    r->Add_Layer(inits[1], new Plotter(0, length, width, height, 0, 0, 0));

    cout << "       executing layer effects..." << endl;
    r->_layers.back()->fade_out(0, length);
    r->_layers.back()->shift_color(0, length, 1);

    cout << "   Running plotter..." << endl;
    r->Add_Layer(inits[2], new Plotter(0, length + 412, width, height, 0, 0, 0));

    cout << "       executing layer effects..." << endl;
    r->_layers.back()->fade_in(0, length + 300);
    r->_layers.back()->fade_out(200, 112);

    cout << "   Running plotter..." << endl;
    cinit = new Frame(width, height);
    cinit->_clear();
    cinit->set_mask(0);
    r->Add_Layer(cinit, new Plotter(length + 100, 312, width, height, 0, 0, 0));

    cout << "       executing layer effects..." << endl;
    r->_layers.back()->fade_in(200, 112);


    


    /*
    Layer* l = r->pop_content();

    Frame* ref = new Frame(width, height);
    Frame* out_f = new Frame(width, height);

    cout << "   executing painterly filter..." << endl;
    for (int i = 0; i < l->_frame_num; i++)
    {
        delete ref;
        ref = new Frame(*l->_frames[i]);

        if ((i + 1) < l->_frame_num)
            delete l->_frames[i];

        if (i % 100 == 0)
            cout << "       calculating frame " << i << endl;

        delete out_f;
        if (i == 0)
        {
            out_f = new Frame(width, height);
            out_f->_clear();
        }
        else
            out_f = new Frame(*r->layer_getLastFrame());

        if (i < 1096)
            r->Add_Layer(ref, new Plotter(i, 1, width, height, 0, 0, 0));
        else
            r->Add_Layer(out_f, new Painter(i, 1, out_f, ref, 10000, 1));
        
        /* pt 1 
        if (i < 274)
            r->Add_Layer(ref, new Plotter(i, 1, width, height, 0, 0, 0));
        else if (i < 486)
            r->Add_Layer(out_f, new Painter(i, 1, out_f, ref, 4000, 3));
        else if (i < 822)
            r->Add_Layer(out_f, new Painter(i, 1, out_f, ref, 15000, 1));
        
        
        /* pt 2
        if (i < 550)
            r->Add_Layer(out_f, new Painter(i, 1, out_f, ref, 5000, 1));
        else 
            r->Add_Layer(out_f, new Painter(i, 1, out_f, ref, 1000, 3));
        
        
    }
    */
   
    r->Export("dreams");

    delete image;
    image = new Bitmap();

    in.open("input/16.bmp");
    in >> *image;
    in.close();

    image->settoFrame(r->_layers.back()->_frames.back()->flip());

    out.open("output/dreams_last_frame.bmp");
    out << *image;
    out.close();

    return 0;
}

void huegene()
{

    int width = 1080;
    int height = 1080;
    int scale = 1;
    int delay = 5;
    int length = 1000;

    Frame* f = new Frame(width, height);
    f->_clear();

    Render* r = new Render(scale, delay);

    r->Add_Layer(f, new Cellular_Hue(0, length, width, height, 3));

    //r->Export("huegene_coral");

    ifstream in;
    ofstream out;
    Bitmap *image = new Bitmap();

        cout << "Reading image..." << endl;
    
    in.open("input/in 0.bmp");
    in >> *image;
    in.close();

    image->settoFrame(f->flip());
    
        cout << "outputting image 1..." << endl;

    out.open("output/huegene_coral.bmp");
    out << *image;
    out.close();
    
    /*
        cout << "blurring image..." << endl;

    f->_blur(5);

    image->settoFrame(f->flip());
    
        cout << "outputting image 2..." << endl;

    out.open("output/huegene_blur.bmp");
    out << *image;
    out.close();
    

    delete image;

    */

}

void Electrum()
{

/* script for Electrum 

    Intro               0:00.00         0
        pop 1           0:00.22         4
        pop 2           0:00.44         8
        fizz            0:01.49         28
        chip in         0:11.86         236     
        flutes in       0:14.83         296     
        flutes dip      0:17.80         356     0     
        flutes in 2     0:23.73         474     118     0
        drums in        0:35.60         712             238
            2           0:41.53         830             
            3           0:47.46         948             236
            4           0:53.39         1066
    Drop                0:59.33         1186            238     0
            2           1:05.26         1304                    118
            3           1:11.19         1424                    120
            4           1:17.13         1542                    118
    out                 1:23:06         1660                    118     0    
            2           1:28.99         1780                            120
            3           1:34.93         1898                            118     238
            4           1:40.86         2016                            118     356
            5           1:46.79         2136                            120     476
            6           1:52.73         2254                            118     594
    finish              1:58.66         2372                            118     712

*/

    int width = 1080;
    int height = 1080;
    int scale = 1;
    int delay = 5;
    
    ifstream in;
    ofstream out;
    Bitmap *image;

    vector<Frame*> inits;

    cout << "   Loading images..." << endl;

    for (int n = 0; n <= 7; n++)
    {
        image = new Bitmap();

        in.open("input/Electrum/in " + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }
        
    int width_d = 1080;
    int height_d = 1080;
    int scale_d = 1;
    int delay_d = 5;
    
    Render *r_d = new Render(scale_d, delay_d);


    Frame* f_d3 = new Frame(width, height);
    Frame* f_d4 = new Frame(width, height);
    Frame* f_d5 = new Frame(*inits[6]);
    f_d4->_negate();

    cout << "   0 - plotting, applying fades..." << endl;

    int l3 = r_d->Add_Layer(f_d3, new Plotter(0, 28, width, height, 0, 0, 0));
    r_d->_layers[r_d->get_index(l3)]->fade_out(0, 28);
    
    int l4 = r_d->Add_Layer(f_d4, new Plotter(0, 356, width, height, 0, 0, 0));
    r_d->_layers[r_d->get_index(l4)]->fade_in(0, 28);
    r_d->_layers[r_d->get_index(l4)]->fade_out(28, 208);
    r_d->_layers[r_d->get_index(l4)]->fade_in(208, 118);

    int l5 = r_d->Add_Layer(f_d5, new Plotter(28, 328, width, height, 0, 0, 0));
    r_d->_layers[r_d->get_index(l5)]->fade_in(0, 208);
    r_d->_layers[r_d->get_index(l5)]->fade_out(208, 118);

    r_d->Composite_All();

    cout << "   0 - applying scans..." << endl;
    r_d->_layers.back()->scan(0, 356, -1, 2);

    Frame *f_d1 = new Frame(*inits[3]);
    Frame *f_d2 = new Frame(*f_d1);
    f_d1->_negate();

    cout << "   1 - plotting, applying fades..." << endl;
    int l1 = r_d->Add_Layer(f_d1, new Plotter(356, 356, width_d, height_d, 0, 0, 0));
    r_d->_layers[r_d->get_index(l1)]->fade_out(0, 236);
    r_d->_layers[r_d->get_index(l1)]->fade_in(236, 120);
    
    int l2 = r_d->Add_Layer(f_d2, new Plotter(356, 356, width_d, height_d, 0, 0, 0));
    r_d->_layers[r_d->get_index(l2)]->fade_in(0, 236);
    r_d->_layers[r_d->get_index(l2)]->fade_out(236, 120);

    r_d->Composite_All();

    cout << "   1 - applying first scan..." << endl;
    r_d->_layers.back()->scan(356, 118, -0.5, 2);

    cout << "   1 - applying glitch 1..." << endl;
    r_d->_layers.back()->glitch(298, 5, 10);
    
    cout << "   1 - applying second scan..." << endl;
    r_d->_layers.back()->scan(474, 238, 1, -1.5);
    
    cout << "   1 - applying glitch 2..." << endl;
    r_d->_layers.back()->glitch(474, 10, 5);

    
    delete f_d1;
    delete f_d2;

    f_d1 = new Frame(*inits[2]);
    f_d2 = new Frame(*f_d1);
    f_d1->_negate();

    int length_d = 474;

    cout << "   2 - plotting, applying fades..." << endl;
    l1 = r_d->Add_Layer(f_d1, new Plotter(712, length_d, width_d, height_d, 0, 0, 0));
    r_d->_layers[r_d->get_index(l1)]->fade_out(0, length_d);
    
    l2 = r_d->Add_Layer(f_d2, new Plotter(712, length_d, width_d, height_d, 0, 0, 0));
    r_d->_layers[r_d->get_index(l2)]->fade_in(0, length_d);

    r_d->Composite_All();

    cout << "   2 - applying first scan..." << endl;
    r_d->_layers.back()->scan(712, 236, 1, 1);

    cout << "   2 - applying glitch 1..." << endl;
    r_d->_layers.back()->glitch(790, 10, 5);
    
    cout << "   2 - applying second scan..." << endl;
    r_d->_layers.back()->scan(948, 238, -1, -1.5);
    
    cout << "   2 - applying glitch 2..." << endl;
    r_d->_layers.back()->glitch(1000, 10, 5);

    delete f_d1;
    delete f_d2;

    f_d1 = new Frame(*inits[4]);
    f_d2 = new Frame(width, height);
    f_d2->_clear();
    f_d1->_negate();

    cout << "   3 - plotting, applying fades..." << endl;
    l1 = r_d->Add_Layer(f_d1, new Plotter(1186, 474, width, height, 0, 0, 0));
    r_d->_layers[r_d->get_index(l1)]->fade_in(0, 118, 0.5);
    r_d->_layers[r_d->get_index(l1)]->fade_in(118, 120, 0.5);
    r_d->_layers[r_d->get_index(l1)]->fade_in(238, 118, 0.5);
    r_d->_layers[r_d->get_index(l1)]->fade_in(356, 118, 0.5);

    l1 = r_d->Add_Layer(f_d2, new Plotter(1186, 474, width, height, 0, 0, 0));
    r_d->_layers[r_d->get_index(l1)]->fade_out(0, 118);
    r_d->_layers[r_d->get_index(l1)]->fade_out(118, 120);
    r_d->_layers[r_d->get_index(l1)]->fade_out(238, 118);
    r_d->_layers[r_d->get_index(l1)]->fade_out(356, 118);

    r_d->Composite_All();

    cout << "   3 - applying scan 1..." << endl;
    r_d->_layers.back()->scan(1186, 118, 1, -1.5);
    
    cout << "   3 - applying scan 2..." << endl;
    r_d->_layers.back()->scan(1304, 120, -1, -1.5);
    r_d->_layers.back()->glitch(1350, 10, 5);
    
    cout << "   3 - applying scan 3..." << endl;
    r_d->_layers.back()->scan(1424, 118, -1, 1.5);
    
    cout << "   3 - applying scan 4..." << endl;
    r_d->_layers.back()->scan(1542, 118, 1, 1.5);


    Frame* f_d1;
    Frame* f_d2;
    int l1;
    int l2;
    
    Frame* f = new Frame(width, height);
    f->_clear();
    f->set_mask(0);

    vector<double> params;
        params.push_back(0);
        params.push_back(0);
        params.push_back(350);
        params.push_back(2);
        params.push_back(45);
        params.push_back(18);
        params.push_back(5);
        params.push_back(1);
        
    vector<int> c_params;
        c_params.push_back(2);
        c_params.push_back(3);
        c_params.push_back(3);
        c_params.push_back(3);
        c_params.push_back(1);
        

    Frame *f2;

    params[1] = 0;
    params[4] = 45;
    params[6] = 3;
    params[7] = 0;

    f = new Frame(width, height);
    f->_clear();
    f->set_mask(0);

    cout << "   L4 - generating lightning strike 1..." << endl;
    r_d->Add_Layer(f, new Fractal_Lightning(238, 20, 1080, 1080, params));
    
    f = new Frame(width, height);
    f->_clear();
    f->set_mask(0);

    params[0] = 1079;
    params[1] = 1079;
    params[4] = -135;

    cout << "   L4 - generating lightning strike 2..." << endl;
    r_d->Add_Layer(f, new Fractal_Lightning(238, 20, 1080, 1080, params));
    
    f = new Frame(width, height);
    f->_clear();
    f->set_mask(0);

    params[0] = 0;
    params[1] = 1079;
    params[4] = 135;

    cout << "   L4 - generating lightning strike 3..." << endl;
    r_d->Add_Layer(f, new Fractal_Lightning(238, 20, 1080, 1080, params));
    
    f = new Frame(width, height);
    f->_clear();
    f->set_mask(0);

    params[0] = 1079;
    params[1] = 0;
    params[4] = -45;

    cout << "   L4 - generating lightning strike 4..." << endl;
    r_d->Add_Layer(f, new Fractal_Lightning(238, 20, 1080, 1080, params));

    r_d->Composite_All();
    
    f = new Frame(*r_d->layer_getLastFrame());

    r_d->Add_Layer(f, new Cellular(258, 216, 1080, 1080, 0, c_params));

    cout << "   Executing fade out..." << endl;

    r_d->Composite_All();
    
    r_d->_layers.back()->fade_out(140, 96);

    Layer *_l = new Layer(*r_d->_layers.back());

    r_d->_layers.pop_back();


    f_d1 = new Frame(*inits[5]);
    f_d2 = new Frame(*f_d1);
    f_d1->_negate();


    cout << "   4 - plotting, applying fades..." << endl;
    l1 = r_d->Add_Layer(f_d1, new Plotter(1660 - 1660, 478, width_d, height_d, 0, 0, 0));
    r_d->_layers[r_d->get_index(l1)]->fade_out(0, 478);
    
    l2 = r_d->Add_Layer(f_d2, new Plotter(1660 - 1660, 478, width_d, height_d, 0, 0, 0));
    r_d->_layers[r_d->get_index(l2)]->fade_in(0, 478);

    
    r_d->Composite_All();

    cout << "   4 - applying scan 1..." << endl;
    r_d->_layers.back()->scan(1660 - 1660, 120, 1, -1.5);
    
    cout << "   4 - applying scan 2..." << endl;
    r_d->_layers.back()->scan(1780 - 1660, 118, -1, -1.5);
    r_d->_layers.back()->glitch(1660 - 1660, 10, 5);
    
    cout << "   4 - applying scan 3..." << endl;
    r_d->_layers.back()->scan(1898 - 1660, 118, -1, 1.5);
    
    cout << "   3 - applying scan 4..." << endl;
    r_d->_layers.back()->scan(2016 - 1660, 118, 1, 1.5);


    f_d1 = new Frame(*inits[7]);
    f_d2 = new Frame(*f_d1);
    f_d2->_negate();

    cout << "   5 - plotting, applying fades..." << endl;
    l1 = r_d->Add_Layer(f_d1, new Plotter(2136 - 1660, 236, width_d, height_d, 0, 0, 0));
    r_d->_layers[r_d->get_index(l1)]->fade_out(0, 236);
    
    l2 = r_d->Add_Layer(f_d2, new Plotter(2136 - 1660, 236, width_d, height_d, 0, 0, 0));
    r_d->_layers[r_d->get_index(l2)]->fade_in(0, 236);

    r_d->Composite_All();
    
    cout << "   5 - applying glitch 1..." << endl;
    r_d->_layers.back()->glitch(2136 - 1660, 10, 5);


    cout << "   applying dither..." << endl;
    Dither* d = new Dither(0, 0, width_d, height_d);

    d->Activate(r_d->_layers.back());
    r_d->_layers.back()->set_mask(2);

    r_d->_layers.push_back(_l);
    
    for (auto l : r_d->_layers)
    {
        cout << l->_start << " to " << l->_end << ", with " << l->_frames.size() << " frames" << endl;
    }


    cout << "   L1 - generating lightning strike..." << endl;
    int fl = r_d->Add_Layer(f, new Fractal_Lightning(1186, 25, 1080, 1080, params));

    Frame* f2 = new Frame(*r_d->_layers[r_d->get_index(fl)]->_frames.back());

    cout << "   L1 - applying cellular..." << endl;
    r_d->Add_Layer(f2, new Cellular(1211, 93, 1080, 1080, 0, c_params));

    params[0] = 1079;
    params[4] = -45;
    
    f = new Frame(width, height);
    f->_clear();

    cout << "   L2 - generating lightning strike..." << endl;
    fl = r_d->Add_Layer(f, new Fractal_Lightning(1304, 25, 1080, 1080, params));

    f2 = new Frame(*r_d->_layers[r_d->get_index(fl)]->_frames.back());

    cout << "   L2 - applying cellular..." << endl;
    r_d->Add_Layer(f2, new Cellular(1329, 95, 1080, 1080, 0, c_params));


    params[0] = 0;
    params[1] = 1079;
    params[4] = 135;
    
    f = new Frame(width, height);
    f->_clear();

    cout << "   L3 - generating lightning strike..." << endl;
    fl = r_d->Add_Layer(f, new Fractal_Lightning(1424, 25, 1080, 1080, params));

    f2 = new Frame(*r_d->_layers[r_d->get_index(fl)]->_frames.back());

    cout << "   L3 - applying cellular..." << endl;
    r_d->Add_Layer(f2, new Cellular(1449, 93, 1080, 1080, 0, c_params));


    params[0] = 1079;
    params[1] = 1079;
    params[4] = -135;
    
    f = new Frame(width, height);
    f->_clear();

    cout << "   L4 - generating lightning strike..." << endl;
    fl = r_d->Add_Layer(f, new Fractal_Lightning(1542, 25, 1080, 1080, params));

    f2 = new Frame(*r_d->_layers[r_d->get_index(fl)]->_frames.back());

    cout << "   L4 - applying cellular..." << endl;
    r_d->Add_Layer(f2, new Cellular(1567, 93, 1080, 1080, 0, c_params));


    r_d->Export("Electrum4");
}

void Run_Ditherer_2()
{
    /*
    ifstream in;
    ofstream out;
    Bitmap *image = new Bitmap();

    cout << "Reading bitmap..." << endl;
    in.open("input/16.bmp");
    in >> *image;
    in.close();

    cout << "Creating Ditherer..." << endl;
    Frame *f = new Frame(*image);
    Dither *d = new Dither(0, 0, f->_width, f->_height);

    cout << "Dithering Image..." << endl;
    f = d->Activate(f);

    image->settoFrame(f->flip());

    out.open("output/dither.bmp");
    out << *image;
    out.close();

    vector<Frame*> fs;
        fs.push_back(new Frame(f, 216, 216, 100, (1080 - 350) - 216));
        fs.push_back(new Frame(f, 216, 216, 530, (1080 - 425) - 216));
        fs.push_back(new Frame(f, 216, 216, 252, (1080 - 35) - 216));
        fs.push_back(new Frame(f, 216, 216, 760, (1080 - 500) - 216));

    delete image;
    image = new Bitmap();

    in.open("input/in 3.bmp");
    in >> *image;
    in.close();
        
    for (int i = 1; i <= 4; i++)
    {
        cout << "Saving dither " << i << endl;

        image->settoFrame(fs[i-1]->flip());

        out.open("output/dither_" + to_string(i) + ".bmp");
        out << *image;
        out.close();
    }    

    */

    /*
    ifstream in;
    Bitmap *image = new Bitmap();

    in.open("input/d2.bmp");
    in >> *image;
    in.close();
    
    int width = 270;
    int height = 270;
    int length = 200;
    int scale = 4;
    int delay = 5;

    Frame* f1 = new Frame(*image);
    Frame* f2 = new Frame(*f1);
    f1->_negate();

    Render *r = new Render(scale, delay);

    int l1 = r->Add_Layer(f1, new Plotter(0, length, width, height, 0, 0, 0));
    r->_layers[r->get_index(l1)]->fade_out(0, length);
    
    int l2 = r->Add_Layer(f2, new Plotter(0, length, width, height, 0, 0, 0));
    r->_layers[r->get_index(l2)]->fade_in(0, length);

    r->Composite_All();

    cout << -10 % 3 << endl;

    cout << "   applying first scan..." << endl;
    r->_layers.back()->scan(0, 100, -0.5, 2);

    cout << "   applying glitch 1..." << endl;
    r->_layers.back()->glitch(20, 20, 2);
    
    cout << "   applying second scan..." << endl;
    r->_layers.back()->scan(100, 100, 1, -1.5);
    
    cout << "   applying glitch 2..." << endl;
    r->_layers.back()->glitch(150, 10, 5);

    cout << "   applying dither..." << endl;
    Dither* d = new Dither(0, 0, width, height);

    d->Activate(r->_layers.back());

    r->Export("dither2", nullptr);
    */

    int width = 270;
    int height = 270;
    int scale = 4;
    int delay = 3;

    ifstream in;
    ofstream out;
    Bitmap *image = new Bitmap;

    Frame* f = new Frame(width, height);
    f->_clear_w();

    Render* r = new Render(4, delay);

    r->Add_Layer(f, new Fractal(0, 500, width, height, 0));

    Dither *d = new Dither(0, 0, width, height);
    d->Activate(r->_layers.back());

    string outfile = "output/frac_out.gif";

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width * scale, height * scale, delay);

    r->Export(gifw);

    GifEnd(&gifw);

/*
    in.open("input/in 1.bmp");
    in >> *image;
    in.close();

    image->settoFrame(r->layer_getLastFrame()->flip());

    out.open("output/frac_out.bmp");
    out << *image;
    out.close();
*/
}

void Run_Ditherer()
{
    
    ifstream in;
    ofstream out;
    Bitmap *image = new Bitmap();

    cout << "Reading bitmap..." << endl;
    in.open("input/16.bmp");
    in >> *image;
    in.close();

    cout << "Creating Ditherer..." << endl;
    Frame *f = new Frame(*image);
    Dither *d = new Dither(0, 0, f->_width, f->_height);

    cout << "Dithering Image..." << endl;
    f = d->Activate(f);

    image->settoFrame(f->flip());

    out.open("output/dither.bmp");
    out << *image;
    out.close();

    /*
    int width = 256;
    int height = 256;
    int length = 200;
    int scale = 2;
    int delay = 5;

    Frame* f1 = new Frame(width, height);
    f1->_clear_w();
    Frame* f2 = new Frame(width, height);
    f2->_clear();

    Render *r = new Render(scale, delay);

    int l1 = r->Add_Layer(f1, new Plotter(0, length, width, height, 0, 0, 0));
    r->_layers[r->get_index(l1)]->fade_out(0, length);
    
    int l2 = r->Add_Layer(f2, new Plotter(0, length, width, height, 0, 0, 0));
    r->_layers[r->get_index(l2)]->fade_in(0, length);

    r->Composite_All();

    Dither* d = new Dither(0, 0, width, height);

    d->Activate(r->_layers.back());

    r->Export("dither", nullptr);
    */

}

void Run_AutoCollager()
{
    vector<Kdata*> data;
    vector<pair<int, int>> queue;
    vector<int> queque;
    ofstream out;
    ifstream in;
    Bitmap *image;
    Frame *f;

    int ftrack = 0;
    int fdur = 10;
    int _q = 12;
    int _t = 112;
    int _k = 5;
    int _n = 50;
    int _o = 0;

    int height;
    int width;

    for (int i = 1 + _o; i <= _n + _o; i++)
    {
        cout << "KMeansing image " << i << "..." << endl;
        image = new Bitmap();
        string instring("input/almond/in " + to_string(i) + ".bmp");
        in.open(instring);
        in >> *image;
        in.close();
        
        f = new Frame(*image);
        
        Kmeans *K = new Kmeans(0, 0, f, image, _k);

        data.push_back(K->get_data());

        delete image;
    }

    width = 1080;
    height = 1080;
    int delay = 5;
    int scale = 1;

    Render* r = new Render(scale, delay);
    Frame* output = new Frame(width, height);
    Frame* reference = new Frame(width, height);
    output->_clear();
    reference->_clear();

    for (int q = 0; q < _q; q++)
        queue.push_back(make_pair(-1, 0));

    for (int q = 0; q < _q; q++)
        queque.push_back(q);
	random_shuffle(queque.begin(), queque.end());

    for (int t = 0; t < _t; t++)
    {
        cout << "Rendering frame " << t << "..." << endl;

        int k = rand() % _k;
        int n = rand() % _n;

        queue[queque[t % _q]].first = n;
        queue[queque[t % _q]].second = k;

        for (int q = 0; q < _q; q++)
        {
            cout << "   queue position " << q << ": (" << queue[q].first << ", " << queue[q].second << ")" << endl;

            if (queue[q].first == -1)
                continue;

            for (int i = 0; i < width; i++)
            {
                if (i >= data[queue[q].first]->_width)
                    break;

                for (int j = 0; j < height; j++)
                {
                    if (j >= data[queue[q].first]->_height)
                        break;

                    if (data[queue[q].first]->_clusters[i * data[queue[q].first]->_height + j] == queue[q].second)
                    {
                        reference->set(i, j, data[queue[q].first]->_data[i * data[queue[q].first]->_height + j]);
                    }
                }
            }
        }
    
        if (t >= 80)
        {
            r->Add_Layer(output, new Painter(ftrack, fdur, output, reference, 8000, 1));
            ftrack += fdur;

            delete output;
            output = new Frame(*r->layer_getLastFrame());
        }
        else
        {
            r->Add_Layer(reference, new Plotter(ftrack, fdur, width, height, 0, 0, 0));
            ftrack += fdur;
        }

        delete reference;
        reference = new Frame(1080, 1080);
        reference->_clear();
    }

    r->Export("AlmondCollage", nullptr);

    for (auto d : data) delete d;
}

void Run_Overlay()
{
    int width = 1000;
    int height = 1000;
    int delay = 5;

    vector<Frame*> inits;
    Bitmap *image;
    ofstream out;
    ifstream in;

    for (int n = 0; n <= 3; n++)
    {
        image = new Bitmap();

        in.open("input/pron/squared/in" + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    Frame* reference_1 = new Frame(width, height);
    reference_1->_clear_w();
    Frame* reference_2 = new Frame(width, height);
    reference_2->_clear();
    Frame* output_1 = new Frame(width, height);
    output_1->_clear();
    Frame* output_2 = new Frame(width, height);
    output_2->_clear_w();

    Render *r = new Render(1, delay);

/*
    cout << "   Running Painterly 1..." << endl;
    int l1 = r->Add_Layer(output_1, new Painter(0, 255, output_1, reference_1, 250, 3));
    cout << "   Running Painterly 2..." << endl;
    int l2 = r->Add_Layer(output_2, new Painter(0, 255, output_2, reference_2, 250, 3));
*/

    int l1 = r->Add_Layer(inits[0], new Plotter(0, 255, width, height, 0, 0, 0));
    int l2 = r->Add_Layer(inits[1], new Plotter(0, 255, width, height, 0, 0, 0));

    cout << "   Executing fades..." << endl;
    r->_layers[r->get_index(l1)]->fade_in(0, 255);
    r->_layers[r->get_index(l2)]->fade_out(0, 255);

/*
    r->Composite_All();
    r->_layers.back()->set_mask(255);
*/

    cout << "   Saving gif..." << endl;

    in.open("input/pron/squared/in0.bmp");
    in >> *image;
    in.close();

    r->Export("output/Overlay", image);
}

void Run_FD()
{
    vector<Frame*> inits;
    Bitmap *image;
    ofstream out;
    ifstream in;
    
    int start = 20;
    int end = 30;

    for (int n = start; n <= end; n++)
    {
        image = new Bitmap();

        in.open("input/fathers day/" + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    int width = 1080;
    int height = 1080;
    int delay = 5;

    cout << "   Running Painterly..." << endl;

    Frame* reference;
    Frame* output;

    int frame_track = 0;
    int frame_count_1 = 50;
    int frame_count_2 = 75;

    reference = new Frame(*inits[0]);
    output = new Frame(1080, 1080);
    output->_clear();

    Render *r = new Render(1, delay);

    for (int i = start; i <= end; i++)
    {
        if (i != start)
        {
            delete reference;
            reference = new Frame(*inits[i - start]);
            delete output;
            output = new Frame(*r->layer_getLastFrame());
        }

        r->Add_Layer(output, new Painter(frame_track, frame_count_1, output, reference, 300, 7));
        frame_track += frame_count_1;

        delete reference;
        reference = new Frame(*inits[i - start]);
        delete output;
        output = new Frame(*r->layer_getLastFrame());

        r->Add_Layer(output, new Painter(frame_track, frame_count_1, output, reference, 400, 5));
        frame_track += frame_count_1;
        
        delete reference;
        reference = new Frame(*inits[i - start]);
        delete output;
        output = new Frame(*r->layer_getLastFrame());

        r->Add_Layer(output, new Painter(frame_track, frame_count_1, output, reference, 500, 3));
        frame_track += frame_count_1;
        
        delete reference;
        reference = new Frame(*inits[i - start]);
        delete output;
        output = new Frame(*r->layer_getLastFrame());

        r->Add_Layer(output, new Painter(frame_track, frame_count_2, output, reference, 5000, 1));
        frame_track += frame_count_2;
        
        delete reference;
        reference = new Frame(*inits[i - start]);
        delete output;
        output = new Frame(*r->layer_getLastFrame());

        r->Add_Layer(output, new Painter(frame_track, frame_count_2, output, reference, 5000, 1));
        frame_track += frame_count_2;

        cout << "       Layer " << i << " Initialized..." << endl;

        r->Composite_All();
    }
    
    r->Export("Family" + to_string(start), nullptr);
}

void Run_Submergence_pt1()
{
    
/* script
*        beginning     
*           flute in        0:03.68     74
*           pad hit 1       0:08.27     166
*           pad hit 2       0:12.41     248
*           pad hit 3       0:16.55     330
*           pad hit 4       0:20.69     414
*           breath          0:24.82     496
*           back in         0:26.48     530
*        groove
*           drums in        0:34.75     696
*           pad hit         0:43.03     860
*           bass in         0:51.31     1026
*           hit 1           0:59.58     1190
*           hit 2           1:07.86     1356
*        groove fade
*           bass out        1:16:13     1522
*           
*        end of pt1         1:40.96     
*/
    int width = 1080;
    int height = 1080;
    int scale = 1;
    int delay = 5;

    ifstream in;
    ofstream out;
    Bitmap *image;

    vector<Frame*> inits;

    cout << "   Loading images..." << endl;

    for (int n = 0; n <= 6; n++)
    {
        image = new Bitmap();

        in.open("input/Submergence/pt1/in " + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    Render *r = new Render(scale, delay);

    Frame* reference;
    Frame* output;

    int frame_track = 0;
    int frame_count = 1;

    reference = new Frame(*inits[0]);
    output = new Frame(width, height);
    output->_clear();

    frame_count = 166;
    r->Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 10));
    frame_track += frame_count;

    cout << "       Layer " << 0 << " Initialized..." << endl;


    delete reference;
    reference = new Frame(*inits[1]);
    delete output;
    output = new Frame(*r->layer_getLastFrame());

    frame_count = 248 - 166;
    r->Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 7));
    frame_track += frame_count;

    cout << "       Layer " << 1 << " Initialized..." << endl;


    delete reference;
    reference = new Frame(*inits[1]);
    delete output;
    output = new Frame(*r->layer_getLastFrame());

    frame_count = 330 - 248;
    r->Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 250, 3));
    frame_track += frame_count;

    cout << "       Layer " << 2 << " Initialized..." << endl;


    delete reference;
    reference = new Frame(*inits[2]);
    delete output;
    output = new Frame(*r->layer_getLastFrame());

    frame_count = 414 - 330;
    r->Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 250, 7));
    frame_track += frame_count;

    cout << "       Layer " << 3 << " Initialized..." << endl;


    delete reference;
    reference = new Frame(*inits[2]);
    delete output;
    output = new Frame(*r->layer_getLastFrame());

    frame_count = 496 - 414;
    r->Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 250, 3));
    frame_track += frame_count;

    cout << "       Layer " << 4 << " Initialized..." << endl;


    delete reference;
    reference = new Frame(*inits[2]);
    delete output;
    output = new Frame(*r->layer_getLastFrame());

    frame_count = 530 - 496;
    r->Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 250, 10));
    frame_track += frame_count;

    cout << "       Layer " << 5 << " Initialized..." << endl;


    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*r->layer_getLastFrame());

    frame_count = 696 - 530;
    r->Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 250, 5));
    frame_track += frame_count;

    cout << "       Layer " << 6 << " Initialized..." << endl;
    
    cout << "Rendering Fractal..." << endl;

    Render* r_frac = new Render(scale, delay, 2);
    Frame* f_frac = new Frame(width, height);
    f_frac->_clear_w();

    r_frac->Add_Layer(f_frac, new Fractal(0, 1026 - 696, width, height, 0));
    r_frac->Add_Layer(inits[6], new Plotter(0, 1026 - 696, width, height, 0, 0, 0));

    cout << "   Transferring Frames..." << endl;
        
    Layer* l_frac = new Layer(r_frac->get_content());
    vector<Frame*> finits;
    Render* pr = new Render(scale, delay, 0);

    for (int i = 0; i < l_frac->_frame_num; i++)
    {
        finits.push_back(new Frame(*l_frac->_frames[i]));
        delete l_frac->_frames[i];
    }

    for (int i = 0; i < finits.size(); i++)
    {
        delete reference;
        reference = new Frame(*finits[i]);
        delete finits[i];
        delete output;

        if (i == 0)
            output = new Frame(*r->layer_getLastFrame());
        else
            output = new Frame(*pr->layer_getLastFrame());

        pr->Add_Layer(output, new Painter(frame_track + i, 1, output, reference, 2500, 2));

        cout << "       Layer f" << i << " Initialized..." << endl;
    }

    delete reference;
    reference = new Frame(*inits[6]);
    delete output;
    output = new Frame(*r->layer_getLastFrame());

    frame_count = 1026 - 696;
    r->Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 250, 2));
    frame_track += frame_count;

    cout << "       Layer " << 7 << " Initialized..." << endl;

    pr->Composite_All();
    pr->_layers.back()->fade_in(0, 1026 - 696);
    r->_layers.push_back(new Layer(pr->_layers.back()));
    delete pr;

    r->Export("Submergence", image);
}

void Run_Submergence_pt2()
{
    /* script
*   Part one
*           bass in         0:51.31     1025 - end
*   Part Two
*           bass in         0:51.31     1026 - start    0
*           hit 1           0:59.58     1191            165
*           hit 2           1:07.86     1356            165
*        groove fade
*           bass out        1:16.13     1521 - end      165
*   Part Three
*           
*        end of pt1         1:40.96     
*/
    int width = 1080;
    int height = 1080;
    int scale = 1;
    int delay = 5;

    ifstream in;
    ofstream out;
    Bitmap *image;

    vector<Frame*> inits;

    cout << "   Loading images..." << endl;

    for (int n = 0; n <= 0; n++)
    {
        image = new Bitmap();

        in.open("input/Submergence/pt2/in " + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    Render *r_f = new Render(scale, delay);
    Frame *f_f = new Frame(width, height);

    int l1 = r_f->Add_Layer(f_f, new Fractal(0, 495, width, height, 1, 0));
    int l2 = r_f->Add_Layer(f_f, new Fractal(0, 495, width, height, 1, 1));
    int l3 = r_f->Add_Layer(f_f, new Fractal(0, 495, width, height, 1, 2));

    cout << "   Executing masking operations..." << endl;

    r_f->_layers[r_f->get_index(l1)]->set_mask(0);
    r_f->_layers[r_f->get_index(l2)]->set_mask(0);
    r_f->_layers[r_f->get_index(l3)]->set_mask(0);

    r_f->_layers[r_f->get_index(l1)]->fade_out(0, 165);
    r_f->_layers[r_f->get_index(l2)]->fade_in(0, 165);
    r_f->_layers[r_f->get_index(l2)]->fade_out(165, 165);
    r_f->_layers[r_f->get_index(l3)]->fade_in(165, 165);
    r_f->_layers[r_f->get_index(l3)]->fade_out(330, 165);
    r_f->_layers[r_f->get_index(l1)]->fade_in(330, 165);

    Layer* l_frac = new Layer(r_f->get_content());
    vector<Frame*> finits;

    cout << "   Transfering frames..." << endl;

    for (int i = 0; i < l_frac->_frame_num; i++)
    {
        finits.push_back(new Frame(*l_frac->_frames[i]));
        delete l_frac->_frames[i];
    }

    Frame* reference = new Frame(width, height);
    Frame* output = new Frame(width, height);
    Render* r = new Render(scale, delay);

    for (int i = 0; i < finits.size(); i++)
    {
        delete reference;
        reference = new Frame(*finits[i]);
        delete finits[i];
        delete output;

        if (i == 0)
            output = new Frame(*inits[0]);
        else
            output = new Frame(*r->layer_getLastFrame());

        r->Add_Layer(output, new Painter(i, 1, output, reference, 4000, 2));

        cout << "       Layer f" << i << " Initialized..." << endl;
    }

    r->Export("Submergence_2_", image);
}

void Run_Submergence_pt3()
{

/*
*           bass out        1:16.13     1521 - end
*   Part Three
*       Transition                                                  Camera Pan
*           bass out        1:16.13     1522 - start    0           0            
*           mid pad 1       1:24.41     1688            166
*           mid pad 2       1:32.69     1854            332
*       Submergence                                     
*           sub begins      1:40.96     2020            498
*           halfway         2:05.79     2516            994         
*           mid pad 1       2:14.06     2682            1160        1160
*           mid pad 2       2:22.34     2846            1324        
*           pad out         2:30.62     3012            1490        330
*           song ends       2:37.13     3142            1620        130
*/

    int width = 1080;
    int height = 1080;
    int scale = 1;
    int delay = 5;

    ifstream in;
    ofstream out;
    Bitmap *image;

    vector<Frame*> inits;

    cout << "   Loading images..." << endl;

    for (int n = 0; n <= 5; n++)
    {
        image = new Bitmap();

        in.open("input/Submergence/pt3/in " + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    cout << "   Executing Fractal..." << endl;

    Render *r_f = new Render(scale, delay);
    Frame *f_f = new Frame(width, height);

    r_f->Add_Layer(f_f, new Fractal(0, 166, width, height, 1, 0));
   
    Layer* l_f = new Layer(r_f->get_content());
    vector<Frame*> finits;

    for (int i = 0; i < l_f->_frame_num; i++)
    {
        finits.push_back(new Frame(*l_f->_frames[i]));
        delete l_f->_frames[i];
    }

    cout << "   Executing Camera Pan..." << endl;
    //Execute camera pan
    Space* s1 = new Space();
        s1->_t = 0;

    Camera* c1 = new Camera(0, 1160, 1080, 1080, 1);
        c1->_s = s1;

        c1->_real_height = 1080;
        c1->_real_width = 1080;

        c1->_coord[0] = 7560;
        c1->_coord[1] = 0;
        c1->_coord[2] = 10;

    Render *r_cc = new Render(scale, delay, 1);

    int fwidth = 1080;
    int fheight = 8640;
    
    vector<vector<double>> params;

        vector<double> params1;
            params1.push_back(200);  //x
            params1.push_back(600);  //y
            params1.push_back(45);   //length
            params1.push_back(2);    //width
            params1.push_back(20);   //theta
            params1.push_back(2);    //branch
            params1.push_back(150);   //n
        params.push_back(params1);
            
        vector<double> params2;
            params2.push_back(400);  //x
            params2.push_back(2500);  //y
            params2.push_back(45);   //length
            params2.push_back(2);    //width
            params2.push_back(30);   //theta
            params2.push_back(2);    //branch
            params2.push_back(50);   //n
        params.push_back(params2);
            
        vector<double> params3;
            params3.push_back(880);  //x
            params3.push_back(5000);  //y
            params3.push_back(25);   //length
            params3.push_back(3);    //width
            params3.push_back(20);   //theta
            params3.push_back(2);    //branch
            params3.push_back(75);   //n
        params.push_back(params3);
            
        vector<double> params4;
            params4.push_back(980);  //x
            params4.push_back(5500);  //y
            params4.push_back(25);   //length
            params4.push_back(3);    //width
            params4.push_back(60);   //theta
            params4.push_back(2);    //branch
            params4.push_back(100);   //n
        params.push_back(params4);
            
        vector<double> params6;
            params6.push_back(1000);  //x
            params6.push_back(1800);  //y
            params6.push_back(35);   //length
            params6.push_back(3);    //width
            params6.push_back(75);   //theta
            params6.push_back(2);    //branch
            params6.push_back(100);   //n
        params.push_back(params6);
            
        vector<double> params5;
            params5.push_back(100);  //x
            params5.push_back(6500);  //y
            params5.push_back(35);   //length
            params5.push_back(3);    //width
            params5.push_back(30);   //theta
            params5.push_back(2);    //branch
            params5.push_back(125);   //n
        params.push_back(params5);
            
        vector<double> params8;
            params8.push_back(1050);  //x
            params8.push_back(8000);  //y
            params8.push_back(35);   //length
            params8.push_back(1);    //width
            params8.push_back(30);   //theta
            params8.push_back(2);    //branch
            params8.push_back(35);   //n
        params.push_back(params8);
            
        vector<double> params9;
            params9.push_back(50);  //x
            params9.push_back(4000);  //y
            params9.push_back(10);   //length
            params9.push_back(5);    //width
            params9.push_back(50);   //theta
            params9.push_back(2);    //branch
            params9.push_back(200);   //n
        params.push_back(params9);

    inits[3]->set_mask(0);

    for (int i = 0; i < 8; i++)
    {
        cout << "   Executing kelp strand " << i << "..." << endl;

        if (i == 7)
            inits[3]->set_mask(1);
        
        r_cc->Add_Layer(inits[3], new Fractal_Kelp(0, 132, fwidth, fheight, params[i]));

        cout << "   Compositing kelp strand " << i << "..." << endl;
        r_cc->Composite_All();
    }
    
    Face* f1;

    vector<double> cs1;

    cs1.push_back(0);
    cs1.push_back(0);
    cs1.push_back(fheight);
    cs1.push_back(fwidth);

    for (int i = 0; i <= 0; i++)
    {
        f1 = new Face(s1, 1, 0, i, cs1, i);

        f1->_content = new Layer(r_cc->get_content());

        s1->_objects.push_back(f1);
    }

    delete r_cc;

    Frame* cinit = new Frame(width, height);

    Render *r_c = new Render(scale, delay);

    cout << "   Running Camera 1..." << endl;

    r_c->Add_Layer(cinit, c1);
    
    //r_c->Export("Submergence_test", nullptr);

    cout << "   Transferring Frames..." << endl;
        
    Layer* cl1 = new Layer(r_c->get_content());
    vector<Frame*> cinits;

    for (int i = 0; i < cl1->_frame_num; i++)
    {
        cinits.push_back(new Frame(*cl1->_frames[i]));
        delete cl1->_frames[i];
    }

    cout << "   Executing Painterly over Fractal..." << endl;

    Render *r_fp = new Render(scale, delay);
    Frame *reference = new Frame(width, height);
    Frame *output = new Frame(width, height);

    int fcount;
    int ftrack = 0;

    //painting fractal frames
    for (int i = 0; i < finits.size(); i++)
    {
        delete reference;
        reference = new Frame(*finits[i]);
        delete finits[i];
        delete output;

        if (i == 0)
            output = new Frame(*inits[0]);
        else
            output = new Frame(*r_fp->layer_getLastFrame());

        r_fp->Add_Layer(output, new Painter(i, 1, output, reference, 4000, 2));

        if (i % 100 == 0)
            cout << "       Layer f" << i << " Initialized..." << endl;
    }

    r_fp->Composite_All();

    //fade out fractal painterly frames
    r_fp->_layers.back()->fade_out(0, 166);


    cout << "   Executing Painterly over Camera..." << endl;

    Render *r_cp = new Render(scale, delay);

    //painting camera frames
    for (int i = 0; i < cinits.size(); i++)
    {
        delete reference;
        reference = new Frame(*cinits[i]);
        delete cinits[i];
        delete output;

        if (i == 0)
            output = new Frame(width, height);
        else
            output = new Frame(*r_cp->layer_getLastFrame());

        r_cp->Add_Layer(output, new Painter(i, 1, output, reference, 8000, 1));

        if (i % 100 == 0)
            cout << "       Layer c" << i << " Initialized..." << endl;
    }

    r_cp->Composite_All();
    r_cp->_layers.back()->fade_in(0, 166);

    r_fp->_layers.push_back(new Layer(r_cp->_layers.back()));

    ftrack = 1160;

    delete reference;
    delete output;

    reference = new Frame(*inits[4]);
    output = new Frame(*r_fp->layer_getLastFrame());

    r_fp->Add_Layer(output, new Painter(ftrack, 330, output, reference, 2000, 1));
    ftrack += 330;
    
    cout << "   Layer 3 Initialized..." << endl;


    delete reference;
    delete output;

    reference = new Frame(*inits[5]);
    output = new Frame(*r_fp->layer_getLastFrame());

    r_fp->Add_Layer(output, new Painter(ftrack, 130, output, reference, 2500, 1));
    ftrack += 130;

    cout << "   Layer 4 Initialized..." << endl;
    
    r_fp->Export("Submergence5", nullptr);
}

void Run_Fractal_Tree()
{
    int width = 1080;
    int height = 8640;
    int scale = 1;
    int delay = 3;

    ifstream in;
    ofstream out;
    Bitmap *image = new Bitmap;

    in.open("input/Submergence/pt3/in 3.bmp");
    in >> *image;
    in.close();

    Render* r = new Render(scale, delay);
    
    Frame* fb = new Frame(*image);   
    
    fb->set_mask(0);

    vector<vector<double>> params;

        vector<double> params1;
            params1.push_back(200);  //x
            params1.push_back(600);  //y
            params1.push_back(45);   //length
            params1.push_back(2);    //width
            params1.push_back(20);   //theta
            params1.push_back(2);    //branch
            params1.push_back(150);   //n
        params.push_back(params1);
            
        vector<double> params2;
            params2.push_back(400);  //x
            params2.push_back(2500);  //y
            params2.push_back(45);   //length
            params2.push_back(2);    //width
            params2.push_back(30);   //theta
            params2.push_back(2);    //branch
            params2.push_back(50);   //n
        params.push_back(params2);
            
        vector<double> params3;
            params3.push_back(880);  //x
            params3.push_back(5000);  //y
            params3.push_back(25);   //length
            params3.push_back(3);    //width
            params3.push_back(20);   //theta
            params3.push_back(2);    //branch
            params3.push_back(75);   //n
        params.push_back(params3);
            
        vector<double> params4;
            params4.push_back(980);  //x
            params4.push_back(5500);  //y
            params4.push_back(25);   //length
            params4.push_back(3);    //width
            params4.push_back(20);   //theta
            params4.push_back(2);    //branch
            params4.push_back(100);   //n
        params.push_back(params4);
            
        vector<double> params6;
            params6.push_back(1000);  //x
            params6.push_back(1800);  //y
            params6.push_back(35);   //length
            params6.push_back(3);    //width
            params6.push_back(30);   //theta
            params6.push_back(2);    //branch
            params6.push_back(100);   //n
        params.push_back(params6);
            
        vector<double> params5;
            params5.push_back(100);  //x
            params5.push_back(6500);  //y
            params5.push_back(35);   //length
            params5.push_back(3);    //width
            params5.push_back(30);   //theta
            params5.push_back(2);    //branch
            params5.push_back(125);   //n
        params.push_back(params5);
            
        vector<double> params7;
            params7.push_back(200);  //x
            params7.push_back(7500);  //y
            params7.push_back(35);   //length
            params7.push_back(2);    //width
            params7.push_back(20);   //theta
            params7.push_back(2);    //branch
            params7.push_back(20);   //n
        params.push_back(params7);
            
        vector<double> params8;
            params8.push_back(1050);  //x
            params8.push_back(8000);  //y
            params8.push_back(35);   //length
            params8.push_back(1);    //width
            params8.push_back(30);   //theta
            params8.push_back(2);    //branch
            params8.push_back(35);   //n
        params.push_back(params8);
            
        vector<double> params9;
            params9.push_back(50);  //x
            params9.push_back(4000);  //y
            params9.push_back(10);   //length
            params9.push_back(5);    //width
            params9.push_back(50);   //theta
            params9.push_back(2);    //branch
            params9.push_back(200);   //n
        params.push_back(params9);

    for (int i = 6; i < 9; i++)
    {
        cout << "   Executing kelp strand " << i << "..." << endl;

        if (i == 8)
            fb->set_mask(1);
        
        r->Add_Layer(fb, new Fractal_Kelp(0, 10 + i, width, height, params[i]));
    }

    r->Export("frac_tree", nullptr);

    /*
    in.open("input/in 0.bmp");
    in >> *image;
    in.close();

    image->settoFrame(r->layer_getLastFrame()->flip());

    out.open("output/frac_tree_out.bmp");
    out << *image;
    out.close();
    */
}

void Run_Fractal()
{
    int width = 1080;
    int height = 1080;
    int scale = 1;
    int delay = 3;

    ifstream in;
    ofstream out;
    Bitmap *image = new Bitmap;

    Frame* f = new Frame(width, height);
    f->_clear_w();

    Render* r = new Render(scale, delay);

    r->Add_Layer(f, new Fractal(0, 60, width, height, 0));

    string outfile = "output/frac_out.gif";

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width * scale, height * scale, delay);

    r->Export(gifw);

    GifEnd(&gifw);

/*
    in.open("input/in 1.bmp");
    in >> *image;
    in.close();

    image->settoFrame(r->layer_getLastFrame()->flip());

    out.open("output/frac_out.bmp");
    out << *image;
    out.close();
*/
}

void Run_Field()
{
    int width = 1080;
    int height = 1080;
    int scale = 1;
    int delay = 3;

    Frame* f = new Frame(width, height);
    f->_clear_w();

    string outfile = "out.gif";

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width * scale, height * scale, delay);

    Render* r = new Render(scale, delay);

    int frame_track = 0;
    int frame_dur = 21;

    r->Add_Layer(f, new Field(frame_track, frame_dur, width, height, 50, 100));

    Frame* f_end = r->layer_getLastFrame();
    frame_track += frame_dur;

    frame_dur = 32;
    r->Add_Layer(f_end, new Plotter(frame_track, frame_dur, width, height, 0, 0, 0));
    frame_track += frame_dur;

    for (int i = 0; i < 6; i++)
    {
        f_end = r->layer_getLastFrame();

        frame_dur = 21;
        r->Add_Layer(f_end, new Field(frame_track, frame_dur, width, height, 50, 100));
        frame_track += frame_dur;

        f_end = r->layer_getLastFrame();

        frame_dur = 32;
        r->Add_Layer(f_end, new Plotter(frame_track, frame_dur, width, height, 0, 0, 0));
        frame_track += frame_dur;
    }

    r->Export(gifw);

    GifEnd(&gifw);
}

void Run_Wanderer_p2()
{
    
/*
    piano in            1:32:53     1850    0
                          35:71     1914    64
                          38:76     1974    124
                          42:02     2040    190
                          43:63     2072    222
    2nd piano go-round  1:45:39     2106    256     34
                          48:58     2170    320     64
                          51:61     2232    382     62 
                          54:89     2298    448     64
    piano riff of myth  1:58:01     2360    510     62
    draws in and high   2:16:55     2730    880     370
                          22:80     2856    1006    126
    bass enters again   2:29:02     2980    1130    124
                          30:58             1162    32
                          32:15             1192    30
                          32:93             1208    16
                          33:69             1224    16
                          35:28             1256    32
                          36:83             1286    30
                          38:36             1316    30
                          40:02             1350    34
    bass riff of myth   2:41:59     3230    1380    30
    fade out begins     3:02:33     3646    1796    416 
    last note           3:23:76     4074    2224

*/
    vector<Frame*> inits;
    Bitmap *image;
    ofstream out;
    ifstream in;

    for (int n = 7; n <= 21; n++)
    {
        image = new Bitmap();

        in.open("input/Wanderer/in " + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    /* Camera zoom 1 */

    Space* s1 = new Space();
        s1->_t = 0;

    Camera* c1 = new Camera(0, 370, 1080, 1080, 1);
        c1->_s = s1;

        c1->_real_height = 1080;
        c1->_real_width = 1080;

        c1->_coord[0] = 0;
        c1->_coord[1] = 0;
        c1->_coord[2] = 10;

    Render fr(1, 5);
    
    int fwidth = 1080;
    int fheight = 1080;

    for (int i = 0; i <= 3; i++)
        fr.Add_Layer(inits[i + 5], new Plotter(i, 1, fwidth, fheight, 0, 0, 0));

    Face* f1;

    vector<double> cs1;

    cs1.push_back(0);
    cs1.push_back(0);
    cs1.push_back(1080);
    cs1.push_back(1080);

    for (int i = 0; i <= 0; i++)
    {
        f1 = new Face(s1, 1, 0, i, cs1, i);

        f1->_content = new Layer(fr.get_content());

        s1->_objects.push_back(f1);
    }

    int width = 1080;
    int height = 1080;

    int delay = 5;

    Frame* cinit1 = new Frame(width, height);

    Render cr1(1, delay);

    cout << "   Running Camera 1..." << endl;

    cr1.Add_Layer(cinit1, c1);
    
    cout << "   Transferring Frames..." << endl;
        
    Layer* cl1 = new Layer(cr1.get_content());
    vector<Frame*> cinits1;

    for (int i = 0; i < cl1->_frame_num; i++)
    {
        cinits1.push_back(new Frame(*cl1->_frames[i]));
        delete cl1->_frames[i];
    }

    /* Camera zoom 2 */

    Space* s2 = new Space();
        s2->_t = 0;

    Camera* c2 = new Camera(0, 416, 1080, 1080, 2);
        c2->_s = s2;

        c2->_real_height = 200;
        c2->_real_width = 200;

        c2->_coord[0] = 300;
        c2->_coord[1] = 350;
        c2->_coord[2] = 10;

    Render fr2(1, 5);

    fr2.Add_Layer(inits[10], new Plotter(0, 1, 1080, 1080, 0, 0, 0));
    fr2.Add_Layer(inits[11], new Plotter(1, 1, 1080, 1080, 0, 0, 0));

    Face* f2;
    Face* f3;

    vector<double> cs2;

    cs2.push_back(300);
    cs2.push_back(350);
    cs2.push_back(500);
    cs2.push_back(550);

    f2 = new Face(s2, 1, 0, 0, cs2, 1);

    f2->_content = new Layer(fr2.get_content());

    s2->_objects.push_back(f2);

    vector<double> cs3;

    cs3.push_back(0);
    cs3.push_back(0);
    cs3.push_back(1080);
    cs3.push_back(1080);

    f3 = new Face(s2, 1, 0, 1, cs3, 0);

    f3->_content = new Layer(fr2.get_content());

    s2->_objects.push_back(f3);

    Frame* cinit2 = new Frame(1080, 1080);

    Render cr2(1, 5);

    cout << "   Running Camera 2..." << endl;

    cr2.Add_Layer(cinit2, c2);
    
    cout << "   Transferring Frames..." << endl;
        
    Layer* cl2 = new Layer(cr2.get_content());
    vector<Frame*> cinits2;

    for (int i = 0; i < cl2->_frame_num; i++)
    {
        cinits2.push_back(new Frame(*cl2->_frames[i]));
        delete cl2->_frames[i];
    }

    /* painterly run */
    
    string outfile = "out.gif";

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width, height, 5);

    Render pr(1, delay);

    cout << "   Running Painterly..." << endl;

    Frame* reference;
    Frame* output;

    int frame_track = 0;
    int frame_count = 50;

    reference = new Frame(*inits[0]);
    output = new Frame(1080, 1080);
    output->_clear();

    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1, 11));
    frame_track += frame_count;

    cout << "       Layer p0 Initialized..." << endl;


    delete reference;
    reference = new Frame(*inits[1]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 60;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 10, 7));
    frame_track += frame_count;

    cout << "       Layer p1 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[2]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 66;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 5));
    frame_track += frame_count;

    cout << "       Layer p2 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 32;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 3));
    frame_track += frame_count;

    cout << "       Layer p3 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[0]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 32;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 1));
    frame_track += frame_count;

    cout << "       Layer p4 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[4]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 64;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 3));
    frame_track += frame_count;

    cout << "       Layer p5a Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[5]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 62;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 3));
    frame_track += frame_count;

    cout << "       Layer p5b Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[5]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 64;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 1));
    frame_track += frame_count;

    cout << "       Layer p6a Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[14]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 62;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 1));
    frame_track += frame_count;

    cout << "       Layer p6b Initialized..." << endl;

    for (int i = 0; i < 370; i++)
    {
        delete reference;
        reference = new Frame(*cinits1[i]);
        delete cinits1[i];
        delete output;
        output = new Frame(*pr.layer_getLastFrame());

        frame_count = 1;
        pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1500, 1));
        frame_track += frame_count;

        cout << "       Layer c1-" << i << " Initialized..." << endl;
    }

    delete reference;
    reference = new Frame(*inits[6]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 126;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 1));
    frame_track += frame_count;

    cout << "       Layer p7 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[6]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 124;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 3));
    frame_track += frame_count;

    cout << "       Layer p8 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[7]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 32;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 7));
    frame_track += frame_count;

    cout << "       Layer p9 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[8]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 30;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 3));
    frame_track += frame_count;

    cout << "       Layer p10 Initialized..." << endl;
    
    delete reference;
    reference = new Frame(*inits[9]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 16;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 7));
    frame_track += frame_count;

    cout << "       Layer p11 Initialized..." << endl;
    
    delete reference;
    reference = new Frame(*inits[1]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 16;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 11));
    frame_track += frame_count;

    cout << "       Layer p12 Initialized..." << endl;
    
    delete reference;
    reference = new Frame(*inits[8]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 32;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 1));
    frame_track += frame_count;

    cout << "       Layer p13 Initialized..." << endl;
    
    delete reference;
    reference = new Frame(*inits[9]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 30;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 3));
    frame_track += frame_count;

    cout << "       Layer p14 Initialized..." << endl;
    
    delete reference;
    reference = new Frame(*inits[1]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 30;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 3));
    frame_track += frame_count;

    cout << "       Layer p15 Initialized..." << endl;
    
    delete reference;
    reference = new Frame(*inits[10]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 34;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 100, 11));
    frame_track += frame_count;

    cout << "       Layer p16 Initialized..." << endl;
    
    delete reference;
    reference = new Frame(*inits[10]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 30;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 10000, 1));
    frame_track += frame_count;

    cout << "       Layer p17 Initialized..." << endl;
    
    for (int i = 0; i < 416; i++)
    {
        delete reference;
        reference = new Frame(*cinits2[i]);
        if (i != 415) delete cinits2[i];
        delete output;
        output = new Frame(*pr.layer_getLastFrame());

        frame_count = 1;
        pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 5000, 1));
        frame_track += frame_count;

        cout << "       Layer c2-" << i << " Initialized..." << endl;
    }

    delete reference;
    reference = new Frame(*cinits2[415]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 100;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 1));
    frame_track += frame_count;

    cout << "       Layer p8 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[12]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 104;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 1));
    frame_track += frame_count;

    cout << "       Layer p8 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[12]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 100;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 1));
    frame_track += frame_count;

    cout << "       Layer p8 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[13]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    frame_count = 124;
    pr.Add_Layer(output, new Painter(frame_track, frame_count, output, reference, 1000, 1));
    frame_track += frame_count;

    cout << "       Layer p8 Initialized..." << endl;


    cout << "Exporting gif..." << endl;

    pr.Export(gifw);

    GifEnd(&gifw);
}

void Run_Wanderer_p1()
{
/*
    first intro build   17:14       342
    bass enters         28:05       560
    2nd bass go-round   40:51       810
    tempo falls away    55:32       1106
    piano in            1:32:53     1850
*/
    vector<Frame*> inits;
    Bitmap *image;
    ofstream out;
    ifstream in;

    for (int n = 0; n <= 6; n++)
    {
        image = new Bitmap();

        in.open("input/Wanderer/in " + to_string(n) + ".bmp");
        in >> *image;
        in.close();

        Frame* f = new Frame(*image);

        inits.push_back(f);

        delete image;
    }

    Space* s = new Space();
        s->_t = 0;

    Camera* c = new Camera(0, 1508, 1080, 1080, 1);
        c->_s = s;

        c->_real_height = 1080;
        c->_real_width = 1080;

        c->_coord[0] = 0;
        c->_coord[1] = 0;
        c->_coord[2] = 10;
        
    Face* f1;
    Face* f2;
    
    Render* fr1 = new Render;

    int fs_width = 8600;
    int fs_height = 1080;

    fr1->Add_Layer(inits[0], new Plotter(0, 1, fs_width, fs_height, 0, 0, 0));

    Render* fr2 = new Render;

    int f2_width = 200;
    int f2_height = 200;

    for (int i = 0; i < 6; i++)
        fr2->Add_Layer(inits[i + 1], new Plotter(i, 1, f2_width, f2_height, 0, 0, 0));

    f1 = new Face(s, 1, 0, 0);

        Vertex* v1 = new Vertex(0, 0, 0);
        f1->_verts.push_back(v1);

        Vertex* v2 = new Vertex(0, 8600, 0);
        f1->_verts.push_back(v2);

        Vertex* v3 = new Vertex(1080, 8600, 0);
        f1->_verts.push_back(v3);

        Vertex* v4 = new Vertex(1080, 0, 0);
        f1->_verts.push_back(v4);

        v1->_conto.push_back(make_pair(v4, v2));
        v2->_conto.push_back(make_pair(v1, v3));
        v3->_conto.push_back(make_pair(v2, v4));
        v4->_conto.push_back(make_pair(v3, v1));
        
        f1->_normal[0] = 0;
        f1->_normal[1] = 0;
        f1->_normal[2] = 1;

        f1->_faces.push_back(f1);

        f1->_content = new Layer(fr1->get_content());



    f2 = new Face(s, 1, 0, 1);

        int w_off = 325;
        int h_off = 375;

        Vertex* v1a = new Vertex(0 + w_off, 0 + h_off, 1);
        f2->_verts.push_back(v1a);

        Vertex* v2a = new Vertex(0 + w_off, 200 + h_off, 1);
        f2->_verts.push_back(v2a);

        Vertex* v3a = new Vertex(200 + w_off, 200 + h_off, 1);
        f2->_verts.push_back(v3a);

        Vertex* v4a = new Vertex(200 + w_off, 0 + h_off, 1);
        f2->_verts.push_back(v4a);

        v1a->_conto.push_back(make_pair(v4a, v2a));
        v2a->_conto.push_back(make_pair(v1a, v3a));
        v3a->_conto.push_back(make_pair(v2a, v4a));
        v4a->_conto.push_back(make_pair(v3a, v1a));
        
        f2->_normal[0] = 0;
        f2->_normal[1] = 0;
        f2->_normal[2] = 1;

        f2->_faces.push_back(f2);

        f2->_content = new Layer(fr2->get_content());

        s->_objects.push_back(f2);
        s->_objects.push_back(f1);
        


/*
    for (int i = 0; i < 10; i++)
        fr->Add_Layer(pinits[i + 2], new Plotter(i, 1, fwidth, fheight, 0, 0, 0));
    
    for (int i = 4; i >= 0; i--)
    {
        int scale = 10;

        f1 = new Face(s, pow(scale, i) * 5.4, 0, i);

        double c1 = 100 - ((double) 100) * pow(scale, -i);
        double c2 = 100 + ((double) 100) * pow(scale, -i);

        Vertex* v1 = new Vertex(c1, c1, i);
        f1->_verts.push_back(v1);

        Vertex* v2 = new Vertex(c1, c2, i);
        f1->_verts.push_back(v2);

        Vertex* v3 = new Vertex(c2, c2, i);
        f1->_verts.push_back(v3);

        Vertex* v4 = new Vertex(c2, c1, i);
        f1->_verts.push_back(v4);

        v1->_conto.push_back(make_pair(v4, v2));
        v2->_conto.push_back(make_pair(v1, v3));
        v3->_conto.push_back(make_pair(v2, v4));
        v4->_conto.push_back(make_pair(v3, v1));
        
        f1->_normal[0] = 0;
        f1->_normal[1] = 0;
        f1->_normal[2] = 1;

        f1->_faces.push_back(f1);

        f1->_content = new Layer(fr->get_content());

        fs.push_back(f1);
    }

    for (auto f : fs)
        c->_fs.push_back(f);

    s->_objects.push_back(fs[3]);
    s->_objects.push_back(fs[4]);
    */

    int width = 1080;
    int height = 1080;

    int delay = 5;

    Frame* cinit = new Frame(width, height);

    Render cr(1, delay);

    cout << "   Running Camera..." << endl;

    cr.Add_Layer(cinit, c);
    
    string coutfile = "cout.gif";

    GifWriter cgifw;
    GifBegin(&cgifw, coutfile.c_str(), width, height, 5);

    cr.Export(cgifw);
    GifEnd(&cgifw);

    Layer* cl = new Layer(cr.get_content());
    vector<Frame*> pinits;

    for (int i = 0; i < cl->_frame_num; i++)
    {
        pinits.push_back(new Frame(*cl->_frames[i]));
        delete cl->_frames[i];
    }
    
    string outfile = "out.gif";

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width, height, 5);

    Render pr(1, delay);

    cout << "   Running Painterly..." << endl;

    Frame* reference;
    Frame* output;

    int frame_track = 0;

    reference = new Frame(*pinits[0]);
    output = new Frame(1080, 1080);
    output->_clear();

    pr.Add_Layer(output, new Painter(frame_track, 100, output, reference, 100, 11));

    frame_track += 100;

    cout << "       Layer p0 - 0 Initialized..." << endl;

    delete reference;
    reference = new Frame(*pinits[0]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    pr.Add_Layer(output, new Painter(frame_track, 100, output, reference, 25, 7));

    frame_track += 100;

    cout << "       Layer p0 - 1 Initialized..." << endl;

    delete reference;
    reference = new Frame(*pinits[0]);
    delete output;
    output = new Frame(*pr.layer_getLastFrame());

    pr.Add_Layer(output, new Painter(frame_track, 142, output, reference, 100, 3));

    frame_track += 142;

    cout << "       Layer p0 - 2 Initialized..." << endl;


    for (int i = 1; i < pinits.size(); i++)
    {
        delete reference;
        reference = new Frame(*pinits[i]);
        delete output;
        output = new Frame(*pr.layer_getLastFrame());

        pr.Add_Layer(output, new Painter(frame_track, 1, output, reference, 5000, 1));

        frame_track += 1;

        cout << "       Layer p" << i << " Initialized..." << endl;
    }


    cout << "Exporting gif..." << endl;

    pr.Export(gifw);

    GifEnd(&gifw);
}

void Run_image_gen()
{
    int height = 10;
    int depth = 10;
    int k = 10;

    Image_Gen ig(height, depth, k, "image_gen/wnbs.csv");
}

void Run_Kmeans_Sorter()
{
    ofstream out;
    ifstream in;
    Bitmap *image = new Bitmap();

    int k_array[6] = {1, 2, 3, 4, 8, 16};

    for (int i = 1; i <= 55; i++)
    {   
        cout << "Run " << i << endl;

        image = new Bitmap();
        string instring("output/Almond Collage/good/" + to_string(i) + ".bmp");
        in.open(instring);
        in >> *image;
        in.close();

        Frame *f = new Frame(*image);
        
        for (int j = 0; j < 6; j++)
            Kmeans *K = new Kmeans(0, 0, f, image, k_array[j], i, BLUE);

        delete image;
        delete f;
    }   
}

void Run_Sorter()
{
    vector<string> strs;
        strs.push_back("pron");
        strs.push_back("Island");
        strs.push_back("color pron");
        strs.push_back("nature pics");

    vector<int> bounds;
        bounds.push_back(33);
        bounds.push_back(71);
        bounds.push_back(1);
        bounds.push_back(19);
        bounds.push_back(72);
        bounds.push_back(150);
        bounds.push_back(1);
        bounds.push_back(13);

    for (int n = 0; n < 4; n++)
    {
        for (int i = bounds[n * 2]; i < bounds[n * 2 + 1]; i++)
        {
            cout << strs[n] << ": run " << i << endl;
            ifstream in;
            ofstream out;

            string file_name("input/" + strs[n] + "/in " + to_string(i) + ".bmp");

            cout << "   Reading bitmap..." << endl;

            Bitmap* image = new Bitmap();

            in.open(file_name, ios::binary);
            in >> *image;
            in.close();

            Frame* f = new Frame(*image);

            Render* r = new Render(1, 2);

            cout << "   Executing Sort..." << endl;

            r->Add_Layer(f, new Sorter(0, f->_height, f->_width, f->_height, BLUE));

            Frame* of = new Frame(*r->layer_getLastFrame());

            image->settoFrame(of->flip());

            string outfile = "output/" + strs[n] + " out " + to_string(i) + ".bmp";

            cout << "   Saving bitmap..." << endl;

            out.open(outfile, ios::binary);
            out << *image;
            out.close();

            delete r;
            delete f;
            delete of;
            delete image;
        }
    }
}

void Run_3D_Telescope_Test()
{
    ifstream in;
    vector<Frame*> pinits;

    for (int i = 0; i <= 14; i++)
    {
        string file_name("input/Eros/" + to_string(i) + ".bmp");

        Bitmap* image = new Bitmap();

        in.open(file_name, ios::binary);
        in >> *image;
        in.close();

        if (i > 11 || i < 3)
            pinits.push_back(new Frame(*image));
        else 
            pinits.push_back(image_flip(new Frame(*image)));

        delete image;
    }

    Space* s = new Space();
        s->_t = 0;

    Camera* c = new Camera(0, 680, 1080, 1080, 1);
        c->_s = s;

        c->_real_height = 200;
        c->_real_width = 200;

        c->_coord[0] = 0;
        c->_coord[1] = 0;
        c->_coord[2] = 10;
        
    vector<Face*> fs;
    Face* f1;
    
    Render* fr = new Render;

        int fwidth = 1080;
        int fheight = 1080;

        for (int i = 0; i < 10; i++)
            fr->Add_Layer(pinits[i + 2], new Plotter(i, 1, fwidth, fheight, 0, 0, 0));
    
    for (int i = 4; i >= 0; i--)
    {
        int scale = 10;

        f1 = new Face(s, pow(scale, i) * 5.4, 0, i);

        double c1 = 100 - ((double) 100) * pow(scale, -i);
        double c2 = 100 + ((double) 100) * pow(scale, -i);

        Vertex* v1 = new Vertex(c1, c1, i);
        f1->_verts.push_back(v1);

        Vertex* v2 = new Vertex(c1, c2, i);
        f1->_verts.push_back(v2);

        Vertex* v3 = new Vertex(c2, c2, i);
        f1->_verts.push_back(v3);

        Vertex* v4 = new Vertex(c2, c1, i);
        f1->_verts.push_back(v4);

        v1->_conto.push_back(make_pair(v4, v2));
        v2->_conto.push_back(make_pair(v1, v3));
        v3->_conto.push_back(make_pair(v2, v4));
        v4->_conto.push_back(make_pair(v3, v1));
        
        f1->_normal[0] = 0;
        f1->_normal[1] = 0;
        f1->_normal[2] = 1;

        f1->_faces.push_back(f1);

        f1->_content = new Layer(fr->get_content());

        fs.push_back(f1);
    }

    for (auto f : fs)
        c->_fs.push_back(f);

    s->_objects.push_back(fs[3]);
    s->_objects.push_back(fs[4]);

    int width = 1080;
    int height = 1080;

    int delay = 5;

    Frame* cinit = new Frame(width, height);

    Render cr(1, delay);

    cout << "   Running Camera..." << endl;

    cr.Add_Layer(cinit, c);

    Layer* cl = new Layer(cr.get_content());
    vector<Frame*> inits;

    for (int i = 0; i < cl->_frame_num; i++)
    {
        inits.push_back(new Frame(*cl->_frames[i]));
    }

/***********************************************************************
 * PAINTERLY SETLIST
 * 
    //script...
    //  0       beginning
    //  150     beeyo beeyo bwah baa
    //  225     doo-doo doo (bah bah bah) 1
    //  300     song opens 1 (pickup)
    //  318     song opens 1
    //  600     drums in
    //  825     doo-doo doo (bah bah bah) 2
    //  900     song opens 2 (pickup)
    //  918     song opens 2
    //  993     tempo break
    //  1053    flute stab
    //  1127    strumming in
    //  1427    drums out (mostly)
    //  1783    strumming out
    //  2025    song over
*/


    string outfile = "out.gif";

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width, height, 2);

    Render r(1, 10);

    cout << "   Running Painterly..." << endl;

    Frame* reference;
    Frame* output;

    int frame_track = 0;

    reference = new Frame(*pinits[0]);
    output = new Frame(1080, 1080);
    output->_clear_w();

    r.Add_Layer(output, new Painter(frame_track, 150, output, reference, 100, 3));

    frame_track += 150;

    cout << "       Layer p" << 0 << " Initialized..." << endl;

    delete reference;
    reference = new Frame(*pinits[1]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    r.Add_Layer(output, new Painter(frame_track, 75, output, reference, 500, 3));

    frame_track += 75;

    cout << "       Layer p" << 1 << " Initialized..." << endl;

    delete reference;
    reference = new Frame(*pinits[2]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    r.Add_Layer(output, new Painter(frame_track, 75, output, reference, 1000, 3));

    frame_track += 75;

    cout << "       Layer p" << 2 << " Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[0]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    r.Add_Layer(output, new Painter(frame_track, 18, output, reference, 10000, 1));

    frame_track += 18;

    cout << "       Layer p" << 3 << " Initialized..." << endl;

    for (int i = 0; i < inits.size(); i++)
    {
        int fnum;

        if (i < 150)
            fnum = 10000;
        else if (i < 450)
            fnum = 10000;
        else
            fnum = 12000;

        delete reference;
        reference = new Frame(*inits[i]);
        delete output;
        output = new Frame(*r.layer_getLastFrame());
        
        r.Add_Layer(output, new Painter(frame_track, 2, output, reference, fnum, 1));

        frame_track += 2;

        cout << "       Layer c" << i << " Initialized..." << endl;
    }

    delete reference;
    reference = new Frame(*pinits[12]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    r.Add_Layer(output, new Painter(frame_track, 120, output, reference, 1000, 1));

    frame_track += 120;

    cout << "       Layer p" << 12 << " Initialized..." << endl;

    delete reference;
    reference = new Frame(*pinits[13]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    r.Add_Layer(output, new Painter(frame_track, 158, output, reference, 2000, 1));

    frame_track += 158;

    cout << "       Layer p" << 13 << " Initialized..." << endl;

    delete reference;
    reference = new Frame(*pinits[14]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    r.Add_Layer(output, new Painter(frame_track, 100, output, reference, 100, 1));

    frame_track += 100;

    cout << "       Layer p" << 14 << " Initialized..." << endl;



    cout << "   Compositing and Exporting Gif..." << endl;

    r.Export(gifw);

    GifEnd(&gifw);
}

void Run_Eros()
{
    cout << "Initializing Gif..." << endl;

    ifstream in;

    vector <Frame*> inits;
 
    string outfile("output/Eros.gif");
    
    int i = 0;

    for (i = 1; i <= 48; i++)
    {
        string file_name("input/Eros/in " + to_string(i) + ".bmp");

        Bitmap* image = new Bitmap();

        in.open(file_name, ios::binary);
        in >> *image;
        in.close();

        inits.push_back(new Frame(*image));

        delete image;
    }

#ifdef DEBUG
    cout << "Seed Images Loaded" << endl;
#endif

    int width = inits[0]->_width;
    int height = inits[0]->_height;
    int delay = 10;

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width, height, delay);

#ifdef DEBUG
    cout << "Gif Writer Created..." << endl;
#endif

    vector<int> temp_layers;
    vector<int> layers;
    Render r(1);

    cout << "Scripting Gif..." << endl;

    Frame* reference = new Frame(*inits[0]);
    Frame* output = new Frame(width, height);

    output->_clear_w();

    int frame_track = 0;

}

vector<vector<vector<double>>> get_trigs(vector<vector<double>> coords)
{
    vector<double> new_coords;
        new_coords.push_back((coords[0][0] + coords[1][0] + coords[2][0]) / 3);
        new_coords.push_back((coords[0][1] + coords[1][1] + coords[2][1]) / 3);
        new_coords.push_back((coords[0][2] + coords[1][2] + coords[2][2]) / 3);
        

    vector<vector<vector<double>>> trigs;

        vector<vector<double>> trig_a;
            vector<double> trig_a_a(3, 0);
            trig_a.push_back(trig_a_a);
            vector<double> trig_a_b(3, 0);
            trig_a.push_back(trig_a_b);
            vector<double> trig_a_c(3, 0);
            trig_a.push_back(trig_a_c);
        trigs.push_back(trig_a);

        vector<vector<double>> trig_b;
            vector<double> trig_b_a(3, 0);
            trig_b.push_back(trig_b_a);
            vector<double> trig_b_b(3, 0);
            trig_b.push_back(trig_b_b);
            vector<double> trig_b_c(3, 0);
            trig_b.push_back(trig_b_c);
        trigs.push_back(trig_b);
        
        vector<vector<double>> trig_c;
            vector<double> trig_c_a(3, 0);
            trig_c.push_back(trig_c_a);
            vector<double> trig_c_b(3, 0);
            trig_c.push_back(trig_c_b);
            vector<double> trig_c_c(3, 0);
            trig_c.push_back(trig_c_c);
        trigs.push_back(trig_c);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                //assigns coordinates of 3 new triangles
                //  where one coordinate from each is assigned to new_coords
                //  alternating, so that each of three possible triangles are described
                if (i != j)
                    trigs[i][j][k] = coords[j][k];
                else
                    trigs[i][j][k] = new_coords[k];
            }
        }
    }    

    return trigs;
}

void Run_Kaleidoscope()
{
    Space* s = new Space();
        s->_t = 0;

    Camera* c = new Camera(0, 200, 1000, 1000, 1);
        c->_s = s;

        c->_real_height = 100;
        c->_real_width = 100;

        c->_coord[0] = -50;
        c->_coord[1] = -50;
        c->_coord[2] = 5;

        c->_delta_c[0] = 0;
        c->_delta_c[1] = 0;

        c->_angle[0] = 0;
        c->_angle[1] = 0;
        c->_angle[2] = -1;
        c->_angle[3] = 0;
        c->_angle[4] = 1;
        c->_angle[5] = 0;
        c->_angle[6] = 1;
        c->_angle[7] = 0;
        c->_angle[8] = 0;
        
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

            int fwidth = 1000;
            int fheight = 1000;

            Frame* init_1 = new Frame(fwidth, fheight);
            vector<Frame*> inits;

            vector<vector<int>> args;
            vector<int> args_1;
                args_1.push_back(0);
                args_1.push_back(2);
                args_1.push_back(1);
                args_1.push_back(1);
                args_1.push_back(1);
            args.push_back(args_1);
                
            vector<int> args_2;
                args_2.push_back(2);
                args_2.push_back(3);
                args_2.push_back(3);
                args_2.push_back(3);
                args_2.push_back(1);
            args.push_back(args_2);
                
            vector<int> args_3;
                args_3.push_back(2);
                args_3.push_back(4);
                args_3.push_back(3);
                args_3.push_back(3);
                args_3.push_back(1);
            args.push_back(args_3);
                
            vector<int> args_4;
                args_4.push_back(0);
                args_4.push_back(1);
                args_4.push_back(1);
                args_4.push_back(1);
                args_4.push_back(1);
            args.push_back(args_4);

            vector<int> args_5;
                args_5.push_back(5);
                args_5.push_back(8);
                args_5.push_back(3);
                args_5.push_back(3);
                args_5.push_back(1);
            args.push_back(args_5);

            vector<int> args_6;
                args_6.push_back(3);
                args_6.push_back(3);
                args_6.push_back(3);
                args_6.push_back(4);
                args_6.push_back(1);
            args.push_back(args_6);

            vector<int> args_7;
                args_7.push_back(6);
                args_7.push_back(7);
                args_7.push_back(2);
                args_7.push_back(3);
                args_7.push_back(1);
            args.push_back(args_7);

            vector<int> args_8;
                args_8.push_back(1);
                args_8.push_back(5);
                args_8.push_back(3);
                args_8.push_back(3);
                args_8.push_back(1);
            args.push_back(args_8);

            vector<int> args_9;
                args_9.push_back(1);
                args_9.push_back(5);
                args_9.push_back(4);
                args_9.push_back(5);
                args_9.push_back(1);
            args.push_back(args_9);

            vector<int> args_10;
                args_10.push_back(4);
                args_10.push_back(8);
                args_10.push_back(3);
                args_10.push_back(3);
                args_10.push_back(1);
            args.push_back(args_10);

            vector<int> args_11;
                args_11.push_back(3);
                args_11.push_back(6);
                args_11.push_back(2);
                args_11.push_back(4);
                args_11.push_back(1);
            args.push_back(args_11);

            vector<int> args_12;
                args_12.push_back(3);
                args_12.push_back(5);
                args_12.push_back(3);
                args_12.push_back(4);
                args_12.push_back(1);
            args.push_back(args_12);

            cout << "   Running Events..." << endl;

            fr.Add_Layer(init_1, new Plotter(0, 1, fwidth, fheight, 1, 8, 1));

            inits.push_back(new Frame(*fr.layer_getLastFrame()));

            int n = rand() % 6;

            fr.Add_Layer(inits.back(), new Cellular(1, 200, fwidth, fheight, 1, args[n]));

        f1->_content = new Layer(fr.get_content());

    int cwidth = 1000;
    int cheight = 1000;

    Frame* cinit = new Frame(cwidth, cheight);

    Render cr(1);

    cout << "   Running Camera 1..." << endl;

    cr.Add_Layer(cinit, c);

    cout << "   Calculating triangulation..." << endl;

    Space* s2 = new Space();
        s2->_t = 0;

        vector<vector<vector<double>>> verts;
            vector<vector<double>> init_v;
                vector<double> init_a;
                    init_a.push_back(-1000);
                    init_a.push_back(-1000);
                    init_a.push_back(0);
                init_v.push_back(init_a);
                vector<double> init_b;
                    init_b.push_back(0);
                    init_b.push_back(1000);
                    init_b.push_back(0);
                init_v.push_back(init_b);
                vector<double> init_c;
                    init_c.push_back(1000);
                    init_c.push_back(-1000);
                    init_c.push_back(0);
                init_v.push_back(init_c);
            verts.push_back(init_v);

        for (int i = 0; i < 12; i++)
        {
            vector<vector<double>> temp(verts.back());
            verts.pop_back();
            vector<vector<vector<double>>> trigs = get_trigs(temp);
            verts.insert(verts.begin(), trigs.begin(), trigs.end());
        }

        for (int i = 0; i < verts.size(); i++)
        {
            Face* f = new Face(s2, 1, 0);
                s2->_objects.push_back(f);

                v1 = new Vertex(verts[i][0][0], verts[i][0][1], verts[i][0][2]);
                v2 = new Vertex(verts[i][1][0], verts[i][1][1], verts[i][1][2]);
                v3 = new Vertex(verts[i][2][0], verts[i][2][1], verts[i][2][2]);
                
                if (i % 3 == 0) {
                    f->_verts.push_back(v1);
                    f->_verts.push_back(v2);
                    f->_verts.push_back(v3);
                } else if (i % 3 == 1) {
                    f->_verts.push_back(v2);
                    f->_verts.push_back(v3);
                    f->_verts.push_back(v1);
                } else {
                    f->_verts.push_back(v3);
                    f->_verts.push_back(v1);
                    f->_verts.push_back(v2);
                }
                
                v1->_conto.push_back(make_pair(v2, v3));
                v2->_conto.push_back(make_pair(v3, v1));
                v3->_conto.push_back(make_pair(v1, v2));

                f->_normal[0] = 0;
                f->_normal[1] = 0;
                f->_normal[2] = 1;

                f->_faces.push_back(f);

                f->_content = cr.get_content();
        }

    Camera* c2 = new Camera(0, 1000, 800, 800, 2);
        c2->_s = s2;

        c2->_real_height = 100;
        c2->_real_width = 100;

        c2->_coord[0] = -50;
        c2->_coord[1] = -50;
        c2->_coord[2] = 5;

        c2->_delta_c[0] = 0;
        c2->_delta_c[1] = 0;

        c2->_angle[0] = 0;
        c2->_angle[1] = 0;
        c2->_angle[2] = -1;
        c2->_angle[3] = 0;
        c2->_angle[4] = 1;
        c2->_angle[5] = 0;
        c2->_angle[6] = 1;
        c2->_angle[7] = 0;
        c2->_angle[8] = 0;

    string outfile = "out.gif";

    int width = 1600;
    int height = 1600;

    int delay = 5;

    Frame* init = new Frame(width, height);

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width, height, delay);

    Render r(2, delay);

    cout << "   Running Camera 2..." << endl;

    r.Add_Layer(init, c2);
    
    cout << "   Compositing and Exporting Gif..." << endl;

    r.Export(gifw);

    GifEnd(&gifw);
}

void Run_cellular_flyby()
{
    Space* s = new Space();
        s->_t = 0;

    Camera* c = new Camera(0, 2000, 800, 800, 1);
        c->_s = s;

        c->_real_height = 100;
        c->_real_width = 100;

        c->_coord[0] = -50;
        c->_coord[1] = -50;
        c->_coord[2] = 5;

        c->_delta_c[0] = 0;
        c->_delta_c[1] = 0;

        c->_angle[0] = 0;
        c->_angle[1] = 0;
        c->_angle[2] = -1;
        c->_angle[3] = 0;
        c->_angle[4] = 1;
        c->_angle[5] = 0;
        c->_angle[6] = 1;
        c->_angle[7] = 0;
        c->_angle[8] = 0;
        
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
            vector<int> args_1;
                args_1.push_back(0);
                args_1.push_back(2);
                args_1.push_back(1);
                args_1.push_back(1);
                args_1.push_back(1);
            args.push_back(args_1);
                
            vector<int> args_2;
                args_2.push_back(2);
                args_2.push_back(3);
                args_2.push_back(3);
                args_2.push_back(3);
                args_2.push_back(1);
            args.push_back(args_2);
                
            vector<int> args_3;
                args_3.push_back(2);
                args_3.push_back(4);
                args_3.push_back(3);
                args_3.push_back(3);
                args_3.push_back(1);
            args.push_back(args_3);
                
            vector<int> args_4;
                args_4.push_back(0);
                args_4.push_back(1);
                args_4.push_back(1);
                args_4.push_back(1);
                args_4.push_back(1);
            args.push_back(args_4);

            vector<int> args_5;
                args_5.push_back(5);
                args_5.push_back(8);
                args_5.push_back(3);
                args_5.push_back(3);
                args_5.push_back(1);
            args.push_back(args_5);

            vector<int> args_6;
                args_6.push_back(3);
                args_6.push_back(3);
                args_6.push_back(3);
                args_6.push_back(4);
                args_6.push_back(1);
            args.push_back(args_6);

            vector<int> args_7;
                args_7.push_back(6);
                args_7.push_back(7);
                args_7.push_back(2);
                args_7.push_back(3);
                args_7.push_back(1);
            args.push_back(args_7);

            vector<int> args_8;
                args_8.push_back(1);
                args_8.push_back(5);
                args_8.push_back(3);
                args_8.push_back(3);
                args_8.push_back(1);
            args.push_back(args_8);

            vector<int> args_9;
                args_9.push_back(1);
                args_9.push_back(5);
                args_9.push_back(4);
                args_9.push_back(5);
                args_9.push_back(1);
            args.push_back(args_9);

            vector<int> args_10;
                args_10.push_back(4);
                args_10.push_back(8);
                args_10.push_back(3);
                args_10.push_back(3);
                args_10.push_back(1);
            args.push_back(args_10);

            vector<int> args_11;
                args_11.push_back(3);
                args_11.push_back(6);
                args_11.push_back(2);
                args_11.push_back(4);
                args_11.push_back(1);
            args.push_back(args_11);

            vector<int> args_12;
                args_12.push_back(3);
                args_12.push_back(5);
                args_12.push_back(3);
                args_12.push_back(4);
                args_12.push_back(1);
            args.push_back(args_12);
        
        /*
            vector<int> args_1;
                args_1.push_back(1);
                args_1.push_back(3);
                args_1.push_back(1);
                args_1.push_back(3);
                args_1.push_back(4);
            args.push_back(args_1);

            vector<int> args_2;
                args_2.push_back(1);
                args_2.push_back(3);
                args_2.push_back(1);
                args_2.push_back(5);
                args_2.push_back(4);
            args.push_back(args_2);

            vector<int> args_3;
                args_3.push_back(1);
                args_3.push_back(3);
                args_3.push_back(1);
                args_3.push_back(5);
                args_3.push_back(4);
            args.push_back(args_3);

            vector<int> args_4;
                args_4.push_back(3);
                args_4.push_back(4);
                args_4.push_back(1);
                args_4.push_back(5);
                args_4.push_back(4);
            args.push_back(args_4);

            vector<int> args_5;
                args_5.push_back(3);
                args_5.push_back(4);
                args_5.push_back(2);
                args_5.push_back(7);
                args_5.push_back(4);
            args.push_back(args_5);

            vector<int> args_6;
                args_6.push_back(3);
                args_6.push_back(7);
                args_6.push_back(2);
                args_6.push_back(7);
                args_6.push_back(4);
            args.push_back(args_6);
            */

            cout << "   Running Events..." << endl;


            for (int i = 0; i < 5; i++)
            {
                fr.Add_Layer(init_1, new Plotter(0 + 200 * i, 1, fwidth, fheight, 1, 8, 4));

                inits.push_back(new Frame(*fr.layer_getLastFrame()));

                int r = rand() % 6;

                fr.Add_Layer(inits.back(), new Cellular(1 + 200 * i, 200, fwidth, fheight, 1, args[r]));
            }
            
            /*
                fr.Add_Layer(init_1, new Plotter(0, 1, fwidth, fheight, 1, 5));

                init_2 = new Frame(*fr.layer_getLastFrame());

                fr.Add_Layer(init_2, new Cellular(1, 400, fwidth, fheight, 1, args[0]));
            */

        f1->_content = new Layer(fr.get_content());


    //write gif

    string outfile = "out.gif";

    int width = 1600;
    int height = 1600;

    int delay = 5;

    Frame* init = new Frame(width, height);

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width, height, delay);

    Render r(2, delay);

    cout << "   Running Camera..." << endl;

    r.Add_Layer(init, c);
    
    cout << "   Compositing and Exporting Gif..." << endl;

    r.Export(gifw);

    GifEnd(&gifw);
}

void Run_Evolutionary()
{
    cout << "Initializing Gif..." << endl;

    ifstream in;

    vector <Frame*> inits;
 
    string outfile("output/Evolutionary.gif");
    
    int i = 0;

    for (i = 1; i <= 48; i++)
    {
        string file_name("input/Evolutionary/in " + to_string(i) + ".bmp");

        Bitmap* image = new Bitmap();

        in.open(file_name, ios::binary);
        in >> *image;
        in.close();

        inits.push_back(new Frame(*image));

        delete image;
    }

#ifdef DEBUG
    cout << "Seed Images Loaded" << endl;
#endif

    int width = inits[0]->_width;
    int height = inits[0]->_height;
    int delay = 10;

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width, height, delay);

#ifdef DEBUG
    cout << "Gif Writer Created..." << endl;
#endif

    vector<int> temp_layers;
    vector<int> layers;
    Render r(1);

    cout << "Scripting Gif..." << endl;

    Frame* reference = new Frame(*inits[0]);
    Frame* output = new Frame(width, height);

    output->_clear_w();

    int frame_track = 0;

    //script...
    //  0       beginning
    //  642     marimba rhythm enters
    //  986     bass/shakers enter
    //  1479    marimba groove enters
    //  1650    drums/samples enter
    //  1821    flutes/snare enter
    //  2336    fade out begins
    //  2655    song ends

    int size[10] = {5, 5, 4, 4, 3, 3, 2, 2, 1, 1};
    int numb[10] = {10, 20, 20, 25, 25, 50, 50, 100, 100, 200};


    for (i = 1; i <= 10; i++)
    {
        if (i != 1)
        {
            delete reference;
            reference = new Frame(*inits[i - 1]);
            delete output;
            output = new Frame(*r.layer_getLastFrame());
        }
        
        layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 55, output, reference, numb[i - 1], size[i - 1])));

        frame_track += 55;

        cout << "Layer " << i << " Initialized..." << endl;
    }

    //frametrack = 550
    
    i = 11;
    delete reference;
    reference = new Frame(*inits[i - 1]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 92, output, reference, 500, 1)));

    frame_track += 92;

    cout << "Layer " << i << " Initialized..." << endl;

    //frametrack = 642

    int sizes[7] = {11, 9, 7, 6, 5, 4, 3};
    int numbs[7] = {50, 100, 150, 250, 350, 500, 750};
    int frames;

    for (i = 12; i <= 18; i++)
    {
        delete reference;
        reference = new Frame(*inits[i - 1]);
        delete output;
        output = new Frame(*r.layer_getLastFrame());
        
        if (i == 18)
            frames = 123;
        else
            frames = 119;

        layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, frames, output, reference, numbs[i - 12], sizes[i - 12])));

        frame_track += frames;

        cout << "Layer " << i << " Initialized..." << endl;
    }

    //frametrack = 1479

    for (i = 19; i <= 43; i++)
    {
        delete reference;
        reference = new Frame(*inits[i - 1]);
        delete output;
        output = new Frame(*r.layer_getLastFrame());

        if (i == 43)
            frames = 41;
        else
            frames = 34;

        layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, frames, output, reference, 2000, 1)));

        frame_track += frames;

        cout << "Layer " << i << " Initialized..." << endl;
    }

    //frametrack = 2336
    
    i = 44;
    delete reference;
    reference = new Frame(*inits[i - 1]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 19, output, reference, 2000, 1)));

    frame_track += 19;

    cout << "Layer " << i << " Initialized..." << endl;

    for (i = 45; i <= 46; i++)
    {
        delete reference;
        reference = new Frame(*inits[i - 1]);
        delete output;
        output = new Frame(*r.layer_getLastFrame());
        
        layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 100, output, reference, 2000, 1)));

        frame_track += 100;

        cout << "Layer " << i << " Initialized..." << endl;
    }

    for (i = 47; i <= 48; i++)
    {
        delete reference;
        reference = new Frame(*inits[i - 1]);
        delete output;
        output = new Frame(*r.layer_getLastFrame());
        
        layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 50, output, reference, 2000, 1)));

        frame_track += 50;

        cout << "Layer " << i << " Initialized..." << endl;
    }

#ifdef DEBUG
    cout << "layers contains: ";

    for (auto l : layers) cout << l << ", ";

    cout << endl;
#endif

    delete output;
    delete reference;
    for (auto f : inits) delete f;

    cout << "Compositing and Exporting Gif..." << endl;

    r.Export(gifw);

    GifEnd(&gifw);

    cout << "...Gif Written" << endl;

}

void Run_Plotter()
{
    ofstream out;
    ifstream in;
    Bitmap *image = new Bitmap();

    in.open("input/in 1.bmp");
    in >> *image;
    in.close();

    Frame *f = new Frame(*image);

    Plotter *p = new Plotter(0, 0, f->_width, f->_height, 2, 0, 0);

    p->Activate(f, nullptr);

    image->settoFrame(f->flip());

    out.open("out.bmp");
    out << *image;
    out.close();
}

void Run_KMeans_Rectangulate()
{
    ofstream out;
    ifstream in;
    Bitmap *image = new Bitmap();

    int k_array[6] = {2, 3, 4, 8, 16, 32};

    for (int i = 1; i <= 19; i++)
    {
        image = new Bitmap();
        string instring("input/in " + to_string(i) + ".bmp");
        in.open(instring);
        in >> *image;
        in.close();

        Frame *f = new Frame(*image);
        
        for (int j = 0; j < 6; j++)
            Kmeans *K = new Kmeans(0, 0, f, image, k_array[j], i);

        delete image;
        delete f;
    }
}

void Run_KMeans()
{
    ofstream out;
    ifstream in;
    Bitmap *image1 = nullptr, *image2 = nullptr;

    Kmeans *K = nullptr;

    string dID[4] = {"0000", "000", "00", "0"};

    for (int j = 0; j < 4; j++)
    {
        if (image1) delete image1;
        image1 = new Bitmap();

        in.open("input/in " + dID[j] + ".bmp", ios::binary);
        in >> *image1;
        in.close();
        
        Frame *f1 = new Frame(*image1);

        for (int i = 1; i < 72; i++)
        {
            if (image2) delete image2;
            image2 = new Bitmap();

            in.open("input/in " + to_string(i) + ".bmp", ios::binary);
            in >> *image2;
            in.close();

            Frame *f2 = new Frame(*image2);
            
            for (int k = 0; k < 5; k++)
            {
                if (K) delete K;
                K = new Kmeans(0, 0, f1, image2, 7);
                K->Activate(f2, nullptr, dID[j], i, k);
            }
        }
    }
}

void Run_Cellular()
{
    for (int i = 0; i < 20; i++)
    {
        int params[4] = {0, 0, 0, 0};
            params[0] = rand() % 8 + 0;
            if (params[0] == 8) params[1] = 8;
            else params[1] = rand() % (8 - params[0]) + params[0];
            params[2] = rand() % 4 + 1;
            if (params[2] == 4) params[3] = 4;
            else params[3] = rand() % (4 - params[2]) + params[2] + 1;

        for (int n = 1; n <= 5; n++)
        {
            cout << "Creating Gif (" << i << ", " << n << ")" << endl;
            Run_Cellular(params, n);
        }
    }

}

void Run_Cellular(int* params, int n)
{
    cout << "   Initializing..." << endl;

    ifstream in;
 
    string outfile("output/CA");
    outfile += to_string(params[0]);
    outfile += to_string(params[1]);
    outfile += to_string(params[2]);
    outfile += to_string(params[3]);
    outfile += '-';
    outfile += to_string(n);
    outfile += ".gif";

    int width = 216;
    int height = 216;
    int scale = 5;
    int delay = 10;

    Frame* init_1 = new Frame(width, height);

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width * scale, height * scale, delay);

    Render r(scale);

    vector<int> args_1;
        args_1.push_back(params[0]);
        args_1.push_back(params[1]);
        args_1.push_back(params[2]);
        args_1.push_back(params[3]);
        args_1.push_back(1);

    cout << "   Running Events..." << endl;

    r.Add_Layer(init_1, new Plotter(0, 1, width, height, 1, 5, 0));

    Frame* init_2 = new Frame(*r.layer_getLastFrame());

    r.Add_Layer(init_2, new Cellular(1, 200, width, height, 1, args_1));
    
    cout << "   Compositing and Exporting Gif..." << endl;

    r.Export(gifw);

    GifEnd(&gifw);

    cout << "   ...Gif Written" << endl;
}

void Run_Painterly() 
{
    cout << "Initializing Gif..." << endl;

    ifstream in;

    vector <Frame*> inits;
 
    string outfile("output/Overflow.gif");

    for (int i = 0; i <= 15; i++)
    {
        string file_name("input/" + to_string(i) + ".bmp");

        Bitmap* image = new Bitmap();

        in.open(file_name, ios::binary);
        in >> *image;
        in.close();

        inits.push_back(new Frame(*image));

        delete image;
    }

#ifdef DEBUG
    cout << "Seed Images Loaded" << endl;
#endif

    int width = inits[0]->_width;
    int height = inits[0]->_height;
    int delay = 10;

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width, height, delay);

#ifdef DEBUG
    cout << "Gif Writer Created..." << endl;
#endif

    vector<int> temp_layers;
    vector<int> layers;
    Render r(1);

    cout << "Scripting Gif..." << endl;

    Frame* reference = new Frame(*inits[0]);
    Frame* output = new Frame(width, height);

#ifdef DEBUG
    cout << "Reference and Output Images Intialized..." << endl << endl;
#endif

    temp_layers.push_back(r.Add_Layer(output, new Painter(0, 300, output, reference, 20, 11)));

    delete reference;
    reference = new Frame(*inits[0]);

    temp_layers.push_back(r.Add_Layer(output, new Painter(50, 250, output, reference, 20, 7)));

    delete reference;
    reference = new Frame(*inits[0]);

    temp_layers.push_back(r.Add_Layer(output, new Painter(100, 200, output, reference, 30, 11)));

    cout << "Layer 0 Initialized..." << endl;

    layers.push_back(r.Composite_Layers(temp_layers));

    temp_layers.clear();

    delete reference;
    reference = new Frame(*inits[1]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    temp_layers.push_back(r.Add_Layer(output, new Painter(300, 100, output, reference, 40, 7)));

    delete reference;
    reference = new Frame(*inits[1]);

    temp_layers.push_back(r.Add_Layer(output, new Painter(300, 100, output, reference, 80, 3)));

    delete reference;
    reference = new Frame(*inits[1]);

    temp_layers.push_back(r.Add_Layer(output, new Painter(300, 100, output, reference, 100, 1)));

    layers.push_back(r.Composite_Layers(temp_layers));

    temp_layers.clear();

    cout << "Layer 1 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[2]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(400, 100, output, reference, 5, 3)));

    cout << "Layer 2 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(500, 25, output, reference, 10, 3)));

    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(525, 25, output, reference, 25, 3)));

    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(550, 50, output, reference, 50, 3)));

    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(600, 80, output, reference, 100, 3)));
    
    cout << "Layer 3 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[4]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(680, 120, output, reference, 1000, 1)));

    cout << "Layer 4 Initialized..." << endl;

    int frame_track = 800;


for (int i = 5; i < 7; i++)
{
    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 60, output, reference, 100, 5)));

    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 60, 60, output, reference, 200, 3)));

    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 120, 76, output, reference, 500, 1)));

    frame_track = frame_track + 196;

    cout << "Layer " << i << " Initialized..." << endl;
}

//frametrack = 1192

for (int i = 7; i < 8; i++)
{
    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 143, output, reference, 2000, 1)));

    frame_track += 143;

    cout << "Layer " << i << " Initialized..." << endl;
}

for (int i = 8; i < 11; i++)
{
    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 142, output, reference, 200, 1)));

    frame_track += 142;

    cout << "Layer " << i << " Initialized..." << endl;
}

//frametrack = 1761 - 2:56.1

delete reference;
reference = new Frame(*inits[11]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 50, output, reference, 2, 9)));
frame_track = frame_track + 50;

delete reference;
reference = new Frame(*inits[11]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 68, output, reference, 10, 9)));
frame_track = frame_track + 68;

//frame_track = 1879 - 3:07.9

delete reference;
reference = new Frame(*inits[11]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 25, output, reference, 50, 7)));
frame_track = frame_track + 25;

delete reference;
reference = new Frame(*inits[11]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 28, output, reference, 100, 7)));
frame_track = frame_track + 28;


cout << "Layer " << 11 << " Initialized..." << endl;

//frametrack = 1932 - 3:13.2

delete reference;
reference = new Frame(*inits[12]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 54, output, reference, 100, 7)));
frame_track = frame_track + 54;

//frametrack = 1986 - 3:18.6

delete reference;
reference = new Frame(*inits[12]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 55, output, reference, 200, 3)));
frame_track = frame_track + 55;

cout << "Layer " << 12 << " Initialized..." << endl;

//frametrack = 2041 - 3:24.1

delete reference;
reference = new Frame(*inits[13]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 218, output, reference, 1000, 1)));
frame_track = frame_track + 218;

cout << "Layer " << 13 << " Initialized..." << endl;

//frametrack = 2259 - 3:45.9

delete reference;
reference = new Frame(*inits[14]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 151, output, reference, 100, 3)));
frame_track = frame_track + 151;

delete reference;
reference = new Frame(*inits[14]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 150, output, reference, 750, 1)));
frame_track = frame_track + 150;

delete reference;
reference = new Frame(*inits[14]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 150, output, reference, 2000, 1)));
frame_track = frame_track + 150;

cout << "Layer " << 14 << " Initialized..." << endl;

//frametrack = 2760

delete reference;
reference = new Frame(*inits[15]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 110, output, reference, 150, 1)));
frame_track = frame_track + 110;

delete reference;
reference = new Frame(*inits[15]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 60, output, reference, 150, 1)));
frame_track = frame_track + 60;

cout << "Layer " << 15 << " Initialized..." << endl;

//frametrack = 2880 - 4:48


#ifdef DEBUG
    cout << "layers contains: ";

    for (auto l : layers) cout << l << ", ";

    cout << endl;
#endif

    delete output;
    delete reference;
    for (auto f : inits) delete f;

    cout << "Compositing and Exporting Gif..." << endl;

    r.Export(gifw);

    GifEnd(&gifw);

    cout << "...Gif Written" << endl;
}