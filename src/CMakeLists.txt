cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(
    libprofiler
    VERSION 0.1
    LANGUAGES CXX )

add_library( ${PROJECT_NAME} )
target_sources( ${PROJECT_NAME}
    PRIVATE
        Profiler.cpp
    PUBLIC
        Profiler.h
        ProfilerGui.h
)
target_include_directories( ${PROJECT_NAME}
    PUBLIC  ${INC_DIR}
            "${CINDER_PATH}/include"
)
target_link_libraries( ${PROJECT_NAME} PUBLIC cinder )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        VERSION ${PROJECT_VERSION}
)
# and LIBRARY directories. Override the directories so, ${CMAKE_BUILD_TYPE} doesn't double up.
set_target_properties(  ${PROJECT_NAME}
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib/$(PlatformTarget)/$(PlatformToolset)
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib/$(PlatformTarget)/$(PlatformToolset)
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/$(PlatformTarget)/$(PlatformToolset)
)
