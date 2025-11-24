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
#include "include/ColorDetector.h"
#include "include/CubeScanner.h"
#include "include/CubeSolver.h"

int main() {
    cv::Mat img;
    cv::VideoCapture cap(1);

    CubeScanner scanner;
    ColorDetector colorDetect;
    Solver cubeSolver;

    while (true) {
        cap.read(img);
        if (img.empty()) break;

        int w = img.cols;
        int h = img.rows;
        int side = std::min(w, h);
        
        int x = (w - side) / 2;
        int y = (h - side) / 2;
      
        cv::Rect roi(x, y, side, side);
        cv::Mat square = img(roi);

        // Draw grid + extract preview
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

        std::cout << "\nFinal Cube State:\n";
        for (int f = 0; f < 6; f++) {
            std::cout << "Face " << f << ":\n";
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    std::cout << cubeState[f][r][c] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }

        std::string kociembaString = cubeSolver.convertToString(cubeState);
        std::string sol = cubeSolver.runKociemba(kociembaString);
        std::cout << "Solution: " << sol << std::endl;
    
    return 0;
};