//
// Created by vincenzo on 12/11/19.
//

#include "object.h"

class Streetlamp : public Object
{
public:
    Streetlamp() : Object() {};
    void DrawStreetlamp(float posx, float posy, float posz);
};