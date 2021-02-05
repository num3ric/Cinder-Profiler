# This only supports build from the parent library. Otherwise cmake install and use the following:
# find_package(RvProfiler 1.0 REQUIRED)
# LIBRARIES RvProfiler::RvProfiler

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ProfilerTest)
include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )
ci_make_app(
    SOURCES     ${CMAKE_CURRENT_SOURCE_DIR}../../../src/ProfilerTestApp.cpp
    CINDER_PATH ${CINDER_PATH}
    LIBRARIES  RvProfiler::rvprofiler
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
