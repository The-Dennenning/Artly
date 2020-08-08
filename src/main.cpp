#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>

#include "..\includes\render.h"
#include "..\includes\events.h"
#include "..\includes\bitmap.h"
#include "..\includes\gif.h"

#include "..\includes\Events\fractal.h"
#include "..\includes\Events\plotter.h"
#include "..\includes\Events\painterly.h"

#define RED 2
#define BLUE 3
#define GREEN 4
#define PURPLE 5

using namespace std;

void Run();

int main(int argc, char** argv)
{
    // Seeding random number generator.
    srand(time(NULL));

    Run();

    return 0;
}

void Run() 
{
    ifstream in;
    Bitmap image;

    string infile("dog.bmp");
 
    string outfile("dog.gif");

    cout << "Seed Image Loading..." << endl;

    in.open(infile, ios::binary);
    in >> image;
    in.close();

    cout << "Seed Image Loaded..." << endl;

    int width = image.getSize(0);
    int height = image.getSize(1);
    int delay = 10;

    Frame* f = new Frame(image);

    cout << "Seed Frame Created..." << endl;

    GifWriter gifw;
    GifBegin(&gifw, outfile.c_str(), width, height, delay);

    cout << "Gif Writer Created..." << endl;

    vector<int> layers;
    Render r;

    Frame* reference = new Frame(*f);
    Frame* output = new Frame(width, height);

    cout << "Reference and Output Images Intialized..." << endl << endl;
    
    layers.push_back(r.Add_Layer(output, new Painter(0, 50, output, reference, 100, 15)));

    delete reference;
    reference = new Frame(*f);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(50, 50, output, reference, 100, 7)));

    delete reference;
    reference = new Frame(*f);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(100, 50, output, reference, 125, 3)));

    delete reference;
    reference = new Frame(*f);
    delete output;
    output = new Frame(*r.layer_getLastFrame());

    layers.push_back(r.Add_Layer(output, new Painter(150, 50, output, reference, 150, 1)));

    layers.push_back(r.Add_Layer(new Frame(*r.layer_getLastFrame()), new Plotter(200, 100, width, height)));

    delete output;
    delete reference;
    delete f;

/*
    for (int i = 0; i < 10; i++)
    {
        layers.push_back(r.Add_Layer(NULL, new Fractal(i * 30, 30, width, height, i * 5)));
        r.layer_SetMask(layers[i + 4], i * 20);
    }
*/

    r.Export(gifw);

    GifEnd(&gifw);

    cout << "...Gif Written" << endl;

}
