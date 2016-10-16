//
// Created by gomchik-1404 on 15/10/16.
//

#include "../includes/MixGaussian.h"
#include "../includes/Global.h"
#include "../includes/HandOperations.h"

// initializing fields

cv::Mat MixGaussian::mean_mat[MAX_NUM_GAUSSIAN];
cv::Mat MixGaussian::cov_mat[MAX_NUM_GAUSSIAN];
cv::Mat MixGaussian::cov_matI[MAX_NUM_GAUSSIAN];
int MixGaussian::n_dim;
int MixGaussian::n_mixture;
float MixGaussian::weight[MAX_NUM_GAUSSIAN];
float MixGaussian::probabilitySkin[256][256][256];
float MixGaussian::probabilityNonSkin[256][256][256];



// method definitions

bool MixGaussian::LoadFile(char const *filename) {
    printf("Loading file %s\n", filename);
    FILE *fp;
    fp = fopen(filename, "rt");

    // if file dies not exist, report load failed
    if (!fp) {
        printf("LoadFile():: Error here..\n");
        return false;
    }

    int local_n_mixture = 0;
    fscanf(fp, "%d", &local_n_mixture);

    int local_n_dim = 0;
    fscanf(fp, "%d", &local_n_dim);

    n_dim = local_n_dim;

    /*
     * A Gaussian mixture model is weighted sum of some M Gaussian functions
     * A Gaussian is defined by its mean and covariance.
     * Here 'Weight' is its weight in the GMM. Summation of weights is 1
     * */

    /*
     * More generally, type name of a Mat object consists of several parts. Here's example for CV_64FC1:
     * CV_ - this is just a prefix
     * 64 - number of bits per base matrix element (e.g. pixel value in grayscale image or single color element in BGR image)
     * F - type of the base element. In this case it's F for float, but can also be S (signed) or U (unsigned)
     * Cx - number of channels in an image as I outlined earlier
     */
    cv::Mat local_mean_mat(local_n_dim, 1, CV_32FC1);   // 3x1
    cv::Mat local_cov_mat(local_n_dim, local_n_dim, CV_32FC1); // 3x3
    local_cov_mat = cv::Scalar::all(0); // set matrix value to zero, Probable error
    float local_weight = 0;

    for (int i = 0; i < local_n_mixture; i++) {

        float value;

        for (int j = 0; j < local_n_dim; ++j) {
            fscanf(fp, "%f", &value);
            local_mean_mat.at<float>(j) = value;
        }

        for (int j = 0; j < local_n_dim; ++j) {
            int cols = local_cov_mat.cols;
            fscanf(fp, "%f", &value);
            local_cov_mat.at<float>(j, j) = value;
        }

        fscanf(fp, "%f", &value);
        local_weight = value;
//
//		// add the gaussian...
        AddGaussian(local_mean_mat, local_cov_mat, local_weight);

    }

    local_cov_mat.release();
    local_mean_mat.release();
    fclose(fp);

    return true;
}

bool MixGaussian::AddGaussian(cv::Mat &pr_mean_mat, cv::Mat &pr_cov_mat, float pr_weight) {

    if (n_mixture == MAX_NUM_GAUSSIAN) {
        return false;
    }

    mean_mat[n_mixture] = pr_mean_mat.clone();  // 3x1 pr for parameter
    cov_mat[n_mixture] = pr_cov_mat.clone();   // 3x3 check for efficiency later
    cov_matI[n_mixture] = pr_cov_mat.clone();   // 3x3
    cv::invert(pr_cov_mat, cov_matI[n_mixture]); // not clear 3x3
    weight[n_mixture] = pr_weight;

    ++n_mixture;

    return true;

}

float MixGaussian::GetProbability(cv::Mat sample) {
    float P = 0.0;

    // difference matrix
    cv::Mat diff_mat = sample.clone();

    // transpose of difference matrix
    cv::Mat diff_mat_trans(1, n_dim, CV_32FC1);

    float expo;

    cv::Mat exp_mat(1, 1, CV_32FC1, &expo);

    for (int i = 0; i < n_mixture; i++) {
        diff_mat = sample - mean_mat[i];
        diff_mat_trans = diff_mat.t();
        diff_mat = cov_matI[i] * diff_mat;

        exp_mat = diff_mat_trans * diff_mat;
        expo *= (-0.5);
        P += (weight[i] * 1.0 / (pow(2 * CV_PI, 1.5) * sqrt(cv::determinant(cov_mat[i])))
              * exp(expo));
    }

    diff_mat.release();
    diff_mat_trans.release();

    return P;
}

void MixGaussian::MakeLookUpTable(bool skin) {
    ALOG("Making look up table\n");
    // making the look up table..
    cv::Mat local_sample_mat(3, 1, CV_32FC1); // 32SC1

    for (int R = 0; R < 256; ++R) {
        for (int G = 0; G < 256; ++G) {
            for (int B = 0; B < 256; ++B) {
                local_sample_mat.at<float>(0) = R;
                local_sample_mat.at<float>(1) = G;
                local_sample_mat.at<float>(2) = B;
                if (skin) {
                    probabilitySkin[R][G][B] = GetProbability(local_sample_mat);
//					std::cout << "R: " << R << " G: " << G << " B: " << 100 << "    probability[R][G][B] = " << probabilitySkin[R][G][100] << std::endl;
                }
                else {
                    probabilityNonSkin[R][G][B] = GetProbability(local_sample_mat);
//					std::cout << "R: " << R << " G: " << G << " B: " << 100 << "    probability[R][G][B] = " << probabilityNonSkin[R][G][100] << std::endl;
                }
            }
        }
    }

    local_sample_mat.release();
}

bool MixGaussian::SaveLookUpTable(char const *filename) {
    printf("Saving look up table for %s \n", filename);
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        return false;
    }

    for (int R = 0; R < 256; ++R) {
        for (int G = 0; G < 256; ++G) {
            if (std::strcmp(filename, HandOperations::filenameLookUpTableSkin) == 0) {
                fwrite(probabilitySkin[R][G], sizeof(float), 256, fp);
            } else {
                fwrite(probabilityNonSkin[R][G], sizeof(float), 256, fp);
            }
        }
    }
    fclose(fp);
    return true;
}

bool MixGaussian::LoadLookUpTable(char const *filename) {

    FILE *fp = fopen(filename, "rb");

    if (!fp) {
        ALOG("NATIVE-LOG: file %s loading failed", filename);
        return false;
    }

    for (int R = 0; R < 256; ++R) {
        for (int G = 0; G < 256; ++G) {
            if (std::strcmp(filename, HandOperations::filenameLookUpTableSkin) == 0) {
                if (fread(probabilitySkin[R][G], sizeof(float), 256, fp) != 256) {
                    ALOG("NATIVE-LOG: file %s reading failed", filename);
                    fclose(fp);
                    return 0;
                }
            } else {
                if (fread(probabilityNonSkin[R][G], sizeof(float), 256, fp) != 256) {
                    fclose(fp);
                    return 0;
                }
            }
        }
    }

    ALOG("NATIVE-LOG: file %s reading complete", filename);

    fclose(fp);
    return true;
}

float MixGaussian::GetProbabilityByLookUp(bool skin, int R, int G, int B) {
    if (skin) {
        return probabilitySkin[R][G][B];
    } else {
        return probabilityNonSkin[R][G][B];
    }
}