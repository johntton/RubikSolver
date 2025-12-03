#include "CubeScanner.h"

CubeScanner::CubeScanner()
    : currentFace(0), gridSize(300), cellSize(gridSize / 3), cap(1)  {}

cv::Mat CubeScanner::initCamera() {
    if (!cap.isOpened()) {
        std::cerr << "Camera failed to open!\n";
        return cv::Mat();
    }

    cap.read(img);
    if (img.empty()) {
        std::cerr << "Frame grab failed!\n";
        return cv::Mat();
    }

    int w = img.cols;
    int h = img.rows;
    int side = std::min(w, h);

    int x = (w - side) / 2;
    int y = (h - side) / 2;

    cv::Rect roi(x, y, side, side);
    cv::Mat square = img(roi).clone(); 

    return square;
}

void CubeScanner::drawGrid(cv::Mat& frame) {
    int frameWidth = frame.cols;
    int frameHeight = frame.rows;

    int startX = (frameWidth - gridSize) / 2;
    int startY = (frameHeight - gridSize) / 2;

    for (int i = 0; i < 4; ++i) {
        int x = startX + i * cellSize;
        cv::line(frame, {x, startY}, {x, startY + gridSize}, {255,255,255}, 2);
    }

    for (int i = 0; i < 4; ++i) {
        int y = startY + i * cellSize;
        cv::line(frame, {startX, y}, {startX + gridSize, y}, {255,255,255}, 2);
    }
}

std::array<std::array<cv::Mat, 3>, 3> CubeScanner::extractROIs(cv::Mat& frame) {
    std::array<std::array<cv::Mat, 3>, 3> roiArray;

    int frameWidth = frame.cols;
    int frameHeight = frame.rows;

    int startX = (frameWidth - gridSize) / 2;
    int startY = (frameHeight - gridSize) / 2;

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int roiX = startX + col * cellSize;
            int roiY = startY + row * cellSize;

            cv::Rect roi(roiX, roiY, cellSize, cellSize);
            roiArray[row][col] = frame(roi).clone();
        }
    }
    return roiArray;
}

void CubeScanner::saveFace(
    const std::array<std::array<cv::Mat, 3>, 3>& faceROI
) {
    if (currentFace < 6) {
        allFaces[currentFace] = faceROI;
        currentFace++;
        std::cout << "Face saved.\n";
    }
}

bool CubeScanner::finished() const {
    return currentFace == 6;
}

const std::array<std::array<std::array<cv::Mat, 3>, 3>, 6>&
CubeScanner::getAllFaces() const {
    return allFaces;
}
