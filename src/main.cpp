#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>

#include "..\includes\render.h"
#include "..\includes\events.h"
#include "..\includes\bitmap.h"
#include "..\includes\gif.h"

#include "..\includes\Events\fractal.h"
#include "..\includes\Events\rewrite.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        cout << "usage:\n" << "artly outputfile.gif" << endl;

        return 0;
    }

    // Seeding random number generator.
    srand(time(NULL));

    // Declaring the object for ifstream  
    ifstream in;
    // Declaring the object bitmap
    Bitmap image;
    // Declaring the object for writing to external data file.
    ofstream out;

    cout << "Program Starting..." << endl;

    // Reading in the name of the bitmap.
    string infile("temp.bmp");
    // note using the string clas you can pass in a string as an argument
    // like a copy constructor to quickly create a string.

    // putting in the output file name into outfile  
    string outfile(argv[1]);

    cout << "Seed Image Loading..." << endl;

    // Opening the file for reading in, using infile as the name of the bitmap file
    // and "ios::binary" means reading in the raw data - binary. This will transfer
    // bitmap from the object in to bitmap image.
    in.open(infile, ios::binary);
    in >> image;
    in.close();

    cout << "Seed Image Loaded..." << endl;

    // Getting width and height from the bitmap since it is stored in the bitmap file. 
    // getSize is just a public function. These values are required for making the 
    // gif writer object.
    int width = image.getSize(1);
    int height = image.getSize(0);
    int delay = 10;
    GifWriter gifw;

    cout << "Gif Writer Created..." << endl;

    GifBegin(&gifw, outfile.c_str(), width, height, delay);

    Render r;
    vector<int> layers;

#define RED 2
#define BLUE 3
#define GREEN 4
#define PURPLE 5

    vector< vector< vector <int> > > rules;
        
        vector<int> rule1_in = {RED};
        vector<int> rule1_out = {RED, GREEN};

        vector< vector <int> > rule1;
        rule1.push_back(rule1_in);
        rule1.push_back(rule1_out);
        rules.push_back(rule1);

        vector<int> rule2_in = {BLUE, RED};
        vector<int> rule2_out = {RED, RED};

        vector< vector <int> > rule2;
        rule2.push_back(rule2_in);
        rule2.push_back(rule2_out);
        rules.push_back(rule2);

        vector<int> rule3_in = {BLUE, RED, RED};
        vector<int> rule3_out = {RED, BLUE};

        vector< vector <int> > rule3;
        rule3.push_back(rule3_in);
        rule3.push_back(rule3_out);
        rules.push_back(rule3);

        vector<int> rule4_in = {GREEN, RED};
        vector<int> rule4_out = {GREEN};

        vector< vector <int> > rule4;
        rule4.push_back(rule4_in);
        rule4.push_back(rule4_out);
        rules.push_back(rule4);

        vector<int> rule5_in = {GREEN, BLUE};
        vector<int> rule5_out = {RED, GREEN};

        vector< vector <int> > rule5;
        rule5.push_back(rule5_in);
        rule5.push_back(rule5_out);
        rules.push_back(rule5);

        vector<int> rule6_in = {GREEN, GREEN};
        vector<int> rule6_out = {BLUE, BLUE, PURPLE};

        vector< vector <int> > rule6;
        rule6.push_back(rule6_in);
        rule6.push_back(rule6_out);
        rules.push_back(rule6);

        vector<int> rule7_in = {PURPLE, RED};
        vector<int> rule7_out = {RED, PURPLE};

        vector< vector <int> > rule7;
        rule7.push_back(rule7_in);
        rule7.push_back(rule7_out);
        rules.push_back(rule7);

        vector<int> rule8_in = {PURPLE, BLUE};
        vector<int> rule8_out = {BLUE, PURPLE};

        vector< vector <int> > rule8;
        rule8.push_back(rule8_in);
        rule8.push_back(rule8_out);
        rules.push_back(rule8);

        vector<int> rule9_in = {PURPLE, GREEN};
        vector<int> rule9_out = {GREEN, PURPLE};

        vector< vector <int> > rule9;
        rule9.push_back(rule9_in);
        rule9.push_back(rule9_out);
        rules.push_back(rule9);

        vector<int> ruleA_in = {PURPLE, PURPLE};
        vector<int> ruleA_out = {RED};

        vector< vector <int> > ruleA;
        ruleA.push_back(ruleA_in);
        ruleA.push_back(ruleA_out);
        rules.push_back(ruleA);

        vector<int> first = {RED};

        //rewrite event
        layers.push_back(r.Add_Layer(&image, new Rewrite(0, 300, width, height, 5, rules, first)));
        r.layer_SetMask(layers[0], 215);


    for (int i = 0; i < 10; i++)
    {
        layers.push_back(r.Add_Layer(&image, new Fractal(i * 25, 75, width, height, i)));
        r.layer_SetMask(layers[i], 128);
    }

    r.Export(gifw);

    GifEnd(&gifw);

    cout << "...Gif Written" << endl; 

    return 0;
}

