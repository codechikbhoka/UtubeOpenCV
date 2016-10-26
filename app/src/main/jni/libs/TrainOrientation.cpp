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

    std::string RF_MODEL_PATH = absPath + "/handy/raw/random_trees.xml";

    ALOG("NATIVE-LOG RF_MODEL_PATH %s", &RF_MODEL_PATH[0]);

    if(!exist(RF_MODEL_PATH))
    {
        ///////// Model Training ///////////////////
        ALOG("NATIVE-LOG Model not found... starting training");

        // 1. K Fold
        // random forest
        model *forest = model::return_obj();

        utils ut;
        ut.gen_main(10);
        ut.k_fold_Cv(10);

        // 2. Final Model
        forest->mytraindataidx = cv::Mat(ut.vec).reshape(1).t();
        forest->save_model();
    } else {
        ALOG("NATIVE-LOG Model found... training not required");
        mPred.load_model(absPath);
    }



    return 0;
}

float getOrientation(cv::Mat mTarget)
{
    float azimuth = mPred.getOrientation(mTarget);
    return azimuth;
}