#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include "bitmap.h"

using namespace std;

#ifndef _EVENT_HPP
#define _EVENT_HPP

class Event {
	public:
		Event(int start, int duration) : _start(start), _duration(duration) {}

		virtual void Activate(Bitmap &b, int frame_num) = 0;

	protected:
		int _start;
		int _duration;
};

class Automata : public Event 
{
	public:

		Automata();
		~Automata();

	private:

};

class Animation : public Event {
	public:
		Animation();
		~Animation();

		//Called once per frame, calls Write_Pixel appropriately
		virtual void Activate(Bitmap &b, int frame_num);   

	private:
		Bitmap *_b;

		int* _rgb_start;     //Initial color values of animation
		int** _rgb_script;   //Frame by frame delta color values
								//To be added to rgb values each frame

		int* _pix_start;     //Initial coordinates of central pixel
		int** _pix_script;   //Frame by frame delta coordinate values
								//To be added to coordinate each frame

		int _shape;          //One of a predetermined number of shapes
		int* _shape_params;  //Parameters unique to shape 

		//Writes a color value to a pixel
		void Write_Pixel(int* coords, int* rgb);

		//Applies filter inside of shape
		void Apply_Filter(int* coords, int shape);
};


void Animation::Write_Pixel(int* coords, int* rgb)
{
	
}

class Filter : public Event {

	public:
		Filter(int start, int dur, int type, int* args_i, float* args_f) :
			Event(start, dur), _type(type), _args_i(args_i), _args_f(args_f) {}

		~Filter()
		{
			if (_args_i) delete[] _args_i;
			if (_args_f) delete[] _args_f;
		}
		
		virtual void Activate(Bitmap &b, int frame_num);
		
		Bitmap *_b;
		
	private:

		int _type;
		int* _args_i;
		float* _args_f;

		//Helper function for Automata
		int* countNeighbors(int i, int j, int AE, int* newRGB, int state_num);

		void automata_2S();		// _type = 1
		void automata_3S();		// _type = 2
		void pixelate();		// _type = 3
		void blur();			// _type = 4
		void cellshade();		// _type = 5
		void zoom();			// _type = 6
		void pan();				// _type = 7
};


void Filter::Activate(Bitmap &b, int frame_num)
{
	_b = &b;

	//Activate if frame number is after start but before end of duration
	if (frame_num >= _start && frame_num < _start + _duration)
	{
		switch(_type)
		{
			case 1:
				automata_2S();
				break;
			case 2:
				automata_3S();
				break;
			case 3:
				pixelate();
				break;
			case 4:
				blur();
				break;
			case 5:
				cellshade();
				break;
			case 6:
				zoom();
				break;
			case 7:
				pan();
				break;
		}
	}
}

//Counts neighbors of pixel at position i, j with area of effect AE
//	Gets new color, returned in newrgb
//	Only considers "neighbors" that are in state specified by 'states'
//		state_num determines how many states to consider
int* Filter::countNeighbors(int i, int j, int AE, int* newrgb, int state_num)
{
	int i_upper_bound, i_lower_bound;
	int j_upper_bound, j_lower_bound;
	int row = _b->getSize(0);
	int col = _b->getSize(1);

	long sumrgb[4] = {0, 0, 0, 0};

	int *neighbors = new int[state_num];
	int not_none = 0;

	for (int i = 0; i < state_num; i++)
		neighbors[i] = 0;
	
	//Ensures bounds respect edges of table given Area of Effect
	if 	(i - AE < 0)		{i_lower_bound = 0;}
	else 					{i_lower_bound = i - AE;}
	if 	(j - AE < 0) 		{j_lower_bound = 0;}
	else 					{j_lower_bound = j - AE;}
	if 	(i + AE >= row) 	{i_upper_bound = row - 1;}
	else 					{i_upper_bound = i + AE;}
	if 	(j + AE >= col) 	{j_upper_bound = col - 1;}
	else 					{j_upper_bound = j + AE;}
	
	//Counts Neighbors of current pixel
	//	in every state given by state_num
	for (int k = i_lower_bound; k <= i_upper_bound; k++) 
	{
		for (int l = j_lower_bound; l <= j_upper_bound; l++) 
		{
			if ((k != i) || (l != j))
			{
				int state = _b->getPixel(k, l).getState();
				neighbors[state]++;
				
				if (state)
				{
					//Get color (for color blending purposes)
					int* rgb = _b->getPixel(k, l).getHeldRGB();

					for (int n = 0; n < 4; n++)
						sumrgb[n] += rgb[n];

					not_none++;
				}
			}
		}
	}

	//Averages color of active neighbors (if any neighbors are alive)
	for (int n = 0; n < 4; n++) 
	{
		if (not_none > 0)
			newrgb[n] = (int) ((float)sumrgb[n] / not_none);
		else
			newrgb[n] = _b->getPixel(i, j).getHeldRGB()[n];
	}

	return neighbors;
}

//Automata transformation
//	Standard 2-state, as in Conway's Game of Life
//
void Filter::automata_2S()
{
	//Factor in area of effect to ruleset
	for (int i = 0; i < 4; i++)
		_args_i[i] * (pow(3 + (_args_i[4] - 1) * 2, 2) - 1) / 8;

	//Loop through bitmap applying automata filter
	for (int i = 0; i < _b->getSize(0); i++){

		for (int j = 0; j < _b->getSize(1); j++) {

			//Get neighbors of current pixel
			int newrgb[4] = {0, 0, 0, 0};
			int* neighbors = countNeighbors(i, j, _args_i[4], newrgb, 2);

			_b->getPixel(i,j).setHeldRGB(newrgb);

			//Calculates whether, based on active neighbors, an inactive pixel turns on
			if (_b->getPixel(i, j).getState() == 0){
				if ((neighbors[1] >= _args_i[2]) && (neighbors[1] <= _args_i[3])) {
					_b->getPixel(i, j).transition(1);
				} else {
					_b->getPixel(i, j).transition(0);
				}
			}
			
			//Calculates whether, based on inative neighbors, an active pixel turns off
			else if (_b->getPixel(i, j).getState() == 1) {
				if ((neighbors[1] >= _args_i[0]) && (neighbors[1] <= _args_i[1])) {
					_b->getPixel(i, j).transition(1);
				} else {
					_b->getPixel(i, j).transition(0);
				}
			}

			delete[] neighbors;
		}
	}

	//Rectifies changes made during cellular automata filter
	for (int i = 0; i < _b->getSize(0); i++){
		for (int j = 0; j < _b->getSize(1); j++) {
			_b->getPixel(i, j).rectify();
		}
	}
}

//Automata transformation
//	3 State - Cerebellum
void Filter::automata_3S()
{

	//Loop through bitmap applying automata filter
	for (int i = 0; i < _b->getSize(0); i++){

		for (int j = 0; j < _b->getSize(1); j++) {
			//Get neighbors of current pixel
	
			int newrgb[4] = {0, 0, 0, 0};
			int* neighbors = countNeighbors(i, j, 1, newrgb, 3);

			_b->getPixel(i,j).setHeldRGB(newrgb);


		
			//Transitions for state 0
			if (_b->getPixel(i, j).getState() == 0)
			{
				if (neighbors[1] <= 2 && neighbors[2] >= 3)
					_b->getPixel(i, j).transition(2);
				else
					_b->getPixel(i, j).transition(0);
			}
			
			//Transitions for state 1
			else if (_b->getPixel(i, j).getState() == 1) 
			{
				_b->getPixel(i, j).transition(0);
			}

			//Transitions for state 2
			else if (_b->getPixel(i, j).getState() == 2)
			{
				if (neighbors[1] >= 3 || neighbors[2] >= 4 || neighbors[2] == 0)
					_b->getPixel(i, j).transition(1);
				else
					_b->getPixel(i, j).transition(2);
			}
			
			delete[] neighbors;
		}
	}

	//Rectifies changes made during cellular automata filter
	for (int i = 0; i < _b->getSize(0); i++){
		for (int j = 0; j < _b->getSize(1); j++) {
			_b->getPixel(i, j).rectify();
		}
	}
}



//Cellshades entire map contained by object of class Bitmap
//	Cellshade - round each RGB value individually
//		    such that color palette is reduced
void Filter::cellshade() {

	int* temp_rgb;
	float temp_f;

	for (int i = 0; i < _b->getSize(0); i++) {
		for (int j = 0; j < _b->getSize(1); j++) {
			temp_rgb = _b->getPixel(i, j).getRGB();
			
			//Performs cell shade by dividing rgb values such that
			//	0, 128, and 255 can be represented by 0, 1, and 2,
			//	which these values are then rounded to. Thus, after 
			//	scaling back up, the rgb values are either 0, 128, or 255.
			for (int k = 0; k < 3; k++) {
				temp_f = (float)(temp_rgb[k] * 2) / 256;
				temp_f = round(temp_f);
				temp_rgb[k] = (int)(temp_f * 256) / 2;
				if (temp_rgb[k] == 256) temp_rgb[k]--;
			}
			
			_b->getPixel(i, j).setRGB(temp_rgb);
		}
	}
}

//Pixellates entire map contained by object of class Bitmap
void Filter::pixelate() {
		
	int sum[3];
	int degree = 4;	//Divides image into 16x16 blocks
	int size = pow(2, degree);
	int* temp_rgb;

	//Loops through all pixels in steps determined by degree of pixellation
	for (int i = 0; i < _b->getSize(0) - size; i = i + size) {
		for (int j = 0; j < _b->getSize(1) - size; j = j + size) {

			//Zeroes out summing intiable
			for (int k = 0; k < 3; k++)
				sum[k] = 0;

			//Loops through all pixels in current step to get average color
			for (int i_p = i; i_p < i + size; i_p++) {
				for (int j_p = j; j_p < j + size; j_p++) {
	
					temp_rgb = _b->getPixel(i_p, j_p).getRGB();

					for (int k = 0; k < 3; k++) 
						sum[k] += temp_rgb[k];
				}
			}
			
			//Averages color aross 2^degree x 2^degree square
			for (int k = 0; k < 3; k++)
				temp_rgb[k] = sum[k] / pow(size, 2);
			
			//Loops through all pixels in current step to assign new color
			for (int i_p = i; i_p < i + size; i_p++) {
				for (int j_p = j; j_p < j + size; j_p++) {	
					_b->getPixel(i_p, j_p).setRGB(temp_rgb);
				}
			}
		}
	}
}

void Filter::blur() 
{
	int blurArray[] = {1, 4, 6, 4, 1};
	double sum[3];
	int total;
	int* temp_rgb;

	//Loops through all pixels
	for (int i = 0; i < _b->getSize(0); i++) {
		for (int j = 0; j < _b->getSize(1); j++) {

			//Zeroes out summing variable
			for (int k = 0; k < 3; k++)
				sum[k] = 0;
			total = 0;

			//Loops through whatever of the gaussian blur matrix is in-bounds 
			//	With gaussian matrix centered on pixel(i, j)
			for (int i_b = -2; i_b < 3; i_b++) {
				for (int j_b = -2; j_b < 3; j_b++) {
					if (i + i_b >= 0 && i + i_b < _b->getSize(0)) { //Skips entry if out of bounds
					if (j + j_b >= 0 && j + j_b < _b->getSize(1)) {
						
						temp_rgb = _b->getPixel(i + i_b, j + j_b).getRGB();
						
						//Multiplies value by gaussian coefficients
						for (int k = 0; k < 3; k++) 
							sum[k] += blurArray[i_b + 2] * blurArray[j_b + 2] * temp_rgb[k];

						//Totals gaussian coefficients (for averaging purposes)
						total += (blurArray[i_b + 2] * blurArray[j_b + 2]);
					}
					}
				}
			}

			temp_rgb = _b->getPixel(i, j).getRGB();
			
			//Averages out pixel RGB values
			for (int k = 0; k < 3; k++)
				temp_rgb[k] = sum[k] / total;

			//Assigns nextRGB value to pixel (i, j)
			_b->getPixel(i, j).setNextRGB(temp_rgb);
		}
	}

	//Sets all pixels to their nextRGB values
	for (int i = 0; i < _b->getSize(0); i++)
		for (int j = 0; j < _b->getSize(1); j++)
			_b->getPixel(i, j).rectifyRGB();
}

void Filter::zoom()
{}

void Filter::pan()
{}

#endif