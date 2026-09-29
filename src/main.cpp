#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <string>
#include <filesystem>
#include "ColorDetector.h"
#include "CubeScanner.h"
#include "CubeSolver.h"
#include "Cube.h"

// Scan order and orientation the rest of the pipeline assumes (see Solver::order
// and the Kociemba facelet net): U, R, F, D, L, B, using the standard WCA color
// scheme (White=U, Red=R, Green=F, Yellow=D, Orange=L, Blue=B). Every step is
// stated as a target orientation (which face points at the camera, which face
// is on top) rather than a motion, since there's no single "correct" way to
// move the cube there - only one correct orientation once it arrives.
static const char* kFaceInstructions[6] = {
    "Face 1/6: Show WHITE to the camera, with BLUE on top",
    "Face 2/6: Keep White on top - turn cube to show RED",
    "Face 3/6: Keep White on top - show GREEN to the camera",
    "Face 4/6: Show YELLOW to the camera, with GREEN on top",
    "Face 5/6: Keep White on top - turn cube to show ORANGE",
    "Face 6/6: Keep White on top - turn cube to show BLUE"
};

void drawScanInstructions(cv::Mat& frame, int faceIndex) {
    if (faceIndex < 0 || faceIndex >= 6) return;

    cv::rectangle(frame, cv::Point(0, 0), cv::Point(frame.cols, 60), cv::Scalar(0, 0, 0), cv::FILLED);
    cv::putText(frame, kFaceInstructions[faceIndex], cv::Point(10, 25),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(frame, "Press SPACE to capture, Q to quit", cv::Point(10, 50),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 200, 200), 1);
}

int main() {
    std::cout << "Camera Index: " << std::endl;
    int cameraIndex;
    std::cin >> cameraIndex;

    CubeScanner scanner(cameraIndex);
    ColorDetector colorDetect;
    Solver cubeSolver;

    while (true) {
        auto square = scanner.initCamera();
        if (square.empty()) {
            if (cv::waitKey(30) == 'q') {
                return 0;
            }
            continue;
        }

        scanner.drawGrid(square);
        drawScanInstructions(square, scanner.getCurrentFace());

        cv::imshow("Video", square);

        char key = cv::waitKey(1);

        if (key == ' ') {
            auto faceROI = scanner.extractROIs(square);
            scanner.saveFace(faceROI);
        }

        if (scanner.finished()) {
            break;
        }

        if (key == 'q') return 0;
    }
    
    const auto& faces = scanner.getAllFaces();

    colorDetect.buildTrainingData(faces);
    colorDetect.runKMeans();
    colorDetect.showClusterSwatches();
    colorDetect.assignClustersToCubeColors(faces);
    auto cubeState = colorDetect.classifyAllFaces(faces);
    cubeSolver.printCube(cubeState);

    std::string kociembaString = cubeSolver.convertToString(cubeState);
    std::string sol = cubeSolver.runKociemba(kociembaString);
    std::cout << "Solution: " << sol << std::endl;

    if (sol.rfind("ERROR", 0) == 0) {
        std::cout << "Skipping verification: solver did not return a solution.\n";
    } else {
        RubikCube verifyCube;
        verifyCube.loadState(cubeState);
        verifyCube.applyMoves(sol);

        if (verifyCube.isSolved()) {
            std::cout << "Verified: applying the solution solves the scanned cube.\n";
        } else {
            std::cout << "WARNING: applying the solution did NOT solve the scanned cube.\n";
            std::cout << "This usually means a misread sticker or a face scanned out of order.\n";
            verifyCube.display();
        }
    }

    return 0;
};