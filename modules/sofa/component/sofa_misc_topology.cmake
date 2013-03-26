cmake_minimum_required(VERSION 2.8)

project("SofaMiscTopology")

include(${SOFA_CMAKE_DIR}/pre.cmake)

set(HEADER_FILES

    initMiscTopology.h
    misc/TopologicalChangeProcessor.h
    )
    
set(SOURCE_FILES

    initMiscTopology.cpp
    misc/TopologicalChangeProcessor.cpp
    )
    
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

set(COMPILE_DEFINES "SOFA_BUILD_MISC_TOPOLOGY;POINT_DATA_VECTOR_ACCESS")
set(LINK_DEPENDENCIES ${ZLIB_LIBRARIES} miniFlowVR SofaBaseTopology )

include(${SOFA_CMAKE_DIR}/post.cmake)
