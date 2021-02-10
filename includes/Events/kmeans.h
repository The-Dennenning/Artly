#include "..\events.h"

#define BIG_NUMBER 1000000000
#define CONVERGE 2

class Kmeans : public Event
{
    public:
        Kmeans(int start, int dur, Frame* donor, Frame* reciever, Bitmap* b, int k) :
            Event(start, dur, reciever->_width, reciever->_height), _b(b), _k(k), _iteration(0) {}

        void Activate (Frame *f, Layer *l);


        //initializes k means algorithm by randomly selecting data to average
        void initialize(Frame *f, vector<vector<int>> *data, vector<vector<int>> *means);

        //iterates k means algorithm, assigns all data to clusters given means, then recalculates means
        //  data given as three columns of ints 
        //      ([_][0] = r, [_][1] = g, [_][2] = b)
        //  k means given as k sets of rgb data 
        //      ([k][0] = kth r, etc)
        //  clusters given as ID of cluster, indexed to align with data
        //      ([n] = cluster ID of data[n][...])
        //  returns TRUE if the means have converged beneath a certain threshold,
        //          FALSE if otherwise
        bool iterate(vector<vector<int>> *data, vector<vector<int>> *means, vector<int> *clusters);

        //prints bitmaps of each cluster
        void print_bitmaps(vector<vector<int>> *data, vector<int> *clusters);

    private:

        //number of iterations of kmeans algorithm
        int _iteration;

        //number of clusters
        int _k;

        //bitmap to print from
        Bitmap* _b;
};

void Kmeans::Activate(Frame *f, Layer *l)
{
    vector<vector<int>> data;
    vector<vector<int>> means;
    vector<int> clusters;

    initialize(f, &data, &means);

    while(!iterate(&data, &means, &clusters)) 
        print_bitmaps(&data, &clusters);
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

        //finds mean of k random points
        for (int j = 0; j < _k * 3; j++)
        {
            int init = rand() % data->size();
            
            for (int k = 0; k < 3; k++)
                sum[k] += (*data)[init][k];
        }

        for (int k = 0; k < 3; k++)
            mean.push_back((int) (sum[k] / (_k * 3)));

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


void Kmeans::print_bitmaps(vector<vector<int>> *data, vector<int> *clusters)
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
        outfile += to_string(_iteration);
        outfile += "k";
        outfile += to_string(i);
        outfile += ".bmp";

        out.open(outfile, ios::binary);
        out << _b;
        out.close();

#ifdef DEBUG
        cout << "           bitmap " << outfile << " printed." << endl;
#endif
    }
}