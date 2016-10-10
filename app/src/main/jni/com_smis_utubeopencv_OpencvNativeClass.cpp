#include "com_smis_utubeopencv_OpencvNativeClass.h"
#include <android/log.h>
#include <errno.h>
#include <android/asset_manager.h>
#include <fstream>

#define  LOG_TAG    "testjni"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

const static std::string ALGO_GAUSSIAN = "algo_gaussian";
const static std::string ALGO_HISOGRAM = "algo_histogram";
const static std::string ALGO_MIXED = "algo_mixed";

float prevGaussianMax = 0;
float prevHistogramMax = 0;
float prevMixedMax = 0;
float prevHistogramPixelMax = 0;
std::string filterAlgo = ALGO_GAUSSIAN;

namespace patch {
    template<typename T>
    std::string to_string(const T &n) {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}

bool LoadSkinColorProbTable() {

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


cv::Mat GetHandRegion(cv::Mat& src_img) {

    cv::Size src_size = src_img.size();
    if (imgSegmented.empty())
        imgSegmented = cv::Mat(src_size, CV_8UC1);

    int R = 0;
    int G = 0;
    int B = 0;


    for (int i = 0; i < src_img.rows; ++i) {
        for (int j = 0; j < src_img.cols; ++j) {
            R = src_img.at<cv::Vec4b>(i, j)[0];
            G = src_img.at<cv::Vec4b>(i, j)[1];
            B = src_img.at<cv::Vec4b>(i, j)[2];


            float p_skin = GetProbabilityByLookUp(true, R, G, B);
            float p_non_skin = GetProbabilityByLookUp(false, R, G, B);
            float prob = 0;
            if (p_non_skin != 0)
                prob = p_skin / p_non_skin;

            if (filterAlgo == ALGO_GAUSSIAN) {
                prob = prob;
                if (prob > prevGaussianMax) {
                    ALOG("NATIVE-LOG : Gaussian Probability is  %f.", prob);
                    prevGaussianMax = prob;
                }
            } else if (filterAlgo == ALGO_HISOGRAM) {
                prob = QueryProbability(R, G, B);
                if (prob > prevHistogramMax) {
                    ALOG("NATIVE-LOG : Histogram Probability is  %f.", prob);
                    prevHistogramMax = prob;
                }
            } else if (filterAlgo == ALGO_MIXED) {
                float ALPHA = 0.01;
                float probHist = QueryProbability(R, G, B);
                prob = ALPHA * prob + (1 - ALPHA) * probHist;
                if (prob > prevMixedMax) {
                    ALOG("NATIVE-LOG : Mixed Probability is  %f.", prob);
                    prevMixedMax = prob;
                }
            }

            if (prob < 0.4) {
                imgSegmented.at<uchar>(i, j) = 0;
//                src_img.at<cv::Vec4b>(i, j)[0] = 0;
//                src_img.at<cv::Vec4b>(i, j)[1] = 0;
//                src_img.at<cv::Vec4b>(i, j)[2] = 0;
//                src_img.at<cv::Vec4b>(i, j)[3] = 0;
            }
            else {
                imgSegmented.at<uchar>(i, j) = 255;
            }
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


float QueryProbability(int R, int G, int B) {
    if (n_total_pixel == 0) {
//        ALOG("NATIVE-LOG : returning 0");
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

float GetProbability(cv::Mat sample) {
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

float GetProbabilityByLookUp(bool skin, int R, int G, int B) {
    if (skin) {
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
            if (std::strcmp(filename, filenameLookUpTableSkin) == 0) {
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
            if (std::strcmp(filename, filenameLookUpTableSkin) == 0) {
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

bool readMask() {

    std::ifstream file(filenameHandyXY);
    std::string str;

    if (!file) {
        ALOG("NATIVE-LOG: file %s loading failed", filenameHandyXY);
        return false;
    }

    while (std::getline(file, str)) {
        std::vector<float> vect;
        std::stringstream ss(str);
        float i;
        while (ss >> i) {
            vect.push_back(i);

            if (ss.peek() == ',')
                ss.ignore();
        }

        std::string x = patch::to_string(vect[0]);
        std::string y = patch::to_string(vect[1]);
        vectPoints.push_back(vect);

//        std::string log = "NATIVE-LOG: " + x + "," + y;
//        ALOG(&log[0]);
    }

    return true;

}

bool drawMask(cv::Mat &img) {
    for (int t = 0; t < vectPoints.size(); ++t) {
        img.at<uchar>(vectPoints[t][0], vectPoints[t][1]) = 255;
    }
}

void FeedFrame(cv::Mat &src_image, cv::Mat &mask_image, cv::Point maxDistPoint) {
    if (src_image.empty() || mask_image.empty()) {
        ALOG("NATIVE-LOG returning");
        return;
    }
    // remove oldest history
//    ALOG("NATIVE-LOG : n_total_pixel is %d", n_total_pixel);
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

    n_hist_index++;
    if (n_hist_index == NUM_HISTOGRAM_HISTORY)
        n_hist_index = 0;

    if (n_hist < NUM_HISTOGRAM_HISTORY)
        n_hist++;
}

void LearnColor(cv::Mat &src_image, cv::Point maxDistPoint, double maxDistValue) {
    if (src_image.empty()) {
        return;
    }

    cv::Mat imgCircle(src_image.size(), CV_8UC1);
    imgCircle = cv::Scalar::all(0);
    cv::circle(imgCircle, maxDistPoint, 0.7 * maxDistValue,
               cv::Scalar(255), -1, 8, 0);
    FeedFrame(src_image, imgCircle, maxDistPoint);
}

void resetLearning() {
    n_hist = 0;
    n_hist_index = 0;
    n_total_pixel = 0;
    for (int i = 0; i < NUM_HISTOGRAM_HISTORY; ++i) {
        n_num_pixel[i] = 0;
    }
}

void MyFilledCircle(cv::Mat &img, cv::Point center) {
    int thickness = 1;
    int lineType = 8;

    cv::circle(img, center, maxDistValue,  // radius
               255, thickness, lineType);
}





JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_convertGray
        (JNIEnv *, jclass, jlong addrRgba, jlong addrGray) {

    cv::Mat &mRgb = *(cv::Mat *) addrRgba;
    cv::Mat &mGray = *(cv::Mat *) addrGray;

    int successInt = toGray(mRgb, mGray);
    return (jint) successInt;

}

JNIEXPORT jboolean JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_initialise(JNIEnv *env,
                                                                                  jclass,
                                                                                  jstring absPath) {

    const char *path;
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
    std::string handxy = buf;

    skindis.append(filenameLookUpTableSkin);
    nonskindis.append(filenameLookUpTableNonSkin);
    skinmgm.append(filenameMgmSkin);
    nonskinmgm.append(filenameMgmNonSkin);
    handxy.append(filenameHandyXY);


    filenameLookUpTableSkin = &skindis[0];
    filenameLookUpTableNonSkin = &nonskindis[0];
    filenameMgmSkin = &skinmgm[0];
    filenameMgmNonSkin = &nonskinmgm[0];
    filenameHandyXY = &handxy[0];

    if (!readMask()) {
        return false;
    }

    if (!LoadSkinColorProbTable()) {
        return false;
    }


    resetLearning();

    return (jboolean) true;

}

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getHandRegion
        (JNIEnv *, jclass, jlong addrSrc, jlong addrTarget) {
    cv::Mat mSrc = *(cv::Mat *) addrSrc;
    cv::Mat &mTarget = *(cv::Mat *) addrTarget;

    mTarget = GetHandRegion(mSrc);
    drawMask(mTarget);
    MyFilledCircle(mTarget, maxDisttPoint);

    if (learn_mode) {
        LearnColor(mSrc, maxDisttPoint, maxDistValue);
    }

    return (jint) 0;
}


JNIEXPORT jboolean JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getLearningMode
        (JNIEnv *, jclass) {
    return learn_mode;
}

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_setLearningMode
        (JNIEnv *, jclass, jboolean newLearnMode) {
    learn_mode = newLearnMode;
}

JNIEXPORT jstring JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getFilterAlgo
        (JNIEnv *, jclass){
    return (jstring)(&filterAlgo[0]);
}

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_setFilterAlgo
        (JNIEnv *env, jclass, jstring algo){

    const char *temp;
    jboolean isCopy;
    temp = env->GetStringUTFChars(algo, &isCopy);
    filterAlgo = temp;
    if (isCopy == JNI_TRUE) {
        (env)->ReleaseStringUTFChars(algo, temp);
    }

    ALOG("NATIVE-LOG : new filter algo -> %s", &filterAlgo[0]);
}