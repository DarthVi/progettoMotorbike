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

// Funzione che prepara tutto per usare un env map
void SetupEnvmapTexture()
{
  // facciamo binding con la texture 1
  glBindTexture(GL_TEXTURE_2D,ENVMAPCARLINGA);
   
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord texture S e T
  glEnable(GL_TEXTURE_GEN_T);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
  glColor3f(1,1,1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
  glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)
}

// funzione che prepara tutto per creare le coordinate texture (s,t) da (x,y,z)
// Mappo l'intervallo [ minY , maxY ] nell'intervallo delle T [0..1]
//     e l'intervallo [ minZ , maxZ ] nell'intervallo delle S [0..1]
void SetupWheelTexture(Point3 min, Point3 max){
  glBindTexture(GL_TEXTURE_2D,LOGO);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  
  // ulilizzo le coordinate OGGETTO
  // cioe' le coordnate originali, PRIMA della moltiplicazione per la ModelView
  // in modo che la texture sia "attaccata" all'oggetto, e non "proiettata" su esso
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
  float sz=1.0/(max.Z() - min.Z());
  float ty=1.0/(max.Y() - min.Y());
  float s[4]={0,0,sz,  - min.Z()*sz };
  float t[4]={0,ty,0,  - min.Y()*ty };
  glTexGenfv(GL_S, GL_OBJECT_PLANE, s); 
  glTexGenfv(GL_T, GL_OBJECT_PLANE, t); 
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
  
  float vxm, vym, vzm; // velocita' in spazio macchina
  
  // da vel frame mondo a vel frame macchina
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
  
  // l'orientamento della macchina segue quello dello sterzo
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

//void drawCube(); // questa e' definita altrove (quick hack)
void drawAxis(); // anche questa

void drawPista () {
        glPushMatrix();
        glColor3f(0.4,0.4,.8);
        glScalef(0.75, 1.0, 0.75);
        glTranslatef(0,0.01,0);
        //pista.RenderNxV();
        pista.RenderNxF();
        glPopMatrix();
}

/*
// diesgna una ruota come due cubi intersecati a 45 gradi
void drawWheel(){
  glPushMatrix();
  glScalef(1, 1.0/sqrt(2.0),  1.0/sqrt(2.0));
  drawCube();
  glRotatef(45,  1,0,0);
  drawCube();
  glPopMatrix();
}
*/

void Controller::Init(){
  for (int i=0; i<NKEYS; i++) key[i]=false;
}

void Motorbike::Init(){
  // inizializzo lo stato della macchina
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
  attritoX = 0.8;  // grande attrito sulla X (per non fare slittare la macchina)
  attritoY = 1.0;  // attrito sulla y nullo
  
  // Nota: vel max = accMax*attritoZ / (1-attritoZ)
  
  raggioRuotaA = 0.25;
  raggioRuotaP = 0.35;
  
  grip = 0.45; // quanto il facing macchina si adegua velocemente allo sterzo

  e[0] = 0;
  e[1] = 0.02;
  e[2] = 0;
  n[0] = 0;
  n[1] = -1;
  n[2] = 0;
}

/*
//vecchio codice ora commentato
// disegna carlinga composta da 1 cubo traslato e scalato
static void drawCarlinga(){
  // disegna carlinga
  
  glColor3f(1,0,0);
  
  // sono nel frame CAR
  glPushMatrix();
  
  // vado al frame pezzo_A
  glScalef(0.25 , 0.14 , 1);
  drawCube();  
  
  // torno al frame CAR
  glPopMatrix();
  
  // vado frame pezzo_B
  glPushMatrix();
  glTranslatef(0,-0.11,-0.95);
  glScalef(0.6, 0.05, 0.15);
  drawCube();
  glPopMatrix();
 
   // vado frame pezzo_C
  glPushMatrix();
  glTranslatef(0,-0.11,0);
  glScalef(0.6, 0.05, 0.3);
  drawCube();
  glPopMatrix();
  
  // vado frame pezzo_D
  glPushMatrix();
  glRotatef(-5,1,0,0);
  glTranslatef(0,+0.2,+0.95);
  glScalef(0.6, 0.05, 0.3);
  drawCube();
  glPopMatrix();
}
*/

// attiva una luce di openGL per simulare un faro della macchina
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


// funzione che disegna tutti i pezzi della macchina
// (carlinga, + 4 route)
// (da invocarsi due volte: per la macchina, e per la sua ombra)
// (se usecolor e' falso, NON sovrascrive il colore corrente
//  e usa quello stabilito prima di chiamare la funzione)
void Motorbike::RenderAllParts(bool usecolor) const{
  
  // drawCarlinga(); // disegna la carliga con pochi parallelepidedi
  
  // disegna la carliga e il pilota con una mesh
  glPushMatrix();
  glScalef(-0.15,0.15,-0.15); // patch: riscaliamo la mesh di 1/10
  glTranslatef(0.0,2.5,0.0); //transalte the veicle up the floor
  if (usecolor) glColor3f(0.5,0.5,0.5);
  //glDisable(GL_TEXTURE_2D);
  pilot.RenderNxV();
  if (!useEnvmap)
  {
    if (usecolor) glColor3f(1,0,0);     // colore rosso, da usare con Lighting
  }
  else {
    if (usecolor) SetupEnvmapTexture();
  }
  carlinga.RenderNxV(); // rendering delle mesh carlinga usando normali per vertice

  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_2D);
  if (usecolor) glEnable(GL_LIGHTING);

    glPushMatrix();

    glDisable(GL_TEXTURE_2D);
    glTranslate(wheelFR1.Center() );
    glRotatef(sterzo,0,1,0);
    glRotatef(-mozzoA,1,0,0);
    glTranslate( -wheelFR1.Center() );

    if (usecolor) glColor3f(.2,.2,.2);
    //if (usecolor) SetupWheelTexture(wheelFR1.bbmin,wheelFR1.bbmax);
    wheelFR1.RenderNxF(); // la ruota viene meglio FLAT SHADED - normali per faccia
                            // provare x credere
    glDisable(GL_TEXTURE_2D);
    if (usecolor) glColor3f(0.9,0.9,0.9);
    wheelFR2.RenderNxV();
    glPopMatrix();

    glPushMatrix();
    glTranslate(wheelBR1.Center() );
    //glRotatef( sterzo,0,1,0);
    glRotatef(-mozzoA,1,0,0);
    glTranslate( -wheelBR1.Center() );

    if (usecolor) glColor3f(.2,.2,.2);
    //if (usecolor) SetupWheelTexture(wheelBR1.bbmin,wheelBR1.bbmax);
    wheelBR1.RenderNxF();
    glDisable(GL_TEXTURE_2D);
    if (usecolor) glColor3f(0.9,0.9,0.9);
    wheelBR2.RenderNxV();
    glPopMatrix();

  glPopMatrix(); 
}
 
// disegna a schermo
void Motorbike::Render() const{
  // sono nello spazio mondo
  
  //drawAxis(); // disegno assi spazio mondo
  glPushMatrix();
     
  glTranslatef(px,py,pz);
  glRotatef(facing, 0,1,0);

  // sono nello spazio MACCHINA
  //drawAxis(); // disegno assi spazio macchina
  
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
