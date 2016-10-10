#include "com_smis_utubeopencv_OpencvNativeClass.h"
#include <android/log.h>
#include <errno.h>
#include <android/asset_manager.h>

#define  LOG_TAG    "testjni"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

bool LoadSkinColorProbTable() {
    // printf("Load skin color probability table\n");
    if (!LoadLookUpTable(filenameLookUpTableSkin)) {
        printf("skin.dis not found or corrupted, creating fresh using skin.txt\n");

        if (!LoadFile(filenameMgmSkin)) {
            printf("skin color distribution load error.\n");
            return false;
        }
        ALOG("making a lookup table which contains probability corresponding to a R,G,B for "
                     "a skin color distribution\n");

        MakeLookUpTable(true);

        if (!SaveLookUpTable(filenameLookUpTableSkin)) {
            printf("skin color distribution look up table save error.\n");
            return false;
        }
    }
    if (!LoadLookUpTable(filenameLookUpTableNonSkin)) {
        if (!LoadFile(filenameMgmNonSkin)) {
            printf("non-skin color distribution load error.\n");
            return false;
        }

        printf("making a lookup table for non-skin color distribution\n");
        MakeLookUpTable(false);

        if (!SaveLookUpTable(filenameLookUpTableNonSkin)) {
            printf("non-skin color distribution look up table save error.\n");
            return false;
        }
    }

    return true;
}


/*
 * returns segmented_image(CV_8UC1) from src_img(3 channel)
 * */
cv::Mat GetHandRegion(cv::Mat& src_img) {

    cv::Size src_size = src_img.size();
    if (imgSegmented.empty())
        imgSegmented = cv::Mat(src_size, CV_8UC1);
    if (p_image_gradient.empty())
        p_image_gradient = cv::Mat(src_size, CV_8UC1);

    int R = 0;
    int G = 0;
    int B = 0;

    for (int i = 0; i < src_img.rows; ++i) {
        for (int j = 0; j < src_img.cols; ++j) {
            R = src_img.at<cv::Vec3b>(i, j)[0];
            G = src_img.at<cv::Vec3b>(i, j)[1];
            B = src_img.at<cv::Vec3b>(i, j)[2];

            float p_skin = GetProbabilityByLookUp(true, R, G, B);
            float p_non_skin = GetProbabilityByLookUp(false, R, G, B);

            double prob = 0;

            if (p_non_skin != 0)
                prob = p_skin / p_non_skin;

//            ALOG("NATIVE LOG : Probability is  %f.", prob);

            if (prob < 0.4)
                imgSegmented.at<uchar>(i, j) = 0;
            else
                imgSegmented.at<uchar>(i, j) = 255;
        }
    }

    return imgSegmented;
}


bool LoadFile(char const *filename) {
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

/*
 * It simply adds the Gaussian function with weight to the array of Gaussians
 * required for GMM.
 * These 3 arrays are effected:
 * MeanMat added to _MeanMat (an array of mean_matrices(3x1))
 * CovMat added to _CovMat (an array of covariance_matrices(3x3))
 * inverse of CovMat added to _CovMatI (an array of inverse_covariance_matrices(3x3))
 * Weight added to _Weight (an array of weights(float))
 * */

bool AddGaussian(cv::Mat &pr_mean_mat, cv::Mat &pr_cov_mat,
                        float pr_weight) {
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

void MakeLookUpTable(bool skin) {
    ALOG("Making look up table\n");
    // making the look up table..
    cv::Mat local_sample_mat(3, 1, CV_32FC1); // 32SC1

    for (int R = 0; R < 256; ++R) {
        for (int G = 0; G < 256; ++G) {
            for (int B = 0; B < 256; ++B) {
                local_sample_mat.at<float>(0) = R;
                local_sample_mat.at<float>(1) = G;
                local_sample_mat.at<float>(2) = B;
                if(skin){
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

/*
 * Mat sample is a 3x1 matrix containing R,G,B value
 * This method will return probability of the color for these R.G,B to be skin color
 * */

float GetProbability(cv::Mat sample) {
    float P = 0.0;

    // difference matrix
    cv::Mat diff_mat = sample.clone();

    // transpose of difference matrix
    cv::Mat diff_mat_trans(1, n_dim, CV_32FC1);

    float expo;

    cv::Mat exp_mat(1, 1, CV_32FC1, &expo);

    for (int i = 0; i < n_mixture; i++) {
//		cout << "sample matrix" << endl;;
//		cout << sample << endl;
//
//		cout << "mean matrix" << endl;;
//		cout << mean_mat[i] << endl;

        diff_mat = sample - mean_mat[i];

//		cout << "diff mat" << endl;;
//		cout << diff_mat << endl;

        diff_mat_trans = diff_mat.t();

//		cout << "diff mat transpose" << endl;;
//		cout << diff_mat_trans << endl;

        diff_mat = cov_matI[i] * diff_mat;

//		cout << "cov_matI * diff_mat" << endl;;
//		cout << diff_mat << endl;

        exp_mat = diff_mat_trans * diff_mat;
        expo *= (-0.5);
        P += (weight[i] * 1.0 / (pow(2 * CV_PI, 1.5) * sqrt(cv::determinant(cov_mat[i])))
              * exp(expo));
    }

    diff_mat.release();
    diff_mat_trans.release();

//	cout << "returned P= " << P << endl;
    return P;
}

float GetProbabilityByLookUp(bool skin, int R, int G, int B) {
    if(skin) {
        return probabilitySkin[R][G][B];
    } else {
        return probabilityNonSkin[R][G][B];
    }
}

bool SaveLookUpTable(char const *filename) {
    printf("Saving look up table for %s \n", filename);
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        return false;
    }

    for (int R = 0; R < 256; ++R) {
        for (int G = 0; G < 256; ++G) {
            if(std::strcmp(filename, filenameLookUpTableSkin) == 0) {
                fwrite(probabilitySkin[R][G], sizeof(float), 256, fp);
            } else {
                fwrite(probabilityNonSkin[R][G], sizeof(float), 256, fp);
            }
        }
    }
    fclose(fp);
    return true;
}


bool LoadLookUpTable(char const *filename) {

    FILE *fp = fopen(filename, "rb");

    if (!fp) {
        ALOG("NATIVE-LOG: file %s loading failed", filename);
        return false;
    }

    for (int R = 0; R < 256; ++R) {
        for (int G = 0; G < 256; ++G) {
            if(std::strcmp(filename, filenameLookUpTableSkin) == 0) {
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

int toGray(cv::Mat img, cv::Mat &gray) {
    cvtColor(img, gray, CV_RGBA2GRAY);
    if (gray.rows == img.rows && gray.cols == img.cols)
        return 1;
    else
        return 0;
}


JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_convertGray
        (JNIEnv *, jclass, jlong addrRgba, jlong addrGray) {

    cv::Mat &mRgb = *(cv::Mat *) addrRgba;
    cv::Mat &mGray = *(cv::Mat *) addrGray;

    int successInt = toGray(mRgb, mGray);
    return (jint) successInt;

}

JNIEXPORT jboolean JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_initialise(JNIEnv *env, jclass, jstring absPath) {

    const char * path;
    jboolean isCopy;
    path = env->GetStringUTFChars(absPath, &isCopy);
    if (isCopy == JNI_TRUE) {
        (env)->ReleaseStringUTFChars(absPath, path);
    }
    std::string buf(path);

    std::string skindis = buf;
    std::string nonskindis = buf;
    std::string skinmgm = buf;
    std::string nonskinmgm = buf;

    skindis.append(filenameLookUpTableSkin);
    nonskindis.append(filenameLookUpTableNonSkin);
    skinmgm.append(filenameMgmSkin);
    nonskinmgm.append(filenameMgmNonSkin);


    filenameLookUpTableSkin = &skindis[0];
    filenameLookUpTableNonSkin = &nonskindis[0];
    filenameMgmSkin = &skinmgm[0];
    filenameMgmNonSkin =&nonskinmgm[0];

    bool successBool = LoadSkinColorProbTable();
    return (jboolean) successBool;
}


JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getHandRegion
        (JNIEnv *, jclass, jlong addrSrc, jlong addrTarget) {
    cv::Mat mSrc = *(cv::Mat *) addrSrc;
    cv::Mat &mTarget = *(cv::Mat *) addrTarget;

    mTarget = GetHandRegion(mSrc);

    ALOG("NATIVE LOG : got hand region");

    int successInt = 0;
    if (mSrc.rows == mTarget.rows && mSrc.cols == mTarget.cols)
        successInt = 1;
    else
        successInt = 0;

    return (jint) successInt;
}


