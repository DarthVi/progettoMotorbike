# Project: progettoCar4

CPP  = g++
CC   = gcc
BIN  = progettoCar4

OBJ  = main.o motorbike.o mesh.o tabellone.o utils.o streetlamp.o barile.o pumpstation.o waypoint.o statua.o bench.o
LINKOBJ  = main.o motorbike.o mesh.o tabellone.o utils.o streetlamp.o barile.o pumpstation.o waypoint.o statua.o bench.o

# Library linking
OS := $(shell uname)
ifeq ($(OS),Darwin)
## caso Mac OS
$(info Mac OS detected)
FRMPATH=-F /Library/Frameworks
LIBS =  -framework OpenGL -framework SDL2 -framework SDL2_image -framework SDL2_ttf -lm
$(info SDL2 libraries must be in: $(FRMPATH))
else
ifeq ($(OS),MINGW32_NT-6.1)
## caso Windows MinGW
$(info Windows MinGW detected)
FRMPATH = -IC:\MinGW\include
LIBS = -LC:\MinGW\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lopengl32 -lglu32 -lm
else
##caso Linux
$(info Linux detected)
#framework presenti solo nel caso MAC OS
FRMPATH =
LIBS = -lGL -lGLU -lSDL2 -lSDL2_image -lSDL2_ttf -lm
endif
endif

FLAG = -Wno-deprecated
RM = rm -f

all: $(BIN)

clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(FRMPATH) $(LIBS)

main.o: main.cpp
	$(CPP) -c $(FRMPATH) main.cpp -o main.o

motorbike.o: motorbike.cpp
	$(CPP) -c $(FRMPATH) motorbike.cpp -o motorbike.o

tabellone.o: tabellone.cpp
	$(CPP) -c $(FRMPATH) tabellone.cpp -o tabellone.o

utils.o: utils.cpp
	$(CPP) -c $(FRMPATH) utils.cpp -o utils.o

streetlamp.o: streetlamp.cpp
	$(CPP) -c $(FRMPATH) streetlamp.cpp -o streetlamp.o

barile.o: barile.cpp
	$(CPP) -c $(FRMPATH) barile.cpp -o barile.o

pumpstation.o: pumpstation.cpp
	$(CPP) -c $(FRMPATH) pumpstation.cpp -o pumpstation.o

waypoint.o: waypoint.cpp
	$(CPP) -c $(FRMPATH) waypoint.cpp -o waypoint.o

statua.o: statua.cpp
	$(CPP) -c $(FRMPATH) statua.cpp -o statua.o

bench.o: bench.cpp
	$(CPP) -c $(FRMPATH) bench.cpp -o bench.o

mesh.o: mesh.cpp
	$(CPP) -c $(FRMPATH) mesh.cpp -o mesh.o
