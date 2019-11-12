// classe Point3: un punto (o vettore) in 3 dimensioni
// comprende le operazioni fra punti

class Point3
{
public: 
  
  float coord[3]; // l'UNICO campo: le cooedinate x, y, z
  
  float X() const { return coord[0]; }
  float Y() const { return coord[1]; }
  float Z() const { return coord[2]; }
  
  // costruttore 1
  Point3( float x, float y, float z ){
    coord[0]=x;
    coord[1]=y;
    coord[2]=z;
  }

  // costruttore vuoto
  Point3(){
    coord[0]=coord[1]=coord[2]=0;
  }
  
  
  // restituisce la versione di se stesso normalizzata
  Point3 Normalize() const{
    return (*this)/modulo();
  }

  // restituisce il modulo
  float modulo() const{
    return 
      sqrt(coord[0]*coord[0] + coord[1]*coord[1] + coord[2]*coord[2]);
  }
  
  // operatore "/" binario: divisione per uno scalare (un float)
  Point3 operator / (float f)const{
    return Point3( 
      coord[0]/f,   
      coord[1]/f,   
      coord[2]/f   
    );
  }
  
  // operatore "-" unario: inversione del verso del vettore
  Point3 operator - ()const{
    return Point3( 
      -coord[0],   
      -coord[1],   
      -coord[2]   
    );
  }

  // operatore "-" binario: differenza fra punti
  Point3 operator - (const Point3 &a)const{
    return Point3( 
      coord[0]-a.coord[0],   
      coord[1]-a.coord[1],   
      coord[2]-a.coord[2]   
    );
  }
    
  // somma fra vettori  
  Point3 operator + (const Point3 &a)const{
    return Point3( 
      coord[0]+a.coord[0],   
      coord[1]+a.coord[1],   
      coord[2]+a.coord[2]   
    );
  }
  
  
  // ridefinisco l'operatore binario "%" per fare il CROSS PRODUCT
  Point3 operator % (const Point3 &a)const{
    return Point3( 
      coord[1]*a.coord[2]-coord[2]*a.coord[1],   
    -(coord[0]*a.coord[2]-coord[2]*a.coord[0]),   
      coord[0]*a.coord[1]-coord[1]*a.coord[0]   
    );
  }
  
  // mandare il punto come vertice di OpenGl
  void SendAsVertex() const{
    glVertex3fv(coord);
  }
  
  // mandare il punto come normale di OpenGl
  void SendAsNormal() const{
    glNormal3fv(coord);
  }
};


// definiamo Vector3 come SINONIMO di Point3
// (solo per chiarezza, per distinguere nel codice fra punti e vettori)
typedef Point3 Vector3;

inline void glTranslate(Point3 v){
  glTranslatef(v.X(), v.Y(), v.Z() );
}


