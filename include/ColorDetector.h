#ifndef COLORDETECTOR_H
#define COLORDETECTOR_H

#include <opencv2/opencv.hpp>
#include <array>
#include <vector>
#include <map>

class ColorDetector {
public:
    ColorDetector();
    void buildTrainingData(const std::array<std::array<std::array<cv::Mat, 3>, 3>, 6>& stickerColor);
    void runKMeans();
    void assignClustersToCubeColors(const std::array<std::array<std::array<cv::Mat,3>,3>,6>& faces);
    std::array<std::array<std::array<char,3>,3>,6> classifyAllFaces(const std::array<std::array<std::array<cv::Mat,3>,3>,6>& faces);
    char classifyROI(const cv::Mat& roi);
private:
    cv::Mat centers;
    cv::Mat labels;
    std::vector<cv::Vec3f> colors;
    std::array<std::array<std::array<cv::Mat, 3>, 3>, 6> stickerColor;
    std::map<int, char> clusterToColor;
};

#endif