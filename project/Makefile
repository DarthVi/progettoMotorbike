# Project: progettoCar4

CPP  = g++
CC   = gcc
BIN  = progettoMotorbike

OBJ  = main.o motorbike.o mesh.o utils.o waypoint.o scene.o
LINKOBJ  = main.o motorbike.o mesh.o utils.o waypoint.o scene.o

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

scene.o: scene.cpp
	$(CPP) -c $(FRMPATH) scene.cpp -o scene.o

utils.o: utils.cpp
	$(CPP) -c $(FRMPATH) utils.cpp -o utils.o

waypoint.o: waypoint.cpp
	$(CPP) -c $(FRMPATH) waypoint.cpp -o waypoint.o

mesh.o: mesh.cpp
	$(CPP) -c $(FRMPATH) mesh.cpp -o mesh.o
