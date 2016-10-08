//
// Created by siddhant on 8/12/16.
//

#include "./global.h"

#ifndef HANDYSELF_MASTER_FINGERTIPTRACKER_H
#define HANDYSELF_MASTER_FINGERTIPTRACKER_H

#define NUM_FINGERTIP              5
#define NUM_MAX_CANDIDATES         20
#define THRESHOLD_CLOSEST_DIST     50
#define THRESHOLD_LOST_TRACKING    5
#define MIN_AGE_TO_TRACK           5
#define THRESHOLD_AGE_TO_DETECT    10

#define NUM_FINGERTIP              5

#define MODE_FINGERTIP_NONE             0
#define MODE_FINGERTIP_FIRST_DETECTED   1
#define MODE_FINGERTIP_TRACKING         2
#define MODE_FINGERTIP_LOST_TRACKING    3

class FingertipTracker {
public:

	FingertipTracker(void);
	~FingertipTracker(void);

	void Reset();
	bool FeedFingertipCandidates(int n_points, cv::Point2f points[],
			float dist_value[], cv::Point2f curr_centroid);

	bool LoadFingertipCoordinates(char* filename);
	bool SaveFingertipCoordinates(char* filename);

	bool TrackFingertips(int nPoints, cv::Point2f points[], float *distValue,
			cv::Point2f prevCentroid, cv::Point2f currCentroid);


	int QueryTrackingMode() { return   _nMode; }

	int _nFingertips;
	cv::Point2f _fingertipPoints[NUM_FINGERTIP];

protected:

	void MatchCorrespondencesByNearestNeighbor(int n_points,
			cv::Point2f points[], float dist_value[]);

private:

	// tracking mode
	int _nMode;

	// candidates
	int _nPoints;
	cv::Point2f _candidatePoints[NUM_MAX_CANDIDATES];
	int _candidateScore[NUM_MAX_CANDIDATES];
	int _candidateAge[NUM_MAX_CANDIDATES];
	int _candidateLost[NUM_MAX_CANDIDATES];
	float _candidateVelocity[NUM_MAX_CANDIDATES][2];
	float _candidateDist[NUM_MAX_CANDIDATES];

	//fingertips
	bool _fiveFingerDetected;
	int _nFingertipIndex[NUM_FINGERTIP];
	float _fingertipDist[NUM_FINGERTIP];
	bool _fTracked[NUM_FINGERTIP];
	bool _fFlipOrder;

	// fingertip coordinates
	bool f_fingertip_coordinates;
	float fingertip_coordinates[NUM_FINGERTIP][3];
};

#endif //HANDYSELF_MASTER_FINGERTIPTRACKER_H
