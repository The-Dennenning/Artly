#include <iostream>
#include <fstream>
#include <string.h>
#include "events.h"
#include "bitmap.h"
#include "gif.h"


int main(int argc, char** argv)
// Main with arguments - from terminal you can add arguments to this.
// int argc and argv standard arguments for main.
// argc - how many arguments supplied to program
//      counts name of program itself.
//
// argv - values of the arguments. 
//      array of strings
//
// argv different values:
//      argv[0] = the program name
//      argv[1] = the bitmap name
//      argv[2] = the output file name
//
// argc = argument count; argv = argument value
{
    // If number of arguments is wrong, then will spit out usage message (error message) and 
    // exit the program.
    if(argc != 3)
    {
        cout << "usage:\n" << "cellular inputfile.bmp outputfile.gif" << endl;

        return 0;
    }


    // Will throw if the bitmap has a bad header.
    try
    {
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
        string infile(argv[1]);
        // note using the string clas you can pass in a string as an argument
        // like a copy constructor to quickly create a string.

        // putting in the output file name into outfile  
        string outfile(argv[2]);

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



// Checking if smol was used when compiling.
#ifdef SMOL
        GifBegin(&gifw, outfile.c_str(), width * 4, height * 4, delay);

// If it wasn't then do this else statement.
#else
        GifBegin(&gifw, outfile.c_str(), width, height, delay);

#endif 



        cout << "Gif Writer Initialized..." << endl;


        // SETTING UP THE EVENTS TO PLAY
        // This is the vector of different types of events
        vector<Event*> events;
        
        // uint8_t is the data value that the gifwriter takes. 
        vector<uint8_t> frame;

        // How long to run the gifwriter for
        int max_frame = 500;

        // How to define the cellular automata
        int* args;

        //Automata Filter
        // this is a 3 state cellular automata
        events.push_back(new Filter(0, 500, 2, NULL, NULL));

        /*
            args = new int[5];
            args[0] = 2;
            args[1] = 3;
            args[2] = 3;
            args[3] = 3;
            args[4] = 1;
        events.push_back(new Filter(0, 100, 1, args, NULL));

            args = new int[5];
            args[0] = 2;
            args[1] = 3;
            args[2] = 3;
            args[3] = 4;
            args[4] = 1;
        events.push_back(new Filter(100, 100, 1, args, NULL));

            args = new int[5];
            args[0] = 1;
            args[1] = 5;
            args[2] = 2;
            args[3] = 2;
            args[4] = 4;
        events.push_back(new Filter(200, 100, 1, args, NULL));

            args = new int[5];
            args[0] = 0;
            args[1] = 0;
            args[2] = 1;
            args[3] = 1;
            args[4] = 1;
        events.push_back(new Filter(300, 100, 1, args, NULL));
        */

        // ACTUAL WRITING TO THE IMAGE 
        for (int n = 0; n < max_frame; n++)
        {
            // Go through all of the events in the events vector and activate all of them.
            for (auto e : events) e -> Activate(image, n); // This is fancy syntax for vectors to loop through the data structure.

            // Translate the bitmap image into the vector of frame data.
            getFrame(frame, image);

// Another if for if the smol yes.
#ifdef SMOL
            GifWriteFrame(&gifw, frame.data(), width * 4, height * 4, delay);
#else
            GifWriteFrame(&gifw, frame.data(), width, height, delay);
#endif

            cout << "   Frame " << n << " Written" << endl;
        }

       GifEnd(&gifw);

       cout << "...Gif Written" << endl; 

       for (auto e : events) delete e;

    }
    catch(BADHEADER)
    {
	cout << "Error: file not recognized." << endl;
    }
    catch(...)
    {
        cout << "Error: an uncaught exception occured." << endl;
    }

    return 0;
}

