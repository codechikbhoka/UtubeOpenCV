#ifndef SMIS_FINAL_PROJ_MIXGAUSSIAN_H

#define SMIS_FINAL_PROJ_MIXGAUSSIAN_H

#include "./global.h"

#define  MAX_NUM_GAUSSIAN 100

class MixGaussian
{
public:
    MixGaussian();
    ~MixGaussian();

    bool LoadFile(char const* filename);
    bool AddGaussian(cv::Mat& mean_mat, cv::Mat& cov_mat, float weight);

    float  GetProbability(cv::Mat sample);

    void MakeLookUpTable();
    bool SaveLookUpTable(char const* filename);
    bool LoadLookUpTable(char const* filename);
    float GetProbabilityByLookUp(int R, int G, int B);

private:
    int n_mixture;
    int n_dim;
    cv::Mat mean_mat[MAX_NUM_GAUSSIAN];
    cv::Mat cov_mat[MAX_NUM_GAUSSIAN];
    cv::Mat cov_matI[MAX_NUM_GAUSSIAN];
    float weight[MAX_NUM_GAUSSIAN];

    float probability[256][256][256];
};

#endif //SMIS_FINAL_PROJ_MIXGAUSSIAN_H
