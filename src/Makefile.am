bin_PROGRAMS = lander

AM_CXXFLAGS=$(LANDER_CFLAGS) $(SDL_CFLAGS) -Wall
AM_LDFLAGS=$(LANDER_LIBS) $(SDL_LIBS) -lGLU $(LTLIBINTL)
lander_SOURCES = Main.cpp Game.hpp Game.cpp \
	Font.cpp Font.hpp HighScores.cpp HighScores.hpp Input.cpp \
	Input.hpp Platform.hpp LoadOnce.hpp \
	OpenGL.cpp OpenGL.hpp Menu.cpp Menu.hpp Emitter.cpp \
	Emitter.hpp ScreenManager.cpp ScreenManager.hpp \
	ObjectGrid.hpp ObjectGrid.cpp Asteroid.hpp Asteroid.cpp \
	Ship.hpp Ship.cpp Viewport.hpp Viewport.cpp LandingPad.hpp \
	LandingPad.cpp Surface.hpp Surface.cpp Mine.hpp Mine.cpp \
	ElectricGate.hpp ElectricGate.cpp Key.hpp Key.cpp \
	Mechanics.hpp Mechanics.cpp Geometry.hpp Image.hpp \
	Image.cpp AnimatedImage.hpp AnimatedImage.cpp \
	Texture.hpp Texture.cpp Options.hpp Options.cpp \
	ConfigFile.hpp ConfigFile.cpp SoundEffect.hpp \
	SoundEffect.cpp

localedir = $(datadir)/locale

# Add -DSHOW_FPS here to show FPS in title bar
DEFS = -DLOCALEDIR=\"$(localedir)\" -DLINUX -DUNIX \
	-DDATADIR=\"$(pkgdatadir)\" @DEFS@
