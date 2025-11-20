#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

cv::Mat img, roiBGR, roiLAB;
std::vector<cv::Vec3f> colors;
std::array<std::array<std::array<cv::Mat, 3>, 3>, 6> stickerColor;
std::map<int, char> clusterToColor;

class ColorDetector{
    public:
    cv::Mat centers;
    cv::Mat labels;

    ColorDetector() {}

    // Collects HSV values from all the ROI's and stores in colors array
    void buildTrainingData(const std::array<std::array<std::array<cv::Mat, 3>, 3>, 6>& stickerColor) {
        // Clear previous samples
        colors.clear();

        // Reduces the ROI, detects dominant color, standardizes HSV values
        for (int face = 0; face < 6; ++face) {
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    roiBGR = stickerColor[face][row][col];
                    if (roiBGR.empty()) continue;

                    // Smooths out image; reduces glares, shadows, weird edge spikes
                    cv::Mat smooth;
                    cv::medianBlur(roiBGR, smooth, 3);

                    // Moves in 25% from each edge, takes middle 50% for sampling
                    int x0 = smooth.cols * 25 / 100;
                    int y0 = smooth.rows * 25 / 100;
                    int w  = std::max(1, smooth.cols * 50 / 100);
                    int h  = std::max(1, smooth.rows * 50 / 100);
                    cv::Rect inner(x0, y0, w, h);
                    cv::Mat sample = smooth(inner);

                    cv::cvtColor(sample, roiLAB, cv::COLOR_BGR2Lab);

                    // Extracts HSV values from each pixel sample
                    for (int y = 0; y < roiLAB.rows; y += 2) {
                        for (int x = 0; x < roiLAB.cols; x += 2) {
                            auto pixel = roiLAB.at<cv::Vec3b>(y, x);
                            
                            // k-means requires floating point data
                            float L = static_cast<float>(pixel[0]); 
                            float A = static_cast<float>(pixel[1]);
                            float B = static_cast<float>(pixel[2]);

                            // Filters out very dark pixels
                            if (L < 20) continue;

                            // Normalize to 0-1
                            colors.push_back(cv::Vec3f(L / 255.0f, (A - 128.0f) / 127.0f, (B - 128.0f) / 127.0f));
                        }
                    }
                }
            }
        }
    }

    // Conducts k-Means model
    void runKMeans() {
        if (colors.empty()) {
            std::cout << "No training data! Did you run buildTrainingData()?\n";
            return;
        }

        // Num of pixel samples
        int N = colors.size();

        // Create Nx3 float matrix for kmeans (using normalized values)
        cv::Mat data(N, 3, CV_32F);
        for (int i = 0; i < N; i++) {
            data.at<float>(i, 0) = colors[i][0]; // H (0..1)
            data.at<float>(i, 1) = colors[i][1]; // S (0..1)
            data.at<float>(i, 2) = colors[i][2]; // V (0..1)
        }

        // Output containers
        labels = cv::Mat();
        centers = cv::Mat();

        // Run k-means
        cv::kmeans(
            data,                   // Nx3 matrix
            6,                      // K = 6 clusters
            labels,                 // List of integers 0-5 to specify clusters
            cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1.0), // 10 max iterations OR centers don't change much (EPS)
            5,                      // Run 5 times with different initial seeds and get best results
            cv::KMEANS_PP_CENTERS,  // "Smart Initialization"
            centers                 // Stores in centers
        );

        // Print centers (debugging)
        std::cout << "\nK-means cluster centers (LAB normalized):\n";
        for (int i = 0; i < 6; i++) {
            float Ln = centers.at<float>(i, 0);
            float An = centers.at<float>(i, 1);
            float Bn = centers.at<float>(i, 2);
            std::cout 
                << "Center " << i << ": "
                << "L=" << (Ln * 255.0f) << ", "
                << "A=" << (An * 127.0f + 128.0f) << ", "
                << "B=" << (Bn * 127.0f + 128.0f) << "\n";
        }
    }

    // Maps clusters to colors
    void assignClustersToCubeColors(const std::array<std::array<std::array<cv::Mat,3>,3>,6>& faces) {
        clusterToColor.clear();

        std::cout << "Canonical mapping incomplete; entering interactive calibration.\n";

        const std::string win = "Center Calibration";
        cv::namedWindow(win, cv::WINDOW_AUTOSIZE);
        std::set<char> usedColors;

        // USER CALIBRATION
        for (int f = 0; f < 6; ++f) {
            cv::Mat roi = faces[f][1][1];
            if (roi.empty()) continue;

            // Inner Region of Center Tile
            int x0 = roi.cols * 40 / 100;
            int y0 = roi.rows * 40 / 100;
            int w  = std::max(1, roi.cols * 50 / 100);
            int h  = std::max(1, roi.rows * 50 / 100);
            cv::Rect inner(x0, y0, w, h);
            cv::Mat tight = roi(inner).clone();

            // compute nearest cluster for this face center
            cv::Mat lab;
            cv::cvtColor(tight, lab, cv::COLOR_BGR2Lab);
            cv::Scalar m = cv::mean(lab);
            
            float L = static_cast<float>(m[0]);
            float A = static_cast<float>(m[1]);
            float B = static_cast<float>(m[2]);

            float Ln = L / 255.0f;
            float An = (A - 128.0f) / 127.0f;
            float Bn = (B - 128.0f) / 127.0f;

            float bestDist = FLT_MAX; 
            int bestCluster = -1;
            for (int c = 0; c < centers.rows; ++c) {
                // Get's each cluster's center
                float clusterL = centers.at<float>(c,0);
                float clusterA = centers.at<float>(c,1);
                float clusterB = centers.at<float>(c,2);
                
                float dL = Ln - clusterL;
                float dA = An - clusterA;
                float dB = Bn - clusterB;

                // Calculate Uclidian distance
                float dist = std::sqrt(dL*dL + dA*dA + dB*dB);
                
                // If distnace for this cluster is smaller, assign to cluster
                if (dist < bestDist) { 
                    bestDist = dist; 
                    bestCluster = c; 
                }
            }

            // DISPLAYS REDUCED CENTER TILE ROI AND HAVE USER CLASSIFY EACH ONE
            cv::imshow(win, tight);
            std::cout << "Face " << f << ": press key for color (w=white,o=orange,g=green,r=red,b=blue,y=yellow)." << std::endl;
            
            while (true) {
                int k = cv::waitKey(0);
                if (k < 0) continue;
                char kc = static_cast<char>(k);
                kc = std::tolower(kc);
                char mapped = 0;
                
                if (kc == 'w') mapped = 'W';
                if (kc == 'o') mapped = 'O';
                if (kc == 'g') mapped = 'G';
                if (kc == 'r') mapped = 'R';
                if (kc == 'b') mapped = 'B';
                if (kc == 'y') mapped = 'Y';
                
                if (mapped == 0) {
                    std::cout << "Invalid key. Use w,o,g,r,b,y." << std::endl;
                    continue;
                }
                if (usedColors.count(mapped)) {
                    std::cout << "Color already assigned. Press another key." << std::endl;
                    continue;
                }
                // Assign user selected color to clusterToColor
                if (bestCluster >= 0) {
                    clusterToColor[bestCluster] = mapped;
                    usedColors.insert(mapped);
                    std::cout << "Assigned cluster " << bestCluster << " -> " << mapped << "\n";
                }
                break;
            }
        }
        cv::destroyWindow(win);
    }

    // Pixel data -> cube color
    std::array<std::array<std::array<char,3>,3>,6> classifyAllFaces(const std::array<std::array<std::array<cv::Mat,3>,3>,6>& faces)
    {
        std::array<std::array<std::array<char,3>,3>,6> result;

        for (int f = 0; f < 6; f++) {
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    // Given the ROI image for this sticker, determine which cube color it is
                    result[f][r][c] = classifyROI(faces[f][r][c]);
                }
            }
        }

        return result;
    }

    // Classify ROI by assigning inner pixels to nearest k-means center and taking majority vote
    char classifyROI(const cv::Mat& roi) {
        // ROI is empty OR K-means cluster centers havn't been computed
        if (roi.empty() || centers.empty()) return '?';

        // Blur ROI
        cv::Mat small;
        cv::medianBlur(roi, small, 3);

        // Moves in 25% from each edge, takes middle 50% for sampling
        int x0 = small.cols * 25 / 100;
        int y0 = small.rows * 25 / 100;
        int w  = std::max(1, small.cols * 50 / 100);
        int h  = std::max(1, small.rows * 50 / 100);
        cv::Rect inner(x0, y0, w, h);
        cv::Mat sample = small(inner);

        cv::Mat lab;
        cv::cvtColor(sample, lab, cv::COLOR_BGR2Lab);

        // Every time a pixel matches a cluster, increment appropriate counter
        std::vector<int> counts(centers.rows, 0);

        // Extracts HSV values from each pixel sample
        for (int y = 0; y < lab.rows; y += 1) {
            for (int x = 0; x < lab.cols; x += 1) {
                auto p = lab.at<cv::Vec3b>(y, x);
                float L = static_cast<float>(p[0]);
                float A = static_cast<float>(p[1]);
                float B = static_cast<float>(p[2]);

                // Filters out very dark pixels
                if (L < 20) continue;

                float Ln = L / 255.0f;
                float An = (A - 128.0f) / 127.0f;
                float Bn = (B - 128.0f) / 127.0f;

                // Find Nearest Cluster
                float bestDist = FLT_MAX;
                int bestC = -1;

                for (int c = 0; c < centers.rows; ++c) {
                    // Get's each cluster's center
                    float clusterL = centers.at<float>(c,0);
                    float clusterA = centers.at<float>(c,1);
                    float clusterB = centers.at<float>(c,2);
                    
                    float dL = Ln - clusterL;
                    float dA = An - clusterA;
                    float dB = Bn - clusterB;
                    
                    // Calculate Uclidian distance
                    float dist = std::sqrt(dL*dL + dA*dA + dB*dB);
                    
                    // If distnace for this cluster is smaller
                    if (dist < bestDist) { 
                        bestDist = dist; 
                        bestC = c; 
                    }
                }

                // Increment counter for that cluster
                if (bestC >= 0) counts[bestC]++;
            }
        }

        // Choose cluster with max count
        int bestCluster = -1; 
        int bestCount = 0;
        for (int c = 0; c < (int)counts.size(); ++c) {
            if (counts[c] > bestCount) { 
                bestCount = counts[c]; 
                bestCluster = c; 
            }
        }
        
        if (bestCluster < 0) {
            return '?';
        }
        
        auto it = clusterToColor.find(bestCluster);
        if (it == clusterToColor.end()) {
            return '?';
        }
        
        // Returns letter
        return it->second;
    }
};