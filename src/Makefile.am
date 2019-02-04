bin_PROGRAMS = lander

# Add -pg for profiling
AM_CXXFLAGS = $(FREETYPE_CFLAGS) $(LIBPNG_CFLAGS) $(SDL_CFLAGS) \
	-Wall -ffast-math

lander_LDADD = $(FREETYPE_LIBS) $(SDL_LIBS) $(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) $(GL_LIBS) $(LTLIBINTL) $(SDL_MIXER_LIBS) \
	$(SDL_IMAGE_LIBS) $(LIBPNG_LIBS) $(GLEW_LIBS) -lstdc++fs

lander_SOURCES = Main.cpp Game.hpp Game.cpp \
	Font.cpp Font.hpp HighScores.cpp HighScores.hpp Input.cpp \
	Input.hpp Platform.hpp GraphicsFwd.hpp \
	OpenGL.cpp OpenGL.hpp Menu.cpp Menu.hpp Emitter.cpp \
	Emitter.hpp ScreenManager.cpp ScreenManager.hpp \
	ObjectGrid.hpp ObjectGrid.cpp Asteroid.hpp Asteroid.cpp \
	Ship.hpp Ship.cpp Viewport.hpp Viewport.cpp LandingPad.hpp \
	LandingPad.cpp Surface.hpp Surface.cpp Mine.hpp Mine.cpp \
	ElectricGate.hpp ElectricGate.cpp Key.hpp Key.cpp \
	Geometry.hpp Image.hpp Missile.hpp Missile.cpp \
	Image.cpp AnimatedImage.hpp AnimatedImage.cpp \
	Texture.hpp Texture.cpp Options.hpp Options.cpp \
	ConfigFile.hpp ConfigFile.cpp SoundEffect.hpp \
	SoundEffect.cpp Fade.hpp Fade.cpp GameObjFwd.hpp \
	InterfaceSounds.hpp InterfaceSounds.cpp

localedir = $(datadir)/locale

# Add -DSHOW_FPS here to show FPS in title bar
DEFS = -DLOCALEDIR=\"$(localedir)\" -DLINUX -DUNIX \
	-DDATADIR=\"$(pkgdatadir)\" @DEFS@
