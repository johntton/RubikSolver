#ifndef CUBESOLVER_H
#define CUBESOLVER_H

#include <array>
#include <string>

class Solver {
public:
    Solver();
    char mapColor(char color);
    std::string convertToString(const std::array<std::array<std::array<char, 3>, 3>, 6>& cube);
    std::string runKociemba(const std::string cubeString);

private:
    enum Face { UP = 0, RIGHT = 1, FRONT = 2, DOWN = 3, LEFT = 4, BACK = 5 };
    int order[6] = { UP, RIGHT, FRONT, DOWN, LEFT, BACK };
    std::array<std::array<std::array<std::string, 3>, 3>, 6> cube;
};

#endif
