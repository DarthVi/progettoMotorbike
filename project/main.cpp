#include <math.h>

#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL2_ttf.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>

#endif

#include <ctime>
#include <cstdio>
#include <cstring>

#include "utils.h"
#include "motorbike.h"
#include "scene.h"
#include "waypoint.h"

#define CAMERA_BACK_CAR 0
#define CAMERA_TOP_FIXED 1
#define CAMERA_TOP_CAR 2
#define CAMERA_PILOT 3
#define CAMERA_MOUSE 4
#define CAMERA_FOLLOW 5
#define CAMERA_TYPE_MAX 6

#define PI 3.14159265
#define TIMEAVAILABLE 181


float viewAlpha=20, viewBeta=40; // angoli che definiscono la vista
float eyeDist=5.0; // distanza dell'occhio dall'origine
int scrH=750, scrW=750; // altezza e larghezza viewport (in pixels)
bool useWireframe=false;
bool useEnvmap=true;
bool useHeadlight=false;
bool useShadow=true;
int cameraType=0;
bool simpleMinimap = true;
// setto la posizione luce
float lightPosition[4] = {0,20,2,  1}; // ultima comp=0 => luce direzionale
int punteggio = 0;
int radarRadius = 110;

Motorbike motorbike; // la nostra moto
Tabellone tabellone;
Streetlamp streetlamp;
Barile barile;
Pumpstation pumpstation;
Waypoint waypoint;
Statua statua;
Bench bench;
Waterpuddle pozzanghera;
int nstep=0; // numero di passi di FISICA fatti fin'ora
const int PHYS_SAMPLING_STEP=10; // numero di millisec che un passo di fisica simula
double initTime;
double currentTime;
double finalTime;

// Frames Per Seconds
const int fpsSampling = 3000; // lunghezza intervallo di calcolo fps
float fps=0; // valore di fps dell'intervallo precedente
int fpsNow=0; // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
Uint32 timeLastInterval=0; // quando e' cominciato l'ultimo intervallo

enum textquality {solid, shaded, blended};
uint font_id = -1;

void SDL_GL_DrawText(TTF_Font *font,char fgR, char fgG, char fgB, char fgA,
                     char bgR, char bgG, char bgB, char bgA, char *text, int x, int y,
                     enum textquality quality)
{
    SDL_Color tmpfontcolor = {fgR,fgG,fgB,fgA};
    SDL_Color tmpfontbgcolor = {bgR, bgG, bgB, bgA};\
	SDL_Surface *initial;
    SDL_Surface *intermediary;
    SDL_Rect location;
    int w,h;

/* Use SDL_TTF to render our text */
    initial=NULL;
    if (quality == solid)
        initial = TTF_RenderText_Solid(font, text, tmpfontcolor);
    else
    if (quality == shaded)
        initial = TTF_RenderText_Shaded(font, text, tmpfontcolor, tmpfontbgcolor);
    else
    if (quality == blended)
        initial = TTF_RenderText_Blended(font, text, tmpfontcolor);

/* Convert the rendered text to a known format */
    w = initial->w;
    h = initial->h;

    intermediary = SDL_CreateRGBSurface(0, w, h, 32,
                                        0x000000ff,0x0000ff00, 0x00ff0000,0xff000000);

    SDL_BlitSurface(initial, 0, intermediary, 0);

/* Tell GL about our new texture */
    glBindTexture(GL_TEXTURE_2D, font_id);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, intermediary->pixels );


/* GL_NEAREST looks horrible, if scaled... */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

/* prepare to render our texture */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    if ( initial != NULL )
    {
        location.x = x;
        location.y = y;
    }

/* Draw a quad at location */
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(location.x    , location.y);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(location.x + w, location.y);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(location.x + w, location.y + h);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(location.x    , location.y + h);
    glEnd();

/*Draw a boundary button */
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f((GLfloat)location.x-1, (GLfloat)location.y-1);
    glVertex2f((GLfloat)location.x + w +1, (GLfloat)location.y-1);
    glVertex2f((GLfloat)location.x + w +1, (GLfloat)location.y + h +1);
    glVertex2f((GLfloat)location.x-1    , (GLfloat)location.y + h +1);
    glVertex2f((GLfloat)location.x-1, (GLfloat)location.y-1);
    glEnd();

/* Bad things happen if we delete the texture before it finishes */
    glFinish();

/* return the deltas in the unused w,h part of the rect */
    location.w = initial->w;
    location.h = initial->h;

/* Clean up */
    glDisable(GL_TEXTURE_2D);
    SDL_FreeSurface(initial);
    SDL_FreeSurface(intermediary);
//	glColor3f(0.0f, 0.0f, 0.0f);
}

char* convertIntToString(int num)
{
    int length = snprintf( NULL, 0, "%d", num );
    char* str = (char*) malloc( sizeof(char) * (length + 1) );
    snprintf( str, length + 1, "%d", num );
    return str;
}

void HUD_RenderText(TTF_Font *font, char* message, int posx, int posy)
{
    SDL_GL_DrawText(font, 0, 0, 0, 0, (char)255, (char)255, (char)255, (char)255,
            message, posx, posy, shaded);
}

// setta le matrici di trasformazione in modo
// che le coordinate in spazio oggetto siano le coord 
// del pixel sullo schemo
void  SetCoordToPixel(){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-1,-1,0);
  glScalef(2.0/scrW, 2.0/scrH, 1);
}

bool LoadTexture(int textbind,char *filename){
GLenum texture_format;

  SDL_Surface *s = IMG_Load(filename);
  if (!s) return false;

  if (s->format->BytesPerPixel == 4){     // contiene canale alpha
    if (s->format->Rmask == 0x000000ff){
      texture_format = GL_RGBA;
    }
    else{
      texture_format = GL_BGRA;
    }
  } else if (s->format->BytesPerPixel == 3){     // non contiene canale alpha
     if (s->format->Rmask == 0x000000ff)
       texture_format = GL_RGB;
     else
       texture_format = GL_BGR;
    } else {
        printf("[ERROR] the image is not truecolor\n");
        exit(1);
      }

  glBindTexture(GL_TEXTURE_2D, textbind);
  gluBuild2DMipmaps(
    GL_TEXTURE_2D, 
    3,
    s->w, s->h, 
    texture_format,
    GL_UNSIGNED_BYTE,
    s->pixels
  );
  glTexParameteri(
  GL_TEXTURE_2D, 
  GL_TEXTURE_MAG_FILTER,
  GL_LINEAR ); 
  glTexParameteri(
  GL_TEXTURE_2D, 
  GL_TEXTURE_MIN_FILTER,
  GL_LINEAR_MIPMAP_LINEAR ); 
  return true;
}

// disegna gli assi nel sist. di riferimento
void drawAxis(){
  const float K=0.10;
  glColor3f(0,0,1);
  glBegin(GL_LINES);
    glVertex3f( -1,0,0 );
    glVertex3f( +1,0,0 );

    glVertex3f( 0,-1,0 );
    glVertex3f( 0,+1,0 );

    glVertex3f( 0,0,-1 );
    glVertex3f( 0,0,+1 );
  glEnd();
  
  glBegin(GL_TRIANGLES);
    glVertex3f( 0,+1  ,0 );
    glVertex3f( K,+1-K,0 );
    glVertex3f(-K,+1-K,0 );

    glVertex3f( +1,   0, 0 );
    glVertex3f( +1-K,+K, 0 );
    glVertex3f( +1-K,-K, 0 );

    glVertex3f( 0, 0,+1 );
    glVertex3f( 0,+K,+1-K );
    glVertex3f( 0,-K,+1-K );
  glEnd();

}

void drawSphere(double r, int lats, int longs) {
int i, j;
  for(i = 0; i <= lats; i++) {
     double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
     double z0  = sin(lat0);
     double zr0 =  cos(lat0);
   
     double lat1 = M_PI * (-0.5 + (double) i / lats);
     double z1 = sin(lat1);
     double zr1 = cos(lat1);
    
     glBegin(GL_QUAD_STRIP);
     for(j = 0; j <= longs; j++) {
        double lng = 2 * M_PI * (double) (j - 1) / longs;
        double x = cos(lng);
        double y = sin(lng);
    
//le normali servono per l'EnvMap
        glNormal3f(x * zr0, y * zr0, z0);
        glVertex3f(r * x * zr0, r * y * zr0, r * z0);
        glNormal3f(x * zr1, y * zr1, z1);
        glVertex3f(r * x * zr1, r * y * zr1, r * z1);
     }
     glEnd();
  }
}

void drawFloor()
{
  const float S=100; // size
  const float H=0;   // altezza
  const int K=150; //disegna K x K quads

  if(!useWireframe)
  {
      // inizializzo la texture della strada
      glBindTexture(GL_TEXTURE_2D, ROAD);
      glEnable(GL_TEXTURE_2D);

      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }
  
  // disegna KxK quads, ad ogni quad viene applicata la texture del terreno
  glBegin(GL_QUADS);
    glColor3f(0.6, 0.6, 0.6); // colore uguale x tutti i quads
    glNormal3f(0,1,0);       // normale verticale uguale x tutti
    for (int x=0; x<K; x++) 
    for (int z=0; z<K; z++) {
      float x0=-S + 2*(x+0)*S/K;
      float x1=-S + 2*(x+1)*S/K;
      float z0=-S + 2*(z+0)*S/K;
      float z1=-S + 2*(z+1)*S/K;
      if(!useWireframe) glTexCoord2f(0.0, 0.0);
      glVertex3d(x0, H, z0);
      if(!useWireframe) glTexCoord2f(1.0, 0.0);
      glVertex3d(x1, H, z0);
      if(!useWireframe) glTexCoord2f(1.0, 1.0);
      glVertex3d(x1, H, z1);
      if(!useWireframe) glTexCoord2f(0.0, 1.0);
      glVertex3d(x0, H, z1);
    }
  glEnd();
}

// setto la posizione della camera
void setCamera(){

        double px = motorbike.px;
        double py = motorbike.py;
        double pz = motorbike.pz;
        double angle = motorbike.facing;
        double cosf = cos(angle*M_PI/180.0);
        double sinf = sin(angle*M_PI/180.0);
        double camd, camh, ex, ey, ez, cx, cy, cz;
        double cosff, sinff;

// controllo la posizione della camera a seconda dell'opzione selezionata
        switch (cameraType) {
        case CAMERA_BACK_CAR:
                camd = 2.5;
                camh = 1.0;
                ex = px + camd*sinf;
                ey = py + camh;
                ez = pz + camd*cosf;
                cx = px - camd*sinf;
                cy = py + camh;
                cz = pz - camd*cosf;
                gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
                break;
        case CAMERA_TOP_FIXED:
                camd = 0.5;
                camh = 0.55;
                angle = motorbike.facing + 40.0;
                cosff = cos(angle*M_PI/180.0);
                sinff = sin(angle*M_PI/180.0);
                ex = px + camd*sinff;
                ey = py + camh;
                ez = pz + camd*cosff;
                cx = px - camd*sinf;
                cy = py + camh;
                cz = pz - camd*cosf;
                gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
                break;
        case CAMERA_TOP_CAR:
                camd = 2.5;
                camh = 1.0;
                ex = px + camd*sinf;
                ey = py + camh;
                ez = pz + camd*cosf;
                cx = px - camd*sinf;
                cy = py + camh;
                cz = pz - camd*cosf;
                gluLookAt(ex,ey+5,ez,cx,cy,cz,0.0,1.0,0.0);
                break;
        case CAMERA_PILOT:
                camd = 0.02;
                camh = 1.0;
                ex = px + camd*sinf;
                ey = py + camh;
                ez = pz + camd*cosf;
                cx = px - camd*sinf;
                cy = py + camh;
                cz = pz - camd*cosf;
                gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
                break;
        case CAMERA_MOUSE:
                glTranslatef(0,0,-eyeDist);
                glRotatef(viewBeta,  1,0,0);
                glRotatef(viewAlpha, 0,1,0);
                break;
        case CAMERA_FOLLOW:
            gluLookAt(motorbike.px+3, motorbike.py+3, motorbike.pz+3, motorbike.px, motorbike.py, motorbike.pz, 0.0, 1.0, 0.0);
            break;
/*
printf("%f %f %f\n",viewAlpha,viewBeta,eyeDist);
                ex=eyeDist*cos(viewAlpha)*sin(viewBeta);
                ey=eyeDist*sin(viewAlpha)*sin(viewBeta);
                ez=eyeDist*cos(viewBeta);
                cx = px - camd*sinf;
                cy = py + camh;
                cz = pz - camd*cosf;
                gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
*/
                break;
        }
}

void drawSky() {
int H = 100;

  if (useWireframe) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0,0,0);
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    drawSphere(100.0, 20, 20);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glColor3f(1,1,1);
    glEnable(GL_LIGHTING);
  }
  else
  {
        glBindTexture(GL_TEXTURE_2D,SKY);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
        glColor3f(1,1,1);
        glDisable(GL_LIGHTING);

        drawSphere(100.0, 20, 20);

        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
  }

}

void HUD_DrawMinimap()
{
    float moto_px, moto_pz, waypoint_px, waypoint_pz;

    //calcolo le coordinate degli oggetti sulla minimappa
    moto_px = (motorbike.px) + 70;
    moto_pz = (motorbike.pz) + 50 + scrH - 120;
    //printf("X: %f\n", motorbike.px);
    //printf("Z: %f\n", motorbike.pz);
    waypoint_px = (waypoint.pos_x) + 70;
    waypoint_pz = (waypoint.pos_z) + 50 + scrH - 120;

    // disegnamo il background della mappa
    // Uso GL_TRIANGLE_FAN per formare un cerchio
    glPushMatrix();
    glTranslatef(+30, scrH-30, 0);
    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(255, 255, 255);  // Bianco
    glVertex2f(0.0f, 0.0f);       // Centro del cerchio
    int numSegments = 100;
    GLfloat angle;
    for (int i = 0; i <= numSegments; i++) { // ultimo vertice uguale al primo vertice
        angle = i * 2.0f * PI / numSegments;  // 360 gradi per tutti i segmenti
        glVertex2f(cos(angle) * radarRadius, sin(angle) * radarRadius);
    }
    glEnd();
    glPopMatrix();

    //disegnamo la cornice della mappa
    glPushMatrix();
    glTranslatef(+30, scrH-30, 0);
    glBegin(GL_LINE_LOOP);
    glColor3ub(0, 0, 0);  // Black
    numSegments = 100;
    angle;
    for (int i = 0; i <= numSegments; i++) { // Last vertex same as first vertex
        angle = i * 2.0f * PI / numSegments;  // 360 deg for all segments
        glVertex2f(cos(angle) * radarRadius, sin(angle) * radarRadius);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-35,+40,0);
    /* disegna l'indicatore della moto in verde */
    glColor3ub(0, 255, 0);
    glPointSize(5);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex2d(moto_px,moto_pz);
    glEnd();
    glDisable(GL_POINT_SMOOTH);


    /* disegno il waypoint sulla minimappa in rosso*/
    glColor3ub(255, 0, 0);
    glPointSize(5);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex2d(waypoint_px,waypoint_pz);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
    glPopMatrix();
}

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win, TTF_Font *font){
  
  // un frame in piu'!!!
  fpsNow++;
  
  glLineWidth(3); // linee larghe
     
  // settiamo il viewport
  glViewport(0,0, scrW, scrH);
  
  // colore sfondo = bianco
  glClearColor(1,1,1,1);


  // settiamo la matrice di proiezione
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 70, //fovy,
		((float)scrW) / scrH,//aspect Y/X,
		0.2,//distanza del NEAR CLIPPING PLANE in coordinate vista
		1000  //distanza del FAR CLIPPING PLANE in coordinate vista
  );

  glMatrixMode( GL_MODELVIEW ); 
  glLoadIdentity();
  
  // riempe tutto lo screen buffer di pixel color sfondo
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  //drawAxis(); // disegna assi frame VISTA

  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition );
  float ambientLight[] = {0.2, 0.2, 0.2, 1};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);


  
  // settiamo matrice di vista
  setCamera();


  static float tmpcol[4] = {1,1,1,  1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);
  
  glEnable(GL_LIGHTING);



  pozzanghera.DrawWaterpuddle(motorbike,-10,0.02,-5);
  drawSky(); // disegna il cielo come sfondo
  drawFloor(); // disegna il suolo
  //drawPista(); // disegna la pista
  tabellone.DrawTabellone(-15, +11.35, -12); //disegna il tabellone
  streetlamp.DrawStreetlamp(0, 0, -10);
  float lampLightPosition[4] = {0, 10, -10, 1}; //posizione della luce della streetlamp
  barile.DrawBarile(-100, +5.3, -100, lightPosition);
  barile.DrawBarile(-120, +5.3, -100, lightPosition);
  pumpstation.DrawPumpstation(0,+0.25,-3.8, lampLightPosition);
  barile.DrawBarile(-10, +5.3, -115, lampLightPosition);
  statua.DrawStatua(-15,+1.3,0);
  bench.DrawBench(-150,0,-20);

  motorbike.Render(); // disegna la moto
  waypoint.Render(motorbike.px, motorbike.py, motorbike.pz);
	
  // attendiamo la fine della rasterizzazione di 
  // tutte le primitive mandate 
  
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
 
// disegnamo i fps (frame x sec) come una barra a sinistra.
// (vuota = 0 fps, piena = 100 fps)
  SetCoordToPixel();

  glBegin(GL_QUADS);
  float y=scrH*fps/100;
  float ramp=fps/100;
  glColor3f(1-ramp,0,ramp);
  glVertex2d(10,0);
  glVertex2d(10,y);
  glVertex2d(0,y);
  glVertex2d(0,0);
  glEnd();

  glLineWidth(1);
  //disegna la minimap in basso a destra
  glPushMatrix();
  glTranslatef(+scrW-150, -scrH+150, 0);
  HUD_DrawMinimap();
  glPopMatrix();


  char *str = convertIntToString(punteggio);
  char *text = (char *) (malloc(sizeof(char) * (8 + strlen(str) + 1)));
  strcpy(text, "Punti: ");
  HUD_RenderText(font, strcat(text, str), scrW-200, scrH-50);
  free(str);
  free(text);

  str = convertIntToString((int)fps);
  char *text2 = (char *) (malloc(sizeof(char) * (6 + strlen(str) + 1)));;
  strcpy(text2, "FPS: ");
  HUD_RenderText(font, strcat(text2, str), +50, scrH - 50);
  free(str);
  free(text2);

  int remainingTime = TIMEAVAILABLE - (currentTime / CLOCKS_PER_SEC);
  str = convertIntToString(remainingTime);
  char *text3 = (char *) (malloc(sizeof(char) * (8 + strlen(str) + 1)));
  strcpy(text3, "Tempo: ");
  HUD_RenderText(font, strcat(text3, str), +50, scrH - 90);
  free(str);
  free(text3);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  
  
  glFinish();
  // ho finito: buffer di lavoro diventa visibile
  SDL_GL_SwapWindow(win);
}

void redraw(){
  // ci automandiamo un messaggio che (s.o. permettendo)
  // ci fara' ridisegnare la finestra
  SDL_Event e;
  e.type=SDL_WINDOWEVENT;
  e.window.event=SDL_WINDOWEVENT_EXPOSED;
  SDL_PushEvent(&e);
}

void endGame(SDL_Window *win, TTF_Font *font)
{
    glViewport(0, 0, scrW, scrH);

    //colore sfondo
    glClearColor(0,0,0,1);

    // riempe tutto lo screen buffer col colore dello sfondo
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetCoordToPixel();

    glLineWidth(1);

    char* points = convertIntToString(punteggio);

    char *ptStr = (char *) (malloc(sizeof(char) * (20 + strlen(points) + 1)));
    strcpy(ptStr, "Punti totalizzati: ");
    char gameOverMsg[] = "GAME OVER";
    char continueMsg[] = "Premere un tasto per chiudere l'applicazione";


    HUD_RenderText(font, gameOverMsg, scrW / 2 - 75, scrH / 2 + 100);

    HUD_RenderText(font, strcat(ptStr, points), scrW / 2 - 100, scrH / 2 + 50);
    free(points);
    free(ptStr);

    HUD_RenderText(font, continueMsg, scrW / 2 -250, scrH / 2);

    glFinish();

    SDL_GL_SwapWindow(win);

}

int handleTime(SDL_Window *win, double currTime, TTF_Font *font)
{
    int done = 0;
    bool pressed = false;
    Uint32 windowID;
    SDL_Event e;

    //se sono passati i secondi disponibili
    if(currTime / CLOCKS_PER_SEC >= TIMEAVAILABLE)
    {
        done = 1;

        while(!pressed)
        {
            if (SDL_PollEvent(&e))
            {
                switch (e.type)
                {
                    case SDL_KEYDOWN:
                        pressed = true;
                        break;
                    case SDL_WINDOWEVENT:
                        windowID = SDL_GetWindowID(win);
                        if (e.window.windowID == windowID)
                        {
                            switch (e.window.event)
                            {
                                case SDL_WINDOWEVENT_SIZE_CHANGED:
                                {
                                    scrW = e.window.data1;
                                    scrH = e.window.data2;
                                    glViewport(0, 0, scrW,
                                               scrH);
                                    break;
                                }
                            }
                        }
                }
            } else
            { // disegno la schermata di fine gioco
                endGame(win, font);
            }
        }
    } else{
        //tempo libero
    }

    return done;
}

int main(int argc, char* argv[])
{
SDL_Window *win;
SDL_GLContext mainContext;
Uint32 windowID;
SDL_Joystick *joystick;
static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s};

  // inizializzazione di SDL
  SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    if (TTF_Init() < 0)
    {
        fprintf(stderr, "Impossibile inizializzare TTF: %s\n", SDL_GetError());
        SDL_Quit();
        return (2);
    }
    //Carico il font per la scrittura
    TTF_Font *font;
    font = TTF_OpenFont("FreeSans.ttf", 29);
    if (font == NULL)
    {
        fprintf(stderr, "Impossibile caricare il font.\n");
    }

  SDL_JoystickEventState(SDL_ENABLE);
  joystick = SDL_JoystickOpen(0);

  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 ); 
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 );
  glClearStencil(0x0);

  // facciamo una finestra di scrW x scrH pixels
  win=SDL_CreateWindow(argv[0], 0, 0, scrW, scrH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

  //Create our opengl context and attach it to our window
  mainContext=SDL_GL_CreateContext(win);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE); // opengl, per favore, rinormalizza le normali 
                          // prima di usarle
  //glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW); // consideriamo Front Facing le facce ClockWise
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_POLYGON_OFFSET_FILL); // caro openGL sposta i 
                                    // frammenti generati dalla
                                    // rasterizzazione poligoni
  glPolygonOffset(1,1);             // indietro di 1

  if (!LoadTexture(SKY,(char *)"textures/sky_ok.jpg")) return -1;
  if (!LoadTexture(ROAD, (char *) "textures/roadTS3.jpg")) return 0;
  if (!LoadTexture(MYPHOTO, (char *) "textures/me.jpg")) return 0;
  if (!LoadTexture(RUST, (char *) "textures/rust.jpg")) return 0;
  if (!LoadTexture(MARBLE, (char *) "textures/marble.jpg")) return 0;
  if (!LoadTexture(GOLD, (char *) "textures/fresnelgoldmap.jpg")) return 0;
  if (!LoadTexture(WOOD, (char *) "textures/wood5.jpg")) return 0;

  initTime = clock();
 
  bool done=0;
  bool pressed = false;
  while (!done) {
    
    SDL_Event e;
    
    // guardo se c'e' un evento:
    if (SDL_PollEvent(&e)) {
     // se si: processa evento
     switch (e.type) {
      case SDL_KEYDOWN:
        motorbike.controller.EatKey(e.key.keysym.sym, keymap , true);
        if (e.key.keysym.sym==SDLK_F1) cameraType=(cameraType+1)%CAMERA_TYPE_MAX;
        if (e.key.keysym.sym==SDLK_F2) useWireframe=!useWireframe;
        if (e.key.keysym.sym==SDLK_F3) useEnvmap=!useEnvmap;
        if (e.key.keysym.sym==SDLK_F4) useHeadlight=!useHeadlight;
        if (e.key.keysym.sym==SDLK_F5) useShadow=!useShadow;
        if (e.key.keysym.sym==SDLK_F6) simpleMinimap=!simpleMinimap;
        break;
      case SDL_KEYUP:
        motorbike.controller.EatKey(e.key.keysym.sym, keymap , false);
        break;
      case SDL_QUIT:  
          done=1;   break;
      case SDL_WINDOWEVENT:
         // dobbiamo ridisegnare la finestra
          if (e.window.event==SDL_WINDOWEVENT_EXPOSED)
            rendering(win, font);
          else{
           windowID = SDL_GetWindowID(win);
           if (e.window.windowID == windowID)  {
             switch (e.window.event)  {
                  case SDL_WINDOWEVENT_SIZE_CHANGED:  {
                     scrW = e.window.data1;
                     scrH = e.window.data2;
                     glViewport(0,0,scrW,scrH);
                     rendering(win, font);
                     //redraw(); // richiedi ridisegno
                     break;
                  }
             }
           }
         }
      break;
        
      case SDL_MOUSEMOTION:
        if (e.motion.state & SDL_BUTTON(1) & cameraType==CAMERA_MOUSE) {
          viewAlpha+=e.motion.xrel;
          viewBeta +=e.motion.yrel;
//          if (viewBeta<-90) viewBeta=-90;
          if (viewBeta<+5) viewBeta=+5; //per non andare sotto la macchina
          if (viewBeta>+90) viewBeta=+90;
          // redraw(); // richiedi un ridisegno (non c'e' bisongo: si ridisegna gia' 
	               // al ritmo delle computazioni fisiche)
        }
        break; 
       
     case SDL_MOUSEWHEEL:
       if (e.wheel.y < 0 ) {
         // avvicino il punto di vista (zoom in)
         eyeDist=eyeDist*0.9;
         if (eyeDist<1) eyeDist = 1;
       };
       if (e.wheel.y > 0 ) {
         // allontano il punto di vista (zoom out)
         eyeDist=eyeDist/0.9;
       };
     break;

     case SDL_JOYAXISMOTION: /* Handle Joystick Motion */
        if( e.jaxis.axis == 0)
         {
            if ( e.jaxis.value < -3200  )
             {
              motorbike.controller.Joy(0 , true);
              motorbike.controller.Joy(1 , false);
//	      printf("%d <-3200 \n",e.jaxis.value);
             }
            if ( e.jaxis.value > 3200  )
            {
              motorbike.controller.Joy(0 , false);
              motorbike.controller.Joy(1 , true);
//	      printf("%d >3200 \n",e.jaxis.value);
            }
            if ( e.jaxis.value >= -3200 && e.jaxis.value <= 3200 )
             {
              motorbike.controller.Joy(0 , false);
              motorbike.controller.Joy(1 , false);
//	      printf("%d in [-3200,3200] \n",e.jaxis.value);
             }            
	    rendering(win, font);
            //redraw();
        }
        break;
      case SDL_JOYBUTTONDOWN: /* Handle Joystick Button Presses */
        if ( e.jbutton.button == 0 )
        {
           motorbike.controller.Joy(2 , true);
//	   printf("jbutton 0\n");
        }
        if ( e.jbutton.button == 2 )
        {
           motorbike.controller.Joy(3 , true);
//	   printf("jbutton 2\n");
        }
        break; 
      case SDL_JOYBUTTONUP: /* Handle Joystick Button Presses */
           motorbike.controller.Joy(2 , false);
           motorbike.controller.Joy(3 , false);
        break; 
     }
    } else {
      // nessun evento: siamo IDLE
      
      Uint32 timeNow=SDL_GetTicks(); // che ore sono?
      
      if (timeLastInterval+fpsSampling<timeNow) {
        fps = 1000.0*((float)fpsNow) /(timeNow-timeLastInterval);
        fpsNow=0;
        timeLastInterval = timeNow;
      }
      
      bool doneSomething=false;
      int guardia=0; // sicurezza da loop infinito
      
      // finche' il tempo simulato e' rimasto indietro rispetto
      // al tempo reale...
      while (nstep*PHYS_SAMPLING_STEP < timeNow ) {
        motorbike.DoStep();
        nstep++;
        doneSomething=true;
        timeNow=SDL_GetTicks();
        if (guardia++>1000) {done=true; break;} // siamo troppo lenti!
      }
      
      if (doneSomething) 
        rendering(win, font);

      finalTime = clock();
      currentTime = finalTime - initTime;

      //gestisce il tempo rimanente
      done = handleTime(win, currentTime, font);
    }
  }
SDL_GL_DeleteContext(mainContext);
SDL_DestroyWindow(win);
SDL_Quit ();
return (0);
}

