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
		vector<vector<int>> _dead_clock;

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

		vector<int> dead(_height, 0);
		_dead_clock.push_back(dead);

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
		if (n % 10 == 0)
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
			int blkrgb[4] = {0, 0, 0, 0};
			int newrgb[4] = {0, 0, 0, 255};
			int revrgb[4] = {0, 0, 0, 255};
			int neighbors = _countNeighbors(f, i, j, _args[4], newrgb);

			for (int k = 0; k < 3; k++)
				revrgb[k] = 255 - newrgb[k];

			//Calculates whether, based on active neighbors, an inactive pixel turns on
			if (_curr_state[i][j] == 0)
			{
				if ((neighbors >= _args[2]) && (neighbors <= _args[3])) 
				{
					_next_state[i][j] = 1;
					f->set(i, j, revrgb);
				} 
				else 
				{
					_next_state[i][j] = 0;
					if (_type == 0)
						f->set(i, j, blkrgb);
					else if (_type == 1)
						f->set(i, j, newrgb);
					else if (_type == 2)
					{
						if (_dead_clock[i][j] > 10)
							f->set(i, j, blkrgb);
						else
							f->set(i, j, newrgb);

						_dead_clock[i][j]++;
					}
				}
			}
			
			//Calculates whether, based on inative neighbors, an active pixel turns off
			else if (_curr_state[i][j] == 1) 
			{
				if ((neighbors >= _args[0]) && (neighbors <= _args[1])) 
				{
					_next_state[i][j] = 1;
					f->set(i, j, revrgb);
				} 
				else 
				{
					_next_state[i][j] = 0;
					if (_type == 0)
						f->set(i, j, blkrgb);
					else if (_type == 1)
						f->set(i, j, newrgb);
					else if (_type == 2)
					{
						f->set(i, j, newrgb);
						_dead_clock[i][j] = 0;
					}
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


class Cellular_Hue : public Event
{
	public:
		Cellular_Hue(int start, int dur, int width, int height, int n)
			: Event(start, dur, width, height), _n(n) {}

		void Activate(Frame* f, Layer* l);


		struct hue_gene;

		struct field
		{
			int _width, _height;

			vector<vector<hue_gene*>> _data;
			vector<vector<int>> _flag;
			vector<vector<int>> _done;
			vector<vector<int>> _todo;

			field(int width, int height, vector<vector<int>> rgb, vector<vector<int>> coords)
				: _width(width), _height(height)
			{
				for (int i = 0; i < _width; i++)
				{
					vector<hue_gene*> line;
					vector<int> f_line;

					for (int j = 0; j < _height; j++)
					{
						line.push_back(nullptr);
						f_line.push_back(0);
					}

					_data.push_back(line);
					_flag.push_back(f_line);
				}
				
				for (int i = 0; i < coords.size(); i++)
					add_cell(coords[i], rgb[i]);
			}

			void activate()
			{
				vector<vector<int>> todo;
				todo.insert(todo.begin(), _todo.begin(), _todo.end());
				_todo.clear();

#ifdef DEBUG
				cout << todo.size() << endl;
#endif

				for (auto t : todo)
					add_cell(t);
			}
			
			void add_cell(vector<int> coords, vector<int> rgb)
			{
				if (_flag[coords[0]][coords[1]] == 1) return;

				_data[coords[0]][coords[1]] =  new hue_gene(coords, rgb, rgb, 0, this);
				_flag[coords[0]][coords[1]] = 1;

				add_todo(coords);
				_done.push_back(coords);
			}
			
			void add_cell(vector<int> coords)
			{
				if (_flag[coords[0]][coords[1]] == 1) return;

				_data[coords[0]][coords[1]] = new hue_gene(coords, this);
				_flag[coords[0]][coords[1]] = 1;

				add_todo(coords);
				_done.push_back(coords);
			}

			void add_todo(vector<int> coords)
			{
				int lb_i, ub_i, lb_j, ub_j;

				if (coords[0] - 1 < 0)
					lb_i = 0;
				else
					lb_i = coords[0] - 1;
					
				if (coords[1] - 1 < 0)
					lb_j = 0;
				else
					lb_j = coords[1] - 1;
					
				if (coords[0] + 1 >= _width)
					ub_i = _width - 1;
				else
					ub_i = coords[0] + 1;
					
				if (coords[1] + 1 >= _height)
					ub_j = _height - 1;
				else
					ub_j = coords[1] + 1;

				for (int i = lb_i; i <= ub_i; i++)
				{
					for (int j = lb_j; j <= ub_j; j++)
					{
						if (_flag[i][j] == 1) continue;
						
						vector<int> new_coords;
							new_coords.push_back(i);
							new_coords.push_back(j);
						_todo.push_back(new_coords);
						
					}
				}
			}
		};
		
		struct hue_gene
		{
			vector<int> _coords;
			vector<int> _rgb;
			vector<int> _p;

			int _conv;

			field* _f;

			hue_gene(vector<int> coords, vector<int> rgb, vector<int> p, int conv, field* f)
				: _coords(coords), _rgb(rgb), _p(p), _conv(conv), _f(f) {}

			hue_gene(vector<int> coords, field* f)
				: _coords(coords), _f(f)
			{
				_p.push_back(0);
				_p.push_back(0);
				_p.push_back(0);
				_rgb.push_back(0);
				_rgb.push_back(0);
				_rgb.push_back(0);

				int lb_i, ub_i, lb_j, ub_j;

				if (_coords[0] - 1 < 0)
					lb_i = 0;
				else
					lb_i = _coords[0] - 1;
					
				if (_coords[1] - 1 < 0)
					lb_j = 0;
				else
					lb_j = _coords[1] - 1;
					
				if (_coords[0] + 1 >= _f->_width)
					ub_i = _f->_width - 1;
				else
					ub_i = _coords[0] + 1;
					
				if (_coords[1] + 1 >= _f->_height)
					ub_j = _f->_height - 1;
				else
					ub_j = _coords[1] + 1;

				int sum = 0;
				int conv_sum = 0;

				for (int i = lb_i; i <= ub_i; i++)
				{
					for (int j = lb_j; j <= ub_j; j++)
					{
						if (_f->_flag[i][j] == 0) continue;

						sum++;

						conv_sum += _f->_data[i][j]->_conv;

						for (int k = 0; k < 3; k++)
							_p[k] += _f->_data[i][j]->_p[k];
					}
				}

				//cout << "	_p is (" << _p[0] << ", " << _p[1] << ", " << _p[2] << ")" << endl;

				if (sum == 0) sum = 1;

				_conv = ((double) conv_sum) / sum;

				/*
				if (rand() % 25 == 0)
					_conv += rand () % 11 - 10;
				*/

				for (int k = 0; k < 3; k++)
				{
					_p[k] = (((double) _p[k]) / sum);

					if (rand() % 10 == 0)
						_p[k] += rand() % 13;

					_rgb[k] = _p[(k + _conv) % 3];
					_rgb[k] %= 255;
				}
			}

			void print(Frame *f)
			{
				f->set(_coords[0], _coords[1], _rgb);
			}
		};

		
		int _n;
		field *_f;
};

void Cellular_Hue::Activate(Frame* f, Layer* l)
{
	vector<vector<int>> n_coords;
	vector<vector<int>> n_rgb;

	for (int i = 0; i < _n; i++)
	{
		vector<int> coords;
			coords.push_back(rand() % _width);
			coords.push_back(rand() % _height);
		n_coords.push_back(coords);

		vector<int> rgb;
			rgb.push_back(rand() % 255);
			rgb.push_back(rand() % 255);
			rgb.push_back(rand() % 255);
		n_rgb.push_back(rgb);
	}

	//cout << "	...creating field" << endl;
	_f = new field(_width, _height, n_rgb, n_coords);

	for (int n = 0; n < l->_frame_num; n++)
	{
		//cout << "	...generating frame " << n << endl;

		if (!_f->_todo.empty())
		{
			for (auto d : _f->_done)
				_f->_data[d[0]][d[1]]->print(f);

			_f->_done.clear();
				
			_f->activate();
		}

		l->_frames.push_back(new Frame(*f));

	}

	for (auto n : _f->_data)
		for (auto h : n)
			delete h;

	delete _f;
}