//
// Created by vincenzo on 19/11/19.
//

#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <vector> // la classe vector di STL

#include "statua.h"
#include "point3.h"
#include "utils.h"
#include "mesh.h"

void drawStatua(float posx, float posy, float posz, bool shadow);

extern bool useShadow;
extern float lightPosition[];

Mesh statuaMesh((char *) "meshes/screaming_statue.obj");

void Statua::DrawStatua(float posx, float posy, float posz)
{
    drawStatua(posx, posy, posz, false);

    //disegna l'ombra
    if (useShadow)
    {
        glPushMatrix();

        glShadowProjection(lightPosition, e, n);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2, 0.2, 0.2);
        drawStatua(posx, posy, posz, true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void drawStatua(float posx, float posy, float posz, bool shadow)
{
    glPushMatrix();
    glScalef(1, 1, 1);


    glPushMatrix();
    if (!shadow)
        glColor3f(.7, .7, .7);
    glTranslatef(posx, posy, posz);
    glRotated(90, 0, 1, 0);

    statuaMesh.RenderNxV();
    glPopMatrix();

    glPopMatrix();
}