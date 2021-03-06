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
extern bool useEnvmap;

Mesh tabelloneStruct((char *) "meshes/smallBillboardBase.obj");
Mesh tabellonePanel((char *) "meshes/smallBillboardPanel.obj");
Mesh barileMesh((char *) "meshes/Gasoline_barrel.obj");
Mesh streetlampMesh((char *) "meshes/streetlamp.obj");
Mesh pumpBody((char *) "meshes/pumpBody.obj");
Mesh pumpGun((char *) "meshes/pumpGun.obj");
Mesh statuaMesh((char *) "meshes/screaming_statue.obj");
Mesh benchMesh((char *) "meshes/bench.obj");
Mesh puddleMesh((char *) "meshes/puddle.obj");

void drawTabelloneHelper(float posx, float posy, float posz, bool shadow);
void drawBarileHelper(float posx, float posy, float posz, bool shadow);
void drawStreetLampHelper(float posx, float posy, float posz, bool shadow);
void drawLampLight(float posx, float posy, float posz, int lightN);
void drawPumpstationHelper(float posx, float posy, float posz, bool shadow);
void drawStatuaHelper(float posx, float posy, float posz, bool shadow);
void drawBenchHelper(float posx, float posy, float posz, bool shadow);
void castShadow(float posx, float posy, float posz, const float e[], const float n[], const float lPos[],
        void (*fun_ptr) (float, float, float, bool));

void setupPumpBodyMaterial();
void setupPumpGunMaterial();
void setupDefaultMaterial();
void setupStatuaMaterial();

//funzione usata per generare l'ombra
//prende un puntatore alla funzione che si occupa di disegnare l'elemento con o senza ombra
void castShadow(float posx, float posy, float posz, const float e[], const float n[], const float lPos[],
        void (*fun_ptr) (float, float, float, bool))
{
    if(useShadow)
    {
        glPushMatrix();

        glShadowProjection(lPos, e, n);
        glDisable(GL_LIGHTING);
        glColor3f(0.2, 0.2, 0.2);
        fun_ptr(posx, posy, posz, true);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void Tabellone::DrawTabellone(float posx, float posy, float posz)
{
    drawTabelloneHelper(posx, posy, posz, false);

    //disegna l'ombra
    castShadow(posx, posy, posz, e, n, lightPosition, drawTabelloneHelper);

}

void Barile::DrawBarile(float posx, float posy, float posz, float lightPos[])
{
    drawBarileHelper(posx, posy, posz, false);

    //disegna l'ombra
    castShadow(posx, posy, posz, e, n, lightPos, drawBarileHelper);
}

void Streetlamp::DrawStreetlamp(float posx, float posy, float posz)
{
    drawStreetLampHelper(posx, posy, posz, false);
}

void Pumpstation::DrawPumpstation(float posx, float posy, float posz, float lightPos[])
{
    drawPumpstationHelper(posx, posy, posz, false);

    castShadow(posx, posy, posz, e, n, lightPos, drawPumpstationHelper);
}

void Statua::DrawStatua(float posx, float posy, float posz)
{
    drawStatuaHelper(posx, posy, posz, false);

    //disegna l'ombra
    castShadow(posx, posy, posz, e, n, lightPosition, drawStatuaHelper);
}

void Bench::DrawBench(float posx, float posy, float posz)
{
    drawBenchHelper(posx, posy, posz, false);

    //disegna l'ombra
    castShadow(posx, posy, posz, e, n, lightPosition, drawBenchHelper);
}

void drawTabelloneHelper(float posx, float posy, float posz, bool shadow)
{
    glPushMatrix();
    //glScalef(-0.02, 0.02, -0.02);

    glPushMatrix();
    if (!shadow)
        glColor3f(.5, .5, .5);
    glTranslatef(posx, posy, posz);
    tabelloneStruct.RenderNxV();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(posx, posy, posz);
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
            glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
            //crea un quads ed inserisci una texture le cui coordinate corrispondono
            //ai vertici del bounding box del pannello del tabellone
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
            glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
            float sz=1.0/(barileMesh.bbmax.Z() - barileMesh.bbmin.Z());
            float ty=1.0/(barileMesh.bbmax.Y() - barileMesh.bbmin.Y());
            float s[4]={0,0,sz,  - barileMesh.bbmin.Z()*sz };
            float t[4]={0,ty,0,  - barileMesh.bbmin.Y()*ty };
            glTexGenfv(GL_S, GL_OBJECT_PLANE, s);
            glTexGenfv(GL_T, GL_OBJECT_PLANE, t);
            //glColor3f(1,1,1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
            //glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)

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

    float tmpDir[4] = { -2, -25, -2, 1 }; // ultima comp=1 => luce posizionale
    glLightfv(usedLight, GL_SPOT_DIRECTION, tmpDir );

    glLightf (usedLight, GL_SPOT_CUTOFF, 15);
    glLightf (usedLight, GL_SPOT_EXPONENT,3);

    //printf("%f\n", &posz);

    //glLightf(usedLight,GL_CONSTANT_ATTENUATION,0);
    //glLightf(usedLight,GL_LINEAR_ATTENUATION,1);
}

void drawPumpstationHelper(float posx, float posy, float posz, bool shadow)
{
    glPushMatrix();
    glScalef(3, 3, 3);

    glPushMatrix();
    if(!shadow)
    {
        bool isColorMaterialEnabled = glIsEnabled(GL_COLOR_MATERIAL);

        if(!isColorMaterialEnabled)
            glEnable(GL_COLOR_MATERIAL);

        setupPumpBodyMaterial();
        glColor3f(1, 0, .0);
    }
    glTranslatef(posx, posy, posz);
    pumpBody.RenderNxV();
    if(!shadow)
    {
        setupPumpGunMaterial();
        glColor3f(0,0,0);
    }
    pumpGun.RenderNxV();
    setupDefaultMaterial();
    glPopMatrix();

    glPopMatrix();

    //setupDefaultMaterial();
}

//pump gun material: black plastic
void setupPumpGunMaterial()
{
    float mat[4] = {0.0, 0.0, 0.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat);
    mat[0] = 0.1;
    mat[1] = 0.1;
    mat[2] = 0.1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat);
    mat[0] = 0.5;
    mat[1] = 0.5;
    mat[2] = 0.5;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.25*128);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

//pump body material: brass
void setupPumpBodyMaterial()
{
    float mat[4] = {0.329412, 0.223529, 0.027451, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat);
    mat[0] = 0.780392;
    mat[1] = 0.568627;
    mat[2] = 0.113725;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat);
    mat[0] = 0.992157;
    mat[1] = 0.941176;
    mat[2] = 0.807843;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.21794872*128);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void setupDefaultMaterial()
{
    float tmpcol[4] = {1,1,1,  1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tmpcol);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);
}
void drawStatuaHelper(float posx, float posy, float posz, bool shadow)
{
    glPushMatrix();
    glScalef(1, 1, 1);

    glPushMatrix();

    //printf("%d\n", isColorMaterialEnabled);
    if (!shadow)
    {
        bool isColorMaterialEnabled = glIsEnabled(GL_COLOR_MATERIAL);

        if(!isColorMaterialEnabled)
            glEnable(GL_COLOR_MATERIAL);

        setupStatuaMaterial();
        glColor3f(.65, .49, .24);
        if(!useWireframe && useEnvmap)
        {
            // facciamo binding con la texture 1
            glBindTexture(GL_TEXTURE_2D,GOLD);

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord texture S e T
            glEnable(GL_TEXTURE_GEN_T);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
            glColor3f(1,1,1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
            glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)
        }
    }
    glTranslatef(posx, posy, posz);
    glRotated(90, 0, 1, 0);

    statuaMesh.RenderNxV();
    setupDefaultMaterial();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPopMatrix();
}

//statua material: bronze
void setupStatuaMaterial()
{
    float mat[4] = {0.2125, 0.1275, 0.054, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat);
    mat[0] = 0.714;
    mat[1] = 0.4284;
    mat[2] = 0.18144;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat);
    mat[0] = 0.393548;
    mat[1] = 0.271906;
    mat[2] = 0.166721;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.2*128);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void drawBenchHelper(float posx, float posy, float posz, bool shadow)
{
    glPushMatrix();
    glScalef(0.1, 0.1, 0.1);

    glPushMatrix();
    glTranslatef(posx, posy, posz);
    glRotated(90, 0, 1, 0);
    if (!shadow)
    {
        glColor3f(1, 1, 1);
    }
    benchMesh.RenderNxV_TexturedFaces(WOOD, shadow);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPopMatrix();
}

void drawFloorPuddle(float h)
{
    const float S = 2;
    const float H = h;

    if(!useWireframe)
    {
        // inizializzo la texture della strada
        glBindTexture(GL_TEXTURE_2D, ROAD);
        glEnable(GL_TEXTURE_2D);

        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    glBegin(GL_QUADS);
    glColor3f(0.6, 0.6, 0.6); // colore uguale x tutti i quads
    glNormal3f(0,1,0);
    if(!useWireframe) glTexCoord2f(0.0, 0.0);
    glVertex3d(-S, H, -S);
    if(!useWireframe) glTexCoord2f(1.0, 0.0);
    glVertex3d(+S, H, -S);
    if(!useWireframe) glTexCoord2f(1.0, 1.0);
    glVertex3d(+S, H, +S);
    if(!useWireframe) glTexCoord2f(0.0, 1.0);
    glVertex3d(-S, H, +S);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
void Waterpuddle::DrawWaterpuddle(Motorbike mbike, float posx, float posy, float posz)
{
    glPushMatrix();
    glTranslatef(posx, posy, posz);
    drawFloorPuddle(posy);
    glPopMatrix();

    /* Clear; default stencil clears to zero. */
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /* Don't update color or depth. */
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    /* Draw 1 into the stencil buffer. */
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

    glPushMatrix();
    glTranslatef(posx, posy, posz);
    puddleMesh.RenderNxV();
    glPopMatrix();

    /* Re-enable update of color and depth. */
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    /* Now, only render where stencil is set to 1. */
    glStencilFunc(GL_EQUAL, 1, 0xffffffff);  /* draw if ==1 */
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glPushMatrix();

    /* The critical reflection step: Reflect motorbike through the floor
         (the Y=0 plane) to make a reflection. */
    glScalef(1.0, -1.0, 1.0);

    mbike.Render();

    glPopMatrix();
    
    glDisable(GL_STENCIL_TEST);

    /* Draw "top" of floor.  Use blending to blend in reflection. */
    glPushMatrix();
    glTranslatef(posx, posy, posz);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.372549, 0.623529, 0.623529, 0.3);
    puddleMesh.RenderNxV();
    glDisable(GL_BLEND);
    glPopMatrix();
}
