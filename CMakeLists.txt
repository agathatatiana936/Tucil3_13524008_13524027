cmake_minimum_required(VERSION 3.16)

# ================================================================
# Force w64devkit compiler on Windows (before project() call!)
# ================================================================
if(WIN32 AND NOT DEFINED CMAKE_CXX_COMPILER)
  if(EXISTS "C:/raylib/w64devkit/bin/g++.exe")
    set(CMAKE_CXX_COMPILER "C:/raylib/w64devkit/bin/g++.exe" CACHE FILEPATH "C++ compiler" FORCE)
    set(CMAKE_C_COMPILER   "C:/raylib/w64devkit/bin/gcc.exe"  CACHE FILEPATH "C compiler"   FORCE)
    message(STATUS "Auto-detected w64devkit compiler")
  endif()
endif()

project(NIMONSPOLY VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output to ./bin (kept consistent across platforms)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Multiconfig generators (Visual Studio/Xcode) use per-config output dirs.
if(CMAKE_CONFIGURATION_TYPES)
  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${cfg}" cfg_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg_upper} "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${cfg_upper} "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${cfg_upper} "${CMAKE_SOURCE_DIR}/bin")
  endforeach()
endif()

# Recursively collect all .cpp sources (auto-updates when files change)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(game ${PROJECT_SOURCES})

target_include_directories(game PRIVATE "${CMAKE_SOURCE_DIR}/include")

if(MSVC)
  target_compile_options(game PRIVATE /W4)
else()
  target_compile_options(game PRIVATE -Wall -Wextra)
endif()

# -------------------------------------------------------------
# Raylib detection (cross-platform, no pkg-config required)
# -------------------------------------------------------------
set(RAYLIB_FOUND FALSE)

set(RAYLIB_HINTS
  "${CMAKE_SOURCE_DIR}/deps/raylib"
  "${CMAKE_SOURCE_DIR}/third_party/raylib"
  "C:/raylib/w64devkit"
  "C:/raylib"
  "$ENV{RAYLIB_ROOT}"
)

set(RAYLIB_PATHS
  /usr/local
  /usr
  /opt/local
  /opt
)

find_path(RAYLIB_INCLUDE_DIR
  NAMES raylib.h
  HINTS ${RAYLIB_HINTS}
  PATHS ${RAYLIB_PATHS}
  PATH_SUFFIXES include
  NO_DEFAULT_PATH
)

find_library(RAYLIB_LIBRARY
  NAMES raylib libraylib
  HINTS ${RAYLIB_HINTS}
  PATHS ${RAYLIB_PATHS}
  PATH_SUFFIXES lib lib64
  NO_DEFAULT_PATH
)

if(RAYLIB_INCLUDE_DIR AND RAYLIB_LIBRARY)
  set(RAYLIB_FOUND TRUE)
  message(STATUS "Found raylib: ${RAYLIB_LIBRARY}")
  message(STATUS "raylib include: ${RAYLIB_INCLUDE_DIR}")
else()
  message(STATUS "raylib not found. Building without GUI support.")
endif()

if(RAYLIB_FOUND)
  target_compile_definitions(game PRIVATE HAVE_RAYLIB=1)
  target_link_libraries(game PRIVATE ${RAYLIB_LIBRARY})

  # Only add raylib include path to files that actually need it.
  # This prevents stdlib.h conflicts in CLI/exception/solver files.
  set(GUI_SOURCES
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/gui/AssetManager.cpp"
    "${CMAKE_SOURCE_DIR}/src/gui/BoardRenderer.cpp"
    "${CMAKE_SOURCE_DIR}/src/gui/BoardViewPage.cpp"
    "${CMAKE_SOURCE_DIR}/src/gui/FileDialogWin32.cpp"
    "${CMAKE_SOURCE_DIR}/src/gui/GuiApp.cpp"
    "${CMAKE_SOURCE_DIR}/src/gui/GuiRenderer.cpp"
  )

  foreach(gui_src ${GUI_SOURCES})
    if(EXISTS ${gui_src})
      set_source_files_properties(${gui_src} PROPERTIES
        INCLUDE_DIRECTORIES ${RAYLIB_INCLUDE_DIR}
      )
    endif()
  endforeach()

  # Windows: prevent raylib <-> windows.h name collisions
  if(WIN32)
    target_compile_definitions(game PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(game PRIVATE opengl32 gdi32 winmm)
  elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(game PRIVATE ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
  else() # Linux
    find_package(X11 QUIET)
    if(X11_FOUND)
      target_link_libraries(game PRIVATE X11)
    endif()
    target_link_libraries(game PRIVATE m pthread dl rt)
  endif()
endif()

# Copy assets to output directory automatically after build
add_custom_command(TARGET game POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:game>/assets"
  COMMENT "Copying assets to output directory"
)

add_custom_target(run
  COMMAND $<TARGET_FILE:game>
  DEPENDS game
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
