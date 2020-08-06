#include "./events.h"


class Stroke { // Data structure for holding painterly strokes.

	public:
		Stroke::Stroke(int iradius, int ix, int iy, int ir, int ig, int ib, int ia) 
			: radius(iradius),x(ix),y(iy),r(ir),g(ig),b(ib),a(ia) {}
   
   // data
   int radius, x, y;	// Location for the stroke
   int r, g, b, a;		// Color
};

class Painter : public Event 
{
	public:
		Painter();
		Painter(int start, int duration, Bitmap *b, Bitmap *r, int firstLayer, int *strokeNum, int strokeMax, int radius) 
			: Event(start, duration), _r(r), _firstLayer(firstLayer), _strokeMax(strokeMax), _radius(radius), _width(b->getSize(0)), _height(b->getSize(1)) 
		{
			cout << "	Generating Reference..." << endl;
			Generate_Reference();
			cout << "	Generating Layer..." << endl;
			Generate_Layer(b);

			*strokeNum = _strokeNum / 20;
			_duration = _strokeNum / 20;
		}

		~Painter();
		
		void Activate(Bitmap *b, int frame_num);

	private:
		int _firstLayer;
		int _strokeNum;
		int _strokeMax;
		int _radius;
		int _width;
		int _height;

		Bitmap* _r;
		vector<Stroke*> _s;

		//Generates reference image
		//	using variously sized gaussian blur
		void Generate_Reference();

		//Generates layer of strokes
		void Generate_Layer(Bitmap *b);

		//Paints a single stroke from layer
		void Paint_Stroke(Bitmap *b, const Stroke& s);
};


void Painter::Activate(Bitmap* b, int frame_num)
{
	if (frame_num < _start || frame_num >= _start + _duration)
		return;

	for (int i = 0; i < 20; i++)
		Paint_Stroke(b, *_s[(frame_num - _start) * 20 + i]);
}


///////////////////////////////////////////////////////////////////////////////
//
//		Helper Function for NPR_Paint
//			Applies gaussian blur of size 2 * R + 1
//
///////////////////////////////////////////////////////////////////////////////
void Painter::Generate_Reference()
{
	int N = 2 * _radius + 1;
	double *Gaussian = new double[N];
	double sum[3];
	double total;

	int*** temp = new int**[_width];
	for (int i = 0; i < _width; i++)
	{
		temp[i] = new int*[_height];
		for (int j = 0; j < _height; j++)
		{
			temp[i][j] = new int[3];
		}
	}

	//Calculates 1D Gaussian
	for (int k = 0; k < N; k++)
		Gaussian[k] = Binomial(N, k);

	cout << "		Initializing Gaussian Blur..." << endl;

	//Loop through image
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			//Zero out summing variable
			for (int k = 0; k < 3; k++)
				sum[k] = 0;

			total = 0;

			int lb = -(N / 2);
			int ub = (N / 2) + (N % 2);

			//Computes value of filter over pixel (i, j) given sized filter N
			for (int g_i = lb; g_i < ub; g_i++)
			{
				for (int g_j = lb; g_j < ub; g_j++)
				{
					//Skips values if out of bounds
					if (i + g_i >= 0 && i + g_i < _width)
					{
						if (j + g_j >= 0 && j + g_j < _height)
						{
							//Gets pixel value at specified pixel
							int* rgb = _r->getPixel(i + g_i, j + g_j).getRGB();

							//Multiplies two 1-D gaussian filter terms together
							//	Simulating 2-D gaussian filter
							for (int k = 0; k < 3; k++)
								sum[k] += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)] * rgb[k];

							total += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)];
						}
					}
				}
			}

			//Assigns value to temporary holding
			//	So as to not mess with other filter calculations
			for (int k = 0; k < 3; k++)
				temp[i][j][k] = (int) (sum[k] / total); //weighted average
		}
	}

	cout << "		Setting Image Values..." << endl;

	//Empties and Deletes temporary holding
	
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			for (int k = 0; k < 3; k++)
				_r->getPixel(i, j).getRGB()[k] = temp[i][j][k];

			delete[] temp[i][j];
		}
		delete[] temp[i];
	}

	delete[] temp;
}

///////////////////////////////////////////////////////////////////////////////
//
//		Helper Function for NPR_Paint
//			Applies paint strokes to canvas
//		
///////////////////////////////////////////////////////////////////////////////
void Painter::Generate_Layer(Bitmap* _b)
{
	//Create new set of strokes
	_strokeNum = 0;

	//Sets grid to f_g * R, with f_g = 1
	int grid = _radius;

	//Sets threshold to 25
	int T = 25;

	//Create difference image
	//	Only holds one value per pixel, so is only width * height
	double *D = new double[_width * _height];

	//Initializes Difference Image
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			if (!_firstLayer)
			{
				//Calculates difference image given initailized canvas
				double sum = 0;

				int* rgb_r = _r->getPixel(i, j).getRGB();
				int* rgb_o = _b->getPixel(i, j).getRGB();

				for (int k = 0; k < 3; k++)
					sum += (rgb_r[k] - rgb_o[k]) * (rgb_r[k] - rgb_o[k]);

				D[i * _height + j] = sqrt(sum);
			}
			else
			{
				//Initializes difference image, in order to initialize canvas
				D[i * _height + j] = 100;
			}
		}
	}

	//Generates Strokes
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			//To store cumulative error
			double areaError = 0;

			//To store maximum error
			double max = 0;

			//To store maximum error coordinates
			int max_i = 0, max_j = 0;
			int N = grid / 2;

			//Sum error near (i, j)
			for (int i_g = i - N; i_g < i + N + 1; i_g++)
			{
				for (int j_g = j - N; j_g < j + N + 1; j_g++)
				{
					if (i_g >= 0 && i_g < _width)
					{
						if (j_g >= 0 && j_g < _height)
						{
							//Add error to sum
							areaError += D[i_g * _height + j_g];

							//Check if error is max
							if (D[i_g * _height + j_g] > max)
							{
								max = D[i_g * _height + j_g];
								max_i = i_g;
								max_j = j_g;
							}
						}
					}
				}
			}

			//Calculate error of area
			areaError /= (grid * grid);

			//Threshold
			if (areaError > T) {

				//Get Pixel rgb value
				int* rgb = _r->getPixel(max_i, max_j).getRGB();

				//cout << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;

				//Make Stroke unsigned int iradius, unsigned int ix, unsigned int iy, unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia
				_s.push_back(new Stroke(_radius, max_j, max_i, rgb[2], rgb[1], rgb[0], 255));

				_strokeNum++;
			}
		}
	}

	//Shuffle, for natural look
	std::random_shuffle(_s.begin(), _s.end());

	//Limit number of strokes layer can have
	if (_strokeNum > _strokeMax) _strokeNum = _strokeMax;
}

void Painter::Paint_Stroke(Bitmap* _b, const Stroke& s) {
   int radius_squared = s.radius * s.radius;
   for (int x_off = -(s.radius); x_off <= s.radius; x_off++) {
      for (int y_off = -(s.radius); y_off <= s.radius; y_off++) {
         int x_loc = s.x + x_off;
         int y_loc = s.y + y_off;
         // are we inside the circle, and inside the image?
         if ((x_loc >= 0 && x_loc < _height && y_loc >= 0 && y_loc < _width)) {
            int dist_squared = x_off * x_off + y_off * y_off;
            if (dist_squared <= radius_squared) {
				/*
               _b->getPixel(y_loc, x_loc).getRGB()[0] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[0] + s.r) / 2;
               _b->getPixel(y_loc, x_loc).getRGB()[1] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[1] + s.g) / 2;
               _b->getPixel(y_loc, x_loc).getRGB()[2] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[2] + s.b) / 2;
				*/
				
               _b->getPixel(y_loc, x_loc).getRGB()[0] = s.r;
               _b->getPixel(y_loc, x_loc).getRGB()[1] = s.g;
               _b->getPixel(y_loc, x_loc).getRGB()[2] = s.b;
			   
            } else if (dist_squared >= radius_squared + 1 && dist_squared <= radius_squared + 2) {
               _b->getPixel(y_loc, x_loc).getRGB()[0] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[0] + s.r) / 2;
               _b->getPixel(y_loc, x_loc).getRGB()[1] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[1] + s.g) / 2;
               _b->getPixel(y_loc, x_loc).getRGB()[2] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[2] + s.b) / 2;
            }
         }
      }
   }
}
