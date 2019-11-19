//
// Created by vincenzo on 19/11/19.
//

#include "object.h"

class Bench : public Object
{
public:
    Bench() : Object() {};
    void DrawBench(float posx, float posy, float posz);
};
