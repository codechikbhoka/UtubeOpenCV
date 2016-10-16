//
// Created by gomchik-1404 on 15/10/16.
//

#ifndef UTUBEOPENCV_COLORHISTOGRAM_H
#define UTUBEOPENCV_COLORHISTOGRAM_H

#include <opencv2/opencv.hpp>

#define  NUM_HISTOGRAM_BINS  16
#define  NUM_HISTOGRAM_HISTORY   5

class ColorHistogram {
public:

    // fields
    static int n_hist;
    static int n_hist_index;
    static int n_hist_pixel[NUM_HISTOGRAM_HISTORY][NUM_HISTOGRAM_BINS][NUM_HISTOGRAM_BINS][NUM_HISTOGRAM_BINS];
    static int n_num_pixel[NUM_HISTOGRAM_HISTORY];
    static int n_total_pixel;
    static float maxDistValue;
    static cv::Point2f maxDisttPoint;
    static float prevHistogramPixelMax;
    static bool learn_color_histogram;

    // methods
    void  FeedFrame(cv::Mat &src_image, cv::Mat &mask_image, cv::Point maxDistPoint);
    void  LearnColor(cv::Mat &src_image, cv::Point maxDistPoint, double maxDistValue);
    float QueryProbability(int R, int G, int B);
    void  ResetLearning();
};


#endif //UTUBEOPENCV_COLORHISTOGRAM_H
