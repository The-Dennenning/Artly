#include "..\events.h"

using namespace std;

class Rewrite : public Event {

	public:
		Rewrite(int start, int dur, int width, int height, int type, vector <vector <vector <int> > > rules, vector<int> first) :
			Event(start, dur, width, height), _step(0), _type(type), _rules(rules)
			{
				for (int i = 0; i < _height; i++)
				{
					vector <int> v;
					v.assign(_width, 0);
					_grid.push_back(v);
				}

				for (int i = 0; i < first.size(); i++)
					_grid[0][i] = first[i];
			}

		~Rewrite()
		{}
		
		virtual void Activate(Bitmap *b, Layer *l);
		
	private:

		int _type;
		int _step;

		vector <vector <vector <int> > > _rules;

		vector <vector <int> > _grid;

		void getRewrite(Bitmap *b);
		int* getColor(int n);
		void setColor(Bitmap *b, int row, int col, int* color, int _type);
};


void Rewrite::Activate(Bitmap *b, Layer *l)
{

	vector<uint8_t> frame;

	for (int n = 0; n < l->_frame_num; n++)
	{
		getRewrite(b);
		_step++;

		getFrame(frame, *b);
		l->_frame_data.push_back(frame);
		
		if (n % 100 == 0)
			cout << "	Rewrite Frame " << n << " Generated" << endl; 
	}
}

//Counts neighbors of pixel at position i, j with area of effect AE
//	Gets new color, returned in newrgb
//	Only considers "neighbors" that are in state specified by 'states'
//		state_num determines how many states to consider
void Rewrite::getRewrite(Bitmap *_b)
{
	/*
	cout << "	Initiating Rewrite " << _step << "..." << endl;
	
	cout << "	Current Row..." << endl << "		";

	int c = -1;
	while (_grid[_step][++c] != 0)
		cout << _grid[_step][c];
	cout << endl;
	*/

	//Tracks column of current row
	int j_cur = 0;

	//Tracks column of output row
	int j_out = 0;

	//Gets number of rules to consider
	int rule_num = _rules.size();

	//If the current cell is live and in bounds
	while(_grid[_step][j_cur] != 0 && j_cur < _width && j_out < _width)
	{
		//Tracks if any rule is implemented at all over a cell
		int no_rule = 1;

		//Iterate over all rules to see if one applies
		for (int r = 0; r < rule_num; r++)
		{
			//Check if rule r is present 
			int eq = 1;

			//Get rule r
			vector<int> rule_in = _rules[r][0];
			vector<int> rule_out = _rules[r][1];

			//If length of rule r does not extend past boundary
			if (j_cur + rule_in.size() < _width)
			{
				for (int l = 0; l < rule_in.size(); l++)
				{
					if (_grid[_step][j_cur + l] != rule_in[l])
						eq = 0;
				}

				//if so, add rule r output to next string
				if (eq == 1)
				{
					/*
					cout << "	Rule " << r << " matches..." << endl;
					*/

					for (int l = 0; l < rule_out.size(); l++)
					{
						if (j_out < _width)
						{
							//Set grid cell to resulting value
							_grid[_step + 1][j_out] = rule_out[l];

							setColor(_b, _step + 1, j_out, getColor(rule_out[1]), _type);

							//iterate output column counter
							j_out++;
						}
					}

					eq = 0;
					j_cur += rule_in.size();
					no_rule = 0;
				} 
			} 
		} 

		if (no_rule == 1 && j_cur < _width && j_out < _width)
		{
			//Set grid cell to resulting value
			_grid[_step + 1][j_out] = _grid[_step][j_cur];

			setColor(_b, _step + 1, j_out, getColor(_grid[_step][j_cur]), _type);

			j_cur++;
			j_out++;
		}
	}
	
	/*
	cout << "	Output Row..." << endl << "		";

	c = -1;
	while (_grid[_step + 1][++c] != 0)
		cout << _grid[_step + 1][c];
	cout << endl;
	*/
}

void Rewrite::setColor(Bitmap *_b, int row, int col, int* color, int _type)
{
	if (_type % 9 == 0)
	{
		_b->getPixel(row, col).setRGB(color);
	}
	else if (_type % 9 == 1)
	{
		_b->getPixel(row, _height - col).setRGB(color);
	}
	else if (_type % 9 == 2)
	{
		_b->getPixel(_width - row, _height - col).setRGB(color);
	}
	else if (_type % 9 == 3)
	{
		_b->getPixel(_width - row, col).setRGB(color);
	}
	else if (_type % 9 == 4)
	{
		_b->getPixel(_width - row, _height - col).setRGB(color);
		_b->getPixel(row, col).setRGB(color);
	}
	else if (_type % 9 == 5)
	{
		_b->getPixel(row, _height - col).setRGB(color);
		_b->getPixel(_width - row, col).setRGB(color);
	}
	else if (_type % 9 == 6)
	{
		_b->getPixel(row, _height - col).setRGB(color);
		_b->getPixel(row, col).setRGB(color);
	}
	else if (_type % 9 == 7)
	{
		_b->getPixel(_width - row, col).setRGB(color);
		_b->getPixel(row, col).setRGB(color);
	}
	else if (_type % 9 == 8)
	{
		_b->getPixel(_width - row, col).setRGB(color);
		_b->getPixel(_width - row, _height - col).setRGB(color);
		_b->getPixel(row, _height - col).setRGB(color);
		_b->getPixel(row, col).setRGB(color);
	}
}

//Takes integer value and returns rgb value
int* Rewrite::getColor(int n)
{
	int* color = new int[3];

	switch (n)
	{
		case 1:  //White
			color[0] = 255;
			color[1] = 255;
			color[2] = 255;
			break;
		case 2:  //Red
			color[0] = 255;
			color[1] = 0;
			color[2] = 0;
			break;
		case 3:  //Blue
			color[0] = 0;
			color[1] = 0;
			color[2] = 255;
			break;
		case 4:  //Green
			color[0] = 0;
			color[1] = 255;
			color[2] = 0;
			break;
		case 5:  //Purple
			color[0] = 128;
			color[1] = 0;
			color[2] = 128;
			break;n c
		default: //Black
			color[0] = 0;
			color[1] = 0;
			color[2] = 0;
			break;
	}

	return color;
}