#include "ColorDetector.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <set>
#include <cmath>

ColorDetector::ColorDetector() {}

void ColorDetector::buildTrainingData(const std::array<std::array<std::array<cv::Mat, 3>, 3>, 6>& stickerColorInput) {
    colors.clear();
    stickerColor = stickerColorInput;

    for (int face = 0; face < 6; ++face) {
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                cv::Mat roiBGR = stickerColor[face][row][col];
                if (roiBGR.empty()) continue;

                cv::Mat smooth;
                cv::medianBlur(roiBGR, smooth, 3);

                int x0 = smooth.cols * 25 / 100;
                int y0 = smooth.rows * 25 / 100;
                int w  = std::max(1, smooth.cols * 50 / 100);
                int h  = std::max(1, smooth.rows * 50 / 100);

                cv::Rect inner(x0, y0, w, h);
                cv::Mat sample = smooth(inner);

                cv::Mat roiLAB;
                cv::cvtColor(sample, roiLAB, cv::COLOR_BGR2Lab);

                for (int y = 0; y < roiLAB.rows; y += 2) {
                    for (int x = 0; x < roiLAB.cols; x += 2) {
                        auto pixel = roiLAB.at<cv::Vec3b>(y, x);

                        float L = pixel[0];
                        float A = pixel[1];
                        float B = pixel[2];

                        if (L < 20) continue;

                        colors.push_back(cv::Vec3f(
                            L / 255.0f,
                            (A - 128.0f) / 127.0f,
                            (B - 128.0f) / 127.0f
                        ));
                    }
                }
            }
        }
    }
}

void ColorDetector::runKMeans() {
    if (colors.empty()) {
        std::cout << "No training data! Did you run buildTrainingData()?\n";
        return;
    }

    int N = colors.size();
    cv::Mat data(N, 3, CV_32F);

    for (int i = 0; i < N; i++) {
        data.at<float>(i, 0) = colors[i][0];
        data.at<float>(i, 1) = colors[i][1];
        data.at<float>(i, 2) = colors[i][2];
    }

    labels = cv::Mat();
    centers = cv::Mat();

    cv::kmeans(
        data,
        6,
        labels,
        cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1.0),
        5,
        cv::KMEANS_PP_CENTERS,
        centers
    );

    std::cout << "\nK-means cluster centers:\n";
    for (int i = 0; i < centers.rows; i++) {
        float Ln = centers.at<float>(i,0);
        float An = centers.at<float>(i,1);
        float Bn = centers.at<float>(i,2);

        std::cout << "Center " << i << ": "
                  << "L=" << Ln * 255.0f << ", "
                  << "A=" << (An * 127.0f + 128.0f) << ", "
                  << "B=" << (Bn * 127.0f + 128.0f) << "\n";
    }
}

void ColorDetector::assignClustersToCubeColors(const std::array<std::array<std::array<cv::Mat,3>,3>,6>& faces) {
    clusterToColor.clear();

    std::cout << "Canonical mapping incomplete; entering interactive calibration.\n";

    const std::string win = "Center Calibration";
    cv::namedWindow(win, cv::WINDOW_AUTOSIZE);
    std::set<char> usedColors;

    for (int f = 0; f < 6; ++f) {
        cv::Mat roi = faces[f][1][1];
        if (roi.empty()) continue;

        int x0 = roi.cols * 40 / 100;
        int y0 = roi.rows * 40 / 100;
        int w  = std::max(1, roi.cols * 50 / 100);
        int h  = std::max(1, roi.rows * 50 / 100);

        cv::Rect inner(x0, y0, w, h);
        cv::Mat tight = roi(inner).clone();

        cv::Mat lab;
        cv::cvtColor(tight, lab, cv::COLOR_BGR2Lab);
        cv::Scalar m = cv::mean(lab);

        float L = m[0];
        float A = m[1];
        float B = m[2];

        float Ln = L / 255.0f;
        float An = (A - 128.0f) / 127.0f;
        float Bn = (B - 128.0f) / 127.0f;

        float bestDist = FLT_MAX;
        int bestCluster = -1;

        for (int c = 0; c < centers.rows; ++c) {
            float cL = centers.at<float>(c,0);
            float cA = centers.at<float>(c,1);
            float cB = centers.at<float>(c,2);

            float dL = Ln - cL;
            float dA = An - cA;
            float dB = Bn - cB;

            float dist = sqrtf(dL*dL + dA*dA + dB*dB);

            if (dist < bestDist) {
                bestDist = dist;
                bestCluster = c;
            }
        }

        cv::imshow(win, tight);
        std::cout << "Face " << f << ": press w,o,g,r,b,y.\n";

        while (true) {
            int key = cv::waitKey(0);
            if (key < 0) continue;

            char c = tolower((char)key);
            char mapped = 0;

            if (c=='w') mapped='W';
            if (c=='o') mapped='O';
            if (c=='g') mapped='G';
            if (c=='r') mapped='R';
            if (c=='b') mapped='B';
            if (c=='y') mapped='Y';

            if (!mapped) {
                std::cout << "Invalid key.\n";
                continue;
            }
            if (usedColors.count(mapped)) {
                std::cout << "Already assigned.\n";
                continue;
            }

            clusterToColor[bestCluster] = mapped;
            usedColors.insert(mapped);
            std::cout << "Assigned cluster " << bestCluster << " -> " << mapped << "\n";
            break;
        }
    }

    cv::destroyWindow(win);
}

std::array<std::array<std::array<char,3>,3>,6>
ColorDetector::classifyAllFaces(const std::array<std::array<std::array<cv::Mat,3>,3>,6>& faces) {
    std::array<std::array<std::array<char,3>,3>,6> result{};

    for (int f = 0; f < 6; f++)
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                result[f][r][c] = classifyROI(faces[f][r][c]);

    return result;
}

char ColorDetector::classifyROI(const cv::Mat& roi) {
    if (roi.empty() || centers.empty())
        return '?';

    cv::Mat small;
    cv::medianBlur(roi, small, 3);

    int x0 = small.cols * 25 / 100;
    int y0 = small.rows * 25 / 100;
    int w  = std::max(1, small.cols * 50 / 100);
    int h  = std::max(1, small.rows * 50 / 100);

    cv::Rect inner(x0, y0, w, h);
    cv::Mat sample = small(inner);

    cv::Mat lab;
    cv::cvtColor(sample, lab, cv::COLOR_BGR2Lab);

    std::vector<int> counts(centers.rows, 0);

    for (int y = 0; y < lab.rows; y++) {
        for (int x = 0; x < lab.cols; x++) {
            auto p = lab.at<cv::Vec3b>(y, x);

            float L = p[0];
            float A = p[1];
            float B = p[2];

            if (L < 20) continue;

            float Ln = L / 255.0f;
            float An = (A - 128.0f) / 127.0f;
            float Bn = (B - 128.0f) / 127.0f;

            float bestDist = FLT_MAX;
            int bestC = -1;

            for (int c = 0; c < centers.rows; c++) {
                float cL = centers.at<float>(c,0);
                float cA = centers.at<float>(c,1);
                float cB = centers.at<float>(c,2);

                float dL = Ln - cL;
                float dA = An - cA;
                float dB = Bn - cB;

                float dist = sqrtf(dL*dL + dA*dA + dB*dB);

                if (dist < bestDist) {
                    bestDist = dist;
                    bestC = c;
                }
            }

            if (bestC >= 0)
                counts[bestC]++;
        }
    }

    int bestCluster = -1;
    int bestCount = 0;

    for (int i = 0; i < counts.size(); i++) {
        if (counts[i] > bestCount) {
            bestCount = counts[i];
            bestCluster = i;
        }
    }

    if (bestCluster < 0)
        return '?';

    if (!clusterToColor.count(bestCluster))
        return '?';

    return clusterToColor[bestCluster];
}
