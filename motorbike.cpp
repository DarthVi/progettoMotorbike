// motorbike.cpp
// implementazione dei metodi definiti in motorbike.h

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

#include "motorbike.h"
#include "point3.h"
#include "mesh.h"
#include "utils.h"

// var globale di tipo mesh
Mesh carlinga((char *)"meshes/hayabusa_carlinga.obj"); // chiama il costruttore
Mesh wheelFR1((char *)"meshes/hayabusa_frontWheel.obj");
Mesh wheelBR1((char *)"meshes/hayabusa_backWheel.obj");
Mesh wheelFR2((char *)"meshes/hayabusa_frontWheel_metal.obj");
Mesh wheelBR2((char *)"meshes/hayabusa_backWheel_metal.obj");
Mesh pilot((char *) "meshes/hayabusa_pilot.obj");
Mesh pista((char *)"meshes/pista.obj");

extern bool useEnvmap; // var globale esterna: per usare l'evnrionment mapping
extern bool useHeadlight; // var globale esterna: per usare i fari
extern bool useShadow; // var globale esterna: per generare l'ombra
extern bool useWireframe;
extern float lightPosition[];

// da invocare quando e' stato premuto/rilasciato il tasto numero "keycode"
void Controller::EatKey(int keycode, int* keymap, bool pressed_or_released)
{
  for (int i=0; i<NKEYS; i++){
    if (keycode==keymap[i]) key[i]=pressed_or_released;
  }
}

// da invocare quando e' stato premuto/rilasciato un jbutton
void Controller::Joy(int keymap, bool pressed_or_released)
{
    key[keymap]=pressed_or_released;
}

//material: chrome
void setupChromeMaterial()
{
    float mat[4] = {0.25, 0.25, 0.25, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat);
    mat[0] = 0.4;
    mat[1] = 0.4;
    mat[2] = 0.4;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat);
    mat[0] = 0.774597;
    mat[1] = 0.774597;
    mat[2] = 0.774597;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.6*128);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

//default material
void setupDefMaterial()
{
    float tmpcol[4] = {1,1,1,  1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tmpcol);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);
}

//material: black rubber
void setupBlackRubberMaterial()
{
    float mat[4] = {0.02, 0.02, 0.02, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat);
    mat[0] = 0.01;
    mat[1] = 0.01;
    mat[2] = 0.01;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat);
    mat[0] = 0.4;
    mat[1] = 0.4;
    mat[2] = 0.4;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.078125*128);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

//fissa un muro invisibile che ci permette di non andare oltre lo skybox
void handleInvisibleWall(float *posx, float *posz)
{
    if (*posz >= 70)
        *posz = 70;
    if (*posz <= -70)
        *posz = -70;
    if (*posx >= 70)
        *posx = 70;
    if (*posx <= -70)
        *posx = -70;
}

// DoStep: facciamo un passo di fisica (a delta_t costante)
//
// Indipendente dal rendering.
//
// ricordiamoci che possiamo LEGGERE ma mai SCRIVERE
// la struttura controller da DoStep
void Motorbike::DoStep(){
  // computiamo l'evolversi della macchina
  static int i=5;
  
  float vxm, vym, vzm; // velocita' in spazio motorbike
  
  // da vel frame mondo a vel frame motorbike
  float cosf = cos(facing*M_PI/180.0);
  float sinf = sin(facing*M_PI/180.0);
  vxm = +cosf*vx - sinf*vz;
  vym = vy;
  vzm = +sinf*vx + cosf*vz;
  
  // gestione dello sterzo
  if (controller.key[Controller::LEFT]) sterzo+=velSterzo;
  if (controller.key[Controller::RIGHT]) sterzo-=velSterzo;
  sterzo*=velRitornoSterzo; // ritorno a volante dritto
  
  if (controller.key[Controller::ACC]) vzm-=accMax; // accelerazione in avanti 
  if (controller.key[Controller::DEC]) vzm+=accMax; // accelerazione indietro
  
  // attirti (semplificando)
  vxm*=attritoX;  
  vym*=attritoY;
  vzm*=attritoZ;
  
  // l'orientamento della moto segue quello dello sterzo
  // (a seconda della velocita' sulla z)
  facing = facing - (vzm*grip)*sterzo;
  
  // rotazione mozzo ruote (a seconda della velocita' sulla z)
  float da ; //delta angolo
  da=(360.0*vzm)/(2.0*M_PI*raggioRuotaA);
  mozzoA+=da;
  da=(360.0*vzm)/(2.0*M_PI*raggioRuotaP);
  mozzoP+=da;
  
  // ritorno a vel coord mondo
  vx = +cosf*vxm + sinf*vzm;
  vy = vym;
  vz = -sinf*vxm + cosf*vzm;
  
  // posizione = posizione + velocita * delta t (ma delta t e' costante)
  px+=vx;
  py+=vy;
  pz+=vz;

  //evito che la moto vada oltre lo skybox
  handleInvisibleWall(&px, &pz);
}

void Controller::Init(){
  for (int i=0; i<NKEYS; i++) key[i]=false;
}

void Motorbike::Init(){
  // inizializzo lo stato della moto
  px=pz=facing=0; // posizione e orientamento
  py=0.0;
  
  mozzoA=mozzoP=sterzo=0;   // stato
  vx=vy=vz=0;      // velocita' attuale
  // inizializzo la struttura di controllo
  controller.Init();
  
  //velSterzo=3.4;         // A
  velSterzo=2.4;         // A
  velRitornoSterzo=0.93; // B, sterzo massimo = A*B / (1-B)
  
  accMax = 0.0011;
  
  // attriti: percentuale di velocita' che viene mantenuta
  // 1 = no attrito
  // <<1 = attrito grande
  attritoZ = 0.991;  // piccolo attrito sulla Z (nel senso di rotolamento delle ruote)
  attritoX = 0.8;  // grande attrito sulla X (per non fare slittare la moto)
  attritoY = 1.0;  // attrito sulla y nullo
  
  // Nota: vel max = accMax*attritoZ / (1-attritoZ)
  
  raggioRuotaA = 0.25;
  raggioRuotaP = 0.35;
  
  grip = 0.45; // quanto il facing moto si adegua velocemente allo sterzo

  e[0] = 0;
  e[1] = 0.02;
  e[2] = 0;
  n[0] = 0;
  n[1] = -1;
  n[2] = 0;
}

// attiva una luce di openGL per simulare un faro della moto
void Motorbike::DrawHeadlight(float x, float y, float z, int lightN, bool useHeadlight) const{
  int usedLight=GL_LIGHT1 + lightN;
  
  if(useHeadlight)
  {
  glEnable(usedLight);
  
  float col0[4]= {0.8,0.8,0.0,  1};
  glLightfv(usedLight, GL_DIFFUSE, col0);
  
  float col1[4]= {0.5,0.5,0.0,  1};
  glLightfv(usedLight, GL_AMBIENT, col1);
   
  float tmpPos[4] = {x,y,z,  1}; // ultima comp=1 => luce posizionale
  glLightfv(usedLight, GL_POSITION, tmpPos );
  
  float tmpDir[4] = {0,0,-1,  0}; // ultima comp=1 => luce posizionale
  glLightfv(usedLight, GL_SPOT_DIRECTION, tmpDir );
  
  glLightf (usedLight, GL_SPOT_CUTOFF, 30);
  glLightf (usedLight, GL_SPOT_EXPONENT,5);
  
  glLightf(usedLight,GL_CONSTANT_ATTENUATION,0);
  glLightf(usedLight,GL_LINEAR_ATTENUATION,1);
  }
  else
   glDisable(usedLight);
}


// funzione che disegna tutti i pezzi della moto
// (carlinga, + 2 route)
// (da invocarsi due volte: per la moto, e per la sua ombra)
// (se usecolor e' falso, NON sovrascrive il colore corrente
//  e usa quello stabilito prima di chiamare la funzione)
void Motorbike::RenderAllParts(bool usecolor) const{

  // disegna la carliga e il pilota con una mesh
  glPushMatrix();
  glScalef(-0.15,0.15,-0.15); // patch: riscaliamo la mesh
  glTranslatef(0.0,2.5,0.0); //transalte the veicle up the floor
  if (usecolor) glColor3f(0.5,0.5,0.5);
  pilot.RenderNxV();

  setupChromeMaterial();
  if (usecolor) glColor3f(1,0,0);     // colore rosso, da usare con Lighting
  carlinga.RenderNxV(); // rendering delle mesh carlinga usando normali per vertice

    glPushMatrix();

    glTranslate(wheelFR1.Center() );
    glRotatef(sterzo,0,1,0);
    glRotatef(-mozzoA,1,0,0);
    glTranslate( -wheelFR1.Center() );

    setupDefMaterial();
    if (usecolor) glColor3f(.2,.2,.2);
    wheelFR1.RenderNxF(); // la ruota viene meglio FLAT SHADED - normali per faccia
                            // provare x credere

    setupChromeMaterial();
    if (usecolor) glColor3f(0.9,0.9,0.9);
    wheelFR2.RenderNxV();
    glPopMatrix();

    glPushMatrix();
    glTranslate(wheelBR1.Center() );
    glRotatef(-mozzoA,1,0,0);
    glTranslate( -wheelBR1.Center() );

    setupDefMaterial();
    if (usecolor) glColor3f(.2,.2,.2);
    wheelBR1.RenderNxF();

    setupChromeMaterial();
    if (usecolor) glColor3f(0.9,0.9,0.9);
    wheelBR2.RenderNxV();
    glPopMatrix();

  glPopMatrix();

  setupDefMaterial();
}
 
// disegna a schermo
void Motorbike::Render() const{
  // sono nello spazio mondo

  glPushMatrix();
     
  glTranslatef(px,py,pz);
  glRotatef(facing, 0,1,0);

  // sono nello spazio MOTO
  
  DrawHeadlight(0,0,-1, 0, useHeadlight); // accendi faro

  RenderAllParts(true);
  
  // ombra!
  if(useShadow)
  {
      glPushMatrix();

      glEnable(GL_STENCIL_TEST);
      glShadowProjection(lightPosition, e, n);
      glDisable(GL_LIGHTING);
      glColor3f(0.2, 0.2, 0.2);

      //non disegna l'ombra se ci si trova sulla pozzanghera
      glStencilFunc(GL_NOTEQUAL, 1, 0xffffffff);  /* draw if !=1 */
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

      RenderAllParts(false);
      glEnable(GL_LIGHTING);
      glDisable(GL_STENCIL_TEST);
      glPopMatrix();
  } 
  glPopMatrix(); 
  
  glPopMatrix();
}
