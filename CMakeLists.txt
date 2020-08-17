cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(
    Cinder-Profiler
    DESCRIPTION "Cinder block for basic cpu and gpu time profiling."
    LANGUAGES CXX
)

option(BUILD_SAMPLES "Build samples" ON)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../Cinder" ABSOLUTE )
include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
find_package( cinder REQUIRED PATHS "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )

add_subdirectory(src)

if( BUILD_SAMPLES )
    add_subdirectory(samples)
    # set startup project in visual studio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ProfilerTest)
else()
    # set startup project in visual studio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT libprofiler)
endif()
