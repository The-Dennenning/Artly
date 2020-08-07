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

    string infile("mountain1.bmp");
 
    string outfile("Out.gif");

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

/*
        Bitmap* br = new Bitmap(image);
        Bitmap* bo = new Bitmap(image);

        int clear[3] = {255, 255, 255};
        bo->clear(clear);
        cout << "Reference and Output Images Intialized..." << endl << endl;
        
        delete br;
        br = new Bitmap(image);
        layers.push_back(r.Add_Layer(bo, new Painter(0, 150, bo, br, 0, 2000, 10)));
        r.layer_SetMask(layers[0], 255);


        delete br;
        br = new Bitmap(image);
        layers.push_back(r.Add_Layer(bo, new Painter(0, 150, bo, br, 0, 2000, 7)));
        r.layer_SetMask(layers[1], 255);
*/

/*
    for (int i = 0; i < 10; i++)
    {
        layers.push_back(r.Add_Layer(NULL, new Fractal(i * 15, 45, width, height, i * 5)));
        r.layer_SetMask(layers[i], 128);
    }
*/
    r.Add_Layer(f, new Plotter(0, 50, width, height));
    
    r.Export(gifw);

    GifEnd(&gifw);

    cout << "...Gif Written" << endl;

}
