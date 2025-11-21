#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

// Global Variables
extern cv::Mat img;
int gridSize = 300;
int cellSize = gridSize / 3;

class CubeScanner {
    private: 
        std::array<std::array<std::array<cv::Mat, 3>, 3>, 6> allFaces;
        int currentFace = 0;

    public:
    CubeScanner() {}

    void drawGrid(cv::Mat& frame) {
        // Draw 3x3 Grid on frame

        // Takes width and height of frame
        int frameWidth = frame.cols;
        int frameHeight = frame.rows;

        // Center of Grid
        int startX = (frameWidth - gridSize) / 2;
        int startY = (frameHeight - gridSize) / 2;

        // Draws 4 horizontal and vertical lines to create a 3x3 grid
        for (int i = 0; i < 4; ++i) {
            int x = startX + i * cellSize;
            line(frame, cv::Point(x, startY), cv::Point(x, startY + gridSize), cv::Scalar(255, 255, 255), 2);
        }
        
        for (int i = 0; i < 4; ++i) {
            int y = startY + i * cellSize;
            line(frame, cv::Point(startX, y), cv::Point(startX + gridSize, y), cv::Scalar(255, 255, 255), 2);
        }
    }

    std::array<std::array<cv::Mat, 3>, 3> extractROIs(cv::Mat& frame) {
        std::array<std::array<cv::Mat, 3>, 3> roiArray;
        int frameWidth = frame.cols;
        int frameHeight = frame.rows;

        int startX = (frameWidth - gridSize) / 2;
        int startY = (frameHeight - gridSize) / 2;

        // Extracts ROI's based on cells in 3x3 grid
        for(int row = 0; row < 3; ++row) {
            for(int col = 0; col < 3; ++col) {
                int roiX = startX + col * cellSize;
                int roiY = startY + row * cellSize;
                int roiWidth = cellSize;
                int roiHeight = cellSize;

                // Takes ROI and stores in array
                cv::Rect roi(roiX, roiY, roiWidth, roiHeight);
                roiArray[row][col] = frame(roi).clone();
            }
        }
        return roiArray;
    }

    void saveFace(const std::array<std::array<cv::Mat, 3>, 3>& faceROI) {
        if (currentFace < 6) {
            allFaces[currentFace] = faceROI;
            std::cout << "Face Saved" << std::endl;
            currentFace++;
        }
    }

    bool finished() const {
        return currentFace == 6;
    }

    const auto& getAllFaces() const {
        return allFaces;
    }
};