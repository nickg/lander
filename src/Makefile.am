bin_PROGRAMS = lander

lander_CXXFLAGS=$(LANDER_CFLAGS) $(SDL_CFLAGS) -DLINUX -DUNIX \
	-DDATADIR=\"$(pkgdatadir)\"
lander_LDFLAGS=$(LANDER_LIBS) $(SDL_LIBS) -lGLU
lander_SOURCES = Main.cpp Lander.hpp Game.cpp \
	Bitmap.cpp Bitmap.hpp FreeType.cpp FreeType.hpp \
	HighScores.cpp HighScores.hpp Input.cpp Input.hpp \
	DataFile.cpp DataFile.hpp File.cpp File.hpp Platform.hpp \
	OpenGL.cpp OpenGL.hpp Menu.cpp Menu.hpp Emitter.cpp \
	Emitter.hpp Screens.cpp Screens.hpp Strings.hpp \
	ObjectGrid.hpp ObjectGrid.cpp Asteroid.hpp Asteroid.cpp \
	Ship.hpp Ship.cpp Viewport.hpp Viewport.cpp LandingPad.hpp \
	LandingPad.cpp Surface.hpp Surface.cpp Mine.hpp Mine.cpp \
	ElectricGate.hpp ElectricGate.cpp Key.hpp Key.cpp

dist_pkgdata_DATA = ../lander.dat ../Default_Font.ttf ../Hollow_Font.ttf
