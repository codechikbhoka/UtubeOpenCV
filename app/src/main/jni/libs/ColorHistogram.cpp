#include "../includes/ColorHistogram.h"
#include "../includes/Global.h"


// initializing fields

int ColorHistogram::n_hist = 0;
int ColorHistogram::n_hist_index = 0;
int ColorHistogram::n_total_pixel = 0;
float ColorHistogram::maxDistValue = 128.799f;
cv::Point2f ColorHistogram::maxDisttPoint = cv::Point2f(457, 213);
float ColorHistogram::prevHistogramPixelMax = 0;
bool ColorHistogram::learn_color_histogram = false;
int ColorHistogram::n_hist_pixel[NUM_HISTOGRAM_HISTORY][NUM_HISTOGRAM_BINS][NUM_HISTOGRAM_BINS][NUM_HISTOGRAM_BINS];
int ColorHistogram::n_num_pixel[NUM_HISTOGRAM_HISTORY];


// method definitions

void  ColorHistogram::FeedFrame(cv::Mat &src_image, cv::Mat &mask_image, cv::Point maxDistPoint) {
    if (src_image.empty() || mask_image.empty()) {
        ALOG("NATIVE-LOG returning because src_image or mask_image is empty ");
        return;
    }
    // remove oldest history
    n_total_pixel -= n_num_pixel[n_hist_index];

    for (int i = 0; i < NUM_HISTOGRAM_BINS; ++i) {
        for (int j = 0; j < NUM_HISTOGRAM_BINS; ++j) {
            for (int k = 0; k < NUM_HISTOGRAM_BINS; ++k) {
                n_hist_pixel[n_hist_index][i][j][k] = 0;
            }
        }
    }

    n_num_pixel[n_hist_index] = 0;


    for (int i = 0; i < src_image.rows; i++) {  // rows = 640
        for (int j = 0; j < src_image.cols; j++) {  // cols = 480
            if (((int) mask_image.at<uchar>(cv::Point(j, i))) == 255) {
                int R = src_image.at<cv::Vec4b>(i, j)[0];
                int G = src_image.at<cv::Vec4b>(i, j)[1];
                int B = src_image.at<cv::Vec4b>(i, j)[2];

                n_hist_pixel[n_hist_index][R / NUM_HISTOGRAM_BINS][G / NUM_HISTOGRAM_BINS][B /
                                                                                           NUM_HISTOGRAM_BINS]++;
                n_num_pixel[n_hist_index]++;
            }
        }
    }

//    ALOG("NATIVE-LOG n_num_pixel[n_hist_index] : %d", n_num_pixel[n_hist_index]);
    n_total_pixel += n_num_pixel[n_hist_index];
    ALOG("NATIVE-LOG : n_total_pixel is %d", n_total_pixel);


    n_hist_index++;
    if (n_hist_index == NUM_HISTOGRAM_HISTORY)
        n_hist_index = 0;

    if (n_hist < NUM_HISTOGRAM_HISTORY)
        n_hist++;
}

void  ColorHistogram::LearnColor(cv::Mat &src_image, cv::Point maxDistPoint, double maxDistValue) {
    if (src_image.empty()) {
        return;
    }

    cv::Mat imgCircle(src_image.size(), CV_8UC1);
    imgCircle = cv::Scalar::all(0);
    cv::circle(imgCircle, maxDistPoint, 0.7 * maxDistValue,
               cv::Scalar(255), -1, 8, 0);
    FeedFrame(src_image, imgCircle, maxDistPoint);
}

float ColorHistogram::QueryProbability(int R, int G, int B) {
    if (n_total_pixel == 0) {
        ALOG("NATIVE-LOG : returning 0 because n_total_pixel is 0");
        return 0;
    }

    float local_num_pixel = 0;
    float local_prob = 0;

    for (int i = 0; i < n_hist; ++i) {
        local_num_pixel += n_hist_pixel[i][R / NUM_HISTOGRAM_BINS][G
                                                                   / NUM_HISTOGRAM_BINS][B /
                                                                                         NUM_HISTOGRAM_BINS];
    }

    if (local_num_pixel > prevHistogramPixelMax) {
        prevHistogramPixelMax = local_num_pixel;
        ALOG("NATIVE-LOG local_num_pixel: %f", local_num_pixel);
        ALOG("NATIVE-LOG n_total_pixel: %d", n_total_pixel);
    }
    local_prob = local_num_pixel / (float) n_total_pixel;
    return local_prob;
}

void  ColorHistogram::ResetLearning() {
    ALOG("NATIVE-LOG : ResetLearning called");
    n_hist = 0;
    n_hist_index = 0;
    n_total_pixel = 0;
    for (int i = 0; i < NUM_HISTOGRAM_HISTORY; ++i) {
        n_num_pixel[i] = 0;
    }
}


