cmake_minimum_required (VERSION 2.8)
project (ReactDiffuse)
set(VERSION "0.1.0")


# include paths
include_directories(src)

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
file(GLOB_RECURSE project_SOURCES "src/*.cpp")

if (APPLE)
    set(OSX_ICON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/Icon.icns)
    # set where in the bundle to put the icns files
    set_source_files_properties(${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    # include the icns files in the target
    set( project_SOURCES ${project_SOURCES} ${OSX_ICON_FILES} )
endif()

#add_executable(ReactDiffuse WIN32 MACOSX_BUNDLE ${project_SOURCES})
add_executable(ReactDiffuse WIN32 ${project_SOURCES})

if (APPLE)
    set_target_properties(ReactDiffuse PROPERTIES
                          MACOSX_BUNDLE_BUNDLE_NAME "React Diffuse"
                          MACOSX_BUNDLE_ICON_FILE   "React Diffuse"
                          MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
                          MACOSX_BUNDLE_LONG_VERSION_STRING "React Diffuse - Version ${VERSION}")
endif()

if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(ReactDiffuse PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

link_directories( /usr/local/lib )

set(CMAKE_CXX_FLAGS "-stdlib=libc++")
target_link_libraries(ReactDiffuse ${SDL_LIBRARY})

