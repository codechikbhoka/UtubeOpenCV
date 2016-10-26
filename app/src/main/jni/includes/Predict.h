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

    void load_model(std::string absPath);
    bool generate_normalized_vector(cv::Mat img);
    void predict_output();
    float getOrientation(cv::Mat img);

    poseEstimation pose;
    CvRTrees rtrees ;
    cv::Mat pred_dist_vec;

    std::vector<float> response_vec;

};