
#include "..\events.h"
#include "..\strokes.h"


class Mathly : public Event
{
    public:
        Mathly(int start, int duration, int width, int height, int type, vector<double> params) :
            Event(start, duration, width, height), _type(type), _params(params)
            {

            }

        void Activate(Frame* f, Layer* l);

        void _circle(Frame* f);
        void _sin(Frame* f);
        void _rational(Frame* f);

        int _type;

        vector<double> _params;
};


void Mathly::Activate(Frame* f, Layer* l)
{
    Frame* _f;

	for (int n = 0; n < l->_frame_num; n++)
	{
        _f = new Frame(*f);

        if (_type == 0)
            _circle(_f);
        else if (_type == 1)
        {
            _params[2] += _params[4];
            _circle(_f);
        }

        l->_frames.push_back(_f);
    }
}


/* params:
    [0], [1] -> (x, y)  location of circle center
    [2]      -> r       radius of circle
    [3]      -> t       thickness of line
    [4]      -> d_r     change in radius of circle frame to frame
*/
void Mathly::_circle(Frame *f)
{
    double _x = _params[0];
    double _y = _params[1];
    double _r = _params[2];
    double _density = 5;

    //goes around circle with number of steps scaled by density and radius
    //  where _density * _r is the number of points plotted around 1/4 of circle
    for (int n = 0; n < _r * _density * 4; n++)
    {
        //calculate parametric step size around circle arc
        double _step = ((3.1415 / 2) / (_r * _density)) * n;

        //plot point at that step
        pen_dot *p = new pen_dot(f, _params[3], _x + cos(_step) * _r, _y + sin(_step) * _r, 255, 255, 255, 255);
    }
}

void Mathly::_sin(Frame* f)
{

}

void Mathly::_rational(Frame* f)
{

}