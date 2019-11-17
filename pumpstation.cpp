//
// Created by vincenzo on 13/11/19.
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
#include "pumpstation.h"
#include "point3.h"
#include "mesh.h"
#include "utils.h"

extern bool useShadow;
extern float lightPosition[];
extern bool useWireframe;

void drawPumpstation(float posx, float posy, float posz, float shadow);

Mesh pumpBody((char *) "meshes/pumpBody.obj");
Mesh pumpGun((char *) "meshes/pumpGun.obj");

void Pumpstation::DrawPumpstation(float posx, float posy, float posz)
{
    drawPumpstation(posx, posy, posz, false);

    if(useShadow)
    {
        glPushMatrix();

        glShadowProjection(lightPosition, e, n);
        glDisable(GL_LIGHTING);
        glColor3f(0.2, 0.2, 0.2);
        drawPumpstation(posx, posy, posz, true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void drawPumpstation(float posx, float posy, float posz, float shadow)
{
    glPushMatrix();
    glScalef(3, 3, 3);

    glPushMatrix();
    if(!shadow) glColor3f(1, 0, .0);
    glTranslatef(posx, posy, posz);
    pumpBody.RenderNxV();
    if(!shadow) glColor3f(0,0,0);
    pumpGun.RenderNxV();
    glPopMatrix();

    glPopMatrix();
}