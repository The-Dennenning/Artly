#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>

#include "..\includes\space.h"
#include "..\includes\camera.h"
#include "..\includes\render.h"
#include "..\includes\events.h"
#include "..\includes\bitmap.h"
#include "..\includes\gif.h"

#include "..\includes\Events\fractal.h"
#include "..\includes\Events\plotter.h"
#include "..\includes\Events\painterly.h"
#include "..\includes\Events\cellular.h"
#include "..\includes\Events\kmeans.h"

#define RED 2
#define BLUE 3
#define GREEN 4
#define PURPLE 5

using namespace std;

void Run_KMeans_Rectangulate();
void Run_KMeans();
void Run_Cellular();
void Run_Cellular(int* params, int n);
void Run_Painterly();
void Run_Plotter();

void Run_Evolutionary();

void Run_3D();

int main(int argc, char** argv)
{
    // Seeding random number generator.
    srand(time(NULL));

    Run_3D();

    return 0;
}


void Run_3D()
{
    Space* s = new Space();
        s->_t = 0;

    Camera* c = new Camera(0, 2000, 800, 800);
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

            cout << "   Running Events..." << endl;


            for (int i = 0; i < 5; i++)
            {
                fr.Add_Layer(init_1, new Plotter(0 + 200 * i, 1, fwidth, fheight, 1, 5));

                inits.push_back(new Frame(*fr.layer_getLastFrame()));

                int r = rand() % 9;

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

    Plotter *p = new Plotter(0, 0, f->_width, f->_height, 2, 0);

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

    r.Add_Layer(init_1, new Plotter(0, 1, width, height, 1, 5));

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