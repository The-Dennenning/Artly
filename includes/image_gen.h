#include <iostream>

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
                    4 * _height * _depth + 3 * _k

            neural network provides as output...

                per pixel...
                    info about which kmeans cluster pixel belongs to
                        1 double = _k id / _k total
                    info about distance from cluser
                        3 doubles e [0, 1]

                which totals...
                    4 * _height

            has _h hidden layers, of _height * _depth * 2 values

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

class Training_Data
{
    public:
        Training_Data(Frame *f, int k)
            : _height(f->_height), _width(f->_width), _k(k)
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

        //gets a 2D slice from the training data, formatted as a 1D input vector
        //  input vector is of of size n_in
        //  2D slice goes from point (i_off, j_off)
        //                  to point (i_off - depth, j_off + height)
        vector<double> get_Input(int n_in, int i_off, int j_off, int height, int depth)
        {
            vector<double> input;

            if (j_off + height > _height)
            {   
                cout << "Hull Breach - vertical offset error" << endl;
                return input;
            }

            if (i_off - depth < -1)
            {
                cout << "Hull Breach - horizontal offset error" << endl;
                return input;
            }

            for (int i = i_off; i > i_off - depth; i--)
            {
                for (int j = j_off; j < j_off + height; j++)
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

        void set_Column(vector<double> output, int i_off, int j_off, int height)
        {
            int o = 0;

            for (int j = j_off; j < j_off + height; j++)
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
        vector<double> get_Cost(vector<double> output, int i_off, int j_off, int height)
        {
            vector<double> cost;
            int o = 0;

            for (int j = j_off; j < j_off + height; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    cost.push_back(_data[i_off * _height * 3 + j * 3 + k] - output[o]);
                    o++;
                }

                cost.push_back(_clusters[i_off * _height + j] - output[o]);
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
        int _k;

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

class Training_Data_2
{
    public:
        Training_Data_2(Frame *f)
            : _height(f->_height), _width(f->_width)
        {
            for (int i = 0; i < _width * _height; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    _data.push_back((((double) f->_frame_data[i * 4 + j])) / 255);
                }
            }
        }

        //gets a 2D slice from the training data, formatted as a 1D input vector
        //  input vector is of of size n_in
        //  2D slice goes from point (i_off, j_off)
        //                  to point (i_off - depth, j_off + height)
        vector<double> get_Input(int n_in, int i_off, int j_off, int height, int depth)
        {
            vector<double> input;

            if (j_off + height > _height)
            {   
                cout << "Hull Breach - vertical offset error" << endl;
                return input;
            }

            if (i_off - depth < -1)
            {
                cout << "Hull Breach - horizontal offset error" << endl;
                return input;
            }

            for (int i = i_off; i > i_off - depth; i--)
            {
                for (int j = j_off; j < j_off + height; j++)
                {
                    //get color information
                    for (int k = 0; k < 3; k++)
                        input.push_back(_data[i * _height * 3 + j * 3 + k]);
                }
            }

            return input;
        }

        void set_Column(vector<double> output, int i_off, int j_off, int height)
        {
            int o = 0;

            for (int j = j_off; j < j_off + height; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    _data[i_off * _height * 3 + j * 3 + k] = output[o];
                    o++;
                }
            }
        }

        //gets raw difference between output data given and training data specified 
        vector<double> get_Cost(vector<double> output, int i_off, int j_off, int height)
        {
            vector<double> cost;
            int o = 0;

            for (int j = j_off; j < j_off + height; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    cost.push_back(_data[i_off * _height * 3 + j * 3 + k] - output[o]);
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

            for (int i = 0; i < _width * _height; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    int rgb = (_data[i * 3 + j] * 255);

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

        //pixel data is stored as a signed normalized distance from cluster mean in each color channel 
        //  e.g. _data = rgb / 255
        vector<double> _data;
};


class Image_Gen
{
    public:
        Image_Gen(int height, int depth, string fileName)
            : _height(height), _depth(depth), _alpha(0.0000005), _n_in(height * depth * 3), _n_hid(height * 2), _d_hid(2), _n_out(_height * 3), _fileName(fileName), _data_type(2)
            {
                //new_data();
                run_images();
            }

        Image_Gen(int height, int depth, int k, string fileName)
            : _height(height), _depth(depth), _k(k), _alpha(0.0003), _n_in(height * depth * 4 + _k * 3), _n_hid(height * 3), _d_hid(3), _n_out(_height * 4), _fileName(fileName), _data_type(1)
            {
                //new_data();
                run_images();
                //generate_images(10);
                
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
            _n = new IG_Net(_n_in, _n_hid, _d_hid, _n_out, "image_gen/wnbs.csv");

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

        void run_images()
        {
            _n = new IG_Net(_n_in, _n_hid, _d_hid, _n_out, _fileName);

            cout << "...csv data read" << endl;

            double total_cost = 0;
            int size = 300;
            int trials = 2;

            vector<int> ms;
            for (int i = 1; i <= size; i++) ms.push_back(i);
            random_shuffle(ms.begin(), ms.end());

            for (int m = 1; m < size; m++)
            {
                cout << "Run " << m << ": in " << ms[m] << endl;

                string filename = "input/pron/smolled/in " + to_string(ms[m]) + ".bmp";

                Bitmap *image = new Bitmap();
                ifstream in;
                ofstream out;

                in.open(filename);
                in >> *image;
                in.close();

                Frame *f = new Frame(*image);

                Training_Data td(f, _k);
                
                _cdel = 0;
                double t_sum = 0;
                vector<double> t_costs;

                for (int t = 0; t < trials; t++)
                {
                    vector<double> run_cost(run_image(&td, 0));
                        
                    _wsum = 0;
                    _bsum = 0;
                    _csum = 0;

                    for (auto c : run_cost)
                        _csum += c * c;

                    t_costs.push_back(_csum / f->_width);
                    t_sum += _csum / f->_width;
                        
                    apply_cost(run_cost);

                    cout << "   Trial " << t << endl;
                    cout << "       ...run adjusted cost = " << _csum / f->_width << endl;
                    cout << "       ...run weight delta = " << _wsum << endl;
                    cout << "       ...run bias delta = " << _bsum << endl;
                }

                cout << endl;
                cout << "   Delta Cost from trials... " << endl; 

                for (int t = 1; t < trials; t++)
                {
                    cout << "       " << t - 1 << " to " << t << ": " << t_costs[t - 1] - t_costs[t] << endl;
                }

                total_cost = ((total_cost * (m - 1)) + (t_sum / trials)) / m;

                cout << endl;
                cout << "   ...total average cost = " << total_cost << endl << endl;

                delete image;
                delete f;

                if (m % 10 == 0)
                generate_image("input/pron/smolled/in " + to_string(ms[m]) + ".bmp", ms[m], m);

                if (m % 50 == 0)
                {
                    _n->save_data();
                    cout << "...csv data written" << endl;
                }
            }
            
            _n->save_data();

            cout << "...csv data written" << endl;

            delete _n;

        }

        vector<double> run_image(Training_Data *td, int type)
        {
            vector<double> cost;

            //run over entire image
            for (int i = _depth - 1; i < td->_width; i++)
            {
                vector<double> output(_n->run(td->get_Input(_n_in, i, 0, _height, _depth)));
                vector<double> run_cost(td->get_Cost(output, i + 1, 0, _height));

                //accumulate cost to run later
                if (type == 0)
                {
                    int n = 0;

                    //accumulate costs for entire image
                    if (cost.empty())
                        for (auto c : run_cost) cost.push_back(c);
                    else
                        for (auto c : run_cost) cost[n++] += c;
                }
                else
                {
                    apply_cost(run_cost);
                }
                
                _n->clear();
            }

            return cost;
        }
        
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
                        _n->_layers[l][i]->_ws[j] += _alpha * _n->_layers[l][i]->_ns[j]->_a * _n->_layers[l][i]->_s;

                        _wsum += pow(_alpha * _n->_layers[l][i]->_ns[j]->_a * _n->_layers[l][i]->_s, 2);
                    }

                    _n->_layers[l][i]->_b += _alpha * _n->_layers[l][i]->_s;

                    _bsum += pow(_alpha * _n->_layers[l][i]->_s, 2);
                }
            }
        }

        void generate_images(int n)
        {
                
            _n = new IG_Net(_n_in, _n_hid, _d_hid, _n_out, _fileName);

            for (int i = 0; i < n; i++)
            {
                int r = rand() % 200 + 1;
                generate_image("input/pron/smolled/in " + to_string(r) + ".bmp", r, i);
            }

            delete _n;
        }

        void generate_image(string file, int n, int j)
        {
            Bitmap *image = new Bitmap();
            ifstream in;
            ofstream out;

            in.open(file);
            in >> *image;
            in.close();

            cout << "...image read" << endl;

            Frame *f = new Frame(*image);

            Training_Data td(f, _k);

            cout << "...training data created" << endl;

            //run over entire image
            for (int i = f->_width - 2; i > _depth - 1; i--)
            {
                //cout << "   running " << i << " of " << f->_width - 1 << endl;
                vector<double> output = _n->run(td.get_Input(_n_in, i, 0, _height, _depth));
                td.set_Column(output, i + 1, 0, _height);
                _n->clear();
            }

            Frame *f2 = td.get_Frame();

            cout << "...new frame gotten" << endl;

            image->settoFrame(f2->flip());

            out.open("output/image_gen/generate_out " + to_string(j) + "-" + to_string(n) + ".bmp");
            out << *image;
            out.close();

            cout << "...image written" << endl;
        }


        class IG_Net 
        {
            public:
                IG_Net() {}

                IG_Net(int n_in, int n_hid, int d_hid, int n_out)
                    : _n_in(n_in), _n_hid(n_hid), _d_hid(d_hid), _n_out(n_out), _fileName("image_gen/wnbs.csv")
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
                            double b = ((double)(rand() % 200 - 100)) / 1000;

                            vector<double> weights(layer_a.size(), 0); 

                            for (int j = 0; j < weights.size(); j++)
                                weights[j] = ((double)(rand() % 200 - 100)) / 1000;

                            layer_b.push_back(new neuron(layer_a, weights, b));
                        }

                        _layers.push_back(layer_b);
                    }

                    for (int i = 0; i < n_out; i++)
                    {
                        double b = ((double)(rand() % 200 - 100)) / 1000;

                        vector<double> weights(layer_a.size(), 0); 

                        for (int j = 0; j < weights.size(); j++)
                            weights[j] = ((double)(rand() % 200 - 100)) / 1000;

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

        int _height;
        
        int _depth;

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

        int _data_type;

};