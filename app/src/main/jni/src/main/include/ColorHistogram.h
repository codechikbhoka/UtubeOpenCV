//
// Created by siddhant on 8/4/16.
//

#ifndef SMIS_FINAL_PROJ_COLORHISTOGRAM_H
#define SMIS_FINAL_PROJ_COLORHISTOGRAM_H

#include "./global.h"

#define NUM_HISTOGRAM_BINS  16
#define NUM_HISTOGRAM_HISTORY   5

class ColorHistogram
{
public:
    ColorHistogram(void);
    ~ColorHistogram(void);

    void Reset();
    void FeedFrame(cv::Mat& src_image, cv::Mat& mask_image, cv::Point maxDistPoint);

    float QueryProbability(int value1, int value2, int value3);

private:

    int n_hist;
    int n_hist_index;
    int n_hist_pixel[NUM_HISTOGRAM_HISTORY][NUM_HISTOGRAM_BINS][NUM_HISTOGRAM_BINS][NUM_HISTOGRAM_BINS];
    int n_num_pixel[NUM_HISTOGRAM_HISTORY];
    int n_total_pixel;
};

#endif //SMIS_FINAL_PROJ_COLORHISTOGRAM_H
