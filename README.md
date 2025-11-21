# Rubik’s Cube Vision Solver

**Language:** C++  
**Libraries:** OpenCV, Kociemba Cube Solver (C implementation)  

This project is an end-to-end **Rubik’s Cube vision solver** built in C++.  
It uses a webcam to scan a physical Rubik’s Cube, detects sticker colors with a k-means–based color classifier, converts the cube state to Kociemba format, and computes an efficient solution.

---

## 🔧 Features

- **Live Webcam Capture**
  - Grabs frames from a webcam.
  - Center-crops to a square.
  - Draws a 3×3 grid overlay so each sticker aligns with a cell.
  - Press **SPACE** to save the current face.
  - Captures all **6 faces** of the cube.

- **Cube Scanning Order (Kociemba-compatible)**
  Scan faces in this order:
  1. Up (U)
  2. Right (R)
  3. Front (F)
  4. Down (D)
  5. Left (L)
  6. Back (B)

- **Color Detection (k-means)**
  - Extracts pixel samples from each sticker region (ROIs).
  - Converts to **CIELAB** color space to reduce sensitivity to lighting.
  - Runs **k-means (K = 6)** to cluster colors.
  - Interactive calibration:
    - Shows each face center and asks the user to label it as `W/O/G/R/B/Y`.
    - Builds a mapping: *cluster index → cube color*.
  - Classifies each of the 54 stickers via majority vote of nearest cluster center.

- **Cube Representation**
  - Cube stored as `std::array<std::array<std::array<char, 3>, 3>, 6>`.
  - Faces ordered to match Kociemba’s required order: **U, R, F, D, L, B**.
  - Converts this 3D array into a 54-character cube string (e.g. `"UUUUUUUUURRRRRRRRR..."`).

- **Solving with Kociemba’s Algorithm**
  - Integrates the C implementation from [`muodov/kociemba`](https://github.com/muodov/kociemba).
  - Uses precomputed / cached pruning tables (stored in a `kociemba_cache` directory).
  - Computes near-optimal solutions typically around 20 moves.
  - Example solution:
    ```text
    D2 R' D' F2 B D R2 D2 R' F2 D' F2 U' B2 L2 U2 D R2 U
    ```

---

## 🗂 Project Structure

```text
RubikSolver/
├── src/
│   ├── main.cpp             # Main pipeline: capture → scan → classify → solve
│   ├── CubeScanner.cpp      # Grid drawing + ROI extraction + face storage
│   ├── ColorDetector.cpp    # k-means color detection and classification
│   └── CubeSolver.cpp       # Kociemba integration + cube string conversion
│
├── include/
│   ├── CubeScanner.h
│   ├── ColorDetector.h
│   └── CubeSolver.h
│
├── kociemba/                # Third-party Kociemba solver (C)
│   ├── coordcube.c
│   ├── cubiecube.c
│   ├── facecube.c
│   ├── prunetable_helpers.c
│   ├── search.c
│   ├── solve.c
│   └── include/
│       ├── search.h
│       ├── color.h
│       ├── facecube.h
│       ├── cubiecube.h
│       └── ...
│
├── CMakeLists.txt
└── README.md