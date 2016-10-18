//
// Created by gomchik-1404 on 15/10/16.
//

#ifndef UTUBEOPENCV_FINGERTIP_H
#define UTUBEOPENCV_FINGERTIP_H

#include "opencv2/opencv.hpp"

#define MAX_END_POINT       1000
#define MIN_CURVATURE_STEP          10
#define MAX_CURVATURE_STEP          50
#define FINGERTIP_ANGLE_THRESHOLD   0.5
#define GAP_POINT_THRESHOLD         10
#define CONNECTED_POINT_THRESHOLD   10

class Fingertip {

    void MyFilledCircle(cv::Mat img, cv::Point center);

    // Single Connected Component for Hand
    cv::Mat _pHandImage;

    // Distance Transform
    cv::Mat _pDistImage;  // Distance Transform Image (only for a hand)

    cv::Mat _pDistImage2; // Temporary Distance Transform (for all image)
public:


    double _maxDistValue;
    cv::Point _maxDistPoint;
    std::vector <std::vector<cv::Point>> contours;
    std::vector <cv::Vec4i> hierarchy;
    // endPoints
    cv::Point2f _candidatePoints[MAX_END_POINT];  // ?? Point
    // ellipse fitting
    cv::RotatedRect _RotatedRects[MAX_END_POINT];

    float _candidatePointDists[MAX_END_POINT];
    float _candidatePointScores[MAX_END_POINT];

    int RefreshMaxDistPoint(cv::Mat segmentedImg);
};


#endif //UTUBEOPENCV_FINGERTIP_H
