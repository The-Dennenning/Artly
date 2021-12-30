#include "../events.h"

class Dither : public Event
{
	public:
		Dither();
		Dither(int start, int dur, int width, int height) :
			Event(start, dur, width, height) {}

		void Activate(Frame* f, Layer* l);
		Frame* Activate(Frame* f);
		Layer* Activate(Layer* l);
		void dither(Frame* f);

};

void Dither::Activate(Frame* f, Layer* l)
{
	Frame* _f = new Frame(*f);
	dither(_f);

	for (int n = 0; n < l->_frame_num; n++)
		l->_frames.push_back(new Frame(*_f));
}

Frame* Dither::Activate(Frame* f)
{
	Frame* _f = new Frame(*f);
	dither(_f);
	return _f;
}

Layer* Dither::Activate(Layer* l)
{
	for (int n = 0; n < l->_frame_num; n++)
		dither(l->_frames[n]);
}

void Dither::dither(Frame *f)
{
    float *out_data = new float[_width * _height];
	float *acc_data = new float[_width * _height];

	for (int i = 0; i < _width * _height; i++)
	{
		out_data[i] = 0;
		acc_data[i] = 0;
	}

	//Pass over image top to bottom, perform FS Dithering
	for (int i = 0; i < _width; i++)
	{
		//Pass over image from left to right
		for (int j = 0; j < _height; j++)
		{
			//Get grayscaled pixel value
			acc_data[i * _height + j] += (.299 * (float) f->_frame_data[i * (_height * 4) + (j * 4) + 0]
									  + .587 * (float) f->_frame_data[i * (_height * 4) + (j * 4) + 1]
									  + .114 * (float) f->_frame_data[i * (_height * 4) + (j * 4) + 2]) / 256;

			//Threshold and output
			if (acc_data[i * _height + j] <= 0.5)
				out_data[i * _height + j] = 0;
			else
				out_data[i * _height + j] = 1;

			//Calculate error between acc and out
			float e = acc_data[i * _height + j] - out_data[i * _height + j];

			//Propogate error to other pixels' i_acc
			if ((j + 1) < _height) 
				acc_data[i * _height + j + 1] += ((float)7 / 16) * e;
			

			if ((i + 1) < _width)
			{
				acc_data[(i + 1) * _height + j] += ((float)5/16) * e;

				if ((j - 1) >= 0) 
					acc_data[(i + 1) * _height + j - 1] += ((float)3/16) * e;
					
				if ((j + 1) < _height) 
					acc_data[(i + 1) * _height + j + 1] += ((float)1/16) * e;
			}
		}

		//Checks if one more row is available to zag
		if (i + 1 < _width)
		{
			//Iterates one row down
			i++;

			//Pass over image from right to left
			for (int j = _height - 1; j >= 0; j--)
			{
				//Get grayscaled pixel value
				acc_data[i * _height + j] += (.299 * (float) f->_frame_data[i * (_height * 4) + (j * 4) + 0]
					                      + .587 * (float) f->_frame_data[i * (_height * 4) + (j * 4) + 1]
					                      + .114 * (float) f->_frame_data[i * (_height * 4) + (j * 4) + 2]) / 256;

				//Threshold and output
				if (acc_data[i * _height + j] <= 0.5)
					out_data[i * _height + j] = 0;
				else
					out_data[i * _height + j] = 1;

				//Calculate error between acc and out
				float e = acc_data[i * _height + j] - out_data[i * _height + j];

				//Propogate error to other pixels' i_acc
				//	Where applicable
				if ((j - 1) >= 0)
					acc_data[i * _height + j - 1] += ((float)7 / 16) * e;

				if ((i + 1) < _width)
				{
					acc_data[(i + 1) * _height + j] += ((float)5 / 16) * e;

					if ((j + 1) < _height)
						acc_data[(i + 1) * _height + j + 1] += ((float)3 / 16) * e;

					if ((j - 1) >= 0)
						acc_data[(i + 1) * _height + j - 1] += ((float)1 / 16) * e;
				}
			}
		}
	}

	//Put dithered image into data
	for (int i = 0; i < _height * _width; i++)
	{
		if (out_data[i] < 0)
			out_data[i] = 0;
		if (out_data[i] == 1)
			out_data[i] = ((float)255 / 256);

		f->_frame_data[i * 4 + 0] = (uint8_t) floor(out_data[i] * 256);
		f->_frame_data[i * 4 + 1] = (uint8_t) floor(out_data[i] * 256);
		f->_frame_data[i * 4 + 2] = (uint8_t) floor(out_data[i] * 256);
	}

	delete[] acc_data;
	delete[] out_data;
}