#include "../includes/TrainOrientation.h"
#include "../includes/Global.h"

predict mPred;

inline bool exist(const std::string& filename)
{
    FILE *fp;
    fp = fopen(&filename[0], "rt");

    if(!fp)            // If the file was not found, then file is 0, i.e. !file=1 or true.
        return false;    // The file was not found.
    else                 // If the file was found, then file is non-0.
        return true;     // The file was found.
}

int initTrainModel(std::string absPath)
{
    ALOG("NATIVE-LOG initTrainModel");

    std::string RF_MODEL_PATH_AZIMUTH = absPath + "/handy/raw/random_trees_azimuth.xml";
    std::string RF_MODEL_PATH_ELEVATION = absPath + "/handy/raw/random_trees_elevation.xml";

    ALOG("NATIVE-LOG RF_MODEL_PATH_AZIMUTH %s", &RF_MODEL_PATH_AZIMUTH[0]);
    ALOG("NATIVE-LOG RF_MODEL_PATH_ELEVATION %s", &RF_MODEL_PATH_ELEVATION[0]);

    if(!exist(RF_MODEL_PATH_AZIMUTH) || !exist(RF_MODEL_PATH_ELEVATION)) {
        ///////// Model Training ///////////////////
        ALOG("NATIVE-LOG Model not found...");

    } else {
        ALOG("NATIVE-LOG Models found... training not required");
        mPred.load_models(absPath);
    }

    return 0;
}

float getAzimuth(cv::Mat mTarget)
{
    float azimuth = mPred.getOrientation(mTarget, mPred.AZIMUTH);
    return azimuth;
}

float getElevation(cv::Mat mTarget)
{
    float elevation = mPred.getOrientation(mTarget, mPred.ELEVATION);
    return elevation;
}