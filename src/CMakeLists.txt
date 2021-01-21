add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE
        Profiler.cpp
        Profiler.h
        ProfilerGui.h
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC cinder)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        POSITION_INDEPENDENT_CODE ON
        VERSION ${PROJECT_VERSION}
)
# if( CINDER_MSW )
#     set_target_properties(${PROJECT_NAME}
#         PROPERTIES
#             ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib/$(PlatformTarget)/$(PlatformToolset)
#             LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib/$(PlatformTarget)/$(PlatformToolset)
#             RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/$(PlatformTarget)/$(PlatformToolset)
#     )
# elseif( CINDER_LINUX )
set_target_properties(${PROJECT_NAME}
PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}
)
# endif()
