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

#include "tabellone.h"
#include "point3.h"
#include "utils.h"
#include "mesh.h"

void drawTabellone(bool shadow);

extern bool useShadow;
extern float lightPosition[];
extern bool useWireframe;

Mesh tabelloneStruct((char *) "tabelloneStruct.obj");
Mesh tabellonePanel((char *) "tabellonePanel.obj");

void Tabellone::Init()
{
    e[0] = 0;
    e[1] = 0.02;
    e[2] = 0;
    n[0] = 0;
    n[1] = -1;
    n[2] = 0;
}

void Tabellone::DrawTabellone()
{
    drawTabellone(false);

    //disegna l'ombra
    if (useShadow)
    {
        glPushMatrix();

        glShadowProjection(lightPosition, e, n);
        glDisable(GL_LIGHTING);
        glColor3f(0.2, 0.2, 0.2);
        drawTabellone(true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }

}

void drawTabellone(bool shadow)
{
    glPushMatrix();
    glScalef(-0.02, 0.02, -0.02);

    glPushMatrix();
    if (!shadow)
        glColor3f(.5, .5, .5);
    glTranslatef(500, 0, 500);
    tabelloneStruct.RenderNxV();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(500, 0, 500);
    if (!shadow)
    {
        glColor3f(1, 1, 1);
        if (!useWireframe)
        {
            glBindTexture(GL_TEXTURE_2D, 4);
            glEnable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            //crea un quads ed inserisci una texture le cui coordinate corrispondono
            //ai vertici del bounding box del tabellone
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 1.0);
            glVertex2d((tabellonePanel.bbmin.X()), tabellonePanel.bbmin.Y());
            glTexCoord2f(1.0, 1.0);
            glVertex2d((tabellonePanel.bbmax.X()), tabellonePanel.bbmin.Y());
            glTexCoord2f(1.0, 0.0);
            glVertex2d((tabellonePanel.bbmax.X()), tabellonePanel.bbmax.Y());
            glTexCoord2f(0.0, 0.0);
            glVertex2d((tabellonePanel.bbmin.X()), tabellonePanel.bbmax.Y());
            glEnd();
            glDisable(GL_TEXTURE_2D);

        }
    }
    tabellonePanel.RenderNxV();
    glPopMatrix();

    glPopMatrix();
}
