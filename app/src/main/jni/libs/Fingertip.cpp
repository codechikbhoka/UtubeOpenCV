#include "../includes/Fingertip.h"
#include "../includes/Global.h"
#include <opencv2/opencv.hpp>
#include "stdio.h"
#include <vector>


cv::Mat DistTransform(cv::Mat silhoutte) {
    cv::threshold(silhoutte, silhoutte, 0, 255,
                  CV_THRESH_BINARY | CV_THRESH_OTSU);

    cv::Mat dist;
    cv::distanceTransform(silhoutte, dist, CV_DIST_L2, 3);

    // use normalize only to see the x-ray type image
    // cv::normalize(dist, dist, 0, 1, cv::NORM_MINMAX);

    // cv::namedWindow("DistTransform", CV_WINDOW_NORMAL);
    // cv::imshow("DistTransform", dist);
    // cv::waitKey(0);

    return dist;
}

bool CheckImageBoundary(std::vector <cv::Point2f> &contour_points,
                        int minPoint, cv::Mat p_dist) {
    return (contour_points[minPoint].x > 10
            && contour_points[minPoint].x < p_dist.size().width - 10
            && contour_points[minPoint].y > 10
            && contour_points[minPoint].y < p_dist.size().height - 10);
}

void Fingertip::MyFilledCircle(cv::Mat img, cv::Point center) {
    int thickness = -1;
    int lineType = 8;

    cv::circle(img, center, 10,  // radius
               cv::Scalar(0, 0, 255), thickness, lineType);
}

int Fingertip::RefreshMaxDistPoint(cv::Mat& segmentedImg) {
    if (segmentedImg.empty()) {
        return -1;
    }

    if (_pDistImage.empty()) {
        _pDistImage = cv::Mat(segmentedImg.size(), CV_32FC1);
    }
    if (_pDistImage2.empty()) {
        _pDistImage2 = cv::Mat(segmentedImg.size(), CV_32FC1);
    }
    if (_pHandImage.empty()) {
        _pHandImage = cv::Mat(segmentedImg.size(), CV_32FC1);
    }

	// printf("Getting Distance Transform\n");
    _pDistImage2 = DistTransform(segmentedImg);    // distance transform


    _maxDistValue = 0;
    _maxDistPoint = cv::Point(0, 0);
    cv::minMaxLoc(_pDistImage2, 0, &_maxDistValue, 0, &_maxDistPoint);

    if (_maxDistPoint.x == 0)
        _maxDistPoint.x++;
    if (_maxDistPoint.y == 0)
        _maxDistPoint.y++;
    if (_maxDistPoint.x >= _pDistImage.size().width - 1)
        _maxDistPoint.x--;
    if (_maxDistPoint.y >= _pDistImage.size().height - 1)
        _maxDistPoint.y--;

    cv::Mat segmentedImgCopy = segmentedImg.clone();
    cv::findContours(segmentedImgCopy, contours, hierarchy, CV_RETR_EXTERNAL,
                     CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));


    _pHandImage = cv::Mat::zeros(segmentedImg.size(), CV_8UC1);

    int hand_contour_id = -1;    // index of the contour we want HAND

    for (int i = 0; i < contours.size(); i++) {

        // Add filled i'th contour to the image if number of points in contour > 200
        if (contours[i].size() > 50) {
//			std::cout << "contours[i].size() > 100" << std::endl;
            cv::drawContours(_pHandImage, contours, i, cv::Scalar(255),
                             CV_FILLED, 8, hierarchy, 0, cv::Point());
        }

        if ((int) (_pHandImage.at<uchar>(
                cv::Point(_maxDistPoint.x, _maxDistPoint.y))) == 255) {

            hand_contour_id = i;
            _pHandImage = cv::Mat::zeros(segmentedImg.size(), CV_8UC1);

            // draw contour for
            cv::drawContours(_pHandImage, contours, i,
                    /*color*/cv::Scalar(255),
                    /*thickness*/CV_FILLED,
                    /*line type*/8,
                    /*hierarchy*/hierarchy,
                    /*max level*/0,
                    /*offset*/cv::Point());

            _pDistImage = DistTransform(_pHandImage);

            // enable normalization in the function to see x-ray type image
//			cv::namedWindow("distanceTransform", CV_WINDOW_NORMAL);
//			cv::imshow("distanceTransform", _pDistImage);

            _maxDistValue = 0;
            _maxDistPoint = cvPoint(0, 0);

            cv::minMaxLoc(_pDistImage, 0, &_maxDistValue, 0, &_maxDistPoint);
            if (_maxDistPoint.x == 0)
                _maxDistPoint.x++;
            if (_maxDistPoint.y == 0)
                _maxDistPoint.y++;
            if (_maxDistPoint.x >= _pDistImage.size().width - 1)
                _maxDistPoint.x--;
            if (_maxDistPoint.y >= _pDistImage.size().height - 1)
                _maxDistPoint.y--;

            segmentedImg = _pHandImage;

            break;
        }

    }

    if (hand_contour_id == -1) {
//        ALOG("NATIVE-LOG hand contour not found");
        return -1;
    }

    return 0;
}

