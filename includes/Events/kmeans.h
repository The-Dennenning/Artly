#include "..\events.h"

#define BIG_NUMBER 1000000000
#define CONVERGE 1

#define CONSTRUCTOR 1
#define ACTIVATION  2

class Kmeans : public Event
{
    public:
        Kmeans(int start, int dur, Frame* f, Bitmap* b, int k) :
            Event(start, dur, b->getSize(0), b->getSize(1)), _b(b), _k(k), _iteration(0) 
            {
                initialize(f, &_data, &_means);

                while(!iterate(&_data, &_means, &_clusters)) {}    
            }

        Kmeans(int start, int dur, Frame* f, Bitmap* b, int k, int i) :
            Event(start, dur, b->getSize(0), b->getSize(1)), _b(b), _k(k), _iteration(0) 
            {
                initialize(f, &_data, &_means);

                while(!iterate(&_data, &_means, &_clusters)) {}  

                _dID = to_string(i);

                rectangulate(); 
            }


        void Activate (Frame *f, Layer *l) {}

        void Activate (Frame *f, Layer *l, string dID, int rID, int n);

        //initializes k means algorithm by randomly selecting data to average
        void initialize(Frame *f, vector<vector<int>> *data, vector<vector<int>> *means);

        //iterates k means algorithm, assigns all data to clusters given means, then recalculates means
        //  returns TRUE if the means have converged beneath a certain threshold,
        //          FALSE if otherwise
        bool iterate(vector<vector<int>> *data, vector<vector<int>> *means, vector<int> *clusters);

        //prints bitmaps of each cluster
        void print_bitmaps(vector<vector<int>> *data, vector<int> *clusters, int type);

        void print_bitmap(vector<vector<int>> *data);

        void print_mean(vector<vector<int>> *means, vector<int> *clusters);
        
        void transfer_style(vector<vector<int>> *data, vector<vector<int>> *means, vector<int> *clusters);

        void rectangulate();

        void set_bitmap(Bitmap *b)
        {
            _b = b;
            _width = _b->getSize(0);
            _height = _b->getSize(1);
        }

    private:

        //number of iterations of kmeans algorithm
        int _iteration;

        //image ID (for IDing result image during batches)
        string _dID;
        int _rID;

        //number of clusters
        int _k;

        //bitmap to print from
        Bitmap* _b;


        //  data given as three columns of ints 
        //      ([_][0] = r, [_][1] = g, [_][2] = b)
        //  k means given as k sets of rgb data 
        //      ([k][0] = kth r, etc)
        //  clusters given as ID of cluster, indexed to align with data
        //      ([n] = cluster ID of data[n])

        //donor image data
        vector<vector<int>> _data;

        //donor means
        vector<vector<int>> _means;

        //donor clusters
        vector<int> _clusters;
};

void Kmeans::Activate(Frame *f, Layer *l, string dID, int rID, int n)
{
    vector<vector<int>> data;
    vector<vector<int>> means;
    vector<int> clusters;

    _dID = dID;
    _rID = rID;
    _iteration = n;

    initialize(f, &data, &means);

    while(!iterate(&data, &means, &clusters)) {}

    transfer_style(&data, &means, &clusters);
}

void Kmeans::initialize(Frame *f, vector<vector<int>> *data, vector<vector<int>> *means)
{
    //initialize data with frame data
    for (int i = 0; i < f->_width * f->_height * 4; i = i + 4)
    {
        vector<int> pixel;
            pixel.push_back(f->_frame_data[i + 0]);
            pixel.push_back(f->_frame_data[i + 1]);
            pixel.push_back(f->_frame_data[i + 2]);
        (*data).push_back(pixel);
    }
#ifdef DEBUG
    cout << "       data initialized" << endl;
#endif

    //initializes k clusters
    //  by finding mean of k random points 
    for (int i = 0; i < _k; i++)
    {
        vector<int> mean;
        double sum[3] = {0, 0, 0};

        /*
        for (int k = 0; k < 3; k++)
            mean.push_back((int) rand() % 255);
        */

        
        for (int j = 0; j < 10; j++)
        {
            int n = rand() % data->size();

            for (int k = 0; k < 3; k++)
                sum[k] += (*data)[n][k];
        }

        for (int k = 0; k < 3; k++)
            mean.push_back((int) sum[k] / 10);

        means->push_back(mean);
    }
#ifdef DEBUG
    cout << "       means initialized to:" << endl;

    for (int i = 0; i < _k; i++)
        cout << "           " << i << " - (" 
             << (*means)[i][0] << ", " << (*means)[i][1] << ", " << (*means)[i][2] 
             << ")" << endl;
#endif
}

bool Kmeans::iterate(vector<vector<int>> *data, vector<vector<int>> *means, vector<int> *clusters)
{
    //assigns all data to respective cluster
    for (auto d : *data)
    {
        double closest_distance = BIG_NUMBER;
        int distance;
        int closest_mean;

        //calculate set of distances between data point and each mean
        //  assigns data point to cluster with closest mean
        for (int i = 0; i < _k; i++)
        {
            distance = pow(pow(d[0] - (*means)[i][0], 2) 
                         + pow(d[1] - (*means)[i][1], 2) 
                         + pow(d[2] - (*means)[i][2], 2)
            , 0.5);

            if (distance < closest_distance)
            {
                closest_distance = distance;
                closest_mean = i;
            }
        }

        //assigns mean
        clusters->push_back(closest_mean);
    }

#ifdef DEBUG
    cout << "       data assigned to clusters" << endl;
#endif

    //recalculates means given new clustering
    vector<vector<double>> sums;
    vector<int> nums;

    for (int i = 0; i < _k; i++)
    {
        vector<double> rgbs;
            rgbs.push_back(0);
            rgbs.push_back(0);
            rgbs.push_back(0);
        sums.push_back(rgbs);
        nums.push_back(0);
    }

    //sums up all clusters into buckets
    //  respective to cluster
    for (int i = 0; i < data->size(); i++)
    {
        //for each rgb, adds value to running sum
        //  indexed by which cluster data i is in
        for (int k = 0; k < 3; k++)
            sums[(*clusters)[i]][k] += (*data)[i][k];

        nums[(*clusters)[i]] += 1;
    }

    //calculates new means
    vector<vector<int>> new_means;

    for (int i = 0; i < _k; i++)
    {
        vector<int> new_mean;

        if (!nums[i]) nums[i] = 1;

        for (int k = 0; k < 3; k++)
            new_mean.push_back((int) sums[i][k] / nums[i]);

        new_means.push_back(new_mean);
    }

    //checks convergence between new and old means
    bool converge = true;

    for (int i = 0; i < _k; i++)
    {
        int distance = pow(pow(new_means[i][0] - (*means)[i][0], 2) 
                         + pow(new_means[i][1] - (*means)[i][1], 2) 
                         + pow(new_means[i][2] - (*means)[i][2], 2)
        , 0.5);

        for (int k = 0; k < 3; k++)
            (*means)[i][k] = new_means[i][k];

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
             << (*means)[i][0] << ", " << (*means)[i][1] << ", " << (*means)[i][2] 
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

void Kmeans::print_bitmap(vector<vector<int>> *data)
{
    ofstream out;
    string outfile;
    Frame temp(_b->getSize(0), _b->getSize(1));

    for (int j = 0; j < data->size(); j++)
    {
        temp._frame_data[(j * 4) + 0] = (*data)[j][0];
        temp._frame_data[(j * 4) + 1] = (*data)[j][1];
        temp._frame_data[(j * 4) + 2] = (*data)[j][2];
        temp._frame_data[(j * 4) + 3] = 255;
    }

    _b->settoFrame(temp.flip());
    
    outfile  = "output/result"; 
    outfile += _dID;
    outfile += "k=" + to_string(_k);
    outfile += ".bmp";

    out.open(outfile, ios::binary);
    out << *_b;
    out.close();

#ifdef DEBUG
        cout << "           bitmap " << outfile << " printed." << endl;
#endif
}


void Kmeans::print_mean(vector<vector<int>> *means, vector<int> *clusters)
{
    ofstream out;
    string outfile;
    Frame temp(_b->getSize(0), _b->getSize(1));

    for (int j = 0; j < clusters->size(); j++)
    {
        temp._frame_data[(j * 4) + 0] = (*means)[(*clusters)[j]][0];
        temp._frame_data[(j * 4) + 1] = (*means)[(*clusters)[j]][1];
        temp._frame_data[(j * 4) + 2] = (*means)[(*clusters)[j]][2];
        temp._frame_data[(j * 4) + 3] = 255;
    }

    _b->settoFrame(temp.flip());
    
    outfile  = "output/means.bmp";

    out.open(outfile, ios::binary);
    out << *_b;
    out.close();

#ifdef DEBUG
        cout << "           bitmap " << outfile << " printed." << endl;
#endif

}


void Kmeans::print_bitmaps(vector<vector<int>> *data, vector<int> *clusters, int type)
{
    ofstream out;
    string outfile;
    Frame temp(_b->getSize(0), _b->getSize(1));

#ifdef DEBUG
    cout << "       printing bitmaps..." << endl;
#endif

    //prints one bitmap per cluster
    for (int i = 0; i < _k; i++)
    {
        //iterates through data
        //  turning on pixels in frame as per cluster
        for (int j = 0; j < data->size(); j++)
        {
            if (i == (*clusters)[j])
            {
                temp._frame_data[(j * 4) + 0] = (*data)[j][0];
                temp._frame_data[(j * 4) + 1] = (*data)[j][1];
                temp._frame_data[(j * 4) + 2] = (*data)[j][2];
                temp._frame_data[(j * 4) + 3] = 255;
            }
            else
            {
                temp._frame_data[(j * 4) + 0] = 0;
                temp._frame_data[(j * 4) + 1] = 0;
                temp._frame_data[(j * 4) + 2] = 0;
                temp._frame_data[(j * 4) + 3] = 255;
            }
        }

        _b->settoFrame(temp.flip());
        
        outfile  = "output/";
        if (type == CONSTRUCTOR)
            outfile += "donor";
        else
            outfile += "reciever";
        outfile += to_string(_iteration);
        outfile += "k";
        outfile += to_string(i);
        outfile += ".bmp";

        out.open(outfile, ios::binary);
        out << *_b;
        out.close();

#ifdef DEBUG
        cout << "           bitmap " << outfile << " printed." << endl;
#endif
    }
}


/********************
 * 
 *      color pallete transfer:
 * 
 *          first, assign each cluster in reciever to one in donor
 *              optimizing for matching closest pairs
 * 
 *          then, per reciever pixel, there are four points of interest:
 *              reciever pixel rgb
 *              reciever cluster rgb
 *              donor cluster rgb
 * 
 *          possible operations:
 *              take vector from reciever cluster to pixel
 *                  apply vector to donor cluster rgb
 * 
 *              take vector from reciever cluster to donor cluster ******* this is the one that is implemented
 *                  apply vector to reciever pixel rgb
*/

void Kmeans::transfer_style(vector<vector<int>> *data, vector<vector<int>> *means, vector<int> *clusters)
{
    vector<int> mapping(_k, 0);

#ifdef DEBUG
    cout << "       Mapping donor means to reciever means..." << endl;
#endif

    for (int i = 0; i < _k; i++)
    {
        double closest_distance = BIG_NUMBER;
        int distance;
        int closest_mean = 0;

        for (int j = 0; j < _k; j++)
        {
            distance = pow(pow(_means[j][0] - (*means)[i][0], 2) 
                         + pow(_means[j][1] - (*means)[i][1], 2) 
                         + pow(_means[j][2] - (*means)[i][2], 2)
            , 0.5);

            if ((distance < closest_distance))
            {
                closest_distance = distance;
                closest_mean = j;
            }
        }

        mapping[i] = closest_mean;
    }

#ifdef DEBUG
    cout << "       Applying color pallete transfer..." << endl;
#endif

    vector<vector<int>> result;

    int N = 9;

    double *Gaussian = new double[N];
    double sum[3];
    double total;

    for (int k = 0; k < N; k++)
        Gaussian[k] = Binomial(N, k);

    for (int i = 0; i < _width; i++)
    {
        for (int j = 0; j < _height; j++)
        {
            vector<int> rgb_value(3, 0);
            int rgb_vector;

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
                        if ((j + g_j >= 0) && (j + g_j < _height))
                        {
                            for (int k = 0; k < 3; k++)
                                sum[k] += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)] * (_means[mapping[(*clusters)[(i + g_i) * _height + (j + g_j)]]][k] - (*means)[(*clusters)[(i + g_i) * _height + (j + g_j)]][k]);

                            total += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)];
                        }
                    }
                }
            }

            //Assigns value to temporary holding
            //	So as to not mess with other filter calculations
            for (int k = 0; k < 3; k++)
            {
                rgb_vector = (int) (sum[k] / total);
                rgb_value[k] = (*data)[i * _height + j][k] + rgb_vector;
                    
                if (rgb_value[k] < 0)           rgb_value[k] = 0;
                else if (rgb_value[k] > 255)    rgb_value[k] = 255;
            }
            
            result.push_back(rgb_value);
        }
    }

#ifdef DEBUG
    cout << "       printing result..." << endl;
#endif

    print_bitmap(&result);
}


void Kmeans::rectangulate()
{
    vector<vector<int>> result;

    for (int n = 0; n < _width * _height; n++)
    {
        vector<int> rgb(4, 0);
        result.push_back(rgb);
    }

    int flag_o = 1;
    int n = 0;

    //makes random rectangles until it can't find space to do so no mo
    while (flag_o)
    {
#ifdef DEBUG
        cout << "   rectangle " << n++ << " rectangulating..." << endl;
#endif

        //gets a point in the image
        int x = rand() % _width;
        int y = rand() % _height;

        int check = 0;

        //makes sure the point isn't yet in a rectangle
        while (result[x * _height + y][3] != 0)
        {
            x = rand() % _width;
            y = rand() % _height;
            
            if (check > 100000)
            {
                flag_o = 0;
                break;
            }
            else
                check++;
        }

        if (!flag_o) break;

        double sum[3] = {0, 0, 0};
        double sum_temp[3] = {0, 0, 0};

        int cluster = _clusters[x * _height + y];

        //cout << "cluster is: " << cluster << endl;

        int size = 0;
        int flag = 1;

        //'grows' a rectangle from given (x, y)
        while(flag) 
        {
            for (int i = -1 * size; i <= size; i++)
            {
                for (int j = -1 * size; j <= size; j++)
                {
                    //checks bounds
                    if ((x + i) < 0 || (x + i) >= _width)
                        flag = 0;

                    if ((y + j) < 0 || (y + j) >= _height)
                        flag = 0;

                    if (!flag) break;
                    
                    if (size > 0)
                    {
                        //checks if pixel is in same cluster
                        if (_clusters[(x + i) * _height + (y + j)] != cluster)
                            flag = 0;
                    }
                    
                    /*
                    //checks if pixel is in another rectangle already
                    if (result[(x + i) * _height + (y + j)][3] != 0)
                        flag = 0;
                    */
                    
                    if (!flag) break;

                    //adds color info to summing variable
                    for (int k = 0; k < 3; k++)
                        sum_temp[k] += _data[(x + i) * _height + (y + j)][k];
                }

                if (!flag) break;
            }

            if (!flag) break;

            for (int k = 0; k < 3; k++)
            {
                sum[k] = sum_temp[k];
                sum_temp[k] = 0;
            }
            size++;
        }

        //because the loop above will exit once it finds a size that fails,
        //  we must decrement the size to the last size that worked
        size--;

        //gets area of square
        //  size = 0 -> area of 1 pixel
        //  size = 1 -> area of 3x3 pixels
        //  size = 2 -> area of 5x5 pixels, so on
        int area = (size * 2 + 1) * (size * 2 + 1);

        //the last '1' marks this pixel as being part of an existing rectangle
        int rgb[4] = {0, 0, 0, 1};

        for (int k = 0; k < 3; k++)
            rgb[k] = sum[k] / area;

        //cout << "for rectangle " << n << ", clusters are: " << endl;

        //plot rectangle in result image
        for (int i = -1 * size; i <= size; i++)
        {
            for (int j = -1 * size; j <= size; j++)
            {
                //cout << "   " << _clusters[(x + i) * _height + (y + j)] << endl;

                for (int k = 0; k < 4; k++)
                    result[(x + i) * _height + (y + j)][k] = rgb[k];
            }
        }

        n++;
    }

    //fills out image with pixels where no square was found
    for (int i = 0; i < _data.size(); i++)
    {
        if (result[i][3] == 0)
        {
            for (int k = 0; k < 3; k++)
                result[i][k] = _data[i][k];
        }
    }

    print_bitmap(&result);
}