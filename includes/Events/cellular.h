#include "..\events.h"

class Cellular : public Event {

	public:
		Cellular(int start, int dur, int width, int height, int type, vector<int> args) :
			Event(start, dur, width, height), _type(type), _args(args) {}

		~Cellular() {}
		
		void Activate(Frame* f, Layer* l);
		
	private:

		int _type;
		vector<int> _args;

		vector<vector<int>> _curr_state;		//current automata state
		vector<vector<int>> _next_state;		//next automata state
		vector<vector<vector<int>>> _held_rgb;	//color held in cell

		//Helper function for Cellular
		int _countNeighbors(Frame* f, int i, int j, int AE, int* newRGB);

		void Cellular_2S(Frame* f);		// _type = 1
		void Cellular_3S(Frame *f);		// _type = 2
};


void Cellular::Activate(Frame* f, Layer* l)
{
	cout << "	prepping metadata..." << endl;
	//Prep metadata
	for (int i = 0; i < _width; i++)
	{
		vector<int> curr(_height, 0);
		_curr_state.push_back(curr);

		vector<int> next(_height, 0);
		_next_state.push_back(next);

		vector<vector<int>> rgbs;
		for (int j = 0; j < _height; j++)
		{
			int rgba[] = {0, 0, 0, 0};
			f->get(i, j, rgba);

				int sum = rgba[0] + rgba[1] + rgba[2];
				if (sum > 64) 
				{
					_curr_state[i][j] = 1;
				}
				
			vector<int> rgb;
				rgb.push_back(rgba[0]);
				rgb.push_back(rgba[1]);
				rgb.push_back(rgba[2]);
			rgbs.push_back(rgb);
		}
		_held_rgb.push_back(rgbs);
	}

	cout << "	beginning cellular run..." << endl;

	for (int n = 0; n < l->_frame_num; n++)
	{
		l->_frames.push_back(new Frame(*f));
		f = l->_frames[n];

		Cellular_2S(f);

		for (int i = 0; i < _width; i++) for (int j = 0; j < _height; j++)
			_curr_state[i][j] = _next_state[i][j];
	
#ifdef DEBUG
		cout << "	Cellular Frame " << n << " Generated" << endl; 
#endif
	}
}

//Counts neighbors of pixel at position i, j with area of effect AE
//	Gets new color, returned in newrgb
//	Only considers "neighbors" that are in state specified by 'states'
//		state_num determines how many states to consider
int Cellular::_countNeighbors(Frame *f, int i, int j, int AE, int* newrgb)
{
	int i_upper_bound, i_lower_bound;
	int j_upper_bound, j_lower_bound;
	int row = _width;
	int col = _height;

	long sumrgb[4] = {0, 0, 0, 0};

	int neighbors = 0;
	int not_none = 0;
	
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
				if (_curr_state[k][l])
				{	
					neighbors++;

					for (int n = 0; n < 3; n++)
						sumrgb[n] += _held_rgb[k][l][n];

					not_none++;
				}
			}
		}
	}

	//Averages color of active neighbors (if any neighbors are alive)
	for (int n = 0; n < 3; n++) 
	{
		if (not_none > 0)
			newrgb[n] = (int) ((float)sumrgb[n] / not_none);
		else
			newrgb[n] = _held_rgb[i][j][n];
	}

	return neighbors;
}

//Cellular transformation
//	Standard 2-state, as in Conway's Game of Life
//
void Cellular::Cellular_2S(Frame* f)
{
	//Factor in area of effect to ruleset
	for (int i = 0; i < 4; i++)
		_args[i] = _args[i] * (pow(3 + (_args[4] - 1) * 2, 2) - 1) / 8;

	//Loop through bitmap applying Cellular Cellular
	for (int i = 0; i < _width; i++){

		for (int j = 0; j < _height; j++) {

			//Get neighbors of current pixel
			int newrgb[4] = {0, 0, 0, 0};
			int revrgb[4] = {0, 0, 0, 0};
			int neighbors = _countNeighbors(f, i, j, _args[4], newrgb);

			for (int k = 0; k < 3; k++)
				revrgb[k] = 255 - newrgb[k];

			//Calculates whether, based on active neighbors, an inactive pixel turns on
			if (_curr_state[i][j] == 0){
				if ((neighbors >= _args[2]) && (neighbors <= _args[3])) {
					_next_state[i][j] = 1;
					f->set(i, j, revrgb);
				} else {
					_next_state[i][j] = 0;
					f->set(i, j, newrgb);
				}
			}
			
			//Calculates whether, based on inative neighbors, an active pixel turns off
			else if (_curr_state[i][j] == 1) {
				if ((neighbors >= _args[0]) && (neighbors <= _args[1])) {
					_next_state[i][j] = 1;
					f->set(i, j, revrgb);
				 } else {
					_next_state[i][j] = 0;
					f->set(i, j, newrgb);
				}
			}

			for (int n = 0; n < 3; n++)
				_held_rgb[i][j][n] = newrgb[n];
		}
	}
}

/*
//Cellular transformation
//	3 State - Cerebellum
void Cellular::Cellular_3S()
{

	//Loop through bitmap applying Cellular Cellular
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

	//Rectifies changes made during cellular Cellular Cellular
	for (int i = 0; i < _b->getSize(0); i++){
		for (int j = 0; j < _b->getSize(1); j++) {
			_b->getPixel(i, j).rectify();
		}
	}
}
*/