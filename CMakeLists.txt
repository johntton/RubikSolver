cmake_minimum_required(VERSION 3.10)
project(RubikSolver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(kociemba/include)

set(KOCIEMBA_SOURCES
    kociemba/coordcube.c
    kociemba/cubiecube.c
    kociemba/facecube.c
    kociemba/prunetable_helpers.c
    kociemba/search.c
    kociemba/solve.c
)

add_executable(RubikSolver
    src/main.cpp
    src/CubeScanner.cpp
    src/ColorDetector.cpp
    src/CubeSolver.cpp
    ${KOCIEMBA_SOURCES}
)

target_link_libraries(RubikSolver ${OpenCV_LIBS})
