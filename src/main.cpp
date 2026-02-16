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

int main() {
    std::cout << "Camera Index: " << std::endl;
    int cameraIndex;
    std::cin >> cameraIndex;

    CubeScanner scanner(cameraIndex);
    ColorDetector colorDetect;
    Solver cubeSolver;

    while (true) {
        auto square = scanner.initCamera();
        scanner.drawGrid(square);
        auto faceROI = scanner.extractROIs(square);

        cv::imshow("Video", square);

        char key = cv::waitKey(1);

        if (key == ' ') {
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

    return 0;
};