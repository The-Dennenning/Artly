#include "../events.h"

class Morph : public Event
{
    public:
        Morph(int start, int dur, int width, int height, Frame *target, int speed)
            : Event(start, dur, width, height), _target(target), _speed(speed)
            {}

        void Activate(Frame *f, Layer *l);

        Frame* _target;
        int _speed;
};


void Morph::Activate(Frame *f, Layer *l)
{
    Frame *f2 = new Frame(*f);
    int rgb[3];
    int rgb_targ[3];
    int rgb_curr[3];
    int rgb_init[3];

    int _k = 2;
    Bitmap *image;
    vector<Kdata*> data;
    
    Kmeans *K = new Kmeans(0, 0, f, image, _k);
    data.push_back(K->get_data());
    delete K;
    
    K = new Kmeans(0, 0, _target, image, _k);
    data.push_back(K->get_data());
    delete K;


    for (int n = 0; n < l->_frame_num; n++)
    {
        
        /* regular fade between images 
        f2 = new Frame(_width, _height);
        for (int i = 0; i < _width; i++)
        {
            for (int j = 0; j < _height; j++)
            {
                f->get(i, j, rgb_init);
                _target->get(i, j, rgb_targ);

                for (int k = 0; k < 3; k++)
                    rgb[k] = rgb_init[k] + (((double) (rgb_targ[k] - rgb_init[k])) / l->_frame_num) * n;

                f2->set(i, j, rgb);
            }
        }*/
        

        /* this is the cool one 
        for(int i = 0; i < _width; i++)
        {
            for (int j = 0; j < _height; j++)
            {
                f->get(i, j, rgb_init);
                _target->get(i, j, rgb_targ);

                int flag = -1 * data[0]->_clusters[i * _height + j];

                for (int k = 0; k < 3; k++)
                    rgb[k] = ((int)(rgb_init[k] + flag * (((double) (rgb_targ[k] - rgb_init[k])) / _speed) * n));

                f2->set(i, j, rgb);
            }
        }
        */
        
        

        f2 = new Frame(_width, _height);

        for(int i = 0; i < _width; i++)
        {
            for (int j = 0; j < _height; j++)
            {
                f->get(i, j, rgb_init);
                f2->get(i, j, rgb_curr);
                _target->get(i, j, rgb_targ);

                if (n < (l->_frame_num / 2))
                {
                    int flag = -1 * data[0]->_clusters[i * _height + j];

                    for (int k = 0; k < 3; k++)
                        rgb[k] = ((int)(rgb_init[k] + flag * (((double) (rgb_targ[k] - rgb_init[k])) / l->_frame_num) * n));
                }
                else
                {
                    for (int k = 0; k < 3; k++)
                        rgb[k] = ((int)(rgb_curr[k] + (((double) (rgb_targ[k] - rgb_curr[k])) / l->_frame_num) * n));
                }

                f2->set(i, j, rgb);
            }
        }

        l->_frames.push_back(f2);
    }
}