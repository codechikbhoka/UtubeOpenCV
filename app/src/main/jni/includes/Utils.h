//
// Created by siddhant on 10/22/16.
//

#ifndef POSEESTIMATION_UTILS_H
#define POSEESTIMATION_UTILS_H

#endif //POSEESTIMATION_UTILS_H

#include "opencv2/opencv.hpp"
#include "opencv2/ml/ml.hpp"
#include "RandomForest.h"

class utils{

public:
    utils() {};

    void shuffle();
    void k_fold_Cv(int num_of_folds);
    void gen_main(int num_of_folds);
    void R2_error(int index);



    int length_of_inputs;   // total length of the input data
    std::vector<int> vec;
    std::vector<float> r2_err;

    model* forest = model::return_obj();

    cv::Mat train_idx;  // training id's
    cv::Mat test_idx;   // testing id's

    cv::Mat k_fold_mat;

    CvMLData data;
};