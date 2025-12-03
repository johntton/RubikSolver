# Rubik’s Cube Vision Solver

**Language:** C++  
**Libraries:** OpenCV, Kociemba Cube Solver (C implementation)  

This project is an end-to-end **Rubik’s Cube vision solver** built in C++.  
It uses a webcam to scan a physical Rubik’s Cube, detects sticker colors with a k-means–based color classifier, converts the cube state to Kociemba format, and computes an efficient solution.

---

## Features
- Real-time camera capture using OpenCV
- Automatic color detection and classification
- Full 54-sticker cube state reconstruction
- Integrates the C implementation from [`muodov/kociemba`](https://github.com/muodov/kociemba). 
- Modular architecture (Scanner, Detector, Solver)

---

## Requirements
- CMake ≥ 3.10
- C++17 compiler
- OpenCV installed on your system (4.x recommended)

macOS OpenCV installation (Homebrew):<br>
`brew install opencv`<br>
`brew link opencv`

---

## Build Instructions (macOS/Linux)

Clone the repo:<br>
`git clone https://github.com/johntton/RubikSolver.git`<br>
`cd RubikSolver`

Create a build directory:<br>
`mkdir build`<br>
`cd build`

Configure with CMake:<br>
`cmake ..`

Build the project:<br>
`make`

Run the executable:<br>
`./RubikSolver`

---

 ## How It Works
1. Captures image from camera  
2. Detects sticker colors (HSV)  
3. Builds cube state  
4. Runs Kociemba solver  
5. Prints move sequence  

---

## License
MIT License.
