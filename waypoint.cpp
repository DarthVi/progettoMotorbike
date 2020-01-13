//
// Created by vincenzo on 14/11/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <vector> // la classe vector di STL

#include "waypoint.h"
#include "point3.h"
#include "utils.h"

extern int punteggio;
extern bool useWireframe;
extern float lightPosition[];
extern bool useShadow;

void Waypoint::RespawnWaypoint()
{
    srand(time(0));
    //posizione random del waypoints
    pos_x = (rand() % 60 + 1) - 30;
    pos_y = 0;
    pos_z = (rand() % 60 + 1) - 30;
    //rotazione random
    rotation = rand() % 360;
}

void Waypoint::drawWaypoint(bool shadow)
{
    glPushMatrix();
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    if (!useWireframe && !shadow)
    {
        glColor3f(1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, MARBLE);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        //Applico la texture al disco
        gluQuadricTexture(quadric, true);
    }
    //traslo sulla posizione scelta ed effettuo una rotazione random
    glTranslatef(pos_x, pos_y, pos_z);
    glRotatef(rotation, 0, 1, 0);
    gluQuadricDrawStyle(quadric, GLU_FILL);		//Disegna un poligono pieno

    //disegno due cilindri concentrici e due dischi agli estremi dei cilindri
    //in modo da creare un arco con spessore
    gluCylinder(quadric, 2, 2, 0.5, 36, 36);
    gluCylinder(quadric, 2.5, 2.5, 0.5, 36, 36);
    glPushMatrix();
        glTranslatef(0, 0, 0.5);
        gluDisk(quadric, 2, 2.5, 36, 36);
        glTranslatef(0, 0, -0.5);
        gluDisk(quadric, 2, 2.5, 36, 36);
    glPopMatrix();
    //gluDisk(quadric, 1.5, 2, 36, 36);

    gluDeleteQuadric(quadric);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//px, py, pz are the coordinates of the motorbike
//pos_x, pos_y, pos_z are the coordinates of the waypoint
//simple check to see if we pass through the waypoint
bool Waypoint::checkCollision(float px, float py, float pz)
{
    return (px >= pos_x - 1 && px <= pos_x + 1 && py >= pos_y - 1 && py <= pos_y + 1
            && pz >= pos_z - 1 && pz <= pos_z + 1);
}

void Waypoint::Render(float px, float py, float pz)
{
    if(respawn)
    {
        RespawnWaypoint();
        respawn = false;
    }

    glPushMatrix();
    drawWaypoint(false);

//    if(useShadow)
//    {
//        glPushMatrix();
//        glColor3f(0.2, 0.2, 0.2);
//        glShadowProjection(lightPosition, e, n);
//        glDisable(GL_LIGHTING);
//        drawWaypoint(true);
//        glEnable(GL_LIGHTING);
//        glPopMatrix();
//    }

    glPopMatrix();
    // se la moto passa attraverso il waypoint
    if (checkCollision(px, py, pz))
    {
        punteggio++;
        respawn = true;
    }
}

