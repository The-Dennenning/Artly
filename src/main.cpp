#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

#include "..\includes\render.h"
#include "..\includes\events.h"
#include "..\includes\bitmap.h"
#include "..\includes\gif.h"

#include "..\includes\Events\fractal.h"
#include "..\includes\Events\plotter.h"
#include "..\includes\Events\painterly.h"
#include "..\includes\Events\cellular.h"

#define RED 2
#define BLUE 3
#define GREEN 4
#define PURPLE 5

using namespace std;

void Run_Cellular();
void Run_Cellular(int* params, int n);
void Run_Painterly();

int main(int argc, char** argv)
{
    // Seeding random number generator.
    srand(time(NULL));

    Run_Painterly();

    return 0;
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

    for (int i = 0; i <= 16; i++)
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

    layers.push_back(r.Add_Layer(output, new Painter(400, 100, output, reference, 75, 3)));

    cout << "Layer 2 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(500, 25, output, reference, 5, 3)));

    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(525, 50, output, reference, 10, 3)));

    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(575, 50, output, reference, 50, 3)));

    delete reference;
    reference = new Frame(*inits[3]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(625, 125, output, reference, 200, 1)));
    
    cout << "Layer 3 Initialized..." << endl;

    delete reference;
    reference = new Frame(*inits[4]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(750, 100, output, reference, 1000, 1)));

    cout << "Layer 4 Initialized..." << endl;

    int frame_track = 850;

for (int i = 5; i < 8; i++)
{
    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 60, output, reference, 50, 7)));

    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 60, 60, output, reference, 75, 5)));

    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 120, 60, output, reference, 200, 3)));

    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 180, 60, output, reference, 500, 1)));

    frame_track = frame_track + 240;

    cout << "Layer " << i << " Initialized..." << endl;
}


for (int i = 9; i < 13; i++)
{
    delete reference;
    reference = new Frame(*inits[i]);
    delete output;
    output = new Frame(*r.layer_getLastFrame());
    
    layers.push_back(r.Add_Layer(output, new Painter(frame_track + 0, 150, output, reference, 1000, 1)));

    frame_track += 150;

    cout << "Layer " << i << " Initialized..." << endl;
}

delete reference;
reference = new Frame(*inits[13]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 150, output, reference, 100, 7)));
frame_track = frame_track + 150;

cout << "Layer " << 13 << " Initialized..." << endl;

delete reference;
reference = new Frame(*inits[14]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 100, output, reference, 1000, 1)));
frame_track = frame_track + 100;

cout << "Layer " << 14 << " Initialized..." << endl;

delete reference;
reference = new Frame(*inits[15]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 400, output, reference, 2000, 1)));
frame_track = frame_track + 400;

cout << "Layer " << 15 << " Initialized..." << endl;

delete reference;
reference = new Frame(*inits[16]);
delete output;
output = new Frame(*r.layer_getLastFrame());

layers.push_back(r.Add_Layer(output, new Painter(frame_track, 200, output, reference, 200, 1)));
frame_track = frame_track + 200;

cout << "Layer " << 15 << " Initialized..." << endl;

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