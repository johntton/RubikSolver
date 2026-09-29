#include "ColorDetector.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

namespace {
    // Standard WCA sticker colors (approximate sRGB), used to seed k-means so
    // clusters start at known anchors instead of a random/++ split. Random
    // seeding is what let clusters merge or split differently under different
    // lighting (e.g. white and yellow drifting into the same cluster).
    struct CanonicalColor { char code; cv::Vec3b bgr; };
    const std::array<CanonicalColor, 6> kCanonicalColors = {{
        {'W', {255, 255, 255}},
        {'O', {0, 88, 255}},
        {'G', {72, 155, 0}},
        {'R', {52, 18, 183}},
        {'B', {173, 70, 0}},
        {'Y', {0, 213, 255}},
    }};

    std::array<cv::Vec3f, 6> canonicalLabCenters() {
        std::array<cv::Vec3f, 6> result;
        for (int i = 0; i < 6; ++i) {
            cv::Mat swatch(1, 1, CV_8UC3, cv::Scalar(
                kCanonicalColors[i].bgr[0],
                kCanonicalColors[i].bgr[1],
                kCanonicalColors[i].bgr[2]));
            cv::Mat lab;
            cv::cvtColor(swatch, lab, cv::COLOR_BGR2Lab);
            auto p = lab.at<cv::Vec3b>(0, 0);
            result[i] = cv::Vec3f(p[0] / 255.0f, (p[1] - 128.0f) / 127.0f, (p[2] - 128.0f) / 127.0f);
        }
        return result;
    }
}

ColorDetector::ColorDetector() {
    std::cout << "=============================\n";
    std::cout << "     Rubik's Cube Solver     \n";
    std::cout << "=============================\n";
}

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

    // Seed each sample's initial label from its nearest canonical color instead
    // of letting k-means pick random/++ starting centers. This makes the
    // clustering converge to the same 6 color identities run-to-run instead of
    // depending on how badly lighting has shifted the raw pixel cloud.
    auto canonical = canonicalLabCenters();
    cv::Mat initialLabels(N, 1, CV_32S);
    for (int i = 0; i < N; ++i) {
        float bestDist = FLT_MAX;
        int bestC = 0;
        for (int c = 0; c < 6; ++c) {
            float dL = colors[i][0] - canonical[c][0];
            float dA = colors[i][1] - canonical[c][1];
            float dB = colors[i][2] - canonical[c][2];
            float dist = dL*dL + dA*dA + dB*dB;
            if (dist < bestDist) {
                bestDist = dist;
                bestC = c;
            }
        }
        initialLabels.at<int>(i, 0) = bestC;
    }

    labels = initialLabels;
    centers = cv::Mat();

    // attempts must be 1 with KMEANS_USE_INITIAL_LABELS: OpenCV only honors the
    // supplied labels on the first attempt and falls back to random centers for
    // any further ones, which would defeat the seeding above.
    cv::kmeans(
        data,
        6,
        labels,
        cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1.0),
        1,
        cv::KMEANS_USE_INITIAL_LABELS,
        centers
    );

    std::vector<int> counts(centers.rows, 0);
    for (int i = 0; i < labels.rows; ++i)
        counts[labels.at<int>(i, 0)]++;

    std::cout << "\nK-means cluster centers:\n";
    for (int i = 0; i < centers.rows; i++) {
        float Ln = centers.at<float>(i,0);
        float An = centers.at<float>(i,1);
        float Bn = centers.at<float>(i,2);

        std::cout << "Center " << i << " (seeded " << kCanonicalColors[i].code << "): "
                  << "L=" << Ln * 255.0f << ", "
                  << "A=" << (An * 127.0f + 128.0f) << ", "
                  << "B=" << (Bn * 127.0f + 128.0f) << ", "
                  << "pixels=" << counts[i] << "\n";
    }
}

void ColorDetector::showClusterSwatches() const {
    if (centers.empty()) {
        std::cout << "No cluster centers yet; run runKMeans() first.\n";
        return;
    }

    std::vector<int> counts(centers.rows, 0);
    for (int i = 0; i < labels.rows; ++i)
        counts[labels.at<int>(i, 0)]++;

    const int patchSize = 120;
    cv::Mat board(patchSize + 30, patchSize * centers.rows, CV_8UC3, cv::Scalar(30, 30, 30));

    for (int c = 0; c < centers.rows; ++c) {
        cv::Mat lab(1, 1, CV_8UC3, cv::Scalar(
            cv::saturate_cast<uchar>(centers.at<float>(c,0) * 255.0f),
            cv::saturate_cast<uchar>(centers.at<float>(c,1) * 127.0f + 128.0f),
            cv::saturate_cast<uchar>(centers.at<float>(c,2) * 127.0f + 128.0f)));
        cv::Mat bgr;
        cv::cvtColor(lab, bgr, cv::COLOR_Lab2BGR);
        cv::Vec3b color = bgr.at<cv::Vec3b>(0, 0);

        cv::Rect patch(c * patchSize, 0, patchSize, patchSize);
        board(patch).setTo(cv::Scalar(color[0], color[1], color[2]));

        cv::putText(board, std::to_string(counts[c]) + "px", cv::Point(c * patchSize + 5, patchSize + 20),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }

    std::cout << "Cluster swatches: if two patches look like the same color, or a color is "
                 "missing, the clustering has already failed before calibration.\n";
    std::cout << "Press any key to continue to calibration.\n";
    cv::imshow("Cluster Swatches", board);
    cv::waitKey(0);
    cv::destroyWindow("Cluster Swatches");
}

void ColorDetector::assignClustersToCubeColors(const std::array<std::array<std::array<cv::Mat,3>,3>,6>& faces) {
    clusterToColor.clear();

    // Scan order is fixed (see kFaceInstructions in main.cpp): U,R,F,D,L,B under
    // the standard WCA color scheme, so each face index's center color is known
    // ahead of time - no need to ask the user to name it face by face.
    static const char kFaceOrder[6] = {'W', 'R', 'G', 'Y', 'O', 'B'};
    std::map<int, int> clusterToFace;

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
        float secondDist = FLT_MAX;
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
                secondDist = bestDist;
                bestDist = dist;
                bestCluster = c;
            } else if (dist < secondDist) {
                secondDist = dist;
            }
        }

        char mapped = kFaceOrder[f];
        clusterToColor[bestCluster] = mapped;
        std::cout << "Face " << f << " center -> cluster " << bestCluster
                  << " -> " << mapped << " (dist=" << bestDist << ")\n";

        // Two independent red flags that the auto-assignment above is wrong:
        // the nearest cluster is barely closer than the next-best one (colors
        // not well separated, e.g. white/yellow), or two different faces
        // matched the same cluster (a merged cluster, or a face scanned out of
        // the expected order/orientation). Neither aborts the scan - just
        // flags it, since the swatch preview already ran before this step.
        if (secondDist != FLT_MAX && bestDist > 0.6f * secondDist) {
            std::cout << "  WARNING: closest cluster isn't clearly separated from "
                         "the next best (dist=" << bestDist << " vs " << secondDist
                      << "). Double-check the swatch preview.\n";
        }
        if (clusterToFace.count(bestCluster)) {
            std::cout << "  WARNING: cluster " << bestCluster << " was already "
                         "matched to face " << clusterToFace[bestCluster]
                      << ". Clustering likely merged two colors.\n";
        }
        clusterToFace[bestCluster] = f;
    }
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
