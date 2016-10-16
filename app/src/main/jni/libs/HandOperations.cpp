#include "../includes/HandOperations.h"
#include "../includes/ColorHistogram.h"
#include "../includes/MixGaussian.h"
#include "../includes/Global.h"


// initializing fields

const std::string HandOperations::ALGO_GAUSSIAN = "algo_gaussian";
const std::string HandOperations::ALGO_HISOGRAM = "algo_histogram";
const std::string HandOperations::ALGO_MIXED = "algo_mixed";

const char* HandOperations::filenameLookUpTableSkin = "/handy/skin.dis";
const char* HandOperations::filenameLookUpTableNonSkin = "/handy/nonskin.dis";
const char* HandOperations::filenameMgmSkin = "/handy/skin.mgm";
const char* HandOperations::filenameMgmNonSkin = "/handy/nonskin.mgm";
const char* HandOperations::filenameHandyXY = "/handy/handxy.txt";


float HandOperations::prevGaussianMax = 0;
float HandOperations::prevHistogramMax = 0;
float HandOperations::prevMixedMax = 0;
cv::Mat HandOperations::imgSegmented;
cv::Mat HandOperations::p_image_gradient;
std::vector< std::vector<int> > HandOperations::vectPoints(0);
std::string HandOperations::filterAlgo = HandOperations::ALGO_GAUSSIAN;





// method definitions

bool HandOperations::drawMask(cv::Mat &img) {
    if(img.empty()){
        ALOG("NATIVE-LOG img is empty");
        return false;
    } else {
        ALOG("NATIVE-LOG img.size is %d:%d and vectPoints.size() is %d", img.cols, img.rows, vectPoints.size());
    }

    int t = 0;
    for (t = 0; t < vectPoints.size(); ++t) {
        img.at<uchar>(vectPoints[t][0], vectPoints[t][1]) = 255;
    }
    return true;
}


cv::Mat HandOperations::GetHandRegion(cv::Mat &src_img) {

    ALOG("NATIVE-LOG Getting Hand Region");
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


            float p_skin = MG.GetProbabilityByLookUp(true, R, G, B);
            float p_non_skin = MG.GetProbabilityByLookUp(false, R, G, B);
            float prob = 0;
            if (p_non_skin != 0)
                prob = p_skin / p_non_skin;

            if (filterAlgo == ALGO_GAUSSIAN) {
                prob = prob;
                if (prob > prevGaussianMax) {
//                    ALOG("NATIVE-LOG : Gaussian Probability is  %f.", prob);
                    prevGaussianMax = prob;
                }
            } else if (filterAlgo == ALGO_HISOGRAM) {
                prob = 100 * CH.QueryProbability(R, G, B);
                if (prob > prevHistogramMax) {
//                    ALOG("NATIVE-LOG : Histogram Probability is  %f.", prob);
                    prevHistogramMax = prob;
                }
            } else if (filterAlgo == ALGO_MIXED) {
                float ALPHA = 0.5;
                float probHist = 100 * CH.QueryProbability(R, G, B);
                prob = ALPHA * prob + (1 - ALPHA) * probHist;
                if (prob > prevMixedMax) {
//                    ALOG("NATIVE-LOG : Mixed Probability is  %f.", prob);
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

bool HandOperations::LoadSkinColorProbTable() {

    if (!MG.LoadLookUpTable(HandOperations::filenameLookUpTableSkin)) {
        printf("skin.dis not found or corrupted, creating fresh using skin.txt\n");

        if (!MG.LoadFile(HandOperations::filenameMgmSkin)) {
            printf("skin color distribution load error.\n");
            return false;
        }
        ALOG("making a lookup table which contains probability corresponding to a R,G,B for "
                     "a skin color distribution\n");

        MG.MakeLookUpTable(true);

        if (!MG.SaveLookUpTable(HandOperations::filenameLookUpTableSkin)) {
            printf("skin color distribution look up table save error.\n");
            return false;
        }
    }
    if (!MG.LoadLookUpTable(HandOperations::filenameLookUpTableNonSkin)) {
        if (!MG.LoadFile(HandOperations::filenameMgmNonSkin)) {
            printf("non-skin color distribution load error.\n");
            return false;
        }

        printf("making a lookup table for non-skin color distribution\n");
        MG.MakeLookUpTable(false);

        if (!MG.SaveLookUpTable(HandOperations::filenameLookUpTableNonSkin)) {
            printf("non-skin color distribution look up table save error.\n");
            return false;
        }
    }

    return true;
}

void HandOperations::MyFilledCircle(cv::Mat &img, cv::Point center) {
    int thickness = 1;
    int lineType = 8;

    cv::circle(img, center, 0.7*ColorHistogram::maxDistValue,  // radius
               255, thickness, lineType);
}

bool HandOperations::readMask() {

    std::ifstream file(filenameHandyXY);
    std::string str;

    if (!file) {
        ALOG("NATIVE-LOG: file %s loading failed", filenameHandyXY);
        return false;
    }

    while (std::getline(file, str)) {
        std::vector<int> vect;
        std::stringstream ss(str);
        int i;
        while (ss >> i) {
            vect.push_back(i);

            if (ss.peek() == ',')
                ss.ignore();
        }

//        ALOG("NATIVE-LOG pushing back into vectPoints");
        vectPoints.push_back(vect);

    }

    return true;

}


