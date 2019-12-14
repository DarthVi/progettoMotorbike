//
// Created by vincenzo on 14/11/19.
//

#include "object.h"

class Waypoint : public Object
{
public:
    int pos_x, pos_y, pos_z;
    bool respawn;
    int rotation;

    void Render(float px, float py, float pz);
    void RespawnWaypoint();
    void drawWaypoint(bool shadow);
    bool checkCollision(float px, float py, float pz);


    Waypoint() : Object()
    {
        pos_x = (rand() % 60 + 1) - 30;
        pos_y = 0;
        pos_z = (rand() % 60 + 1) - 30;
        respawn = false;
    };
};
