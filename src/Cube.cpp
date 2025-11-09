#include <iostream>
#include <string>
using namespace std;

class RubikCube {
    public:
    
    string cube[6][3][3]; // cube[face][row][col]
    enum Face { UP = 0, LEFT = 1, FRONT = 2, RIGHT = 3, BACK = 4, DOWN = 5 }; // Face indices
    
    RubikCube() {
        // Initialize a solved Rubik's Cube
        cout << "Initialized a solved Rubik's Cube." << endl;

        string colors[6] = {"W", "O", "G", "R", "B", "Y"}; // White, Orange, Green, Red, Blue, Yellow

        // Fills each face with its respective color
        for (int f = 0; f < 6; ++f) {
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 3; ++c) {
                    cube[f][r][c] = colors[f] + to_string(r * 3 + c + 1);
                }
            }
        }
    }

    void rotateFaceHelper(Face face, bool isClockwise) {
        // Helper function to rotate a face
        if (isClockwise) {
            // Rotate face 90 degrees clockwise
            string temp[3][3];
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
            // Rotate face 90 degrees counter-clockwise
            string temp[3][3];
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

    void rotateEdgeHelper(Face face, bool isClockwise) {
        // Helper function to rotate the edges adjacent to a face
        string temp[3];

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
                    for (int i = 0; i < 3; ++i) cube[UP][i][2] = cube[BACK][2 - i][0];
                    for (int i = 0; i < 3; ++i) cube[BACK][2 - i][0] = cube[DOWN][i][2];
                    for (int i = 0; i < 3; ++i) cube[DOWN][i][2] = cube[FRONT][i][2];
                    for (int i = 0; i < 3; ++i) cube[FRONT][i][2] = temp[i];
                } else {
                    for (int i = 0; i < 3; ++i) temp[i] = cube[UP][i][2];
                    for (int i = 0; i < 3; ++i) cube[UP][i][2] = cube[FRONT][i][2];
                    for (int i = 0; i < 3; ++i) cube[FRONT][i][2] = cube[DOWN][i][2];
                    for (int i = 0; i < 3; ++i) cube[DOWN][i][2] = cube[BACK][2 - i][0];
                    for (int i = 0; i < 3; ++i) cube[BACK][2 - i][0] = temp[i];
                }
                break;
            case UP:
                if (isClockwise) {
                    for (int i = 0; i < 3; ++i) temp[i] = cube[BACK][0][i];
                    for (int i = 0; i < 3; ++i) cube[BACK][0][i] = cube[RIGHT][0][i];
                    for (int i = 0; i < 3; ++i) cube[RIGHT][0][i] = cube[FRONT][0][i];
                    for (int i = 0; i < 3; ++i) cube[FRONT][0][i] = cube[LEFT][0][i];
                    for (int i = 0; i < 3; ++i) cube[LEFT][0][i] = temp[i];
                } else {
                    for (int i = 0; i < 3; ++i) temp[i] = cube[BACK][0][i];
                    for (int i = 0; i < 3; ++i) cube[BACK][0][i] = cube[LEFT][0][i];
                    for (int i = 0; i < 3; ++i) cube[LEFT][0][i] = cube[FRONT][0][i];
                    for (int i = 0; i < 3; ++i) cube[FRONT][0][i] = cube[RIGHT][0][i];
                    for (int i = 0; i < 3; ++i) cube[RIGHT][0][i] = temp[i];
                }
                break;
            case DOWN:
                if (isClockwise) {
                    for (int i = 0; i < 3; ++i) temp[i] = cube[BACK][2][i];
                    for (int i = 0; i < 3; ++i) cube[BACK][2][i] = cube[LEFT][2][i];
                    for (int i = 0; i < 3; ++i) cube[LEFT][2][i] = cube[FRONT][2][i];
                    for (int i = 0; i < 3; ++i) cube[FRONT][2][i] = cube[RIGHT][2][i];
                    for (int i = 0; i < 3; ++i) cube[RIGHT][2][i] = temp[i];
                } else {
                    for (int i = 0; i < 3; ++i) temp[i] = cube[BACK][2][i];
                    for (int i = 0; i < 3; ++i) cube[BACK][2][i] = cube[RIGHT][2][i];
                    for (int i = 0; i < 3; ++i) cube[RIGHT][2][i] = cube[FRONT][2][i];
                    for (int i = 0; i < 3; ++i) cube[FRONT][2][i] = cube[LEFT][2][i];
                    for (int i = 0; i < 3; ++i) cube[LEFT][2][i] = temp[i];
                }
                break;
            case FRONT:
                if (isClockwise) {
                    for (int i = 0; i < 3; ++i) temp[i] = cube[UP][2][i];
                    for (int i = 0; i < 3; ++i) cube[UP][2][i] = cube[RIGHT][i][0];
                    for (int i = 0; i < 3; ++i) cube[RIGHT][i][0] = cube[DOWN][0][2 - i];
                    for (int i = 0; i < 3; ++i) cube[DOWN][0][2 - i] = cube[LEFT][2 - i][2];
                    for (int i = 0; i < 3; ++i) cube[LEFT][2 - i][2] = temp[i];
                } else {
                    for (int i = 0; i < 3; ++i) temp[i] = cube[UP][2][i];
                    for (int i = 0; i < 3; ++i) cube[UP][2][i] = cube[LEFT][2 - i][2];
                    for (int i = 0; i < 3; ++i) cube[LEFT][2 - i][2] = cube[DOWN][0][2 - i];
                    for (int i = 0; i < 3; ++i) cube[DOWN][0][2 - i] = cube[RIGHT][i][0];
                    for (int i = 0; i < 3; ++i) cube[RIGHT][i][0] = temp[i];
                }
                break;
            case BACK:
                if (isClockwise) {
                    for (int i = 0; i < 3; ++i) temp[i] = cube[UP][0][i];
                    for (int i = 0; i < 3; ++i) cube[UP][0][i] = cube[LEFT][2 - i][0];
                    for (int i = 0; i < 3; ++i) cube[LEFT][2 - i][0] = cube[DOWN][2][2 - i];
                    for (int i = 0; i < 3; ++i) cube[DOWN][2][2 - i] = cube[RIGHT][i][2];
                    for (int i = 0; i < 3; ++i) cube[RIGHT][i][2] = temp[i];
                } else {
                    for (int i = 0; i < 3; ++i) temp[i] = cube[UP][0][i];
                    for (int i = 0; i < 3; ++i) cube[UP][0][i] = cube[RIGHT][i][2];
                    for (int i = 0; i < 3; ++i) cube[RIGHT][i][2] = cube[DOWN][2][2 - i];
                    for (int i = 0; i < 3; ++i) cube[DOWN][2][2 - i] = cube[LEFT][2 - i][0];
                    for (int i = 0; i < 3; ++i) cube[LEFT][2 - i][0] = temp[i];
                }
                break;  
            default:
                cout << "Edge rotation for face " << face << " not implemented yet." << endl;
                break;
        }
    }

    void rotateFace(string face) {
        // Rotate the specified face of the cube
        cout << "Rotating face: " << face << endl;

        char faceChar = face[0]; // first character
        char modifier = '\0';    // default: no modifier

        // check if there's a second character
        if (face.length() > 1) {    
            modifier = face[1];      
        }

        if (faceChar != 'U' && faceChar != 'D' && faceChar != 'L' && faceChar != 'R' && faceChar != 'F' && faceChar != 'B') {
            cout << "Invalid Face Character: " << faceChar << endl;
            return;
        }

        if (modifier != '2' && modifier != '\'' && modifier != '\0') {
            cout << "Invalid Rotation Modifier: " << modifier << endl;
            return;
        }

        // Possible Rotations: U, U', U2, D, D', D2, L, L', L2, R, R', R2, F, F', F2, B, B', B2
        // Map char to enum
        Face f;
        switch(faceChar) {
            case 'U': f = UP; break;
            case 'D': f = DOWN; break;
            case 'L': f = LEFT; break;
            case 'R': f = RIGHT; break;
            case 'F': f = FRONT; break;
            case 'B': f = BACK; break;
            default:
                cout << "Rotation for face " << face << " not implemented yet." << endl;
            return;
        }

        // Determine rotation direction
        bool clockwise = (modifier == '\'');
        int turns;
        if (modifier == '2')
            turns = 2;
        else
            turns = 1;

        for (int i = 0; i < turns; i++) {
            rotateFaceHelper(f, clockwise);
            rotateEdgeHelper(f, clockwise);
        }
    }

    void display() {
        // Display the current state of the cube
        cout << "Displaying the cube state." << endl;

        cout << "         | " << cube[0][0][0] << " " << cube[0][0][1] << " " << cube[0][0][2] << " |" << endl;
        cout << "         | " << cube[0][1][0] << " " << cube[0][1][1] << " " << cube[0][1][2] << " |" << endl;
        cout << "         | " << cube[0][2][0] << " " << cube[0][2][1] << " " << cube[0][2][2] << " |" << endl;
        cout << "--------- ---------- --------------------" << endl;

        for (int i = 0; i < 3; ++i) {
            cout << cube[1][i][0] << " " << cube[1][i][1] << " " << cube[1][i][2] << " | ";
            cout << cube[2][i][0] << " " << cube[2][i][1] << " " << cube[2][i][2] << " | ";
            cout << cube[3][i][0] << " " << cube[3][i][1] << " " << cube[3][i][2] << " | ";
            cout << cube[4][i][0] << " " << cube[4][i][1] << " " << cube[4][i][2] << endl;
        } 

        cout << "--------- ---------- --------------------" << endl;
        cout << "         | " << cube[5][0][0] << " " << cube[5][0][1] << " " << cube[5][0][2] << " |" << endl;
        cout << "         | " << cube[5][1][0] << " " << cube[5][1][1] << " " << cube[5][1][2] << " |" << endl;
        cout << "         | " << cube[5][2][0] << " " << cube[5][2][1] << " " << cube[5][2][2] << " |" << endl;
    }  
};

int main() {
    RubikCube cube;
    string moves[] = {"F2", "U2", "R2", "D2", "F2", "D'", "L2", "F2", "L2", "R2", "U'", "F", "U", "B", "R'", "F'", "L", "D2", "B2", "R2", "F"};
    for (string& move : moves) {
        cube.rotateFace(move);
    }
    
    cube.display();
    return 0;
}