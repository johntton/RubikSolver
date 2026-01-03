#ifndef CUBESCANNER_H
#define CUBESCANNER_H

#include <opencv2/opencv.hpp>
#include <array>

class CubeScanner {
public:
    CubeScanner(int cameraIndex);
    cv::Mat initCamera();
    void drawGrid(cv::Mat& frame);
    std::array<std::array<cv::Mat, 3>, 3> extractROIs(cv::Mat& frame);
    void saveFace(const std::array<std::array<cv::Mat, 3>, 3>& faceROI);
    bool finished() const;
    const std::array<std::array<std::array<cv::Mat, 3>, 3>, 6>& getAllFaces() const;

private:
    cv::Mat img;
    cv::VideoCapture cap;
    int currentFace;
    int gridSize;
    int cellSize;
    std::array<std::array<std::array<cv::Mat, 3>, 3>, 6> allFaces;
};

#endif
