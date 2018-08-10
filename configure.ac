AC_INIT([Lunar Lander], [0.7],
        [Nick Gasson <nick@nickg.me.uk>],
        [lander])
AM_INIT_AUTOMAKE([1.11 -Wall -Werror -Wno-extra-portability foreign])
AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_CPP
AC_PROG_MKDIR_P

AX_CXX_COMPILE_STDCXX_11

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18.3])

dnl Check for Boost
AX_BOOST_BASE(1.44)
AX_BOOST_FILESYSTEM
AX_BOOST_SYSTEM

dnl Check for OpenGL
AX_CHECK_GL

dnl Check for SDL
SDL_VERSION=1.2.5
AM_PATH_SDL($SDL_VERSION,
        :,
        AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)

dnl Check for SDL_Image
AC_CHECK_LIB(SDL_image, IMG_Load, [],
             AC_MSG_ERROR([*** SDL_image not found!]))

dnl Check for SDL_Mixer
AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio, [],
             AC_MSG_ERROR([*** SDL_mixer not found!]))

dnl Check for Freetype and libpng
PKG_CHECK_MODULES(FREETYPE, freetype2)
PKG_CHECK_MODULES(LIBPNG, libpng)

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile data/Makefile src/Makefile po/Makefile.in])
AC_OUTPUT
