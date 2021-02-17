cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(
        librvprofiler
        VERSION 1.0
        DESCRIPTION "Cinder block for basic cpu and gpu time profiling."
        LANGUAGES CXX
)
option(BUILD_RVPROFILER_SAMPLES "Build samples" OFF)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

if (NOT DEFINED CINDER_PATH)
    get_filename_component(CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
endif ()

include("${CINDER_PATH}/proj/cmake/configure.cmake")
find_package(cinder REQUIRED PATHS "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}")

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_library(rvprofiler)
add_library(RvProfiler::rvprofiler ALIAS rvprofiler)

target_sources(rvprofiler
        PRIVATE
        ${SRC_DIR}/Profiler.cpp
        ${INC_DIR}/Profiler.h
        ${INC_DIR}/ProfilerGui.h
        )
target_include_directories(rvprofiler
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
target_link_libraries(rvprofiler PUBLIC cinder)
set_target_properties(rvprofiler
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        POSITION_INDEPENDENT_CODE ON
        VERSION ${PROJECT_VERSION}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}
        )

if (BUILD_RVPROFILER_SAMPLES)
    add_subdirectory(samples)
endif()


export( TARGETS rvprofiler FILE ${PROJECT_BINARY_DIR}/lib/${CINDER_TARGET_SUBFOLDER}/RvProfilerTargets.cmake NAMESPACE RvProfiler:: )
configure_file( ${CMAKE_CURRENT_LIST_DIR}/cmake/RvProfilerConfig.cmake.in
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CINDER_TARGET_SUBFOLDER}/RvProfilerConfig.cmake"
)
