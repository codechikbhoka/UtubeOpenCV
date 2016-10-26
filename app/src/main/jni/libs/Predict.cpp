#include "../includes/Predict.h"
#include "../includes/Global.h"

void predict::load_model(std::string absPath)
{
    std::string filename = absPath + "/handy/raw/random_trees.xml";
    rtrees.load(&filename[0]);

}

bool predict::generate_normalized_vector(cv::Mat img)
{
    pose.image = img;

    if(!pose.find_contour(img)){
        return false;
    }

    pose.calculate_distance_features();
    pose.sampling();
    pose.minmaxscaler(pose.sampled_distance_vector);

    cv::Mat test_feature_matrix ;
    test_feature_matrix = cv::Mat(1, 500, CV_32F);

    for (int i = 0; i < 500; ++i) {
        test_feature_matrix.at<float>(0,i) = (float) pose.normalized_distance_vector[i];
    }
    pred_dist_vec.push_back(test_feature_matrix);

    return true;
}

void predict::predict_output()
{
    for(int i=0; i<pred_dist_vec.rows; i++)
    {
        cv::Mat samplemat(pred_dist_vec, cv::Range(i,i+1));
        float response = rtrees.predict(samplemat,cv::Mat());
        response_vec.push_back(response);
    }
}

float predict::getOrientation(cv::Mat img) {

    if(!generate_normalized_vector(img)){
        return 0;
    }

    predict_output();

    ALOG("NATIVE-LOG response : %f", response_vec[0]);

    return response_vec[0];
}




