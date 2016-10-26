//
// Created by siddhant on 10/15/16.
//

#ifndef POSEESTIMATION_POSEESTIMATION_H
#define POSEESTIMATION_POSEESTIMATION_H

#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "cmath"
//#include "opencv2/ml"
#include <fstream>
#include <iterator>
#include <vector>
#include "algorithm"

#define PI 3.14159265

using namespace std;

class poseEstimation
{
public:
    poseEstimation() {};

    void init_extraction(int threadId, int startIndex, int endIndex);

    // fit ellipse to largest contour
    bool find_contour(cv::Mat img);
    void fit_ellipse(cv::Mat hand_contour_img);
    void calculate_prominent_point(cv::Mat hand_contour_img,cv::Mat ellipse_image, cv::RotatedRect box);
    void calculate_distance_features();
    void sampling();
    void save_to_file(std::string filename, vector<double> vec);
    void minmaxscaler(std::vector<double> vec);

    std::vector<std::vector<cv::Point> > contours;
    int largest_index = -1;
    cv::Mat image;
    cv::Point prominent;
    std::vector<double > distance_vector;
    std::vector<double > sampled_distance_vector;
    std::vector<double > normalized_distance_vector;

    double normal_sum = 0;
};



#endif //POSEESTIMATION_POSEESTIMATION_H
