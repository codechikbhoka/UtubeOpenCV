#ifndef SMIS_FINAL_PROJ_HANDREGION_H
#define SMIS_FINAL_PROJ_HANDREGION_H

#include "./global.h"
#include "MixGaussian.h"
#include "ColorHistogram.h"
#include "Fingertip.h"

#define ALPHA  0.9

class HandRegion
{
    public:
        HandRegion();
        ~HandRegion();

        bool LoadColorConfig();
        bool LoadSkinColorProbTable();
        bool LoadHandMask(char* filename);
        bool InitHandMask(cv::Mat& src_image);
        void ResetLearning();
        void LearnColor(cv::Mat src_image, cv::Point maxDistPoint, double maxDistValue);
        cv::Mat GetHandRegion(cv::Mat& src_img, bool f_screenshot = false);

        cv::Mat QueryHandRegion() { return imgSegmented;}
        cv::Mat QueryHandMask() { return p_image_hand_mask; }

        void DrawContour(cv::Mat& dst_image, cv::Point start, cv::Mat& dist_image);

    private:
        MixGaussian skin_color;
        MixGaussian non_skin_color;
        ColorHistogram _ColorHistogram;

        cv::Mat imgSegmented;
        cv::Mat p_image_gradient;
        cv::Mat p_image_hand_mask;
        bool _fLearned;

    #ifndef USE_GAUSSIAN_MODEL
        cv::Mat p_hsv;
        cv::Mat p_h;
        cv::Mat p_s;
        cv::Mat p_v;

        cv::Mat p_hmin;
        cv::Mat p_hmax;

        cv::Mat p_smin;
        cv::Mat p_smax;

        cv::Mat p_vmin;
        cv::Mat p_vmax;

        cv::Mat p_h_image;
        cv::Mat p_s_image;
        cv::Mat p_v_image;

        int h_min, h_max, s_min, s_max, v_min, v_max;
    #endif

        // HISTOGRAM

        bool f_learned; // learn histogram

        int hist_pixel[16][16][16];
        int hist_pixel_crcb[16][16][16];
        int n_total_pixel;

};



#endif //SMIS_FINAL_PROJ_HANDREGION_H
