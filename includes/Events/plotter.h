#include "..\events.h"

class Plotter : public Event
{
    public:
        Plotter(int start, int duration, int width, int height) 
            : Event(start, duration, width, height) {}
        ~Plotter() {}

		void Activate(Frame* f, Layer* l);
};

void Plotter::Activate(Frame* f, Layer* l)
{
    /*
	vector<uint8_t> frame;
	
    for (int n = 0; n < l->_frame_num; n++)
    {
        frame.clear();

        for (int i = 0; i < l->_width; i++)
        {
            for (int j = 0; j < l->_height; j++)
            {
                int rgba[] = {0, 0, 0, 255};

                if (((float)i / l->_width) < 0.25)
                {
                    rgba[0] = (n * 5 < 255) ? n * 5 : 255;
                }
                else if (((float)i / l->_width) < 0.5)
                {
                    rgba[1] = (n * 5 < 255) ? n * 5 : 255;
                }
                else if (((float)i / l->_width) < 0.75)
                {
                    rgba[2] = (n * 5 < 255) ? n * 5 : 255;
                }
                else
                {
                    rgba[0] = (n * 5 < 255) ? n * 5 : 255;
                    rgba[1] = (n * 5 < 255) ? n * 5 : 255;
                }

                //cout << "n is " << n << ", rgb is " << rgba[0] << ", " << rgba[1] << ", " << rgba[2] << ", " << rgba[3] << "." << endl;

                for (int k = 0; k < 4; k++)
                    frame.push_back((uint8_t)rgba[k]);
            }
        }

        l->_frames.push_back(new Frame(frame, l->_width, l->_height));
    }
    */

    for (int n = 0; n < l->_frame_num; n++)
    {
        l->_frames.push_back(new Frame(*f));
    }
}