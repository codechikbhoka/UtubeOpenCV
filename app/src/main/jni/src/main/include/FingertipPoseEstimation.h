//
// Created by siddhant on 8/12/16.
//

#ifndef HANDYSELF_MASTER_FINGERTIPPOSEESTIMATION_H
#define HANDYSELF_MASTER_FINGERTIPPOSEESTIMATION_H

#include "./global.h"
#include "HandRegion.h"
#include "FingertipTracker.h"
#include "Fingertip.h"

class FingertipPoseEstimation
{
public:
    FingertipPoseEstimation(void);
    ~FingertipPoseEstimation(void);

    bool Initialize(cv::Mat src_image, char* filename);

    void Reset();
    void onCapture();
    void OnProcess();

    bool LoadFingertipCoordinates(char* filename);
    bool SaveFingertipCoordinates(char* filename);

};

#endif //HANDYSELF_MASTER_FINGERTIPPOSEESTIMATION_H
