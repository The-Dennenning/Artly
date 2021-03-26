#include "..\events.h"

class Plotter : public Event
{
    public:
        Plotter(int start, int duration, int width, int height, int op, int p1) 
            : Event(start, duration, width, height), _op(op), _p1(p1) {}
        ~Plotter() {}

		void Activate(Frame* f, Layer* l);

        void _draw(Frame* f, int op, int symm);

        int _op;
        int _p1;
};

void Plotter::Activate(Frame* f, Layer* l)
{

    if (_op == 0)
    //Plots frame as passed
    {
        for (int n = 0; n < l->_frame_num; n++)
        {
            l->_frames.push_back(new Frame(*f));
            cout << "   Plotter Frame " << n << " Generated" << endl;
        }
    }

    if (_op == 1)
    {
        f->_clear();

        for (int i = 0; i < _p1; i++)
            _draw(f, 1, 1);

        l->_frames.push_back(new Frame(*f));
    }

    if (_op == 2)
    {
        f->_clear();

        int rgb[4] = {255, 255, 255, 0};

        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                f->set(i, j, rgb);
            }
        }
    }
}

void Plotter::_draw(Frame* f, int op, int symm)
{
    if (op == 1)
    //Draw Rectangle
    {
        int v_bound_1, v_bound_2, h_bound_1, h_bound_2;
        int v_edge = (int) ((float) f->_height * 0.1);
        int h_edge = (int) ((float) f->_width  * 0.1);

        //generate rectangle coordinates
        if (symm)
        {
            v_bound_1 = rand() % (int)(((float)f->_height / 2) - 2 * v_edge) + (int)((float)(f->_height / 2) + v_edge);
            v_bound_2 = f->_height - v_bound_1;
            h_bound_1 = rand() % (int)(((float)f->_width  / 2) - 2 * h_edge) + (int)((float)(f->_width  / 2) + h_edge);
            h_bound_2 = f->_width  - h_bound_1;
        }
        else
        {
            int temp;

            v_bound_1 = rand() % (f->_height - 2 * v_edge) + v_edge;
            v_bound_2 = rand() % (f->_height - 2 * v_edge) + v_edge;
            h_bound_1 = rand() % (f->_width  - 2 * h_edge) + h_edge;
            h_bound_2 = rand() % (f->_width  - 2 * h_edge) + h_edge;

            //ensure bound 1 > bound 2
            if (v_bound_1 < v_bound_2)
            {
                temp = v_bound_1;
                v_bound_1 = v_bound_2;
                v_bound_2 = temp;
            }

            if (h_bound_1 < h_bound_2)
            {
                temp = h_bound_1;
                h_bound_1 = h_bound_2;
                h_bound_2 = temp;
            }
        }

        //pick color
        int rgba[4] = {0, 0, 0, 0};

        for (int i = 0; i < 3; i++)
            rgba[i] = rand() % 255 + 0;

        //draw horizontal sides
        for (int i = v_bound_2; i <= v_bound_1; i++)
        {
            f->set(i, h_bound_1, rgba);
            f->set(i, h_bound_2, rgba);
        }

        //draw vertical sides
        for (int i = h_bound_2; i <= h_bound_1; i++)
        {
            f->set(v_bound_1, i, rgba);
            f->set(v_bound_2, i, rgba);
        }
    }
}