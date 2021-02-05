cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(
        librvprofiler
        VERSION 1.0
        DESCRIPTION "Cinder block for basic cpu and gpu time profiling."
        LANGUAGES CXX
)
option(BUILD_SAMPLES "Build samples" ON)

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

if (BUILD_SAMPLES)
    add_subdirectory(samples)
endif()


##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/RvProfiler)

install(TARGETS rvprofiler
        EXPORT rvprofiler-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#Export the targets to a script
install(EXPORT rvprofiler-targets
        FILE
        RvProfilerTargets.cmake
        NAMESPACE
        RvProfiler::
        DESTINATION
        ${INSTALL_CONFIGDIR}
        )

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/RvProfilerConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/RvProfilerConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/RvProfilerConfig.cmake
        INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
        )

#Install the config, config version and custom find modules
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/RvProfilerConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/RvProfilerConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR}
        )

##############################################
## Exporting from the build tree
export(EXPORT rvprofiler-targets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/RvProfilerTargets.cmake
        NAMESPACE RvProfiler::)

#Register package in user's package registry
export(PACKAGE RvProfiler)
