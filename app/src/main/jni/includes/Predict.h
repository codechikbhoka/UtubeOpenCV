//
// Created by siddhant on 10/24/16.
//

#ifndef POSEESTIMATION_PREDICT_H
#define POSEESTIMATION_PREDICT_H

#endif //POSEESTIMATION_PREDICT_H

#include "fstream"
#include "PoseEstimation.h"
#include <fstream>

class predict
{
public:
    predict() {};

    void load_models(std::string absPath);
    bool generate_normalized_vector(cv::Mat img);
    void predict_output(std::string which);
    float getOrientation(cv::Mat img, std::string which);

    poseEstimation pose;
    CvRTrees rtreesAzimuth;
    CvRTrees rtreesElevation ;
    cv::Mat feature_matrix;
    static const std::string AZIMUTH;
    static const std::string ELEVATION;
    float response;

};