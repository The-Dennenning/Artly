
#include "layer.h"

#include <vector>

#ifndef SPACE_H
#define SPACE_H

class Space;
class Object;
class Vertex;
class Face;

struct Ray
{
    double _coord[3];
    double _angle[3];
    double _height, _width;

    vector<int> rgb;
};

class Space
{
    public:
        Space() {}

        //space time
        //  cue PBS spacetime intro theme
        int _t;

        //List of all objects in space
        vector<Object*> _objects;

        Face* _f;

        //Takes a ray and intersects it with any objects in space
        //  to produce an RGB sample
        void intersect(Ray *r);
};

class Object
{
    public:
        Object() {}

        Object(Space* s) : _s(s) {}

        //link back to space
        //  to access universal time _t
        Space* _s; 

        //faces that make up this object
        //  indexed by face ID
        vector<Face*> _faces;

        //verticies that make up this object
        vector<Vertex*> _verts;
};


class Vertex
{
    public:
        Vertex() {}
        Vertex(int x, int y, int z)
        {
            _coord[0] = x;
            _coord[1] = y;
            _coord[2] = z;
        }

        //pair of both verticies this one connects to
        //  as indexed by face ID
        //      e.g. this vertex connects to verticies (a, b) on face 1
        //           but then connects to verticies (b, c) on face 2
        vector<pair<Vertex*, Vertex*>> _conto;

        //coordinates of this vertex
        int _coord[3];
        
        int check(double p[], int id);
};


class Face : public Object
{
    public:
        Face() : Object() {}
        
        Face(Space* s, double scale, int ID) 
            : Object(s), _scale(scale), _ID(ID) {}

        //face ID
        //  the index that this face has in object's face vector
        int _ID;

        //some unit vector normal to the plane of the face
        double _normal[3];

        //scales from content pixel to space coordinates
        double _scale;

        //contains the visual content played on the face
        Layer* _content;

        //checks if given ray intersects with face
        //  returns 1 if intersects, 0 if not
        //      if intersection happens, the ray will be given the respective RGB value
        int check(Ray* r);

        void get_content_value(Ray* r, double p[3]);  
};


void Space::intersect(Ray *r)
{
#ifdef DEBUG
    cout << "   checking intersection with face..." << endl;
#endif

    for (auto o : _objects)
    {
        for (auto f : o->_faces)
        {
            if (f->_ID == 1)
                cout << "checking face " << f->_ID << endl;
            if (f->check(r)) return;
        }
    }

    if (r->rgb.empty())
    {
        for (int k = 0; k < 3; k++)
            r->rgb.push_back(0);
    }
}


int Vertex::check(double p[], int id)
{
    Vertex *b = _conto[id].first;
    Vertex *c = _conto[id].second;

    double coords[9]; 
    
        for (int k = 0; k < 3; k++) 
            coords[k + 0] = _coord[k];
        for (int k = 0; k < 3; k++) 
            coords[k + 3] = b->_coord[k];
        for (int k = 0; k < 3; k++) 
            coords[k + 6] = c->_coord[k];

    //cycles through all three angles
    for (int i = 0; i < 3; i++)
    {
        int v1 = i;
        int v2 = (i + 1) % 3;
        int v3 = (i + 2) % 3;

        double x[5];
            x[0] = 0;
            x[1] = coords[v1 * 3 + 0];
            x[2] = coords[v2 * 3 + 0];
            x[3] = coords[v3 * 3 + 0];
            x[4] = p[0];

        double y[5];
            y[0] = 0;
            y[1] = coords[v1 * 3 + 1];
            y[2] = coords[v2 * 3 + 1];
            y[3] = coords[v3 * 3 + 1];
            y[4] = p[1];

        double z[5];
            z[0] = 0;
            z[1] = coords[v1 * 3 + 2];
            z[2] = coords[v2 * 3 + 2];
            z[3] = coords[v3 * 3 + 2];
            z[4] = p[2];

#ifdef DEGUG 
        cout << "       checking angle " << i << " between..." << endl;
        cout << "           x = [" << x[1] << ", " << x[2] << ", " << x[3] << ", " << x[4] << "]" << endl;
        cout << "           y = [" << y[1] << ", " << y[2] << ", " << y[3] << ", " << y[4] << "]" << endl;
        cout << "           z = [" << z[1] << ", " << z[2] << ", " << z[3] << ", " << z[4] << "]" << endl;
#endif

        double t;
        double den;
        double num;
        bool flag = false;

        if (x[2] - x[3] != 0)
        {
            if ((y[1] - y[4] - (y[2] - y[3]) * (x[1] - x[4]) / (x[2] - x[3])) != 0)
            {
                num = y[2] - y[1] + (y[2] - y[3]) * (x[1] - x[2]) / (x[2] - x[3]);
                den = y[1] - y[4] - (y[2] - y[3]) * (x[1] - x[4]) / (x[2] - x[3]);
                flag = true;
            }
            else if ((z[1] - z[4] - (z[2] - z[3]) * (x[1] - x[4]) / (x[2] - x[3])) != 0)
            {
                num = z[2] - z[1] + (z[2] - z[3]) * (x[1] - x[2]) / (x[2] - x[3]);
                den = z[1] - z[4] - (z[2] - z[3]) * (x[1] - x[4]) / (x[2] - x[3]);
                flag = true;
            }
        }

        if ((y[2] - y[3] != 0) && !flag)
        {
            if ((z[1] - z[4] - (z[2] - z[3]) * (y[1] - y[4]) / (y[2] - y[3])) != 0)
            {
                num = z[2] - z[1] + (z[2] - z[3]) * (y[1] - y[2]) / (y[2] - y[3]);
                den = z[1] - z[4] - (z[2] - z[3]) * (y[1] - y[4]) / (y[2] - y[3]);
                flag = true;
            }
            else if ((x[1] - x[4] - (x[2] - x[3]) * (y[1] - y[4]) / (y[2] - y[3])) != 0)
            {
                num = x[2] - x[1] + (x[2] - x[3]) * (y[1] - y[2]) / (y[2] - y[3]);
                den = x[1] - x[4] - (x[2] - x[3]) * (y[1] - y[4]) / (y[2] - y[3]);
                flag = true;
            }
        }

        if ((z[2] - z[3] != 0) && !flag)
        {
            if ((y[1] - y[4] - (y[2] - y[3]) * (z[1] - z[4]) / (z[2] - z[3])) != 0)
            {
                num = y[2] - y[1] + (y[2] - y[3]) * (z[1] - z[2]) / (z[2] - z[3]);
                den = y[1] - y[4] - (y[2] - y[3]) * (z[1] - z[4]) / (z[2] - z[3]);
                flag = true;
            }
            else if ((x[1] - x[4] - (x[2] - x[3]) * (z[1] - z[4]) / (z[2] - z[3])) != 0)
            {
                num = x[2] - x[1] + (x[2] - x[3]) * (z[1] - z[2]) / (z[2] - z[3]);
                den = x[1] - x[4] - (x[2] - x[3]) * (z[1] - z[4]) / (z[2] - z[3]);
                flag = true;
            }
        }

        if (!flag) return 0;
        t = num / den;

        double d[3];
        double l[3];

            d[0] = (x[1] + t * (x[1] - x[4])) - x[1];
            d[1] = (y[1] + t * (y[1] - y[4])) - y[1];
            d[2] = (z[1] + t * (z[1] - z[4])) - z[1];

            l[0] = x[4] - x[1];
            l[1] = y[4] - y[1];
            l[2] = z[4] - z[1];

        double l1 = pow((pow(l[0], 2) + pow(l[1], 2) + pow(l[2], 2)), 0.5);
        double l2 = pow((pow(d[0], 2) + pow(d[1], 2) + pow(d[2], 2)), 0.5);

#ifdef DEBUG
        cout << "       t = " << t << endl;
        cout << "       l = [" << l[0] << ", " << l[1] << ", " << l[2] << "]" << endl;
        cout << "       d = [" << d[0] << ", " << d[1] << ", " << d[2] << "]" << endl;
        cout << "       l1 = " << l1 << endl;
        cout << "       l2 = " << l2 << endl;
#endif


        //tests if p4 (intersecton of ray w/ plane of face) is inside or outside the line opposite this angle
        //  if outside, return that the ray does not intersect face
        if (l2 < l1)
            return 0;
    }
    //rejoice, for p is inside the face
    return 1;
}


int Face::check(Ray* r)
{
    //solve for parametric t, 
    //  to get point where ray intersects plane of face
    double num = (
        (_verts[0]->_coord[0] - r->_coord[0]) * _normal[0] +
        (_verts[1]->_coord[1] - r->_coord[1]) * _normal[1] +
        (_verts[2]->_coord[2] - r->_coord[2]) * _normal[2]
    );

    double denom = (
        r->_angle[0] * _normal[0] +
        r->_angle[1] * _normal[1] +
        r->_angle[2] * _normal[2]
    );

#ifdef DEBUG
        cout << "       num = " << num << endl;
        cout << "           normal = [" << _normal[0] << ", " << _normal[1] << ", " << _normal[2] << "]" << endl;
        cout << "           ray = [" << r->_angle[0] << ", " << r->_angle[1] << ", " << r->_angle[2] << "]" << endl;
        cout << "       denom = " << denom << endl;
#endif

    //is parallel to plane that face is on
    if (denom == 0)
        return 0;

    double t = num / denom;

#ifdef DEBUG
    cout << "       t = " << t << endl;
#endif

    //intersects with face behind camera, thus not applicable
    if (t < 0)
        return 0;

    double p[3] = {0, 0, 0};

    //gets coordinates of intersection
    for (int k = 0; k < 3; k++)
        p[k] = r->_coord[k] + r->_angle[k] * t;

#ifdef DEBUG
    cout << "       ray intersects face at (" << p[0] << ", " << p[1] << ", " << p[2] << ")" << endl;
#endif

    //for every vertex in face,
    //  checks that it falls "inside" of vertex
    //      i.e. does it fall between the two verticies this vertex connects to
    //  this only works for convex faces
    //      will have to find a better algorithm to check for inclusion in concave faces
    for (auto v : _verts)
    {
#ifdef DEBUG
        cout << "   checking verticies..." << endl;
#endif
        if (v->check(p, _ID))
        {
            get_content_value(r, p);
            return 1;
        }
    }

    return 0;
}

void Face::get_content_value(Ray* r, double p[3])
{
#ifdef DEBUG
    cout << "   Getting Content Value..." << endl;
#endif

    double v1[3]; 
    double v2[3];
    double v3[3];

        //Gets vector normal to _normal, and in x-y plane
        //  defines horizontal 'edge' of content
        v1[0] = -_normal[1];
        v1[1] = _normal[0];
        v1[2] = 0;

        //if no direction was established by above operation
        //  estabish direction
        if (v1[0] == 0 && v1[1] == 0)
            v1[1] = 1;

        //normalize vector
        v1[0] = v1[0] / pow(pow(v1[0], 2) + pow(v1[1], 2), 0.5);
        v1[1] = v1[1] / pow(pow(v1[0], 2) + pow(v1[1], 2), 0.5);

        //Gets vector normal to _normal and v1 (using cross product)
        //  defines vertical 'edge' of content
        v2[0] = _normal[1] * v1[2] - _normal[2] * v1[1];
        v2[1] = _normal[0] * v1[2] - _normal[2] * v1[0];
        v2[2] = _normal[0] * v1[1] - _normal[1] * v1[0];

        //normalize vector
        v2[0] = v2[0] / pow(pow(v2[0], 2) + pow(v2[1], 2) + pow(v2[2], 2), 0.5);
        v2[1] = v2[1] / pow(pow(v2[0], 2) + pow(v2[1], 2) + pow(v2[2], 2), 0.5);
        v2[2] = v2[2] / pow(pow(v2[0], 2) + pow(v2[1], 2) + pow(v2[2], 2), 0.5);

        //gets position vector of 'main' vertex of face
        v3[0] = _verts.front()->_coord[0];
        v3[1] = _verts.front()->_coord[1];
        v3[2] = _verts.front()->_coord[2];

    //j value of content pixel
    //  solves v3 + j*v2 + i*v1 = p for j
    int j;

        if (v2[0] * v1[1] - v1[0] != 0) 
        {
            j = (v1[0] * (p[1] - v3[1]) + v1[0] - p[0]) /
            (v2[0] * v1[1] - v1[0]);
        }
        else if (v2[0] * v1[2] - v1[0] != 0) 
        {
            j = (v1[0] * (p[2] - v3[2]) + v1[0] - p[0]) /
            (v2[0] * v1[2] - v1[0]);
        }
        else if (v2[1] * v1[2] - v1[1] != 0) 
        {
            j = (v1[1] * (p[2] - v3[2]) + v1[1] - p[1]) /
            (v2[1] * v1[2] - v1[1]);
        }
        else if (v2[1] * v1[0] - v1[1] != 0) 
        {
            j = (v1[1] * (p[0] - v3[0]) + v1[1] - p[1]) /
            (v2[1] * v1[0] - v1[1]);
        }
        else if (v2[2] * v1[0] - v1[2] != 0) 
        {
            j = (v1[2] * (p[0] - v3[0]) + v1[2] - p[2]) /
            (v2[2] * v1[0] - v1[2]);
        }
        else if (v2[2] * v1[1] - v1[2] != 0) 
        {
            j = (v1[2] * (p[1] - v3[1]) + v1[2] - p[2]) /
            (v2[2] * v1[1] - v1[2]);
        }
        else j = 0;

    int i;

        if (v1[0] != 0)    
            i = (p[0] - v3[0] - j * v2[0]) / v1[0];
        else if (v1[1] != 0)
            i = (p[1] - v3[1] - j * v2[1]) / v1[1];
        else if (v1[2] != 0)
            i = (p[2] - v3[2] - j * v2[2]) / v1[2];
        else i = 0;

#ifdef DEBUG
    cout << "       pre scale and mod... (" << i << ", " << j << ")" << endl;
#endif 

    i = ((int) (i * _scale)) % _content->_width;
    j = ((int) (j * _scale)) % _content->_height;

    for (int k = 0; k < 3; k++)
        r->rgb.push_back(_content->_frames[_s->_t % _content->_frames.size()]->_frame_data[(i * _content->_height + j) * 4 + k]);

#ifdef DEBUG
    if (rand() % 10000 == 1) {
        cout << "       Sampling frame " << _s->_t % _content->_frames.size() << endl;
        cout << "           at pixel (" << i << ", " << j << ")" << endl;
    }
#endif

    /*
    if (r->rgb[0] != 0)
        cout << "       in func with value (" << r->rgb[0] << ", " << r->rgb[1] << ", " << r->rgb[2] << ")" << endl;
    */
} 


#endif