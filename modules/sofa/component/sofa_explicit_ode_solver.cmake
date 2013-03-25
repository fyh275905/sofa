cmake_minimum_required(VERSION 2.8)

project("SofaExplicitOdeSolver")

include(${SOFA_CMAKE_DIR}/pre.cmake)

set(HEADER_FILES

    initExplicitODESolver.h 
    odesolver/CentralDifferenceSolver.h 
    odesolver/EulerSolver.h 
    odesolver/RungeKutta2Solver.h 
    odesolver/RungeKutta4Solver.h

    )
    
set(SOURCE_FILES

    initExplicitODESolver.cpp 
    odesolver/CentralDifferenceSolver.cpp 
    odesolver/EulerSolver.cpp 
    odesolver/RungeKutta2Solver.cpp 
    odesolver/RungeKutta4Solver.cpp

    )
    
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} SofaSimulationTree )
    
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "${GLOBAL_DEFINES};SOFA_BUILD_EXPLICIT_ODE_SOLVER")
    
include(${SOFA_CMAKE_DIR}/post.cmake)
