
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_VERBOSE_MAKEFILE ON)
project(ProfilerTest)

get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../.." ABSOLUTE )
    include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
    #if( CINDER_MSW )
        #get_filename_component( LIBPROFILER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/$(PlatformTarget)/$(PlatformToolset)/$(Configuration)/libprofiler.lib" ABSOLUTE )
    #elseif( CINDER_LINUX )
        get_filename_component( LIBPROFILER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}/libRvProfiler.so" ABSOLUTE )
    #endif()
    add_library( RvProfiler SHARED IMPORTED )
    set_target_properties( RvProfiler PROPERTIES IMPORTED_LOCATION ${LIBPROFILER_PATH} )

    #get_filename_component( LIBPROFILER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
    get_filename_component( LIBPROFILER_INC "${CMAKE_CURRENT_SOURCE_DIR}/../../../../src" ABSOLUTE)
else()
    set( LIBPROFILER_PATH RvProfiler )
    get_filename_component( LIBPROFILER_INC "${CMAKE_CURRENT_SOURCE_DIR}/../../../../src" ABSOLUTE)
endif()

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )
ci_make_app(
    SOURCES     ${APP_PATH}/src/ProfilerTestApp.cpp
    CINDER_PATH ${CINDER_PATH}
    INCLUDES    ${LIBPROFILER_INC}
    LIBRAIRIES  RvProfiler
)
#target_link_libraries(${PROJECT_NAME} RvProfiler)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
