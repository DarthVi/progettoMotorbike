//
// Created by vincenzo on 14/11/19.
//

#include <stdio.h>
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

void Waypoint::RespawnWaypoint()
{
    if(respawn)
    {
        srand(time(0));
        pos_x = (rand() % 59 + 1) - 30;
        pos_y = 0;
        pos_z = (rand() % 59 + 1) - 30;
        respawn = false;
    }
}

void Waypoint::drawWaypoint()
{
    glPushMatrix();
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    if (!useWireframe)
    {
        glColor3f(0, 0, 0.8);
        glBindTexture(GL_TEXTURE_2D, 6); //Texture da applicare alla sfera
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        //Applico la texture al disco
        gluQuadricTexture(quadric, true);
    }
    //Imposto la posizione generata ed una piccola rotazione
    glTranslatef(pos_x, pos_y, pos_z);
    //glRotatef(45, 1, 1, 0);
    gluQuadricDrawStyle(quadric, GLU_FILL);		//Disegna un disco pieno
    gluDisk(quadric, 1.5, 2, 36, 36);

    gluDeleteQuadric(quadric);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Waypoint::Render(float px, float py, float pz)
{
    glPushMatrix();
    drawWaypoint();
    glPopMatrix();
    // se la moto passa attraverso il waypoint
    if (px >= pos_x - 1 && px <= pos_x + 1 && py >= pos_y - 1 && py <= pos_y + 1
        && pz >= pos_z - 1 && pz <= pos_z + 1)
    {
        punteggio++;
        respawn = true;
        isDrawn = false;
    }
}

