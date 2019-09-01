project('lander', 'cpp',
        default_options : [ 'cpp_std=c++17',
                            'buildtype=release',
                          ],
       version : '0.7.0')

subdir('po')

src = [
  'src/AnimatedImage.cpp',
  'src/Asteroid.cpp',
  'src/ConfigFile.cpp',
  'src/ElectricGate.cpp',
  'src/Emitter.cpp',
  'src/Fade.cpp',
  'src/Font.cpp',
  'src/Game.cpp',
  'src/HighScores.cpp',
  'src/Image.cpp',
  'src/Input.cpp',
  'src/Input.cpp',
  'src/InterfaceSounds.cpp',
  'src/Key.cpp',
  'src/LandingPad.cpp',
  'src/Main.cpp',
  'src/Menu.cpp',
  'src/Mine.cpp',
  'src/Missile.cpp',
  'src/ObjectGrid.cpp',
  'src/OpenGL.cpp',
  'src/Options.cpp',
  'src/ScreenManager.cpp',
  'src/Ship.cpp',
  'src/SoundEffect.cpp',
  'src/Surface.cpp',
  'src/TestDriver.cpp',
  'src/Texture.cpp',
  'src/Viewport.cpp',
]

freetype = dependency('freetype2')
sdl2 = dependency('sdl2')
gl = dependency('gl')
glew = dependency('glew')
mixer = dependency('SDL2_mixer')
image = dependency('SDL2_image')

add_project_link_arguments(['-lstdc++fs'], language : 'cpp')

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
                    dependencies : [freetype, sdl2, gl, glew, mixer, image])

install_subdir('data/images', install_dir : pkgdatadir)
install_subdir('data/sounds', install_dir : pkgdatadir)
install_data('data/fonts/VeraBd.ttf',
             install_dir : join_paths(pkgdatadir, 'fonts'))

run_target('run', command : lander)

test('sanity', lander, args : ['test'],
     env : ['MESON_SOURCE_ROOT=' + meson.source_root()])
