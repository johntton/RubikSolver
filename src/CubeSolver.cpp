#include "include/CubeSolver.h"
#include <iostream>
#include <filesystem>

extern "C" {
    #include "kociemba/include/search.h"
}

Solver::Solver() {
    std::cout << "Initialized a solved Rubik's Cube." << std::endl;

    // Correct solved cube colors
    std::string colors[6] = {"W", "O", "G", "R", "B", "Y"};

    for (int f = 0; f < 6; ++f) {
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                cube[f][r][c] = colors[f];
            }
        }
    }
}

char Solver::mapColor(char color) {
    switch (color) {
        case 'W': return 'U';
        case 'Y': return 'D';
        case 'G': return 'F';
        case 'B': return 'B';
        case 'R': return 'R';
        case 'O': return 'L';
    }
    return '?';
}

std::string Solver::convertToString(const std::array<std::array<std::array<char, 3>, 3>, 6>& cubeRef) {
    std::string cubeString;
    cubeString.reserve(54);

    for (int i = 0; i < 6; i++) {
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                cubeString += mapColor(cubeRef[order[i]][r][c]);
            }
        }
    }
    return cubeString;
}

std::string Solver::runKociemba(const std::string cubeString) {
    int maxDepth = 21;
    long timeOut = 5;
    int useSeparator = 0;

    const char* cacheDir = "./kociemba_cache";
    std::filesystem::create_directories(cacheDir);

    char* result = solution(
        const_cast<char*>(cubeString.c_str()),
        maxDepth,
        timeOut,
        useSeparator,
        cacheDir
    );

    if (!result)
        return "ERROR: Invalid cube or timeout.";

    return std::string(result);
}