#ifndef CUBE_H
#define CUBE_H

#include <array>
#include <string>

class RubikCube {
public:
    // Matches the UP,RIGHT,FRONT,DOWN,LEFT,BACK face order used by
    // ColorDetector::classifyAllFaces and Solver::convertToString, so a
    // scanned cube state can be loaded directly with loadState().
    enum Face { UP = 0, RIGHT = 1, FRONT = 2, DOWN = 3, LEFT = 4, BACK = 5 };

    RubikCube();

    // Loads a scanned cube state (same face/row/col convention as Solver).
    void loadState(const std::array<std::array<std::array<char, 3>, 3>, 6>& state);

    // Applies a Kociemba-style move sequence, e.g. "U R2 F' D2 L B' ".
    void applyMoves(const std::string& moves);

    void rotateFace(std::string face);
    bool isSolved() const;
    void display() const;

private:
    std::array<std::array<std::array<char, 3>, 3>, 6> cube;

    void rotateFaceHelper(Face face, bool isClockwise);
    void rotateEdgeHelper(Face face, bool isClockwise);
};

#endif
