#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

// Global Variables
cv::Mat img;
int gridSize = 400;
int cellSize = gridSize / 3;

class CubeScanner {
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
                cv::Rect roi(roiX, roiY, cellSize, cellSize);
                roiArray[row][col] = frame(roi).clone();
            }
        }
        return roiArray;
    }
};

int main() {
    cv::VideoCapture cap(1);
    CubeScanner Scanner;
    std::array<std::array<cv::Mat, 3>, 3> roiArray;

    while (true) {
        cap.read(img);
        if (img.empty()) break;

        int w = img.cols;
        int h = img.rows;
        int side = std::min(w, h); // Find the smaller dimension

        // Center crop
        int x = (w - side) / 2;
        int y = (h - side) / 2;

        cv::Rect roi(x, y, side, side);
        cv::Mat square = img(roi);

        Scanner.drawGrid(square);
        roiArray = Scanner.extractROIs(square);
        cv::imshow("Video", square);

        // Test that each cell is correct
        cv::imshow("0, 0", roiArray[0][0]);
        cv::imshow("0, 1", roiArray[0][1]);
        cv::imshow("0, 2", roiArray[0][2]);

        cv::imshow("1, 0", roiArray[1][0]);
        cv::imshow("1, 1", roiArray[1][1]);
        cv::imshow("1, 2", roiArray[1][2]);

        cv::imshow("2, 0", roiArray[2][0]);
        cv::imshow("2, 1", roiArray[2][1]);
        cv::imshow("2, 2", roiArray[2][2]);

        if (cv::waitKey(30) == 'q') break;
    }

    return 0;
}