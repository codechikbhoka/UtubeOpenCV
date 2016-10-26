#include "../includes/RandomForest.h"

void model::load_data()
{

    // load data and set test train split
    data.read_csv("../raw/features_norm_shuffled.csv");

    temp = data.get_values(); //https://github.com/kislayabhi/docs/blob/master/OpenCV%20SVM%20vs%20Shogun%20SVM.md
    int numFeatures = temp->cols - 1;

    data.set_response_idx(numFeatures); // azimuth from features.csv

    CvTrainTestSplit split(0.8f, true); //0.8 is train sample data set
    data.set_train_test_split(&split);

}
void model::set_up_Mat()
{
//    const CvMat* traindata_idx = data.get_train_sample_idx();// need the indices
//    const CvMat* testdata_idx = data.get_test_sample_idx();
//    cv::Mat mytraindataidx(traindata_idx);// convert to cv::Mat
//    cv::Mat mytestdataidx(testdata_idx);

    std::cout << "train size : " << mytraindataidx.size() << "\n";

    cv::Mat all_Data(temp);
    cv::Mat all_responses = data.get_responses();

    traindata = cv::Mat(mytraindataidx.cols,temp->cols -1,CV_32F);
    trainresponse = cv::Mat(mytraindataidx.cols,1,CV_32F);

    testdata = cv::Mat(mytestdataidx.cols,temp->cols -1,CV_32F);
    testresponse = cv::Mat(mytestdataidx.cols,1,CV_32F);

    for(int i=0; i<mytraindataidx.cols; i++)
    {
        trainresponse.at<float>(i)=all_responses.at<float>(mytraindataidx.at<int>(i));
        for(int j=0; j<=temp->cols -1; j++)
        {
            traindata.at<float>(i, j)=all_Data.at<float>(mytraindataidx.at<int>(i), j);
        }
    }

    for(int i=0; i<mytestdataidx.cols; i++)
    {
        testresponse.at<float>(i)=all_responses.at<float>(mytestdataidx.at<int>(i));
        for(int j=0; j<=temp->cols -1; j++)
        {
            testdata.at<float>(i, j)=all_Data.at<float>(mytestdataidx.at<int>(i), j);
        }
    }

    var_type = cv::Mat(temp->cols, 1, CV_8U);
    var_type.setTo(cv::Scalar(CV_VAR_NUMERICAL));
    var_type.at<uchar>(temp->cols-1,0) = CV_VAR_ORDERED; // cool done here!

}

void model::set_forest_params()
{
    params=CvRTParams(2, // max_depth
                                 2, // min sample count
                                 0, // regression_accuracy -> Not sure abt this.
                                 false,
                                 2, // max categories -> not used in regression
                                 0,
                                 true,
                                 2, // feature subset to look for split feature node.
                                 5, // no of trees
                                 0.0001f,
                      CV_TERMCRIT_ITER | CV_TERMCRIT_EPS
    );

}



#include <fstream>

void writeCSV(std::string filename, cv::Mat m)
{
    std::ofstream myfile;
    myfile.open(filename.c_str());
    myfile<< cv::format(m, "CSV") << std::endl;
    myfile.close();
    std::cout << "written\n";
}

void model::train_model()
{
    static int i = 0;
    std::cout << "train model size : " << traindata.size() << " " << trainresponse.size() << "\n";

//    std::string str = "csv/file_train_"+std::to_string(i)+".csv";
//    std::string str2 = "csv/file_test_"+std::to_string(i)+".csv";
//
//    writeCSV(str, traindata);
//    writeCSV(str2, testdata);
    rtrees.train(traindata, CV_ROW_SAMPLE, trainresponse, cv::Mat(),
                 cv::Mat(), var_type, cv::Mat(), params);
    std::cout << "Done\n";
    i+=1;
}

void model::predict_test()
{

    int k=0;
    for(int i=0; i<testdata.rows; i++)
    {
        cv::Mat samplemat(testdata, cv::Range(i,i+1));
        float response = rtrees.predict(samplemat,cv::Mat());
        k=(response==testresponse.at<int>(i))?++k:k;
//        std::cout << "\nresponse : " << response << "\n" ;

        response_vec.push_back(response); // response vector for r2 calc
    }

}

void model::calculate_error()
{
    float train_error = rtrees.calc_error(&data, CV_TRAIN_ERROR);

    float test_error = rtrees.calc_error(&data, CV_TEST_ERROR);

    std::cout << "Train error : " << train_error << std::endl;
    std::cout << "Test Error : " << test_error << std::endl;
}

void model::start_model()
{
//    load_data();
    set_up_Mat();
    set_forest_params();
    train_model();
    predict_test();
}

void model::save_model()
{
    // Final model saving
    set_up_Mat();
    train_model();
    rtrees.save("../raw/random_trees.xml");
}


model* model::myInstance = 0;

model* model::return_obj()
{
    if(myInstance != NULL)
        return myInstance;
    else
        myInstance = new model();
        return myInstance;
}

