//
// Created by vincenzo on 13/11/19.
//

#include "object.h"

class Pumpstation : public Object
{
public:
    Pumpstation() : Object() {};
    void DrawPumpstation(float posx, float posy, float posz);
};
