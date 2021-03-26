

#include <vector>
#include <math.h>

#include "space.h"


#ifndef OBJECT_H
#define OBJECT_H


class Space;
class Face;
class Vertex;

struct Ray;

class Object
{
    public:
        Object();

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
        Vertex();

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
        Face();

        //face ID
        //  the index that this face has in object's face vector
        int _ID;

        //some unit vector normal to the plane of the face
        double _normal[3];

        //contains the visual content played on the face
        Layer* _content;

        //checks if given ray intersects with face
        //  returns 1 if intersects, 0 if not
        //      if intersection happens, the ray will be given the respective RGB value
        int check(Ray* r);

        void get_content_value(Ray* r, double p[3]);  
};



#endif