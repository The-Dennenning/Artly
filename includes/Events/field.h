#include "../events.h"

class Field : public Event
{
    public:
        Field(int start, int dur, int width, int height, int n, int t) :
            Event(start, dur, width, height), _n(n), _t(t), _s(0)
            {
                cout << "   generating field..." << endl;
                generate_field();
                cout << "   generating particles..." << endl;
                generate_particles();
            }

        void Activate(Frame* f, Layer* l);

        //gives field strengths for every point in 2D plane
        //  _field[i][j][_] is field strength at point i, j
        //              [0] is field strength in i direction
        //              [1] is field strength in j direction
        vector<vector<vector<double>>> _field;

        //gives list of particles and their physical quantities
        //  _particles[n][_] is the nth particle
        //               [0] is position in i
        //               [1] is position in j
        //               [2] is velocity in i
        //               [3] is velocity in j
        //               [4] is acceleration in i
        //               [5] is acceleration in j
        //               [6] is r value
        //               [7] is g value
        //               [8] is b value
        vector<vector<double>> _particles;

        //gives trails to each particle
        //  _trails[n][_][_] is the nth particle trail
        //            [j][_] is the ((_s - j) % _t)th trail piece
        //               [0] is the position of this trail in i
        //               [1] is the position of this trail in i
        vector<vector<vector<double>>> _trails;

        //number of particles to simulate
        int _n;

        //length of trail to simulate
        int _t;

        //step count
        //  for use in simulating tail
        int _s;

        void generate_field();
        void generate_particles();
        void update();
        void print_to_frame(Frame* f);
};

void Field::Activate(Frame* f, Layer* l)
{
    for (int i = 0; i < _duration; i++)
    {
        cout << "   calculating frame " << i << endl;
        print_to_frame(f);
        update();
        l->_frames.push_back(f);
        f = new Frame(*f);
        f->_clear();
    }
}

void Field::generate_field()
{
    vector<vector<vector<double>>> temp;

    for (int i = 0; i < _width; i++)
    {
        vector<vector<double>> rows;

        for (int j = 0; j < _height; j++)
        {
            vector<double> vals;
                vals.push_back(((double)(rand() % 21 - 10)) / 10);
                vals.push_back(((double)(rand() % 21 - 10)) / 10);
                vals.push_back(rand() % 255 + 1);
                vals.push_back(rand() % 255 + 1);
                vals.push_back(rand() % 255 + 1);
            rows.push_back(vals);
        }

        temp.push_back(rows);
    }
    
    int N = 9;

    double *Gaussian = new double[N];

    for (int k = 0; k < N; k++)
        Gaussian[k] = Binomial(N, k);

    for (int i = 0; i < _width; i++)
    {
        vector<vector<double>> _rows;

        for (int j = 0; j < _height; j++)
        {
            vector<double> _vals(5, 0);
            vector<double> sum(5, 0);
            double total;

            //Zero out summing variable
            for (int k = 0; k < 5; k++)
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
                            for (int k = 0; k < 2; k++)
                            {
                                sum[k] += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)] * (temp[i + g_i][j + g_j][k]);
                            }

                            total += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)];
                        }
                    }
                }
            }

            //Assigns value to temporary holding
            //	So as to not mess with other filter calculations
            for (int k = 0; k < 2; k++)
                _vals[k] = sum[k] / total;

            for (int k = 2; k < 5; k++)
                _vals[k] = temp[i][j][k];
            
            _rows.push_back(_vals);
        }

        _field.push_back(_rows);
    }

    for (int i = 0; i < _width; i++)
    {
        for(int j = 0; j < _height; j++)
        {
            _field[i][j][0] *= sin(2 * 3.1415 * (((double) i) / _width));
            _field[i][j][1] *= cos(2 * 3.1415 * (((double) j) / _height));
        }
    }

    /*
    //adds attractors / repulsors
    for (int n = 0; n < 100; n++)
    {
        double m = rand() % 1000;
        int x = rand() % _width;
        int y = rand() % _height;

        for (int i = 0; i < _width; i++)
        {  
            for (int j = 0; j < _height; j++)
            {
                double d = pow(pow(i - x, 2) + pow(j - y, 2), 0.5);
                double f;

                if (d > 0)
                {
                    f = m / pow(d, 1.2);

                    if (rand() % 2 == 1)
                    {
                        _field[i][j][0] -= ((i - x) / d) * f;
                        _field[i][j][1] -= ((j - y) / d) * f;
                    }
                    else
                    {
                        _field[i][j][0] += ((i - x) / d) * f;
                        _field[i][j][1] += ((j - y) / d) * f;
                    }
                }
            }
        }
    }
    */
}

void Field::generate_particles()
{
    for (int n = 0; n < _n; n++)
    {
        vector<double> particle;
            //sets starting position of particle
            particle.push_back(rand() % _width);
            particle.push_back(rand() % _height);
            //sets starting velocity of particle to field value
            particle.push_back(_field[particle[0]][particle[1]][0]);
            particle.push_back(_field[particle[0]][particle[1]][1]);
            //sets starting acceleration of particle to field value
            particle.push_back(_field[particle[0]][particle[1]][0]);
            particle.push_back(_field[particle[0]][particle[1]][1]);
            //sets mass of particle
            particle.push_back(rand() % 5 + 1);
            //adjusts acceleration by mass
            particle[4] /= particle[6];
            particle[5] /= particle[6];
            //sets initial held color of particle
            particle.push_back(rand() % 255 + 1);
            particle.push_back(rand() % 255 + 1);
            particle.push_back(rand() % 255 + 1);
            //sets initial shown color of particle
            particle.push_back(particle[7]);
            particle.push_back(particle[8]);
            particle.push_back(particle[9]);
        _particles.push_back(particle);

        vector<vector<double>> trail;

        for (int t = 0; t < _t; t++)
        {
            vector<double> vals;
                vals.push_back(particle[0]);
                vals.push_back(particle[1]);
            trail.push_back(vals);
        }

        _trails.push_back(trail);
    }
}

void Field::update()
{
    //updates trail step counter
    _s++;

    for (int n = 0; n < _n; n++)
    {
        //update velocities
        _particles[n][2] += _particles[n][4];
        _particles[n][3] += _particles[n][5];
    
        //adjusts down velocity and acceleration (to simulate a little drag)
        _particles[n][2] *= 0.95;
        _particles[n][3] *= 0.95;
        _particles[n][4] *= 0.95;
        _particles[n][5] *= 0.95;

        //update positions
        _particles[n][0] += _particles[n][2];
        _particles[n][1] += _particles[n][3];
        
        //check boundary conditions
        while (_particles[n][0] < 0)
            _particles[n][0] += _width;
            
        while (_particles[n][0] >= _width)
            _particles[n][0] -= _width;
        
        while (_particles[n][1] < 0)
            _particles[n][1] += _height;
            
        while (_particles[n][1] >= _height)
            _particles[n][1] -= _height;

        //update accelerations
        _particles[n][4] = _field[(int)_particles[n][0]][(int)_particles[n][1]][0] / _particles[n][6];
        _particles[n][5] = _field[(int)_particles[n][0]][(int)_particles[n][1]][1] / _particles[n][6];

        //update trail
        _trails[n][_s % _t][0] = _particles[n][0];
        _trails[n][_s % _t][1] = _particles[n][1];

        /*
        //update color
        for (int k = 0; k < 3; k++)
            _particles[n][10 + k] *= pow(_field[(int)_particles[n][0]][(int)_particles[n][1]][2 + k] / _particles[n][10 + k], 0.1);
        */
    }
}

void Field::print_to_frame(Frame* f)
{
    for (int n = 0; n < _n; n++)
    {
        for (int t = 0; t < _t; t++)
        {
            double ratio = 1;

            for (int r = 0; r <= _t - t; r++)
                ratio *= 0.95;

            int j = (t + _s) % _t;

            for (int k = 0; k < 3; k++)
                f->_frame_data[((int)_trails[n][j][0] * _height + (int)_trails[n][j][1]) * 4 + k] = (int) ((double) _particles[n][10 + k] * ratio);
        }

        for (int k = 0; k < 3; k++)
        {
            f->_frame_data[((int)_particles[n][0] * _height + (int)_particles[n][1]) * 4 + k] = (int)_particles[n][10 + k];
        }
    }
}
