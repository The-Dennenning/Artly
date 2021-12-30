
#include "frame.h"

class Zine
{
    public:
        Zine(int width, int height, vector<Frame*> input) :
            _width(width), _height(height), _input(input)
            {
                Frame* page = new Frame(_width, _height);
                page->_clear_w();
                vector<int> blank_rgb(3, 0);

                int p_width = _width / 4;
                int p_height = _height / 2;

                int i_width = input[0]->_width;
                int i_height = input[0]->_height;

                int b_width = (p_width - i_width) / 2;
                int b_height = (p_height - i_height) / 2;

                int n = 4;
                bool top = true;

                cout << "page dims: " << p_width << ", " << p_height << endl;
                cout << "input dims: " << i_width << ", " << i_height << endl;
                cout << "border dims: " << b_width << ", " << b_height << endl;

                for (int p = 0; p < 8; p++)
                {
                    cout << "       page " << p << endl;

                    int w_off = (p % 4) * p_width + b_width;
                    int h_off;

                    if (p < 4)
                    {
                        h_off = b_height;
                        input[p]->_flip();
                    }
                    else
                    {
                        h_off = p_height + b_height;
                    }
                    
                    page->set_image(input[p], w_off, h_off);
                }

                page->_offset(0, _height / 2);

                _pages.push_back(page);
            }

        int _height;
        int _width;

        vector<Frame*> _input;
        vector<Frame*> _pages;
};


