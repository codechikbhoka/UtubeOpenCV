#include "../includes/OpencvNativeClass.h"
#include "../includes/ColorHistogram.h"
#include "../includes/HandOperations.h"
#include "../includes/Fingertip.h"
#include "../includes/Global.h"
#include "../includes/TrainOrientation.h"

HandOperations _HO;
ColorHistogram _CH;
Fingertip _FT;
cv::Point2f _PrevCentroid, _CurrCentroid;
float ringPositionX=0, ringPositionY=0;

void MyFilledCircle(cv::Mat &img, cv::Point center) {
    int thickness = 1;
    int lineType = 8;

    cv::circle(img, center, 0.7*ColorHistogram::maxDistValue,  // radius
               255, thickness, lineType);
}

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_convertGray
        (JNIEnv *, jclass, jlong addrRgba, jlong addrGray) {

    cv::Mat &mRgb = *(cv::Mat *) addrRgba;
    cv::Mat &mGray = *(cv::Mat *) addrGray;

    cvtColor(mRgb, mGray, CV_RGBA2GRAY);

    if (mGray.rows == mRgb.rows && mGray.cols == mRgb.cols)
        return 1;
    else
        return 0;

}


JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getHandRegion
(JNIEnv * env, jclass myJclass, jlong addrSrc, jlong addrTarget) {
    cv::Mat mSrc = *(cv::Mat *) addrSrc;
    cv::Mat &mTarget = *(cv::Mat *) addrTarget;

    mTarget = _HO.GetHandRegion(mSrc);

    _FT.RefreshMaxDistPoint(mTarget);
    _PrevCentroid = _CurrCentroid;
    _CurrCentroid = _FT._maxDistPoint;
    ringPositionX = _CurrCentroid.x;
    ringPositionY = _CurrCentroid.y;

    //getAzimuth(mTarget);
    //getElevation(mTarget);

// Draw Mask and Circle on silhoutte image
    //_HO.drawMask(mTarget);
    _HO.MyFilledCircle(mTarget, ColorHistogram::maxDisttPoint);

    if (ColorHistogram::learn_color_histogram) {
        _CH.LearnColor(mSrc, ColorHistogram::maxDisttPoint, ColorHistogram::maxDistValue);
    }


// Draw circle on RGB image
    // cv::Mat &mSrcRef = *(cv::Mat *) addrSrc;
    // MyFilledCircle(mSrcRef, _CurrCentroid);

    return (jint) 0;
}

JNIEXPORT jboolean JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getLearningMode
        (JNIEnv *, jclass) {
    return ColorHistogram::learn_color_histogram;
}

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_setLearningMode
        (JNIEnv *, jclass, jboolean shouldLearn) {
    ColorHistogram::learn_color_histogram = shouldLearn;
    return 0;
}

JNIEXPORT jstring JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getFilterAlgo
        (JNIEnv *, jclass){
    return (jstring)(&HandOperations::filterAlgo[0]);
}

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_setFilterAlgo
        (JNIEnv *env, jclass, jstring algo){

    const char *temp;
    jboolean isCopy;
    temp = env->GetStringUTFChars(algo, &isCopy);
    HandOperations::filterAlgo = temp;
    if (isCopy == JNI_TRUE) {
        (env)->ReleaseStringUTFChars(algo, temp);
    }

    ALOG("NATIVE-LOG : new filter algo -> %s", &HandOperations::filterAlgo[0]);
    return 0;
}

JNIEXPORT jfloat JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getRingPositionX
        (JNIEnv *env, jclass){

    return ringPositionX;
}


JNIEXPORT jfloat JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getRingPositionY
        (JNIEnv *env, jclass){

    return ringPositionY;
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

    initTrainModel(buf);

    skindis.append(HandOperations::filenameLookUpTableSkin);
    nonskindis.append(HandOperations::filenameLookUpTableNonSkin);
    skinmgm.append(HandOperations::filenameMgmSkin);
    nonskinmgm.append(HandOperations::filenameMgmNonSkin);
    handxy.append(HandOperations::filenameHandyXY);

    HandOperations::filenameLookUpTableSkin = &skindis[0];
    HandOperations::filenameLookUpTableNonSkin = &nonskindis[0];
    HandOperations::filenameMgmSkin = &skinmgm[0];
    HandOperations::filenameMgmNonSkin = &nonskinmgm[0];
    HandOperations::filenameHandyXY = &handxy[0];

    if (!_HO.readMask()) {
        return false;
    }

    if (!_HO.LoadSkinColorProbTable()) {
        return false;
    }


    _CH.ResetLearning();

    return (jboolean) true;

}