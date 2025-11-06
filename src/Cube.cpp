#include <iostream>
#include <array>
using namespace std;

class RubikCube {
    public:
    
    string cube[6][3][3]; // cube[face][row(down)][col(right)]
    
    RubikCube() {
        // Initialize a solved Rubik's Cube
        cout << "Initialized a solved Rubik's Cube." << endl;

        string colors[6] = {"W", "O", "G", "R", "B", "Y"}; // White, Orange, Green, Red, Blue, Yellow

        // Fills each face with its respective color
        for (int f = 0; f < 6; ++f) {
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 3; ++c) {
                    cube[f][r][c] = colors[f];
                }
            }
        }
    }

    void rotateFace(string face) {
        // Rotate the specified face of the cube
        cout << "Rotating face: " << face << endl;

        // Possible Rotations: U, U', U2, D, D', D2, L, L', L2, R, R', R2, F, F', F2, B, B', B2 
        if(face == "U") {
            // Rotate Up face clockwise
            
            // Rotate the face itself
            
        } else if(face == "U'") {

        } else if(face == "U2") {

        } else if(face == "D") {

        } else if(face == "D'") {

        } else if(face == "D2") {

        } else if(face == "L") {

        } else if(face == "L'") {

        } else if(face == "L2") {

        } else if(face == "R") {

        } else if(face == "R'") {

        } else if(face == "R2") {

        } else if(face == "F") {

        } else if(face == "F'") {

        } else if(face == "F2") {

        } else if(face == "B") {

        } else if(face == "B'") {

        } else if(face == "B2") {

        } else {
            cout << "Rotation Not Possible" << endl;
        }
    }

    void display() {
        // Display the current state of the cube
        cout << "Displaying the cube state." << endl;

        cout << "      " << cube[0][0][0] << " " << cube[0][0][1] << " " << cube[0][0][2] << endl;
        cout << "      " << cube[0][1][0] << " " << cube[0][1][1] << " " << cube[0][1][2] << endl;
        cout << "      " << cube[0][2][0] << " " << cube[0][2][1] << " " << cube[0][2][2] << endl;

        for (int i = 0; i < 3; ++i) {
            cout << cube[1][i][0] << " " << cube[1][i][1] << " " << cube[1][i][2] << " ";
            cout << cube[2][i][0] << " " << cube[2][i][1] << " " << cube[2][i][2] << " ";
            cout << cube[3][i][0] << " " << cube[3][i][1] << " " << cube[3][i][2] << " ";
            cout << cube[4][i][0] << " " << cube[4][i][1] << " " << cube[4][i][2] << endl;
        } 

        cout << "      " << cube[5][0][0] << " " << cube[5][0][1] << " " << cube[5][0][2] << endl;
        cout << "      " << cube[5][1][0] << " " << cube[5][1][1] << " " << cube[5][1][2] << endl;
        cout << "      " << cube[5][2][0] << " " << cube[5][2][1] << " " << cube[5][2][2] << endl;
    }  
};

int main() {
    RubikCube cube;
    cube.display();
    cube.rotateFace("U'"); 
    
    return 0;
}