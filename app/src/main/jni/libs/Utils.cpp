#include "../includes/Utils.h"
#include "../includes/RandomForest.h"

void utils::shuffle()
{
    // implement later
    ;
}

void utils::gen_main(int num_of_folds)
{
    forest->load_data();

    length_of_inputs = forest->temp->rows;

    int flag = length_of_inputs%10;
    length_of_inputs-=flag;
    int div = length_of_inputs/num_of_folds;


    for (int i = 0; i < length_of_inputs; ++i)
    {
        vec.push_back(i);
    }

    k_fold_mat = cv::Mat(vec).reshape(1,10);    // first arg -> channels
}

void utils::k_fold_Cv(int num_of_folds)
{

    std::cout << forest->check;

    length_of_inputs = forest->temp->rows;

    int div = length_of_inputs/num_of_folds;

    train_idx = cv::Mat(1,div,CV_32S);
    test_idx = cv::Mat(1,div,CV_32S);

    std::vector<int> temp_vec;

    // data has been shuffled.
    for (int i = 0; i < num_of_folds; ++i)
    {
        test_idx = k_fold_mat.row(i);

        temp_vec = vec;
        temp_vec.erase(temp_vec.begin()+i*div,
                        temp_vec.begin()+(i+1)*div);

        train_idx = cv::Mat(temp_vec).reshape(1).t();

        std::cout << "size : " << test_idx.size() << "\n";

        forest->mytestdataidx = test_idx;   // set ids for variables used to
        forest->mytraindataidx = train_idx; // generate train test data in Mat format

//        std::cout << "loop : " << temp_vec[i*div] << "\n";    // correct partitions
        forest->start_model();
        R2_error(i);
    }

    float mean = 0;

    for (int j = 0; j < num_of_folds; ++j) {
        mean += r2_err[j];
    }

    mean /= num_of_folds;

    std::cout << "\n\n mean is : " << mean << "\n";

}

void utils::R2_error(int index)
{
    // calculate R2 error

    std::vector<float> resp = forest->response_vec;
    std::vector<float> pred ;

    forest->testresponse.reshape(1,1).copyTo(pred);

    assert(pred.size()==resp.size());

    float avg = 0.0f;

    for (int i = 0; i < pred.size(); ++i)
    {
        avg += pred[i];
    }

    avg /= pred.size();

    float SSRes=0, SStot=0;

    for (int i = 0; i < pred.size(); ++i)
    {
        SSRes += pow((pred[i] - resp[i]),2);

        SStot += pow((resp[i]-avg), 2);
    }

    float R2 = 1 - (SSRes/SStot);
    std::cout << "i : " << index << " " << R2 << "\n";
    r2_err.push_back(R2);

    // clear forest response vec
    forest->response_vec.clear();

}