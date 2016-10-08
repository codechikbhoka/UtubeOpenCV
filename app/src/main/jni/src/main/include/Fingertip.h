#ifndef SMIS_FINAL_PROJ_FINGERTIP_H
#define SMIS_FINAL_PROJ_FINGERTIP_H

#include "./global.h"

#define MAX_END_POINT       1000

#ifdef PROCESS_320x240
    #define MIN_CURVATURE_STEP          5
    #define MAX_CURVATURE_STEP          25
    #define FINGERTIP_ANGLE_THRESHOLD   0.5
    #define GAP_POINT_THRESHOLD         5
    #define CONNECTED_POINT_THRESHOLD   5
#else
    #define MIN_CURVATURE_STEP          10
    #define MAX_CURVATURE_STEP          50
    #define FINGERTIP_ANGLE_THRESHOLD   0.5
    #define GAP_POINT_THRESHOLD         10
    #define CONNECTED_POINT_THRESHOLD   10
#endif

class Fingertip {
public:
	Fingertip();
	~Fingertip();

	void MyFilledCircle(cv::Mat img, cv::Point center);
	int FindFingerTipCandidatesByCurvature(cv::Mat handRegion);

	// Single Connected Component for Hand
	cv::Mat _pHandImage;

	// Distance Transform
	cv::Mat _pDistImage;  // Distance Transform Image (only for a hand)
	cv::Mat _pDistImage2; // Temporary Distance Transform (for all image)

public:
	int _numCandidatePoints;
	cv::Point _maxDistPoint;
	double _maxDistValue;
	std::vector < std::vector<cv::Point> > contours;
	std::vector < cv::Vec4i > hierarchy;

    // endPoints
    cv::Point2f _candidatePoints[MAX_END_POINT];  // ?? Point

    // ellipse fitting
    cv::RotatedRect _RotatedRects[MAX_END_POINT];
    float  _candidatePointDists[ MAX_END_POINT ];
    float _candidatePointScores[MAX_END_POINT];
};

#endif //SMIS_FINAL_PROJ_FINGERTIP_H
