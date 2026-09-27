#include "Cube.h"
#include <iostream>
#include <sstream>

RubikCube::RubikCube() {
    char colors[6] = {'W', 'R', 'G', 'Y', 'O', 'B'}; // UP,RIGHT,FRONT,DOWN,LEFT,BACK

    for (int f = 0; f < 6; ++f) {
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                cube[f][r][c] = colors[f];
            }
        }
    }
}

void RubikCube::loadState(const std::array<std::array<std::array<char, 3>, 3>, 6>& state) {
    cube = state;
}

void RubikCube::rotateFaceHelper(Face face, bool isClockwise) {
    if (isClockwise) {
        char temp[3][3];
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                temp[2 - c][r] = cube[face][r][c];
            }
        }
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                cube[face][r][c] = temp[r][c];
            }
        }
    } else {
        char temp[3][3];
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                temp[c][2 - r] = cube[face][r][c];
            }
        }
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                cube[face][r][c] = temp[r][c];
            }
        }
    }
}

void RubikCube::rotateEdgeHelper(Face face, bool isClockwise) {
    char temp[3];

    switch (face) {
        case LEFT:
            if (isClockwise) {
                for (int i = 0; i < 3; ++i) temp[i] = cube[UP][i][0];
                for (int i = 0; i < 3; ++i) cube[UP][i][0] = cube[FRONT][i][0];
                for (int i = 0; i < 3; ++i) cube[FRONT][i][0] = cube[DOWN][i][0];
                for (int i = 0; i < 3; ++i) cube[DOWN][i][0] = cube[BACK][2 - i][2];
                for (int i = 0; i < 3; ++i) cube[BACK][2 - i][2] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = cube[UP][i][0];
                for (int i = 0; i < 3; ++i) cube[UP][i][0] = cube[BACK][2 - i][2];
                for (int i = 0; i < 3; ++i) cube[BACK][2 - i][2] = cube[DOWN][i][0];
                for (int i = 0; i < 3; ++i) cube[DOWN][i][0] = cube[FRONT][i][0];
                for (int i = 0; i < 3; ++i) cube[FRONT][i][0] = temp[i];
            }
            break;
        case RIGHT:
            if (isClockwise) {
                for (int i = 0; i < 3; ++i) temp[i] = cube[UP][i][2];
                for (int i = 0; i < 3; ++i) cube[UP][i][2] = cube[FRONT][i][2];
                for (int i = 0; i < 3; ++i) cube[FRONT][i][2] = cube[DOWN][i][2];
                for (int i = 0; i < 3; ++i) cube[DOWN][i][2] = cube[BACK][2 - i][0];
                for (int i = 0; i < 3; ++i) cube[BACK][2 - i][0] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = cube[UP][i][2];
                for (int i = 0; i < 3; ++i) cube[UP][i][2] = cube[BACK][2 - i][0];
                for (int i = 0; i < 3; ++i) cube[BACK][2 - i][0] = cube[DOWN][i][2];
                for (int i = 0; i < 3; ++i) cube[DOWN][i][2] = cube[FRONT][i][2];
                for (int i = 0; i < 3; ++i) cube[FRONT][i][2] = temp[i];
            }
            break;
        case UP:
            if (isClockwise) {
                for (int i = 0; i < 3; ++i) temp[i] = cube[BACK][0][i];
                for (int i = 0; i < 3; ++i) cube[BACK][0][i] = cube[LEFT][0][i];
                for (int i = 0; i < 3; ++i) cube[LEFT][0][i] = cube[FRONT][0][i];
                for (int i = 0; i < 3; ++i) cube[FRONT][0][i] = cube[RIGHT][0][i];
                for (int i = 0; i < 3; ++i) cube[RIGHT][0][i] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = cube[BACK][0][i];
                for (int i = 0; i < 3; ++i) cube[BACK][0][i] = cube[RIGHT][0][i];
                for (int i = 0; i < 3; ++i) cube[RIGHT][0][i] = cube[FRONT][0][i];
                for (int i = 0; i < 3; ++i) cube[FRONT][0][i] = cube[LEFT][0][i];
                for (int i = 0; i < 3; ++i) cube[LEFT][0][i] = temp[i];
            }
            break;
        case DOWN:
            if (isClockwise) {
                for (int i = 0; i < 3; ++i) temp[i] = cube[BACK][2][i];
                for (int i = 0; i < 3; ++i) cube[BACK][2][i] = cube[RIGHT][2][i];
                for (int i = 0; i < 3; ++i) cube[RIGHT][2][i] = cube[FRONT][2][i];
                for (int i = 0; i < 3; ++i) cube[FRONT][2][i] = cube[LEFT][2][i];
                for (int i = 0; i < 3; ++i) cube[LEFT][2][i] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = cube[BACK][2][i];
                for (int i = 0; i < 3; ++i) cube[BACK][2][i] = cube[LEFT][2][i];
                for (int i = 0; i < 3; ++i) cube[LEFT][2][i] = cube[FRONT][2][i];
                for (int i = 0; i < 3; ++i) cube[FRONT][2][i] = cube[RIGHT][2][i];
                for (int i = 0; i < 3; ++i) cube[RIGHT][2][i] = temp[i];
            }
            break;
        case FRONT:
            if (isClockwise) {
                for (int i = 0; i < 3; ++i) temp[i] = cube[UP][2][i];
                for (int i = 0; i < 3; ++i) cube[UP][2][i] = cube[LEFT][2 - i][2];
                for (int i = 0; i < 3; ++i) cube[LEFT][2 - i][2] = cube[DOWN][0][2 - i];
                for (int i = 0; i < 3; ++i) cube[DOWN][0][2 - i] = cube[RIGHT][i][0];
                for (int i = 0; i < 3; ++i) cube[RIGHT][i][0] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = cube[UP][2][i];
                for (int i = 0; i < 3; ++i) cube[UP][2][i] = cube[RIGHT][i][0];
                for (int i = 0; i < 3; ++i) cube[RIGHT][i][0] = cube[DOWN][0][2 - i];
                for (int i = 0; i < 3; ++i) cube[DOWN][0][2 - i] = cube[LEFT][2 - i][2];
                for (int i = 0; i < 3; ++i) cube[LEFT][2 - i][2] = temp[i];
            }
            break;
        case BACK:
            if (isClockwise) {
                for (int i = 0; i < 3; ++i) temp[i] = cube[UP][0][i];
                for (int i = 0; i < 3; ++i) cube[UP][0][i] = cube[RIGHT][i][2];
                for (int i = 0; i < 3; ++i) cube[RIGHT][i][2] = cube[DOWN][2][2 - i];
                for (int i = 0; i < 3; ++i) cube[DOWN][2][2 - i] = cube[LEFT][2 - i][0];
                for (int i = 0; i < 3; ++i) cube[LEFT][2 - i][0] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = cube[UP][0][i];
                for (int i = 0; i < 3; ++i) cube[UP][0][i] = cube[LEFT][2 - i][0];
                for (int i = 0; i < 3; ++i) cube[LEFT][2 - i][0] = cube[DOWN][2][2 - i];
                for (int i = 0; i < 3; ++i) cube[DOWN][2][2 - i] = cube[RIGHT][i][2];
                for (int i = 0; i < 3; ++i) cube[RIGHT][i][2] = temp[i];
            }
            break;
    }
}

void RubikCube::rotateFace(std::string face) {
    if (face.empty()) return;

    char faceChar = face[0];
    char modifier = (face.length() > 1) ? face[1] : '\0';

    if (faceChar != 'U' && faceChar != 'D' && faceChar != 'L' &&
        faceChar != 'R' && faceChar != 'F' && faceChar != 'B') {
        std::cout << "Invalid Face Character: " << faceChar << std::endl;
        return;
    }

    if (modifier != '2' && modifier != '\'' && modifier != '\0') {
        std::cout << "Invalid Rotation Modifier: " << modifier << std::endl;
        return;
    }

    Face f;
    switch (faceChar) {
        case 'U': f = UP; break;
        case 'D': f = DOWN; break;
        case 'L': f = LEFT; break;
        case 'R': f = RIGHT; break;
        case 'F': f = FRONT; break;
        case 'B': f = BACK; break;
        default: return;
    }

    // No modifier means a plain clockwise quarter turn; ' means counter-clockwise.
    bool clockwise = (modifier != '\'');
    int turns = (modifier == '2') ? 2 : 1;

    for (int i = 0; i < turns; i++) {
        rotateFaceHelper(f, clockwise);
        rotateEdgeHelper(f, clockwise);
    }
}

void RubikCube::applyMoves(const std::string& moves) {
    std::istringstream iss(moves);
    std::string token;
    while (iss >> token) {
        rotateFace(token);
    }
}

bool RubikCube::isSolved() const {
    for (int f = 0; f < 6; ++f) {
        char center = cube[f][1][1];
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (cube[f][r][c] != center) return false;
            }
        }
    }
    return true;
}

void RubikCube::display() const {
    for (int f = 0; f < 6; ++f) {
        std::cout << "Face " << f << ":\n";
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                std::cout << cube[f][r][c] << " ";
            }
            std::cout << "\n";
        }
    }
}
