//
// Created by vincenzo on 30/11/19.
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

#include "scene.h"
#include "point3.h"
#include "utils.h"
#include "mesh.h"

extern bool useShadow;
extern float lightPosition[];
extern bool useWireframe;

Mesh tabelloneStruct((char *) "meshes/tabelloneStruct.obj");
Mesh tabellonePanel((char *) "meshes/tabellonePanel.obj");
Mesh barileMesh((char *) "meshes/Gasoline_barrel.obj");
Mesh streetlampMesh((char *) "meshes/streetlamp.obj");
Mesh pumpBody((char *) "meshes/pumpBody.obj");
Mesh pumpGun((char *) "meshes/pumpGun.obj");
Mesh statuaMesh((char *) "meshes/screaming_statue.obj");
Mesh benchMesh((char *) "meshes/bench.obj");

void drawTabelloneHelper(bool shadow);
void drawBarileHelper(float posx, float posy, float posz, bool shadow);
void drawStreetLampHelper(float posx, float posy, float posz, bool shadow);
void drawLampLight(float posx, float posy, float posz, int lightN);
void drawPumpstationHelper(float posx, float posy, float posz, float shadow);
void drawStatuaHelper(float posx, float posy, float posz, bool shadow);
void drawBenchHelper(float posx, float posy, float posz, bool shadow);

void Tabellone::DrawTabellone()
{
    drawTabelloneHelper(false);

    //disegna l'ombra
    if (useShadow)
    {
        glPushMatrix();

        glShadowProjection(lightPosition, e, n);
        glDisable(GL_LIGHTING);
        glColor3f(0.2, 0.2, 0.2);
        drawTabelloneHelper(true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }

}

void Barile::DrawBarile(float posx, float posy, float posz, float lightPos[])
{
    drawBarileHelper(posx, posy, posz, false);

    //disegna l'ombra
    if (useShadow)
    {
        glPushMatrix();

        glShadowProjection(lightPos, e, n);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2, 0.2, 0.2);
        drawBarileHelper(posx, posy, posz, true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void Streetlamp::DrawStreetlamp(float posx, float posy, float posz)
{
    drawStreetLampHelper(posx, posy, posz, false);

    //disegna l'ombra
//    if (useShadow)
//    {
//        glPushMatrix();
//
//        glShadowProjection(lightPosition, e, n);
//        glDisable(GL_LIGHTING);
//        glColor3f(0.2, 0.2, 0.2);
//        drawStreetLampHelper(posx, posy, posz, true);
//        glEnable(GL_LIGHTING);
//        glPopMatrix();
//    }
}

void Pumpstation::DrawPumpstation(float posx, float posy, float posz, float lightPos[])
{
    drawPumpstationHelper(posx, posy, posz, false);

    if(useShadow)
    {
        glPushMatrix();

        glShadowProjection(lightPos, e, n);
        glDisable(GL_LIGHTING);
        glColor3f(0.2, 0.2, 0.2);
        drawPumpstationHelper(posx, posy, posz, true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void Statua::DrawStatua(float posx, float posy, float posz)
{
    drawStatuaHelper(posx, posy, posz, false);

    //disegna l'ombra
    if (useShadow)
    {
        glPushMatrix();

        glShadowProjection(lightPosition, e, n);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2, 0.2, 0.2);
        drawStatuaHelper(posx, posy, posz, true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void Bench::DrawBench(float posx, float posy, float posz)
{
    drawBenchHelper(posx, posy, posz, false);

    //disegna l'ombra
    if (useShadow)
    {
        glPushMatrix();

        glShadowProjection(lightPosition, e, n);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2, 0.2, 0.2);
        drawBenchHelper(posx, posy, posz, true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void drawTabelloneHelper(bool shadow)
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
            glBindTexture(GL_TEXTURE_2D, MYPHOTO);
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

void drawBarileHelper(float posx, float posy, float posz, bool shadow)
{
    glPushMatrix();
    glScalef(0.1, 0.1, 0.1);

    glPushMatrix();
    if (!shadow)
        glColor3f(.5, .5, .5);
    glTranslatef(posx, posy, posz);
    if (!shadow)
    {
        glColor3f(1, 1, 1);
        if (!useWireframe)
        {
            glBindTexture(GL_TEXTURE_2D,RUST);

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord texture S e T
            glEnable(GL_TEXTURE_GEN_T);
            // ulilizzo le coordinate OGGETTO
            // cioe' le coordnate originali, PRIMA della moltiplicazione per la ModelView
            // in modo che la texture sia "attaccata" all'oggetto, e non "proiettata" su esso
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
            float sz=1.0/(barileMesh.bbmax.Z() - barileMesh.bbmin.Z());
            float ty=1.0/(barileMesh.bbmax.Y() - barileMesh.bbmin.Y());
            float s[4]={0,0,sz,  - barileMesh.bbmin.Z()*sz };
            float t[4]={0,ty,0,  - barileMesh.bbmin.Y()*ty };
            glTexGenfv(GL_S, GL_OBJECT_PLANE, s);
            glTexGenfv(GL_T, GL_OBJECT_PLANE, t);
            glColor3f(1,1,1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
            glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)

        }
    }
    barileMesh.RenderNxV();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPopMatrix();
}

void drawStreetLampHelper(float posx, float posy, float posz, bool shadow)
{
    glPushMatrix();
    glScalef(1, 1, 1);

    glPushMatrix();
    if (!shadow)
        glColor3f(.1, .1, .1);
    glTranslatef(posx, posy, posz);
    streetlampMesh.RenderNxV();
    if(!shadow)
        drawLampLight(0, 10, 0, 1);
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

    float tmpPos[4] = {posx, posy, posz,  1}; // ultima comp=1 => luce posizionale
    glLightfv(usedLight, GL_POSITION, tmpPos );

    float tmpDir[4] = { -2, -25, -2, 0 }; // ultima comp=1 => luce posizionale
    glLightfv(usedLight, GL_SPOT_DIRECTION, tmpDir );

    glLightf (usedLight, GL_SPOT_CUTOFF, 15);
    glLightf (usedLight, GL_SPOT_EXPONENT,3);

    //printf("%f\n", &posz);

    //glLightf(usedLight,GL_CONSTANT_ATTENUATION,0);
    //glLightf(usedLight,GL_LINEAR_ATTENUATION,1);
}

void drawPumpstationHelper(float posx, float posy, float posz, float shadow)
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

void drawStatuaHelper(float posx, float posy, float posz, bool shadow)
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

void drawBenchHelper(float posx, float posy, float posz, bool shadow)
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
            float tx=1.0 / (benchMesh.bbmax.X() - benchMesh.bbmin.X());
            float s[4]={0,0,sz,  - benchMesh.bbmin.Z()*sz };
            float t[4]={0,ty,0,  - benchMesh.bbmin.Y()*ty };
            float r[4] = { tx, 0, 0, -benchMesh.bbmin.X() * tx };
            glTexGenfv(GL_S, GL_OBJECT_PLANE, s);
            glTexGenfv(GL_T, GL_OBJECT_PLANE, t);
            glTexGenfv(GL_R, GL_OBJECT_PLANE, r);
            glColor3f(1,1,1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
            glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)

        }
    }
    benchMesh.RenderNxV();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPopMatrix();
}
