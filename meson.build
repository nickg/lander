project('lander', 'cpp',
        default_options : [ 'cpp_std=c++14',
                            'buildtype=release'
                          ],
       version : '0.7')

subdir('po')

src = [ 'src/Main.cpp',
        'src/Game.cpp',
        'src/OpenGL.cpp',
        'src/Texture.cpp',
        'src/Input.cpp',
        'src/ScreenManager.cpp',
        'src/Input.cpp',
        'src/Emitter.cpp',
        'src/Font.cpp',
        'src/Image.cpp',
        'src/Key.cpp',
        'src/Ship.cpp',
        'src/Missile.cpp',
        'src/ObjectGrid.cpp',
        'src/Surface.cpp',
        'src/Viewport.cpp',
        'src/Menu.cpp',
        'src/SoundEffect.cpp',
        'src/AnimatedImage.cpp',
        'src/ElectricGate.cpp',
        'src/LandingPad.cpp',
        'src/Fade.cpp',
        'src/HighScores.cpp',
        'src/InterfaceSounds.cpp',
        'src/Asteroid.cpp',
        'src/ConfigFile.cpp',
        'src/Mine.cpp',
        'src/Options.cpp',
      ]

freetype = dependency('freetype2')
sdl2 = dependency('sdl2')
gl = dependency('gl')
glew = dependency('glew')
mixer = dependency('SDL2_mixer')
boost = dependency('boost', modules : ['filesystem', 'system'])
image = dependency('SDL2_image')

pkgdatadir = join_paths(get_option('datadir'), 'lander')

conf_data = configuration_data()

if target_machine.system() == 'windows'
  conf_data.set('WIN32', true)
elif target_machine.system() == 'darwin'
  conf_data.set('MACOSX', true)
  conf_data.set('UNIX', true)
else
  conf_data.set('LINUX', true)
  conf_data.set('UNIX', true)
endif

conf_data.set_quoted('VERSION', meson.project_version())
conf_data.set_quoted('DATADIR', join_paths(get_option('prefix'), pkgdatadir))
configure_file(input : 'config.h.in',
               output : 'config.h',
               configuration : conf_data)

lander = executable('lander', src, install : true,
                    dependencies : [freetype, sdl2, gl, glew, mixer, boost, image])

install_subdir('data/images', install_dir : pkgdatadir)
install_subdir('data/sounds', install_dir : pkgdatadir)
install_data('data/fonts/VeraBd.ttf', install_dir : pkgdatadir)

run_target('run', command : lander)
