#include "../includes/Predict.h"
#include "../includes/Global.h"

const std::string predict::AZIMUTH = "azimuth";
const std::string predict::ELEVATION = "elevation";

/*
 * which : AZIMUTH or ELEVATION
 * */
void predict::load_models(std::string absPath) {
    std::string filename = "";

    filename = absPath + "/handy/raw/random_trees_azimuth.xml";
    rtreesAzimuth.load(&filename[0]);

    filename = absPath + "/handy/raw/random_trees_elevation.xml";
    rtreesElevation.load(&filename[0]);
}

bool predict::generate_normalized_vector(cv::Mat img) {
    pose.image = img;

    if (!pose.find_contour(img)) {
        return false;
    }

    pose.calculate_distance_features();
    pose.sampling();
    pose.minmaxscaler(pose.sampled_distance_vector);

    feature_matrix = cv::Mat(1, 500, CV_32F);

    for (int i = 0; i < 500; ++i) {
        feature_matrix.at<float>(0, i) = (float) pose.normalized_distance_vector[i];
    }

    return true;
}

/*
 * which : AZIMUTH or ELEVATION
 * */
void predict::predict_output(std::string which) {
    if (AZIMUTH.compare(which) == 0) { // equal
        response = rtreesAzimuth.predict(feature_matrix, cv::Mat());
        ALOG("NATIVE-LOG azimuth : %f", response);
    } else {
        response = rtreesElevation.predict(feature_matrix, cv::Mat());
        ALOG("NATIVE-LOG elevation : %f", response);
    }
}

float predict::getOrientation(cv::Mat img, std::string which) {

    if (!generate_normalized_vector(img)) {
        return 0;
    }

    predict_output(which);

    return response;
}



