#include "..\events.h"

#define RED 0
#define GREEN 1
#define BLUE 2
#define BRIGHT 3

typedef vector<pair<int, vector<int>>> srt;

class Sorter : public Event{

	public:
		Sorter(int start, int duration, int width, int height, int type) :
			Event(start, duration, width, height), _lower_bound(nullptr), _upper_bound(nullptr), _type(type) {}

		Sorter(int start, int duration, int width, int height, int type, float* lower_bound, float* upper_bound) :
			Event(start, duration, width, height), _lower_bound(lower_bound), _upper_bound(upper_bound), _type(type) {}

		void Activate(Frame* f, Layer* l);

	private:

		int _type;

		float* _lower_bound;
		float* _upper_bound;

		void sort_line(Frame* f, int line_num);

		int getComp(srt *arr, int index);

		int partition(srt *arr, int low, int high);
		void quickSort(srt *arr, int low, int high);

		int eval_lower(int n)
			{ 

				int val = (int) (n * _lower_bound[1] + _lower_bound[0]);

				if (val < 0)				return 0;
				if (val > _height - 1)		return _height - 1; 
				else						return val;
			}

		int eval_upper(int n)
			{ 
				int val = (int) (n * _upper_bound[1] + _upper_bound[0]);

				if (val < 0)				return 0;
				if (val > _height - 1)		return _height - 1; 
				else						return val;
			}
};

void Sorter::Activate(Frame* f, Layer* l)
{
	Frame* _f;

	for (int i = 0; i < _height; i++)
	{
		_f = new Frame(*f);
		sort_line(_f, i);
		l->_frames.push_back(_f);
		f = _f;
	}
}

int Sorter::getComp(srt *arr, int index)
{
	return (*arr)[index].second[_type];
}

int Sorter::partition(srt *arr, int low, int high)
{
    // pivot (Element to be placed at right position)
    int pivot = getComp(arr, high);  
 
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {

        // If current element is smaller than the pivot
        if (getComp(arr, j) < pivot)
        {
            i++;    // increment index of smaller element
			(*arr)[i].swap((*arr)[j]);
        }
    }
			
	(*arr)[i + 1].swap((*arr)[high]);

    return (i + 1);
}

void Sorter::quickSort(srt *arr, int low, int high)
{

    if (low < high)
    {
        /* pi is partitioning index, arr[pi] is now
           at right place */
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);  // Before pi
        quickSort(arr, pi + 1, high); // After pi
    }
}

void Sorter::sort_line(Frame* f, int line_num)
{
#ifdef DEBUG
	cout << "	Sort Initializing..." << endl;
#endif

	srt pixels;

	for (int i = 0; i < _width; i++)
	{
		vector<int> vals;
			vals.push_back(f->_frame_data[(i * _height + line_num) * 4 + 0]);
			vals.push_back(f->_frame_data[(i * _height + line_num) * 4 + 1]);
			vals.push_back(f->_frame_data[(i * _height + line_num) * 4 + 2]);
			vals.push_back(((double) (vals[0] + vals[1] + vals[2])) / 3);
		pixels.push_back(make_pair(i, vals));
	}

#ifdef DEBUG
	cout << "	Sort Executing..." << endl;

/*
	cout << "		bounds are: " << eval_lower(line_num) << ", " << eval_upper(line_num) << endl;
*/
#endif


	//quickSort(&pixels, eval_lower(line_num), eval_upper(line_num));
	quickSort(&pixels, 0, _width - 1);

#ifdef DEBUG
	cout << "	Extracting values..." << endl;
#endif

	for (int i = 0; i < _width; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			f->_frame_data[(i * _height + line_num) * 4 + k] = pixels[i].second[k];
		}
	}

	pixels.clear();

#ifdef DEBUG
	cout << "	Pixel Sort finished over line " << line_num << endl;
#endif
}
