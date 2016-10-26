//
// Created by siddhant on 10/15/16.
//

#ifndef POSEESTIMATION_RANDOMFOREST_H
#define POSEESTIMATION_RANDOMFOREST_H

#include "opencv2/opencv.hpp"
#include "opencv2/ml/ml.hpp"

class model
{
public:

    model() {};

    CvRTrees rtrees;    // random forest
    CvRTParams params;  // forest parameters
    CvMLData data;
    cv::Mat testdata; // test data
    int check = 0;
    const CvMat* temp;
    cv::Mat traindata;
    cv::Mat trainresponse;
    cv::Mat testresponse;
    cv::Mat var_type;

    cv::Mat mytraindataidx;
    cv::Mat mytestdataidx;

    std::vector<float> response_vec;

    static model* myInstance;
    static model* return_obj();

    void set_forest_params();
    void load_data();
    void train_model();
    void set_up_Mat();

    void predict_test();

    void calculate_error();
    void start_model();

    void save_model();

};

#endif //POSEESTIMATION_RANDOMFOREST_H
