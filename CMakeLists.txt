cmake_minimum_required (VERSION 3.7)

project (Lander)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

if (NOT DEFINED CMAKE_CROSSCOMPILING_EMULATOR)
  find_package (SDL2 REQUIRED)
  find_package (SDL2_mixer REQUIRED)
  find_package (SDL2_image REQUIRED)
  find_package (OpenGL REQUIRED)
  find_package (Freetype REQUIRED)
endif ()

add_definitions (-DVERSION=0.7)

if (DEFINED CMAKE_CROSSCOMPILING_EMULATOR)
  add_compile_options (-Wall)
  add_compile_options (-fno-math-errno)
  add_compile_options (-fno-exceptions)
  add_compile_options (-fno-rtti)
  string (APPEND CMAKE_CXX_FLAGS "-s USE_FREETYPE=1 -s USE_SDL=2")
  add_definitions (-DEMSCRIPTEN)
elseif (NOT MSVC)
  add_compile_options (-Wall)
  add_compile_options (-fno-math-errno)
  add_compile_options (-fno-exceptions)
  add_compile_options (-fno-rtti)
  add_definitions (-DLINUX -DUNIX -DDATADIR=\"${CMAKE_INSTALL_PREFIX}/share/lander\")
  set (STD_CXX_FS_LIBRARIES stdc++fs)
else (NOT MSVC)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)
  add_definitions (-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions (-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions (-DLINUX -DWIN32)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "")
  if (DEFINED CMAKE_CROSSCOMPILING_EMULATOR)
    set (CMAKE_BUILD_TYPE "MinSizeRel")
  else ()
    set (CMAKE_BUILD_TYPE "RelWithDebInfo")
  endif ()
endif ()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set (CMAKE_CXX_FLAGS_RELEASE "-O3")
  if (MINGW)
    set (CMAKE_CXX_FLAGS_RELEASE "-Wl,-subsystem,windows ${CMAKE_CXX_FLAGS_RELEASE}")
  endif ()
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  set (CMAKE_CXX_FLAGS_MINSIZEREL "-Os")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set (CMAKE_CXX_FLAGS_DEBUG "-Og -g")
endif ()

file (GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable (lander WIN32 ${SRC_FILES})

target_include_directories (lander PRIVATE src ${SDL2_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS}
  ${SDL2_MIXER_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

target_link_libraries (lander PRIVATE ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES}
  ${FREETYPE_LIBRARIES} ${SDL2_MIXER_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES} ${STD_CXX_FS_LIBRARIES})

set_target_properties (lander PROPERTIES CXX_STANDARD 14)

install (TARGETS lander RUNTIME DESTINATION bin)

install (DIRECTORY ${PROJECT_SOURCE_DIR}/data/images
  DESTINATION share/lander
  FILES_MATCHING PATTERN "*.png")

install (DIRECTORY ${PROJECT_SOURCE_DIR}/data/sounds
  DESTINATION share/lander
  FILES_MATCHING PATTERN "*.wav")

install (FILES ${PROJECT_SOURCE_DIR}/data/Default_Font.ttf
  DESTINATION share/lander/)
