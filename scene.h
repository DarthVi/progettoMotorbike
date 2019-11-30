//
// Created by vincenzo on 30/11/19.
//

#ifndef SCENE_H
#define SCENE_H

#include "object.h"

class Tabellone : public Object{
public:
    Tabellone() : Object() {};
    void DrawTabellone();
};

class Barile : public Object
{
public:
    Barile() : Object() {};
    void DrawBarile(float posx, float posy, float posz);
};

class Pumpstation : public Object
{
public:
    Pumpstation() : Object() {};
    void DrawPumpstation(float posx, float posy, float posz);
};

class Streetlamp : public Object
{
public:
    Streetlamp() : Object() {};
    void DrawStreetlamp(float posx, float posy, float posz);
};

class Statua : public Object
{
public:
    Statua() : Object() {};
    void DrawStatua(float posx, float posy, float posz);
};

class Bench : public Object
{
public:
    Bench() : Object() {};
    void DrawBench(float posx, float posy, float posz);
};

#endif //SCENE_H
