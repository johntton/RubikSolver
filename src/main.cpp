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