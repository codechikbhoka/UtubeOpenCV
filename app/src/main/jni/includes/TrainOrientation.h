#include <fstream>
#include <iostream>
#include "../includes/RandomForest.h"
#include "../includes/Utils.h"
#include "../includes/PoseEstimation.h"
#include "../includes/Predict.h"

#ifndef UTUBEOPENCV_TRAINORIENTATION_H
#define UTUBEOPENCV_TRAINORIENTATION_H

int initTrainModel(std::string absPath);
float getAzimuth(cv::Mat mTarget);
float getElevation(cv::Mat mTarget);

#endif //UTUBEOPENCV_TRAINORIENTATION_H
