OBJECTS=OpenGL.o FreeType.o Game.o HighScores.o Input.o Main.o \
	Menu.o DataFile.o Bitmap.o File.o Screens.o Emitter.o
HEADERS=DataFile.hpp FreeType.hpp Input.hpp Lander.hpp OpenGL.hpp \
	Platform.hpp Bitmap.hpp File.hpp Screens.hpp Emitter.o
BINARY=lander
CPP=g++
CPPFLAGS=-Wall -DUNIX -D$(PLATFORM)
LDFLAGS=

# Try to work out the platform
UNAME:=$(shell uname)
ifeq ($(UNAME),Darwin)
  CPPFLAGS+=-I/usr/X11R6/include -I/usr/X11R6/include/freetype2
  PLATFORM:=MACOSX
endif
ifeq ($(UNAME),Linux)
  CPPFLAGS+=-I/usr/include/freetype2
  LDFLAGS+=-lfreetype -lGLU
  PLATFORM:=LINUX
endif

ifeq ($(PLATFORM),)
  $(error Unsupported platform $(UNAME))
endif

# Get SDL options
SDL_CFLAGS:=$(shell sdl-config --cflags)
SDL_LIBS:=$(shell sdl-config --libs)	

CPPFLAGS+=$(SDL_CFLAGS)
LDFLAGS+=$(SDL_LIBS)	

$(BINARY): $(OBJECTS)
	$(CPP) $(LDFLAGS) -o $(BINARY) $(OBJECTS)

clean:
	rm $(OBJECTS) $(BINARY)