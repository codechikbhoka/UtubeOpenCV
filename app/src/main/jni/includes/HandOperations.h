//
// Created by gomchik-1404 on 15/10/16.
//

#ifndef UTUBEOPENCV_HANDOPERATIONS_H
#define UTUBEOPENCV_HANDOPERATIONS_H

#include <opencv2/opencv.hpp>
#include <jni.h>
#include <android/asset_manager.h>
#include <cstring>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "ColorHistogram.h"
#include "MixGaussian.h"

class HandOperations {

public:

    // fields
    static cv::Mat imgSegmented;
    static cv::Mat p_image_gradient;


    static const char *filenameLookUpTableSkin;
    static const char *filenameLookUpTableNonSkin;
    static const char *filenameMgmSkin;
    static const char *filenameMgmNonSkin;
    static const char *filenameHandyXY;
    static std::vector <std::vector<int>> vectPoints;

    static const std::string ALGO_GAUSSIAN;
    static const std::string ALGO_HISOGRAM;
    static const std::string ALGO_MIXED;

    static float prevGaussianMax;
    static float prevHistogramMax;
    static float prevMixedMax;
    static std::string filterAlgo;


    ColorHistogram CH;
    MixGaussian MG;



    // methods

    bool readMask();

    bool drawMask(cv::Mat &img);

    void MyFilledCircle(cv::Mat &img, cv::Point center);

    cv::Mat GetHandRegion(cv::Mat &src_img);

    bool LoadSkinColorProbTable();


};


#endif //UTUBEOPENCV_HANDOPERATIONS_H
