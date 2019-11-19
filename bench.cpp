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
#include "bench.h"
#include "point3.h"
#include "mesh.h"
#include "utils.h"

extern bool useShadow;
extern float lightPosition[];
extern bool useWireframe;

void drawBench(float posx, float posy, float posz, bool shadow);

Mesh benchMesh((char *) "meshes/bench.obj");

void Bench::DrawBench(float posx, float posy, float posz)
{
    drawBench(posx, posy, posz, false);

    //disegna l'ombra
    if (useShadow)
    {
        glPushMatrix();

        glShadowProjection(lightPosition, e, n);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2, 0.2, 0.2);
        drawBench(posx, posy, posz, true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}


void drawBench(float posx, float posy, float posz, bool shadow)
{
    glPushMatrix();
    glScalef(0.1, 0.1, 0.1);

    glPushMatrix();
    if (!shadow)
        glColor3f(.5, .5, .5);
    glTranslatef(posx, posy, posz);
    glRotated(90, 0, 1, 0);
    if (!shadow)
    {
        glColor3f(1, 1, 1);
        if (!useWireframe)
        {
            glBindTexture(GL_TEXTURE_2D,WOOD);

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord texture S e T
            glEnable(GL_TEXTURE_GEN_T);
            // ulilizzo le coordinate OGGETTO
            // cioe' le coordnate originali, PRIMA della moltiplicazione per la ModelView
            // in modo che la texture sia "attaccata" all'oggetto, e non "proiettata" su esso
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
            float sz=1.0/(benchMesh.bbmax.Z() - benchMesh.bbmin.Z());
            float ty=1.0/(benchMesh.bbmax.Y() - benchMesh.bbmin.Y());
            float s[4]={0,0,sz,  - benchMesh.bbmin.Z()*sz };
            float t[4]={0,ty,0,  - benchMesh.bbmin.Y()*ty };
            glTexGenfv(GL_S, GL_OBJECT_PLANE, s);
            glTexGenfv(GL_T, GL_OBJECT_PLANE, t);
            glColor3f(1,1,1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
            glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)

        }
    }
    benchMesh.RenderNxV();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPopMatrix();
}