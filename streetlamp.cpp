//
// Created by vincenzo on 12/11/19.
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
#include "streetlamp.h"
#include "point3.h"
#include "mesh.h"
#include "utils.h"


void drawStreetLamp(float posx, float posy, float posz, bool shadow);
void drawLampLight(float posx, float posy, float posz, int lightN);

extern bool useShadow;
extern float lightPosition[];
Mesh streetlampMesh((char *) "streetlamp.obj");


void Streetlamp::Init()
{
    e[0] = 0;
    e[1] = 0.02;
    e[2] = 0;
    n[0] = 0;
    n[1] = -1;
    n[2] = 0;
}

void Streetlamp::DrawStreetlamp(float posx, float posy, float posz)
{
    drawStreetLamp(posx, posy, posz, false);

    //disegna l'ombra
//    if (useShadow)
//    {
//        glPushMatrix();
//
//        glShadowProjection(lightPosition, e, n);
//        glDisable(GL_LIGHTING);
//        glColor3f(0.2, 0.2, 0.2);
//        drawStreetLamp(posx, posy, posz, true);
//        glEnable(GL_LIGHTING);
//        glPopMatrix();
//    }
}


void drawStreetLamp(float posx, float posy, float posz, bool shadow)
{
    glPushMatrix();
    glScalef(1, 1, 1);

    glPushMatrix();
    if (!shadow)
        glColor3f(.1, .1, .1);
    glTranslatef(posx, posy, posz);
    streetlampMesh.RenderNxV();
    if(!shadow)
        drawLampLight(posx, posy, posz, 1);
    glPopMatrix();

    glPopMatrix();
}

void drawLampLight(float posx, float posy, float posz, int lightN)
{
    int usedLight=GL_LIGHT1 + lightN;

    glEnable(usedLight);

    float col0[4]= {0.8,0.8,0.0,  1};
    glLightfv(usedLight, GL_DIFFUSE, col0);

    float col1[4]= {0.5,0.5,0.0,  1};
    glLightfv(usedLight, GL_AMBIENT, col1);

    float tmpPos[4] = {posx, posy +10, posz,  1}; // ultima comp=1 => luce posizionale
    glLightfv(usedLight, GL_POSITION, tmpPos );

    float tmpDir[4] = { -2, -25, -2, 0 }; // ultima comp=1 => luce posizionale
    glLightfv(usedLight, GL_SPOT_DIRECTION, tmpDir );

    glLightf (usedLight, GL_SPOT_CUTOFF, 20);
    glLightf (usedLight, GL_SPOT_EXPONENT,3);

    //printf("Accesa luce lampione\n");

    //glLightf(usedLight,GL_CONSTANT_ATTENUATION,0);
    //glLightf(usedLight,GL_LINEAR_ATTENUATION,1);
}