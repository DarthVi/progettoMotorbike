//
// Created by vincenzo on 19/11/19.
//

#include "object.h"

class Statua : public Object
{
public:
    Statua() : Object() {};
    void DrawStatua(float posx, float posy, float posz);
};
