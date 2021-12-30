#include <iostream>
#include <algorithm>

#include "Events\kmeans.h"

#include "bitmap.h"
#include "frame.h"
#include "csvReader.hpp"

#define SIG(x) 1 / (1 + exp(-x))
#define SIGP(x) SIG(x) * (1 - SIG(x))

/*
    Notes

        At last, I have a project perfectly suited to the development of a neural network!!!
        
        I want to train a neural network to produce images from right to left, 
            working to produce each new next column of data with the data from the previous n columns

        First draft idea:

            neural network takes as input...
                per kmeans cluster...
                    info about color value of cluster
                        3 doubles e [0, 1] 

                per pixel...
                    info about which kmeans cluster pixel belongs to
                        1 double = _k id / _k total
                    info about distance of pixel color value from cluster color value
                        3 doubles e [0, 1] 
                            where 0.5 == no distance from cluster
                                < 0.5 == that much * 2 less than cluster
                                > 0.5 == that much * 2 greater than cluster
                    
                which totals...
                    4 * _height * _w + 3 * _k

            neural network provides as output...

                per pixel...
                    info about which kmeans cluster pixel belongs to
                        1 double = _k id / _k total
                    info about distance from cluser
                        3 doubles e [0, 1]

                which totals...
                    4 * _height

            has _h hidden layers, of _height * _w * 2 values

*/





using namespace std;


class neuron
{
    public:
        neuron(vector<neuron*> ns, vector<double> ws, double b)
            : _ns(ns), _ws(ws), _b(b), _ran(false), _a(0), _s(0) {}
            
        neuron(double a)
            : _ran(true), _a(a), _s(0) {}

        //TRUE if _a has been calculated
        //FALSE if not
        bool _ran;

        //collects sensitivities of neurons ahead
        //  during backpropagation step
        double _s;

        //pre-squished activation value
        double _z; 

        //activation value for this neuron
        double _a;

        //link to each neuron connected to this one
        //  in the layer behind it
        vector<neuron*> _ns;

        //weights indexed to each link above
        vector<double> _ws;

        //bias value for this neuron
        double _b;

        double run()
        {
            if (_ran)
                return _a;
            
            _z = _b;

            for (int i = 0; i < _ns.size(); i++)
                _z += _ns[i]->run() * _ws[i];
            
            _a = SIG(_z);
            _ran = true;
            return _a;
        }
};

class Training_Data_2
{
    public:
        Training_Data_2(Frame *f, int h, int w)
            : _height(f->_height), _width(f->_width), _h(h), _w(w)
        {
            for (int i = 0; i < _width * _height * 4; i = i + 4)
            {
                for (int j = 0; j < 3; j++)
                {
                    _data.push_back(((double)f->_frame_data[i + j]) / 255);
                }
            }
        }

        Training_Data_2(Training_Data_2 *td)
            : _height(td->_height), _width(td->_width), _h(td->_h), _w(td->_w), _data(td->_data)
        {}

        void prep_random()
        {
            for (int i = 0; i < _w; i++)
            {
                for (int j = 0; j < _height; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        _data[i * _height * 3 + j * 3 + k] = ((double)(rand() % 100) / 100);
                    }
                }
            }
        }

        //gets a 2D slice from the training data, formatted as a 1D input vector
        //  input vector is of of size n_in
        //  2D slice goes from point (i_off, j_off)
        //                  to point (i_off - _w, j_off + _h)
        vector<double> get_Input(int n_in, int i_off, int j_off)
        {
            vector<double> input;

            if (j_off + _h > _height)
            {   
                cout << "Hull Breach - vertical offset error" << endl;
                return input;
            }

            if (i_off - _w < -1)
            {
                cout << "Hull Breach - horizontal offset error" << endl;
                return input;
            }

            for (int i = i_off; i > i_off - _w; i--)
            {
                for (int j = j_off; j < j_off + _h; j++)
                {
                    //get color information
                    for (int k = 0; k < 3; k++)
                        input.push_back(_data[i * _height * 3 + j * 3 + k]);
                }
            }

            return input;
        }

        void set_Column(vector<double> output, int i_off, int j_off, int l)
        {
            int o = 0;

            for (int j = j_off; j < j_off + l; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    _data[i_off * _height * 3 + j * 3 + k] = output[o];
                    o++;
                }
            }
        }

        //gets raw difference between output data given and training data specified 
        vector<double> get_Cost(vector<double> output, int i_off, int j_off)
        {
            vector<double> cost;
            int o = 0;

            for (int j = j_off; j < j_off + _h; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    cost.push_back(output[o] - _data[i_off * _height * 3 + j * 3 + k]);
                    o++;
                }
            }

            return cost;
        }

        //prints frame from training data
        //  rgb value = (int) (mean - (adj - 0.5) / 255 * 2)
        Frame* get_Frame()
        {
            Frame* f = new Frame(_width, _height);

            f->_frame_data.clear();

            for (int i = 0; i < _width * _height * 3; i = i + 3)
            {
                for (int j = 0; j < 3; j++)
                {
                    int rgb = _data[i + j] * 255;

                    if (rgb < 0)    rgb = 0;
                    if (rgb > 255)  rgb = 255;

                    f->_frame_data.push_back((uint8_t)rgb);
                }
                f->_frame_data.push_back((uint8_t) 255);
            }

            return f;
        }

        int _height;
        int _width;

        //height of window
        int _h;

        //width of window
        int _w;

        //pixel data is stored as a signed normalized distance from cluster mean in each color channel 
        //  e.g. _data = raw / 255
        vector<double> _data;
};

class Training_Data
{
    public:
        Training_Data(Frame *f, int k, int h, int w)
            : _height(f->_height), _width(f->_width), _k(k), _h(h), _w(w)
        {
            Kmeans km(f, k);

            for (int i = 0; i < _width * _height; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    _data.push_back(((double)(km._means[km._clusters[i]][j] - km._data[i][j])) / (255 * 2) + 0.5);
                }
                _clusters.push_back(((double)km._clusters[i]) / _k);
            }

            for (int i = 0; i < _k; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    _means.push_back(((double)km._means[i][j]) / 255);
                }
            }
        }

        Training_Data(Training_Data *td)
            : _height(td->_height), _width(td->_width), _k(td->_k), _h(td->_h), _w(td->_w), _data(td->_data), _clusters(td->_clusters), _means(td->_means)
        {}

        void prep_random()
        {
            for (int i = 0; i < _w; i++)
            {
                for (int j = 0; j < _height; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        _data[i * _height * 3 + j * 3 + k] = ((double)(rand() % 100) / 100);
                    }

                    _clusters[i * _height + j] = ((double)(rand() % 100) / 100);
                }
            }

            for (int k = 0; k < _k; k++)
            {
                for (int n = 0; n < 3; n++)
                {
                    _means[k * 3 + n] = ((double)(rand() % 100) / 100);
                }
            }
        }

        //gets a 2D slice from the training data, formatted as a 1D input vector
        //  input vector is of of size n_in
        //  2D slice goes from point (i_off, j_off)
        //                  to point (i_off - _w, j_off + _h)
        vector<double> get_Input(int n_in, int i_off, int j_off)
        {
            vector<double> input;

            if (j_off + _h > _height)
            {   
                cout << "Hull Breach - vertical offset error" << endl;
                return input;
            }

            if (i_off - _w < -1)
            {
                cout << "Hull Breach - horizontal offset error" << endl;
                return input;
            }

            for (int i = i_off; i > i_off - _w; i--)
            {
                for (int j = j_off; j < j_off + _h; j++)
                {
                    //get color information
                    for (int k = 0; k < 3; k++)
                        input.push_back(_data[i * _height * 3 + j * 3 + k]);
                    
                    //get cluster information
                    input.push_back(_clusters[i * _height + j]);
                }
            }

            for (int i = 0; i < _means.size(); i++)
                input.push_back(_means[i]);

            return input;
        }

        void set_Column(vector<double> output, int i_off, int j_off, int l)
        {
            int o = 0;

            for (int j = j_off; j < j_off + l; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    _data[i_off * _height * 3 + j * 3 + k] = output[o];
                    o++;
                }

                //round cluster ID to nearest kth fraction
                output[o] = round(_k * output[o]) * _k;

                _clusters[i_off * _height + j] = output[o];
                o++;
            }
        }

        //gets raw difference between output data given and training data specified 
        vector<double> get_Cost(vector<double> output, int i_off, int j_off)
        {
            vector<double> cost;
            int o = 0;

            for (int j = j_off; j < j_off + _h; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    cost.push_back(output[o] - _data[i_off * _height * 3 + j * 3 + k]);
                    o++;
                }

                cost.push_back(output[o] - _clusters[i_off * _height + j]);
                o++;
            }

            return cost;
        }

        //prints frame from training data
        //  rgb value = (int) (mean - (adj - 0.5) / 255 * 2)
        Frame* get_Frame()
        {
            Frame* f = new Frame(_width, _height);

            f->_frame_data.clear();

            for (int i = 0; i < _width * _height; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    int rgb = _means[(int) (_clusters[i] * _k) * 3 + j] * 255 - (_data[i * 3 + j] - 0.5) * (255 * 2);

                    if (rgb < 0)    rgb = 0;
                    if (rgb > 255)  rgb = 255;

                    f->_frame_data.push_back((uint8_t)rgb);
                }
                f->_frame_data.push_back((uint8_t) 255);
            }

            return f;
        }

        int _height;
        int _width;

        //number of clusters for kmeans clustering
        int _k;

        //height of window
        int _h;

        //width of window
        int _w;

        //pixel data is stored as a signed normalized distance from cluster mean in each color channel 
        //  e.g. _data = (mean - raw) / (255 * 2) + 0.5
        vector<double> _data;
        
        //mean data is stored as normalized color value
        //  e.g. mean / 255
        vector<double> _means;

        //cluster data is stored as _k id / _k total
        //  e.g. to mark a pixel as belonging to the 4th of 10 clusters,
        //       _clusters[i] = (4 / 10)
        vector<double> _clusters;
};


class Image_Gen
{
    public:

        Image_Gen(int h, int w, int k, string fileName)
            : _h(h), _w(w), _k(k), _alpha(.00000001), _n_in(h * w * 3), _n_hid(h * 3), _d_hid(2), _n_out(h * 3), _fileName(fileName)
            {
                //new_data();
                //run_images();
                generate_images(500, 1);
                
                for (int i = 0; i < 20; i++)
                    generate_noise_image(i);
            }

        ~Image_Gen() {} 

        void new_data()
        {
            _n = new IG_Net(_n_in, _n_hid, _d_hid, _n_out);

            vector<double> data_in(_n_in, 0);

            for (int i = 0; i < _n_in; i++)
                data_in[i] = ((double)(rand() % 200 - 100)) / 100;

            cout << "Running first neural network..." << endl;
            vector<double> results(_n->run(data_in));

            cout << "Saving first neural network..." << endl;
            _n->save_data();

            delete _n;

            cout << "Loading second neural network..." << endl;
            _n = new IG_Net(_n_in, _n_hid, _d_hid, _n_out, _fileName);

            cout << "Running second neural network..." << endl;
            vector<double> results2(_n->run(data_in));

            cout << "checking values..." << endl;
            double sum = 0;
            for (int i = 0; i < results.size(); i++)
            {
                if (results[i] != results2[i]) sum++;
            }

            cout << "...Complete, with " << sum << " errors" << endl;

            delete _n;
        }

        /*
        void run_images()
        {
            _n = new IG_Net(_n_in, _n_hid, _d_hid, _n_out, _fileName);

            cout << "...csv data read" << endl;

            double total_cost = 0;
            //size of input image bank
            int size = 25;
            //number of trials to run over each image
            int trials = 1;

            //creates random order of images to run
            vector<int> ms;
            for (int i = 1; i <= size; i++) ms.push_back(i);
            random_shuffle(ms.begin(), ms.end());

            for (int m = 1; m < size; m++)
            {
                cout << endl << "Run " << m << ": in " << ms[m] << endl;

                string filename = "input/pron/smolled/" + to_string(ms[m]) + ".bmp";

                Bitmap *image = new Bitmap();
                ifstream in;
                ofstream out;

                in.open(filename);
                in >> *image;
                in.close();

                Frame *f = new Frame(*image);

                //creates training data set from image
                Training_Data_2 td(f, _h, _w);
                
                _cdel = 0;
                double t_sum = 0;
                vector<double> t_costs;

                for (int t = 0; t < trials; t++)
                {
                    _wsum = 0;
                    _bsum = 0;
                    _csum = 0;
                    
                    vector<double> run_cost(run_image(&td, ms[m], m));
                        
                    for (auto c : run_cost)
                        _csum += pow(c * c, 0.5);

                    t_costs.push_back(_csum);
                    t_sum += _csum;

                    cout << "       ...run total cost = " << _csum << endl;
                    cout << "       ...run weight delta = " << _wsum << endl;
                    cout << "       ...run bias delta = " << _bsum << endl;
                }

                total_cost = ((total_cost * (m - 1)) + (t_sum / trials)) / m;

                cout << endl;
                cout << "   ...running average cost = " << total_cost << endl << endl;

                delete image;
                delete f;

                generate_image("input/pron/smolled/in " + to_string(ms[m]) + ".bmp", ms[m], m);

                _n->save_data();
                cout << "...csv data written" << endl;
            }

            _n->save_data();
            cout << "...csv data written" << endl;

            delete _n;
        }

        vector<double> run_image(Training_Data_2 *td, int img_num, int run_num)
        {
            Training_Data_2 to_print(td);
            vector<double> cost;
            
            int r = 1;

            //run over entire image
            for (int i = _w - 1; i < td->_width; i++)
            {
                for (int t = 0; t < 1; t++)
                {
                    vector<double> col_cost;

                    _wsum = 0;
                    _bsum = 0;
                    int m = 0;

                    for (int j = 0; j < td->_height - _h; j++)
                    {
                        vector<double> output(_n->run(td->get_Input(_n_in, i, j)));
                        vector<double> run_cost(td->get_Cost(output, i + 1, j));
                        
                        apply_cost(run_cost);

                        _n->clear();

                        int n = 0;

                        //accumulate costs for entire image
                        if (col_cost.empty())
                            for (auto c : run_cost) col_cost.push_back(c);
                        else
                            for (auto c : run_cost) col_cost[n++] += c;
                    }

                    if (i % 50 == 0)     
                    {
                        double sum = 0;
                        for (auto c : col_cost) sum += c;

                        sum /= m;
                        _wsum /= m;
                        _bsum /= m;

                        cout << "   ...at trial " << t << ", column " << i << ": cpc = " << sum << ", _wsum = " << _wsum << ", _bsum = " << _bsum << endl;

                        m = 0;
                    }

                    int n = 0;

                    //accumulate costs for entire image
                    if (cost.empty())
                        for (auto c : col_cost) cost.push_back(c);
                    else
                        for (auto c : col_cost) cost[n++] += c;
                    
                    _n->clear();
                }
            }

            return cost;
        }
        */

        //Backpropagation step
        void apply_cost(vector<double> cost)
        {
            int L = _n->_layers.size();

            //calculates sensitivity for each output neuron
            for (int i = 0; i < _n->_layers[L - 1].size(); i++)
                _n->_layers[L - 1][i]->_s = 2 * cost[i] * SIGP(_n->_layers[L - 1][i]->_z);

            //backcalculate sensitivities for previous layers
            for (int l = L - 2; l >= 0; l--)
            {
                //uses sensitivities of layer l + 1 to calculate sensitivities of layer l
                for (int i = 0; i < _n->_layers[l + 1].size(); i++)
                {
                    //Adds contribution of layer l + 1 neurons sensitivity to all neurons behind it
                    for (int j = 0; j < _n->_layers[l + 1][i]->_ns.size(); j++)
                        _n->_layers[l + 1][i]->_ns[j]->_s += _n->_layers[l + 1][i]->_s * _n->_layers[l + 1][i]->_ws[j];
                }

                //for each neuron in layer l,
                //  multiplies this cumulative value by sig'(z)
                for (int i = 0; i < _n->_layers[l].size(); i++)
                    _n->_layers[l][i]->_s *= SIGP(_n->_layers[l][i]->_z);
            }

            //beginning with first non-input layer,
            //  adjusts all weights and biases by computed gradient
            for (int l = 1; l < L; l++)
            {
                for (int i = 0; i < _n->_layers[l].size(); i++)
                {
                    for (int j = 0; j < _n->_layers[l][i]->_ws.size(); j++)
                    {
                        _n->_layers[l][i]->_ws[j] -= _alpha * _n->_layers[l][i]->_ns[j]->_a * _n->_layers[l][i]->_s;

                        _wsum += pow(pow(_alpha * _n->_layers[l][i]->_ns[j]->_a * _n->_layers[l][i]->_s, 2), 0.5);
                    }

                    _n->_layers[l][i]->_b -= _alpha * _n->_layers[l][i]->_s;

                    _bsum += pow(pow(_alpha * _n->_layers[l][i]->_s, 2), 0.5);
                }
            }
        }

        void generate_noise_image(int n)
        {
            _n = new IG_Net(_n_in, _n_hid, _d_hid, _n_out, _fileName);

            cout << "Generating noise... " << endl;

            int r = rand() % 25 + 1;
            string file = "input/bored/smolled/" + to_string(r) + ".bmp";

            Bitmap *image = new Bitmap();
            ifstream in;
            ofstream out;

            in.open(file);
            in >> *image;
            in.close();

            Frame *f = new Frame(*image);

            Training_Data_2 td(f, _h, _w);
            //td.prep_random();

            for (int i = _w - 1; i < f->_width - 1; i++)
            {
                vector<double> ns(f->_height * 3, 0);
                vector<double> col(f->_height * 3, 0);

                for (int j = f->_height - _h; j >= 0; j = j - 3)
                {
                    vector<double> output = _n->run(td.get_Input(_n_in, i, j));

                    _n->clear();

                    for (int k = j * 3; k < (j + _h) * 3; k++)
                    {
                        //adds random noise
                        double r = (rand() % 100) / 100;

                        //takes weighted average of current value + new value + random value
                        col[k] = col[k] * ns[k] + output[k - j * 3];
                        ns[k]++;
                        col[k] = col[k] / ns[k];
                    }
                }
                
                /* for training data 1
                for (int j = f->_height - _h; j >= 0; j -= _h)
                {
                    vector<double> output = _n->run(td.get_Input(_n_in, i, j));

                    for (int k = j * 4; k < (j + _h) * 4; k++)
                    {
                        col[k] = col[k] * ns[k] + output[k - j * 4];
                        ns[k]++;
                        col[k] = col[k] / ns[k];
                    }

                    _n->clear();
                }
                */

                td.set_Column(col, i + 1, 0, f->_height);
            }

            Frame *f2 = td.get_Frame();

            image->settoFrame(f2->flip());

            out.open("output/noise_out " + to_string(n) + ".bmp");
            out << *image;
            out.close();

            cout << "...image written" << endl;

            delete _n;
        }

        void generate_images(int n, int m)
        {
            _n = new IG_Net(_n_in, _n_hid, _d_hid, _n_out, _fileName);

            for (int i = 0; i < n; i++)
            {
                int r = rand() % 25 + 1;
                generate_image("input/bored/smolled/" + to_string(r) + ".bmp", r, (m - 1) * n + i);

                _n->save_data();
                cout << "...csv data written" << endl;
            }

            delete _n;
        }

        void generate_image(string file, int n, int j)
        {
            cout << "Generating image " << j << "-" << n << "... " << endl;

            Bitmap *image = new Bitmap();
            ifstream in;
            ofstream out;

            in.open(file);
            in >> *image;
            in.close();

            Frame *f = new Frame(*image);

            Training_Data_2 td(f, _h, _w);

            //run over entire image
            for (int i = f->_width - 2; i >= _w; i--)
            {
                vector<double> ns(f->_height * 3, 0);
                vector<double> col(f->_height * 3, 0);
                //cout << "   running " << i << " of " << f->_width - 1 << endl;

                if (i % 50 == 0)
                    cout << "   ...generating column " << i << endl;

                for (int j = f->_height - _h; j >= 0; j--)
                {
                    vector<double> output = _n->run(td.get_Input(_n_in, i, j));

                    vector<double> run_cost(td.get_Cost(output, i + 1, j));
                    
                    apply_cost(run_cost);

                    _n->clear();

                    for (int k = j * 3; k < (j + _h) * 3; k++)
                    {
                        col[k] = col[k] * ns[k] + output[k - j * 3];
                        ns[k]++;
                        col[k] = col[k] / ns[k];
                    }
                }

                td.set_Column(col, i + 1, 0, f->_height);
            }

            Frame *f2 = td.get_Frame();

            image->settoFrame(f2->flip());

            out.open("output/generate_out " + to_string(j) + "-" + to_string(n) + ".bmp");
            out << *image;
            out.close();

            cout << "...image written" << endl;
        }

        class IG_Net 
        {
            public:
                IG_Net() {}

                IG_Net(int n_in, int n_hid, int d_hid, int n_out)
                    : _n_in(n_in), _n_hid(n_hid), _d_hid(d_hid), _n_out(n_out), _fileName("image_gen/bored_wnbs.csv")
                {
                    vector<neuron*> layer_in;
                    vector<neuron*> layer_a;
                    vector<neuron*> layer_b;
                    vector<neuron*> layer_out;

                    for (int i = 0; i < n_in; i++)
                        layer_in.push_back(new neuron(0));

                    _layers.push_back(layer_in);

                    for (int l = 0; l < d_hid; l++)
                    {
                        if (l == 0)
                            layer_a.assign(layer_in.begin(), layer_in.end());
                        else
                        {
                            layer_a.clear();
                            layer_a.assign(layer_b.begin(), layer_b.end());
                            layer_b.clear();
                        }

                        for (int i = 0; i < n_hid; i++)
                        {
                            double b = ((double)(rand() % 200 - 100)) / 100;

                            vector<double> weights(layer_a.size(), 0); 

                            for (int j = 0; j < weights.size(); j++)
                                weights[j] = ((double)(rand() % 200 - 100)) / 100;

                            layer_b.push_back(new neuron(layer_a, weights, b));
                        }

                        _layers.push_back(layer_b);
                    }

                    for (int i = 0; i < n_out; i++)
                    {
                        double b = ((double)(rand() % 200 - 100)) / 100;

                        vector<double> weights(layer_a.size(), 0); 

                        for (int j = 0; j < weights.size(); j++)
                            weights[j] = ((double)(rand() % 200 - 100)) / 100;

                        layer_out.push_back(new neuron(layer_b, weights, 0));
                    }

                    _layers.push_back(layer_out);
                }

                IG_Net(int n_in, int n_hid, int d_hid, int n_out, string fileName)
                    : _n_in(n_in), _n_hid(n_hid), _d_hid(d_hid), _n_out(n_out), _fileName(fileName)
                {
                    vector<neuron*> layer_in;
                    vector<neuron*> layer_a;
                    vector<neuron*> layer_b;
                    vector<neuron*> layer_out;

                    double d_pos = 0;
                    double b_pos = 0;
                    vector<vector<string>> data = readCSV(fileName);

                    for (int i = 0; i < n_in; i++)
                        layer_in.push_back(new neuron(0));

                    _layers.push_back(layer_in);

                    for (int l = 0; l < d_hid; l++)
                    {
                        if (l == 0)
                            layer_a.assign(layer_in.begin(), layer_in.end());
                        else
                        {
                            layer_a.clear();
                            layer_a.assign(layer_b.begin(), layer_b.end());
                            layer_b.clear();
                        }
                        
                        b_pos = d_pos;
                        d_pos++;

                        for (int i = 0; i < n_hid; i++)
                        {
                            double b = atof(data[b_pos][i].c_str());

                            vector<double> weights(layer_a.size(), 0); 

                            for (int j = 0; j < weights.size(); j++)
                                weights[j] = atof(data[d_pos][j].c_str());

                            d_pos++;

                            layer_b.push_back(new neuron(layer_a, weights, b));
                        }

                        _layers.push_back(layer_b);
                    }

                    b_pos = d_pos;
                    d_pos++;

                    for (int i = 0; i < n_out; i++)
                    {
                        double b = atof(data[b_pos][i].c_str());

                        vector<double> weights(layer_a.size(), 0); 

                        for (int j = 0; j < weights.size(); j++)
                            weights[j] = atof(data[d_pos][j].c_str());
                        d_pos++;

                        layer_out.push_back(new neuron(layer_b, weights, 0));
                    }

                    _layers.push_back(layer_out);
                }

                ~IG_Net()
                {
                    for (auto l : _layers)
                    {
                        for (auto n : l) delete n;
                    }
                }

                void clear()
                {
                    for (int l = 1; l < _layers.size(); l++)
                    {
                        for (auto n : _layers[l])
                        {
                            n->_ran = false;
                            n->_a = 0;
                            n->_z = 0;
                            n->_s = 0;
                        }
                    }
                }

                //Calculates Neural Network given input activations 'as'
                vector<double> run(vector<double> as)
                {
                    vector<double> out(_n_out, 0);

                    for (int i = 0; i < _n_in; i++)
                        _layers[0][i]->_a = as[i];

                    for (int i = 0; i < _n_out; i++)
                        out[i] = _layers[_d_hid + 1][i]->run();

                    return out;
                }

                //Calculates Neural Network given random input activation
                vector<double> run()
                {
                    vector<double> out(_n_out, 0);

                    for (int i = 0; i < _n_in; i++)
                        _layers[0][i]->_a = ((double)(rand() % 200 - 100)) / 100;

                    for (int i = 0; i < _n_out; i++)
                        out[i] = _layers[_d_hid + 1][i]->run();

                    return out;
                }

                //Saves weights and biases into csv file 
                void save_data()
                {
                    vector<vector<string>> data;
                    for (int i = 0; i < _d_hid; i++)
                    {
                        vector<string> bias;
                        for (int j = 0; j < _n_hid; j++)
                            bias.push_back(to_string(_layers[i + 1][j]->_b));

                        data.push_back(bias);

                        for (int j = 0; j < _n_hid; j++)
                        {
                            vector<string> weights;

                            for (int k = 0; k < _layers[i + 1][j]->_ws.size(); k++)
                                weights.push_back(to_string(_layers[i + 1][j]->_ws[k]));

                            data.push_back(weights);
                        }
                    }
                    
                    vector<string> bias;
                    for (int j = 0; j < _n_out; j++)
                        bias.push_back(to_string(_layers[_d_hid + 1][j]->_b));

                    data.push_back(bias);

                    for (int j = 0; j < _n_out; j++)
                    {
                        vector<string> weights;

                        for (int k = 0; k < _layers[_d_hid + 1][j]->_ws.size(); k++)
                            weights.push_back(to_string(_layers[_d_hid + 1][j]->_ws[k]));

                        data.push_back(weights);
                    }

                    writeCSV(data, _fileName);
                }

                vector<vector<neuron*>> _layers;

                //size of input layer
                int _n_in;

                //size of hidden layer
                int _n_hid;

                //number of hidden layers
                int _d_hid;

                //size of output layer
                int _n_out;

                //csv where weights and bias info is stored
                string _fileName;

        };

        IG_Net *_n;

        string _fileName;

        int _k;

        //height of input window
        int _h;
        
        //width of input window
        int _w;

        double _alpha;

        //size of input layer
        int _n_in;

        //size of hidden layer
        int _n_hid;

        //number of hidden layers
        int _d_hid;

        //size of output layer
        int _n_out;

        double _cdel;
        double _csum;
        double _wsum;
        double _bsum;
};