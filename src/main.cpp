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
    cout << "Initializing Gif..." << endl;

    ifstream in;

    vector <Frame*> inits;
 
    string outfile("output/v6.gif");

    for (int i = 1; i <= 13; i++)
    {
        string file_name("input/bmp" + to_string(i) + ".bmp");

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
    Render r;

    cout << "Scripting Gif..." << endl;

    Frame* reference = new Frame(*inits[0]);
    Frame* output = new Frame(width, height);

#ifdef DEBUG
    cout << "Reference and Output Images Intialized..." << endl << endl;
#endif

    temp_layers.push_back(r.Add_Layer(output, new Painter(0, 15, output, reference, 400, 15)));

    delete reference;
    reference = new Frame(*inits[0]);

    temp_layers.push_back(r.Add_Layer(output, new Painter(0, 15, output, reference, 300, 7)));

    delete reference;
    reference = new Frame(*inits[0]);

    temp_layers.push_back(r.Add_Layer(output, new Painter(0, 15, output, reference, 300, 3)));

    delete reference;
    reference = new Frame(*inits[0]);

    temp_layers.push_back(r.Add_Layer(output, new Painter(0, 15, output, reference, 300, 1)));

    layers.push_back(r.Composite_Layers(temp_layers));

    for (int i = 1; i < 3; i++)
    {
        temp_layers.clear();
`
        delete reference;
        reference = new Frame(*inits[i]);
        delete output;
        output = new Frame(*r.layer_getLastFrame());

        temp_layers.push_back(r.Add_Layer(output, new Painter(i * 15, 15, output, reference, 500, 7)));

        delete reference;
        reference = new Frame(*inits[i]);

        temp_layers.push_back(r.Add_Layer(output, new Painter(i * 15, 15, output, reference, 500, 3)));

        delete reference;
        reference = new Frame(*inits[i]);

        temp_layers.push_back(r.Add_Layer(output, new Painter(i * 15, 15, output, reference, 500, 1)));

        layers.push_back(r.Composite_Layers(temp_layers));
    }

#ifdef DEBUG
    cout << "layers contains: ";

    for (auto l : layers) cout << l << ", ";

    cout << endl;
#endif

    temp_layers.clear();

    delete output;
    delete reference;
    for (auto f : inits) delete f;

    cout << "Compositing and Exporting Gif..." << endl;

    r.Export(gifw);

    GifEnd(&gifw);

    cout << "...Gif Written" << endl;
}