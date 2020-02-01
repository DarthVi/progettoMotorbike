//
// Created by vincenzo on 17/11/19.
//

#ifndef OBJECT_CLASS
#define OBJECT_CLASS

class Object
{
public:
    float e[3];
    float n[3];
    Object()
    {
        e[0] = 0;
        e[1] = 0.02;
        e[2] = 0;
        n[0] = 0;
        n[1] = -1;
        n[2] = 0;
    }
};
#endif