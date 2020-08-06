#include "..\events.h"

class Fractal : public Event
{
	public:

		Fractal();
		Fractal(int start, int duration, int width, int height, int scaleFactor) :
			Event(start, duration, width, height), _scaleFactor(scaleFactor) {
				cout << "Fractal created, with start " << start << " and duration " << duration << endl;
			}

		~Fractal();

	private:

		int _scaleFactor;

		void Activate(Bitmap* b, Layer* l);
		int* getColor(int i, int j, int n);
		int evalFractal(float re, float im, int n);
};

void Fractal::Activate(Bitmap* b, Layer* l)
{
	vector<uint8_t> frame;

	for (int n = 0; n < l->_frame_num; n++)
	{
		for (int i = 0; i < _width; i++)
			for (int j = 0; j < _height; j++)
				b -> getPixel(i, j).setRGB(getColor(i, j, n));

		getFrame(frame, *b);
		l->_frame_data.push_back(frame);
		cout << "	Fractal Frame " << n << " Generated" << endl; 
	}
}

//
int* Fractal::getColor(int i, int j, int n)
{
	int* ret_color = new int[3];
	int* div_color = new int[3];
	int* con_color = new int[3];

	if (_scaleFactor % 4 == 3)
	{
		//Sets base convergent color
		con_color[0] = 32;
		con_color[1] = 64;
		con_color[2] = 0;

		//Sets base divergent color
		div_color[0] = 200;
		div_color[1] = 64;
		div_color[2] = 0;
	}
	else if (_scaleFactor % 4 == 1)
	{
		//Sets base convergent color
		con_color[0] = 0;
		con_color[1] = 32;
		con_color[2] = 64;

		//Sets base divergent color
		div_color[0] = 0;
		div_color[1] = 200;
		div_color[2] = 64;
	}
	else if (_scaleFactor % 4 == 0)
	{
		//Sets base convergent color
		con_color[0] = 64;
		con_color[1] = 0;
		con_color[2] = 32;

		//Sets base divergent color
		div_color[0] = 64;
		div_color[1] = 0;
		div_color[2] = 200;
	}
	else if (_scaleFactor % 4 == 2)
	{
		//Sets base convergent color
		con_color[0] = 100;
		con_color[1] = 25;
		con_color[2] = 25;

		//Sets base divergent color
		div_color[0] = 50;
		div_color[1] = 150;
		div_color[2] = 50;
	}

	float re, im;


	//Transform row and col coordinates
	//	to float coordiates over box [-2, -1] x [1, 1]
	if (_scaleFactor % 6 == 0)
	{
		re = ((float) i / _width) * 4 - 2;
		im = ((float) j / _height) * 2.5 - 1.25;
	}
	else if (_scaleFactor % 6 == 4)
	{
		re = ((float) i / _width) * 2 - 2;
		im = ((float) j / _height) * 1.25 - 1.25;
	}
	else if (_scaleFactor % 6 == 1)
	{
		re = ((float) i / _width) * 1 - 2;
		im = ((float) j / _height) * 0.75 - 1.25;
	}
	else if (_scaleFactor % 6 == 2)
	{
		re = ((float) i / _width) * 2 - 1;
		im = ((float) j / _height) * 1.25 - .75;
	}
	else if (_scaleFactor % 6 == 3)
	{
		re = ((float) i / _width) * 1 - 1;
		im = ((float) j / _height) * .75 - 75;
	}
	else if (_scaleFactor % 6 == 5)
	{
		re = ((float) i / _width) * 1;
		im = ((float) j / _height) * 0.75;
	}

	//Returns how many iterations it takes to converge
	//	Or returns 0 if diverges
	int eval = evalFractal(re, im, n);

	//Assigns color value given eval
	//	Divergent Case
	if (eval > 1) {	
		ret_color[0] = div_color[0] / (eval / 2);
		ret_color[1] = div_color[1] / (eval / 2);
		ret_color[2] = div_color[2] / (eval / 2);
	//	Convergent Case
	} else {			
		ret_color[0] = con_color[0];
		ret_color[1] = con_color[1];
		ret_color[2] = con_color[2];
	}
	
	//cout << "return color (" << i << ", " << j << ") = " << ret_color[0] 
	//     << ", " << ret_color[1] << " – Eval = " << eval << endl;

	delete[] div_color;
	delete[] con_color;

	return ret_color;
}

//Evaluates whether mandlebrot set converges or diverges
//	And returns number of iterations taken
int Fractal::evalFractal(float c_re, float c_im, int n)
{
	float z_re = 0, z_im = 0;
	int i = 0;
	bool diverge = false;
	bool converge = false;

	while (!diverge && !converge)
	{
		/* Multiplying Complex Numbers...
			(a + bi) * (c + di) + (e + fi)
			real = a*c - b*d + e
			imag = a*d + b*c + f 
		*/

		z_re = (z_re * z_re) - (z_im * z_im) + c_re;
		z_im = (z_re * z_im) + (z_re * z_im) + c_im;
		i++;

		if (z_re <= -2 || z_re >= 2 || z_im <= -2 || z_im >= 2)
			diverge = true;

		if (i >= n)
			converge = true;
	}

	if (diverge) return i;

	return 0;
}