//
// Created by gomchik-1404 on 15/10/16.
//

#ifndef UTUBEOPENCV_MIXGAUSSIAN_H
#define UTUBEOPENCV_MIXGAUSSIAN_H

#include <opencv2/opencv.hpp>

#define  MAX_NUM_GAUSSIAN 100

class MixGaussian {

public:

    // fields
    static cv::Mat mean_mat[MAX_NUM_GAUSSIAN];
    static cv::Mat cov_mat[MAX_NUM_GAUSSIAN];
    static cv::Mat cov_matI[MAX_NUM_GAUSSIAN];
    static int n_mixture;
    static int n_dim;
    static float weight[MAX_NUM_GAUSSIAN];
    static float probabilitySkin[256][256][256];
    static float probabilityNonSkin[256][256][256];

    // methods
    bool LoadFile(char const *filename);
    bool AddGaussian(cv::Mat &mean_mat, cv::Mat &cov_mat, float weight);
    float GetProbability(cv::Mat sample);
    void MakeLookUpTable(bool skin);
    bool SaveLookUpTable(char const *filename);
    bool LoadLookUpTable(char const *filename);
    float GetProbabilityByLookUp(bool skin, int R, int G, int B);
};


#endif //UTUBEOPENCV_MIXGAUSSIAN_H
