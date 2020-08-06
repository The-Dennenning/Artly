#include ".\events.h"

class Automata : public Event {

	public:
		Automata(int start, int dur, int type, int* args_i, float* args_f) :
			Event(start, dur), _type(type), _args_i(args_i), _args_f(args_f) {}

		~Automata()
		{
			if (_args_i) delete[] _args_i;
			if (_args_f) delete[] _args_f;
		}
		
		virtual void Activate(Bitmap* b, int frame_num);
		
		Bitmap* _b;
		
	private:

		int _type;
		int* _args_i;
		float* _args_f;

		//Helper function for Automata
		int* countNeighbors(int i, int j, int AE, int* newRGB, int state_num);

		void automata_2S();		// _type = 1
		void automata_3S();		// _type = 2
};


void Automata::Activate(Bitmap* b, int frame_num)
{
	_b = b;

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
		}
	}
}

//Counts neighbors of pixel at position i, j with area of effect AE
//	Gets new color, returned in newrgb
//	Only considers "neighbors" that are in state specified by 'states'
//		state_num determines how many states to consider
int* Automata::countNeighbors(int i, int j, int AE, int* newrgb, int state_num)
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
void Automata::automata_2S()
{
	//Factor in area of effect to ruleset
	for (int i = 0; i < 4; i++)
		_args_i[i] * (pow(3 + (_args_i[4] - 1) * 2, 2) - 1) / 8;

	//Loop through bitmap applying automata Automata
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

	//Rectifies changes made during cellular automata Automata
	for (int i = 0; i < _b->getSize(0); i++){
		for (int j = 0; j < _b->getSize(1); j++) {
			_b->getPixel(i, j).rectify();
		}
	}
}

//Automata transformation
//	3 State - Cerebellum
void Automata::automata_3S()
{

	//Loop through bitmap applying automata Automata
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

	//Rectifies changes made during cellular automata Automata
	for (int i = 0; i < _b->getSize(0); i++){
		for (int j = 0; j < _b->getSize(1); j++) {
			_b->getPixel(i, j).rectify();
		}
	}
}