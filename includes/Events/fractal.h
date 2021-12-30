#include "..\events.h"


/*
class Fractal_Tree_ISO : public Event
{
	public:
		Fractal_Tree_ISO(int start, int duration, int width, int height, vector<double> params) :
			Event(start, duration, width, height), _x(params[0]), _y(params[1]), _length(params[2]), _w(params[3]), _b_l(params[4]), _b_r(params[5]) {}

		double _x;
		double _y;
		double _length;
		double _w;
		double _b_l;
		double _b_r;

		struct b_data {
			double _x;
			double _y;
			double _length;
			double _width;
			double _b;
			int _dir;
			int _t;
			int _tmax;

			Fractal_Tree_ISO* _tree;

			b_data(double x, double y, double length, double width, double b, int dir, int tmax, Fractal_Tree_ISO* tree) 
				: _x(x), _y(y), _length(length), _width(width), _b(b), _dir(dir), _t(0), _tmax(tmax), _tree(tree) 
				{}
		};

		void Activate(Frame* f, Layer* l);
};

void Fractal_Tree_ISO::Activate(Frame *f, Layer *l)
{
	double x_t;
	double y_t;
	double theta_t;
	double d;
	double scale;

	vector<int> rgb;
		rgb.push_back(rand() % 125 + 60);
		rgb.push_back(rand() % 125 + 60);
		rgb.push_back(rand() % 125 + 60);
		
	Frame* f2 = new Frame(*f);

	x_t = _x;
	y_t = _y + 1;
	d = 1;

	pen_line p(f2, _w, _x, _y, rgb[0], rgb[1], rgb[2], 255, x_t, y_t, d);

	vector<b_data*> _next_ls;
	vector<b_data*> _ls;
		_ls.push_back(new b_data(x_t, y_t, _length, _w, _theta, _theta_b));
		
	for (int n = 0; n < l->_frame_num; n += _flash + 1)
	{
		for (b_data* _l : _ls)
		{
			int branch;

			if (n != 0)
				branch = rand() % _branch + rand() % 2;
			else
				branch = _branch;

			int lb = -(branch / 2);
			int ub = (branch / 2) + (branch % 2);

			for (double b = lb; b < ub; b++)
			{
				if (_branch % 2 == 1)
					theta_t = ((b * _l->_theta_b + _l->_theta) * 3.141529) / 180;
				else
					theta_t = (((b + 0.5) * _l->_theta_b + _l->_theta) * 3.141529) / 180;

				theta_t *= ((double) ((rand() % 3) + 9)) / 10;
				scale = ((double) ((rand() % 8) + 2)) / 10;

				x_t = _l->_x + _l->_length * sin(theta_t) * scale;
				y_t = _l->_y + _l->_length * cos(theta_t) * scale;
				
				d = pow(pow(x_t - _l->_x, 2) + pow(y_t - _l->_y, 2), 0.5);

				if (_t > _tmax)
				{
					x_t = _x;
					y_t = _y + (_length / ((double) _tmax));
					d = pow(pow(x_t - _x, 2) + pow(y_t - _y, 2), 0.5);
					
					pen_line p(f, _width, _x, _y, rgb[0], rgb[1], rgb[2], 255, x_t, y_t, d);
				}

				_next_ls.push_back(new b_data(x_t, y_t, d, _w, ((theta_t * 180) / 3.141529), _theta_b));
			}
		}

		_ls.clear();
		_ls.insert(_ls.begin(), _next_ls.begin(), _next_ls.end());
		_next_ls.clear();

		l->_frames.push_back(f2);
#ifdef DEBUG
		cout << "	lightning frame " << n << " processed..." << endl;
#endif

		Frame* temp = new Frame(*f2);
		f2 = temp;

		for (int f = 0; f < _flash; f++)
		{
			if (n + f == l->_frame_num) break;

			temp = new Frame(_width, _height);
			temp->_clear();

			l->_frames.push_back(temp);
		}

		if (n + 1 >= l->_frame_num) break;

#ifdef DEBUG
		cout << "	lightning frame " << n + 1 << " processed..." << endl;
#endif
	}

	delete f2;
}

*/

class Fractal_Lightning : public Event
{
	public:
		Fractal_Lightning();
		Fractal_Lightning(int start, int duration, int width, int height, vector<double> params) :
			Event(start, duration, width, height), _x(params[0]), _y(params[1]), _length(params[2]), _w(params[3]), _theta(params[4]), _theta_b(params[5]), _branch(params[6]), _flash(params[7])
			{
			}

		double _x;
		double _y;
		double _length;
		double _w;
		double _theta;
		double _theta_b;
		int _branch;
		int _flash;

		struct l_data {
			double _x;
			double _y;
			double _length;
			double _width;
			double _theta;
			double _theta_b;

			l_data(double x, double y, double length, double width, double theta, double theta_b) 
				: _x(x), _y(y), _length(length), _width(width), _theta(theta), _theta_b(theta_b) {}
		};

		void Activate(Frame* f, Layer* l);
};

void Fractal_Lightning::Activate(Frame* f, Layer* l)
{
	double x_t;
	double y_t;
	double theta_t;
	double d;
	double scale;

	vector<int> rgb;
		rgb.push_back(rand() % 35 + 220);
		rgb.push_back(rgb[0] - 12);
		rgb.push_back(rand() % 32 + 32);
		
	Frame* f2 = new Frame(*f);

	x_t = _x + _length * sin(_theta);
	y_t = _y + _length * cos(_theta);
	d = pow(pow(x_t - _x, 2) + pow(y_t - _y, 2), 0.5);

	pen_line p(f2, _w, _x, _y, rgb[0], rgb[1], rgb[2], 255, x_t, y_t, d);

	vector<l_data*> _next_ls;
	vector<l_data*> _ls;
		_ls.push_back(new l_data(x_t, y_t, _length, _w, _theta, _theta_b));
		
	for (int n = 0; n < l->_frame_num; n += _flash + 1)
	{
		for (l_data* _l : _ls)
		{
			int branch;

			if (n != 0)
				branch = rand() % _branch + rand() % 2;
			else
				branch = _branch;

			int lb = -(branch / 2);
			int ub = (branch / 2) + (branch % 2);

			for (double b = lb; b < ub; b++)
			{
				if (_branch % 2 == 1)
					theta_t = ((b * _l->_theta_b + _l->_theta) * 3.141529) / 180;
				else
					theta_t = (((b + 0.5) * _l->_theta_b + _l->_theta) * 3.141529) / 180;

				theta_t *= ((double) ((rand() % 3) + 9)) / 10;
				scale = ((double) ((rand() % 8) + 2)) / 10;

				x_t = _l->_x + _l->_length * sin(theta_t) * scale;
				y_t = _l->_y + _l->_length * cos(theta_t) * scale;
				
				d = pow(pow(x_t - _l->_x, 2) + pow(y_t - _l->_y, 2), 0.5);

				pen_line p(f2, _w, _l->_x, _l->_y, rgb[0], rgb[1], rgb[2], 255, x_t, y_t, d);

				_next_ls.push_back(new l_data(x_t, y_t, d, _w, ((theta_t * 180) / 3.141529), _theta_b));
			}
		}

		_ls.clear();
		_ls.insert(_ls.begin(), _next_ls.begin(), _next_ls.end());
		_next_ls.clear();

		l->_frames.push_back(f2);
#ifdef DEBUG
		cout << "	lightning frame " << n << " processed..." << endl;
#endif

		Frame* temp = new Frame(*f2);
		f2 = temp;

		for (int f = 0; f < _flash; f++)
		{
			if (n + f == l->_frame_num) break;

			temp = new Frame(_width, _height);
			temp->_clear();

			l->_frames.push_back(temp);
		}

		if (n + 1 >= l->_frame_num) break;

#ifdef DEBUG
		cout << "	lightning frame " << n + 1 << " processed..." << endl;
#endif
	}

	delete f2;
}

class Fractal_Kelp : public Event
{
	public:
		Fractal_Kelp();
		Fractal_Kelp(int start, int duration, int width, int height, vector<double> params) :
			Event(start, duration, width, height), _x(params[0]), _y(params[1]), _length(params[2]), _w(params[3]), _theta(params[4]), _theta_save(params[4]), _branch(params[5]), _n(params[6])
			{
			}

		double _x;
		double _y;
		double _length;
		double _w;
		double _theta;
		double _theta_save;
		int _branch;
		int _n;

		void Activate(Frame* f, Layer* l);
		void draw_tree(Frame* f, int iteration, double x, double y, double phi, vector<int> rgb);
		void draw_cap(Frame* f, int iteration, double x, double y, double phi, vector<int> rgb);
};

void Fractal_Kelp::Activate(Frame* f, Layer* l)
{
	vector<int> rgb;
		rgb.push_back(rand() % 64);
		rgb.push_back(rand() % 255);
		rgb.push_back(rand() % 255);

	for (int n = 0; n < l->_frame_num; n++)
	{
		Frame* f2 = new Frame(*f);

		/*
		double x_t = _x + _length * sin(((double)n) / 120);
		double y_t = _y + _length * abs(cos(((double)n) / 120));
		*/

		double x_t = _x;
		double y_t = _y + _length;

		pen_line p(f2, _w, _x, _y, rgb[0], rgb[1], rgb[2], 255, x_t, y_t, _length);

		if ((_x - (_width / 2)) < 0)
			pen_line p2(f2, _w + 2, 0, _y - (2 * _x), rgb[0], rgb[1], rgb[2], 255, _x, _y, 3 * _x);
		else
			pen_line p2(f2, _w + 2, _width, _y - 2 * (_width - _x), rgb[0], rgb[1], rgb[2], 255, _x, _y, _width);

		draw_tree(f2, _n, x_t, y_t, 0, rgb);

		l->_frames.push_back(f2);

		for (int k = 0; k < 3; k++)
		{
			rgb[k] -= 1;

			if (k == 0)
				rgb[k] %= 128;
			else
				rgb[k] %= 255;
		}
	
		_theta = _theta_save * (cos(((double)n) / 33));

        if (n % 100 == 0)
			cout << "Fractal_Tree Frame " << n << " processed" << endl;
	}
}

void Fractal_Kelp::draw_tree(Frame* f, int iteration, double x, double y, double phi, vector<int> rgb)
{
	//base case - recursion finished
	if (iteration == 0)
		return;

	//int branch = _fib[_n - iteration];

	int lb = -(_branch / 2);
	int ub = (_branch / 2) + (_branch % 2);

	double x_t;
	double y_t;
	double theta_t;
	
	for (double b = lb; b < ub; b++)
	{
		if (_branch % 2 == 1)
			theta_t = (b * _theta * 3.141529) / 180;
		else
			theta_t = ((b + 0.5) * _theta * 3.141529) / 180;

		theta_t += sin(iteration * 3.1415) * _theta;
		
		for (int k = 0; k < 3; k++)
		{
			rgb[k] -= 2;

			if (k == 0)
				rgb[k] %= 128;
			else
				rgb[k] %= 255;
		}

		x_t = x + _length * sin(theta_t);
		y_t = y + _length * cos(theta_t);

		int d = pow(pow(x_t - x, 2) + pow(y_t - y, 2), 0.5);

		double w = _w - ((_n - iteration) / 10);

		if (w < 1)
			w = 1;

		pen_line p(f, w, x, y, rgb[0], rgb[1], rgb[2], 255, x_t, y_t, d);

		if ((iteration + (int) b) % 2 == 0)
			draw_tree(f, iteration - 1, x_t, y_t, 0, rgb);
		else
			draw_cap(f, iteration - 1, x_t, y_t, 0, rgb);
	}
}

void Fractal_Kelp::draw_cap(Frame* f, int iteration, double x, double y, double phi, vector<int> rgb)
{
	//base case - recursion finished
	if (iteration == 0)
		return;

	//int branch = _fib[_n - iteration];

	int lb = -(_branch / 2);
	int ub = (_branch / 2) + (_branch % 2);

	double x_t;
	double y_t;
	double theta_t;
	
	for (double b = lb; b < ub; b++)
	{
		if (_branch % 2 == 1)
			theta_t = (b * _theta * 3.141529) / 180;
		else
			theta_t = ((b + 0.5) * _theta * 3.141529) / 180;

		theta_t += sin(iteration * 3.1415) * _theta;
		
		for (int k = 0; k < 3; k++)
		{
			rgb[k] -= 5;
			rgb[k] %= 255;
		}

		x_t = x + (_length / 2) * sin(theta_t);
		y_t = y + (_length / 2) * cos(theta_t);

		int d = pow(pow(x_t - x, 2) + pow(y_t - y, 2), 0.5);

		double w = _w - ((_n - iteration) / 2);

		if (w < 1)
			w = 1;

		pen_line p(f, w, x, y, rgb[0], rgb[1], rgb[2], 255, x_t, y_t, d);
	}
}

class Fractal_Tree : public Event
{
	public:
		Fractal_Tree();
		Fractal_Tree(int start, int duration, int width, int height, vector<double> params) :
			Event(start, duration, width, height), _x(params[0]), _y(params[1]), _length(params[2]), _w(params[3]), _theta(params[4]), _theta_save(params[4]), _branch(params[5]), _n(params[6])
			{
			}

		double _x;
		double _y;
		double _length;
		double _w;
		double _theta;
		double _theta_save;
		int _branch;
		int _n;

		void Activate(Frame* f, Layer* l);
		void draw_tree(Frame* f, int iteration, double x, double y, double phi, vector<int> rgb);
};

void Fractal_Tree::Activate(Frame* f, Layer* l)
{
	vector<int> rgb;
		rgb.push_back(64);
		rgb.push_back(196);
		rgb.push_back(128);

	for (int n = 0; n < l->_frame_num; n++)
	{
		Frame* f2 = new Frame(*f);

		/*
		double x_t = _x + _length * sin(((double)n) / 120);
		double y_t = _y + _length * abs(cos(((double)n) / 120));
		*/

		double x_t = _x;
		double y_t = _y + _length;

		pen_line p(f2, _w, _x, _y, rgb[0], rgb[1], rgb[2], 255, x_t, y_t, _length);

		draw_tree(f2, _n, x_t, y_t, 0, rgb);

		l->_frames.push_back(f2);

		for (int k = 0; k < 3; k++)
		{
			rgb[k] -= 1;
			rgb[k] %= 255;
		}
	
		_theta = _theta_save * (cos(((double)n) / 120) * 0.25 + 0.75);

		cout << "Fractal_Tree Frame " << n << " processed" << endl;
	}
}

void Fractal_Tree::draw_tree(Frame* f, int iteration, double x, double y, double phi, vector<int> rgb)
{
	//base case - recursion finished
	if (iteration == 0)
		return;

	//int branch = _fib[_n - iteration];

	int lb = -(_branch / 2);
	int ub = (_branch / 2) + (_branch % 2);

	double x_t;
	double y_t;
	double theta_t;
	
	for (double b = lb; b < ub; b++)
	{
		//if odd number of branches,
		//	one branch will be aligned parallel to trunk
		if (_branch % 2 == 1)
			theta_t = (b * _theta * 3.141529) / 180 + phi;
		//if even number of branches,
		//	branches will have gap aligned over trunk
		else
			theta_t = ((b + 0.5) * _theta * 3.141529) / 180 + phi;
		
		for (int k = 0; k < 3; k++)
		{
			rgb[k] -= 5;
			rgb[k] %= 255;
		}

		x_t = x + ((_length * iteration) / _n) * sin(theta_t);
		y_t = y + ((_length * iteration) / _n) * cos(theta_t);

		int d = pow(pow(x_t - x, 2) + pow(y_t - y, 2), 0.5);

		pen_line p(f, _w - ((_n - iteration) / 2), x, y, rgb[0], rgb[1], rgb[2], 255, x_t, y_t, d);

		draw_tree(f, iteration - 1, x_t, y_t, (theta_t + phi) / 2, rgb);
	}
}

class Fractal : public Event
{
	public:

		Fractal();
		Fractal(int start, int duration, int width, int height, int scaleFactor) :
			Event(start, duration, width, height), _scaleFactor(scaleFactor), _type(0) {
				//cout << "Fractal created, with start " << start << " and duration " << duration << endl;
			}
		Fractal(int start, int duration, int width, int height, int scaleFactor, int type) :
			Event(start, duration, width, height), _scaleFactor(scaleFactor), _type(type) {
				//cout << "Fractal created, with start " << start << " and duration " << duration << endl;
			}

		~Fractal();

	private:

		int _scaleFactor;
		int _type;

		void Activate(Frame* f, Layer* l);
		vector<int> getColor(int i, int j, int n);
		int evalFractal(double re, double im, int n);
};

bool vec_compare(vector<int> a, vector<int> b)
{
	for (int i = 0; i < a.size(); i++)
		if (a[i] != b[i]) return false;
	return true;
}

void Fractal::Activate(Frame* f, Layer* l)
{
	vector<uint8_t> frame;
	
	int length = 4;

	for (int n = 0; n < l->_frame_num; n++)
	{
		frame.clear();

		/* interleave method 
		int i = 0;
		while (i <= _width)
		{
			int j = 0; 
			int length_back;

			//stores color of cell being considered
			vector<int> rgb = getColor(i, j, n);
			//stores color of cell at the front of length
			vector<int> rgb_front(rgb);
			//stores color of cell at back of length (next front of length)
			vector<int> rgb_back;

			for (int k = 0; k < 3; k++)
				frame.push_back((uint8_t)rgb[k]);
			frame.push_back((uint8_t)255);

			cout << "committing pixel " << j << endl;

			j += length;
			length_back = length;

			while (j + length < _height)
			{
				cout << 1 << endl;
				//get color 
				rgb = getColor(i, j, n);
				
				//commit pixel to frame
				for (int k = 0; k < 3; k++)
					frame.push_back((uint8_t)rgb[k]);
				frame.push_back((uint8_t)255);
				
				cout << "committing pixel " << j << endl;

				if (length_back == length)
				{
					cout << 2 << endl;
					rgb_back = rgb;
				}

				//if current pixel is equal to pixel at front of length
				if (vec_compare(rgb, rgb_front))
				{
					cout << 1.1 << endl;
					//fill in all between this and the front pixel
					for (int l = 1; l < length_back; l++)
					{
						//commit pixel to frame
						for (int k = 0; k < 3; k++)
							frame.push_back((uint8_t)rgb[k]);
						frame.push_back((uint8_t)255);	
						
						cout << "committing pixel " << j + l << endl;
					}

					//go to next length
					j += length;
					rgb_front = rgb_back;
					length_back = length;
				}
				//if this pixel is consecutive to the saved pixel
				else if (length_back == 1)
				{
					cout << 1.2 << endl;
					//go to next length (and traverse the rest of this length)
					j += length * 2 - 1;
					rgb_front = rgb_back;
					length_back = length;
				}
				//else iterate backwards
				else
				{
					cout << 1.3 << endl;
					//iterate length and j backwards by 1
					j--;
					length_back--;
				}
			}

			//mop up any extra values
			while (j < _height)
			{
				//stores color of cell being considered
				vector<int> rgb = getColor(i, j, n);

				for (int k = 0; k < 3; k++)
					frame.push_back((uint8_t)rgb[k]);
				frame.push_back((uint8_t)255);

				cout << "committing pixel " << j << endl;

				j++;
			}

			i++;
		}
		*/

		/* conventional method */
		for (int i = 0; i < _width; i++)
		{
			for (int j = 0; j < _height; j++)
			{
				vector<int> rgb = getColor(i, j, n);

				for (int k = 0; k < 3; k++)
					frame.push_back((uint8_t)rgb[k]);

				frame.push_back((uint8_t)255);

				rgb.clear();
			}
		}
		
		l->_frames.push_back(new Frame(frame, _width, _height));

		if (n % 100 == 0)
			cout << "	Fractal Frame " << n << " Generated" << endl; 
	}
}

//
vector<int> Fractal::getColor(int i, int j, int n)
{
	vector<int> ret_color(3, 0);
	vector<int> div_color(3, 0);
	vector<int> dip_color(3, 0);
	vector<int> con_color(3, 0);

	//Sets base convergent color
	con_color[0] = 0;
	con_color[1] = 0;
	con_color[2] = 0;

/*
	//Sets base divergent color
	div_color[0] = 32;
	div_color[1] = 196;
	div_color[2] = 256;

	//sets other divergence color
	dip_color[0] = 0;
	dip_color[1] = 0;
	dip_color[2] = 128;

	//Sets base divergent color
	div_color[0] = 196;
	div_color[1] = 32;
	div_color[2] = 256;

	//sets other divergence color
	dip_color[0] = 0;
	dip_color[1] = 16;
	dip_color[2] = 128;
*/

	if (_type == 0)
	{
		//Sets base divergent color
		div_color[0] = 0;
		div_color[1] = 256;
		div_color[2] = 32;

		//sets other divergence color
		dip_color[0] = 0;
		dip_color[1] = 128;
		dip_color[2] = 16;
	}
	else if (_type == 1)
	{
		//Sets base divergent color
		div_color[0] = 256;
		div_color[1] = 32;
		div_color[2] = 0;

		//sets other divergence color
		dip_color[0] = 128;
		dip_color[1] = 16;
		dip_color[2] = 0;
	}
	else if (_type == 2)
	{
		//Sets base divergent color
		div_color[0] = 32;
		div_color[1] = 0;
		div_color[2] = 256;

		//sets other divergence color
		dip_color[0] = 16;
		dip_color[1] = 0;
		dip_color[2] = 128;
	}

	double re, im;

	//Transform row and col coordinates
	//	to float coordiates over box [-2, -1] x [1, 1]

    //double scale = pow(10, -1 * ((double) (n * n - n)) / 10000);
    double scale = pow(10, -1 * ((double) (n)) / 100);

	re = ((float) i / _width) * ((double) 4 * scale) - ((double) 2) * scale - (double) 1.5475;
	im = ((float) j / _height) * ((double) 4 * scale) - ((double) 2) * scale;

/*
	if (n >= 0)
	{
		double theta = ((n) * 3.1415) / 180;
		double re_temp = cos(theta / 50) * re - sin(theta / 50) * im;
		double im_temp = sin(theta / 50) * re + cos(theta / 50) * im;

		re = re_temp;
		im = im_temp;
	}
	*/
	
	if (re < -2)
		re = (re + 4) * -1;

	int maxEval = 1000;

	//Returns how many iterations it takes to diverge
	//	Or returns 0 if converges
	int eval = evalFractal(re, im, (n / 10) + 50);

	//Assigns color value given eval
	if (eval == 0)
	{
		for (int k = 0; k < 3; k++)
			ret_color[k] = con_color[k];
	}
	else
	{
		for (int k = 1; k < 3; k++)
		{
			ret_color[k] = div_color[k] - eval * 25;
			ret_color[k] += dip_color[k] * eval;
			ret_color[k] = ret_color[k] % 255;

			if ((k == 1) && (_type == 0))
				ret_color[k] % 128;
		}
	}
	
	/*
	 if (eval <= maxEval)
	{
		for (int k = 0; k < 3; k++)
			ret_color[k] = ((double) ((div_color[k] * eval) + (dip_color[k] * (maxEval - eval)))) / maxEval;
	}
	else
	{
		for (int k = 0; k < 3; k++)
			ret_color[k] = div_color[k];
	}
	*/

	return ret_color;
}

//Evaluates whether mandlebrot set converges or diverges
//	And returns number of iterations taken
int Fractal::evalFractal(double c_re, double c_im, int n)
{
	double z_re = 0;
	double z_im = 0;
	int i = 0;
	double s_re = 0;
	double s_im = 0;

	while (i < n)
	{
		/* Multiplying Complex Numbers...
			(a + bi) * (c + di) + (e + fi)
			real = a*c - b*d + e
			imag = a*d + b*c + f 
		*/

		z_re = (z_re * z_re) - (z_im * z_im) + c_re;
		z_im = (z_re * z_im) + (z_re * z_im) + c_im;
		i++;

		if(i % 10 == 0)
		{
			if (z_re == s_re && z_im == s_im)
				return 0;
			else
			{
				s_re = z_re;
				s_im = z_im;
			}
		}

		if (z_re <= -2 || z_re >= 2 || z_im <= -2 || z_im >= 2)
			return i;
	}

	return 0;
}


class NFractal : public Event
{
	public:
		NFractal(int start, int dur, int width, int height, vector<double> coefs) :
			Event(start, dur, width, height), _coefs(coefs)
			{
				for (int i = 0; i < _coefs.size() - 1; i++)
					_dervs.push_back(_coefs[i] * (_coefs.size() - i - 1));
			}


		//stores polynomial as coefficients
		vector<double> _coefs;
		//stores calculated polynomial derivative as coefficients
		vector<double> _dervs;
		//stores calculated Newton Method trajectory endpoints
		vector<vector<vector<double>>> _field;

		//stores kmeans iteration tracking
		int _iteration;
		//stores means (roots) for clusters
		vector<vector<double>> _means;
		//stores which cluster a data point belongs to
		vector<vector<int>> _clusters;

		//stores color pallete
		vector<vector<int>> _colors;


		void Activate(Frame* f, Layer* l);
		vector<int> getColor(int i, int j);
		void setColor();
		void evalFractal();
		void evalNewton(double *xs);

		void initialize();
		bool iterate();
};

void imml(double *xs, double *ys)
{
	double _re = xs[0];
	double _im = xs[1];

	xs[0] = _re * ys[0] - _im * ys[1];
	xs[1] = _re * ys[1] + _im * ys[0];
}

void imsq(double *xs, int _n)
{
	if (_n == 1)
		return;

	if (_n == 0)
	{
		xs[0] = 1;
		xs[1] = 0;
		return;
	}

	double *ys = new double[2];
	ys[0] = xs[0];
	ys[1] = xs[1];

	for (int n = 2; n <= _n; n++)
		imml(xs, ys);
	
	delete ys;
}

void imdv(double *xs, double* ys)
{
	double _re = xs[0];
	double _im = xs[1];

	//set ys to be conjugate of ys
	ys[1] *= -1;

	//multiply xs by conjugate of ys
	imml(xs, ys);

	xs[0] /= (_re * _re + _im * _im);
	xs[1] /= (_re * _re + _im * _im);

	//reset ys to ys
	ys[1] *= -1;
}

void test()
{
	double *xs = new double[2];
	double *ys = new double[2];

	xs[0] = 3;
	xs[1] = 2;
	ys[0] = 1;
	ys[1] = 7;

	imml(xs, ys);

	cout << "(3 + 2i)(1 + 7i) = (" << xs[0] << " + " << xs[1] << "i)" << endl;

	xs[0] = 1;
	xs[1] = 1;

	imsq(xs, 3);

	cout << "(1 + i)^3 = (" << xs[0] << ", " << xs[1] << "i)" << endl;
}

void NFractal::Activate(Frame* f, Layer* l)
{
	vector<uint8_t> frame;

	cout << "	setting color..." << endl;

	setColor();

	for (int n = 0; n < l->_frame_num; n++)
	{
		
		frame.clear();

		cout << "	evaluating fractal..." << endl;
		evalFractal();

		for (int i = 0; i < _width; i++)
		{
			for (int j = 0; j < _height; j++)
			{
				vector<int> rgb = getColor(i, j);

				for (int k = 0; k < 3; k++)
					frame.push_back((uint8_t)rgb[k]);

				frame.push_back((uint8_t)255);

				rgb.clear();
			}
		}
		
		l->_frames.push_back(new Frame(frame, _width, _height));

		if (n % 100 == 0)
			cout << "	Fractal Frame " << n << " Generated" << endl; 
	}
}

void NFractal::setColor()
{
	vector<int> steps;

	for (int k = 0; k < 3; k++)
		steps.push_back(rand() % 40 + 40);

	for (int n = 0; n < _coefs.size() - 1; n++)
	{
		vector<int> color;

		for (int k = 0; k < 3; k++)
		{
			if (n == 0)
				color.push_back(rand() % 255);
			else
				color.push_back((_colors[n - 1][k] + steps[k]) % 255);
		}

		_colors.push_back(color);
	}
}

vector<int> NFractal::getColor(int i, int j)
{
	return _colors[_clusters[i][j]];
}

void NFractal::evalNewton(double *rs)
{
	//real and imaginary values of polynomial
	double sum_rv = _coefs[_coefs.size() - 1];
	double sum_iv = 0;
	
	//real and imaginary values of derivative
	double sum_rd = _dervs[_dervs.size() - 1];
	double sum_id = 0;
	
	double *xs = new double[2];
	double *ys = new double[2];

	//calculate polynomial value at re, im
	for (int n = 1; n < _coefs.size(); n++)
	{
		xs[0] = rs[0];
		xs[1] = rs[1];

		ys[0] = _coefs[_coefs.size() - n - 1];
		ys[1] = 0;

		imsq(xs, n);
		imml(xs, ys);

		sum_rv += xs[0];
		sum_iv += xs[1];
	}

	//calculate derivative value at re, im
	for (int n = 1; n < _dervs.size(); n++)
	{
		xs[0] = rs[0];
		xs[1] = rs[1];

		ys[0] = _dervs[_dervs.size() - n - 1];
		ys[1] = 0;

		imsq(xs, n);
		imml(xs, ys);

		sum_rd += xs[0];
		sum_id += xs[1];
	}

	xs[0] = sum_rv;
	xs[1] = sum_iv;
	ys[0] = sum_rd;
	ys[1] = sum_id;

	imdv(xs, ys);

	rs[0] -= xs[0];
	rs[1] -= xs[1];

	delete[] xs;
	delete[] ys;
}

void NFractal::evalFractal()
{
	double scale = 35;
	int max = 24;

	double re;
	double im;

	double s_re;
	double s_im;

	for (double i = 0; i < _width; i++)
	{
		if (((int) i) % 100 == 0) 
			cout << "		processing line " << i << " of " << _width << endl;
		
		vector<vector<double>> line;

		for (double j = 0; j < _height; j++)
		{
			double *xs = new double[2];
			xs[0] = ((i / _width) - 0.5) * scale;
			xs[1] = ((j / _height) - 0.5) * scale;

#ifdef DEBUG
			int flag = 0;
			if (rand() % 100000 == 0)
			{
				cout << "		before, xs = (" << xs[0] << ", " << xs[1] << ")" << endl;
				flag = 1;
			}
#endif

			//iterate newtons method to determine trajectory endpoint
			for (int n = 0; n < max; n++)
				evalNewton(xs);

#ifdef DEBUG
			if (flag == 1)
			{
				cout << "		after, xs = (" << xs[0] << ", " << xs[1] << ")" << endl << endl;
			}
#endif
			
			vector<double> point;
				point.push_back(xs[0]);
				point.push_back(xs[1]);
			
			line.push_back(point);
			delete xs;
		}

		_field.push_back(line);
	}

	_means.clear();
	_clusters.clear();

	initialize();

    while(!iterate()) {}   
}

void NFractal::initialize()
{
	int _k = _coefs.size() - 1;

	_iteration = 0;

    //initializes k clusters
    //  by finding mean of k random points 
    for (int i = 0; i < _k; i++)
    {
		double sum[2] = {0, 0};
        vector<double> mean;

        for (int j = 0; j < 10; j++)
        {
            int w = rand() % _width;
            int h = rand() % _height;

            for (int k = 0; k < 2; k++)
                sum[k] += _field[w][h][k];
        }

        for (int k = 0; k < 2; k++)
            mean.push_back(sum[k] / 10);
			
		_means.push_back(mean);
    }

#ifdef DEBUG
    cout << "       means initialized to:" << endl;

    for (int i = 0; i < _k; i++)
        cout << "           " << i << " - (" 
             << _means[i][0] << ", " << _means[i][1]
             << ")" << endl;
#endif
}

bool NFractal::iterate()
{
	int _k = _coefs.size() - 1;

    //assigns all data to respective cluster
    for (int i = 0; i < _width; i++)
	{
		vector<int> line;

		for (int j = 0; j < _height; j++)
		{
			double closest_distance = 1000000;
			double distance;
			int closest_mean;

			//calculate set of distances between data point and each mean
			//  assigns data point to cluster with closest mean
			for (int k = 0; k < _k; k++)
			{
				distance = pow(pow(_field[i][j][0] - _means[k][0], 2) 
							 + pow(_field[i][j][1] - _means[k][1], 2)
				, 0.5);

				if (distance < closest_distance)
				{
					closest_distance = distance;
					closest_mean = k;
				}
			}

			//assigns mean
			line.push_back(closest_mean);
		}

		_clusters.push_back(line);
    }

#ifdef DEBUG
    cout << "       data assigned to clusters" << endl;
#endif

    //recalculates means given new clustering
    vector<vector<double>> sums;
    vector<int> nums;

    for (int i = 0; i < _k; i++)
    {
        vector<double> vals;
            vals.push_back(0);
            vals.push_back(0);
        sums.push_back(vals);
        nums.push_back(0);
    }

    //sums up all clusters into buckets
    //  respective to cluster
    for (int i = 0; i < _width; i++)
    {
		for (int j = 0; j < _height; j++)
		{
			//for each rgb, adds value to running sum
			//  indexed by which cluster data i is in
			for (int k = 0; k < 2; k++)
				sums[_clusters[i][j]][k] += _field[i][j][k];

			nums[_clusters[i][j]] += 1;
		}
    }

    //calculates new means
    vector<vector<int>> new_means;

    for (int i = 0; i < _k; i++)
    {
        vector<int> new_mean;

        if (!nums[i]) nums[i] = 1;

        for (int k = 0; k < 2; k++)
            new_mean.push_back(sums[i][k] / nums[i]);

        new_means.push_back(new_mean);
    }

    //checks convergence between new and old means
    bool converge = true;

    for (int i = 0; i < _k; i++)
    {
        int distance = pow(pow(new_means[i][0] - _means[i][0], 2) 
                         + pow(new_means[i][1] - _means[i][1], 2)
        , 0.5);

        for (int k = 0; k < 2; k++)
            _means[i][k] = new_means[i][k];

        if (distance > CONVERGE)
            converge = false;
    }

    _iteration++;
    
    if (_iteration > 100)
        converge = true;
    
#ifdef DEBUG
    cout << "       means adjusted to:" << endl;

    for (int i = 0; i < _k; i++)
        cout << "           " << i << " - (" 
             << _means[i][0] << ", " << _means[i][1]
             << ")" << endl;

    if (converge)
        cout << "       convergence achieved" << endl;
    else if (_iteration > 100)
        cout << "       convergence forced by iteration" << endl;
    else
        cout << "       convergence not yet achieved" << endl;
#endif

    return converge;
}