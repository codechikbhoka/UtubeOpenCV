#include <vector>
#include "../includes/Global.h"
#include "../includes/PoseEstimation.h"

void draw_image(cv::Mat img, std::string name)
{
    cv::namedWindow(name);
    cv::imshow(name, img);
    cv::waitKey(0);
}

void poseEstimation::init_extraction(int threadId, int startIndex, int endIndex)
{
    // call function with cv::Mat to return the orientation angles
}

bool poseEstimation::find_contour(cv::Mat img)
{

    if(img.empty()) {
        ALOG("NATIVE-LOG img is empty");
    }

    cv::cvtColor(img, img, CV_GRAY2BGR);
    cv::Mat img_copy = img.clone(); // create a gray copy of main image
    cv::cvtColor(img, img_copy, CV_BGR2GRAY);
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(img_copy, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    double largest_area=0;


    for (int i = 0; i < contours.size(); ++i)
    {
        double area = cv::contourArea(contours[i], false);// no orientation
        if (area>largest_area)
        {
            largest_area = area;
            largest_index = i;
        }
    }

    if(largest_index == -1)
    {
        ALOG("NATIVE-LOG No contours detected");
        return false;
    }

    cv::drawContours(img, contours, largest_index, cv::Scalar(255,0,0), 2);

    cv::Mat hand_contour_img;   // image to view hand contour and later the distance
    hand_contour_img = cv::Mat::zeros(img.size(), CV_8UC1); // coloury picture
    cv::drawContours(hand_contour_img, contours, largest_index, cv::Scalar(255));

    fit_ellipse(hand_contour_img);

    return true;
}

void poseEstimation::fit_ellipse(cv::Mat hand_contour_img)
{

    cv::RotatedRect box = cv::fitEllipse(contours[largest_index]);

    cv::Mat ellipse_image ;
    ellipse_image =  cv::Mat::zeros(hand_contour_img.size(), CV_8UC1);   // 8uc1 -> gray

    cv::ellipse(ellipse_image, box, (255));

//    draw_image(ellipse_image, "ellipse");
    calculate_prominent_point(hand_contour_img, ellipse_image, box);

}

void poseEstimation::calculate_prominent_point(cv::Mat hand_contour_img,cv::Mat ellipse_image, cv::RotatedRect box)
{


    cv::Point2f center = box.center;
    float ma = box.size.height/2;   // minor semi-axis

    float angle = box.angle;

    float ct = (float) std::cos(angle*PI/180);  // cos return double cast to float
    float st = (float) std::sin(angle*PI/180);

    float prom_temp_x = center.x + (ma)*st; // temp points
    float prom_temp_y = center.y - (ma)*ct; // to be overwritten

    // image containing only line
    cv::Mat line_prom_image ;   // used as both line and prominent point image
    line_prom_image = cv::Mat::zeros(hand_contour_img.size(), CV_8UC1); // gray image
    cv::line(line_prom_image, center, cv::Point((int) prom_temp_x+10,(int) prom_temp_y), (255));

//    draw_image(hand_contour_img, "line image");
    cv::bitwise_and(line_prom_image, hand_contour_img,line_prom_image);
//    draw_image(line_prom_image, "point image");

    int prominent_X=0, prominent_Y=0,countIntersection=0;

    //http://docs.opencv.org/2.4/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html#the-efficient-way
    for (int i = 0; i < line_prom_image.rows; ++i)
    {
        for (int j = 0; j < line_prom_image.cols; ++j)
        {

            if(line_prom_image.at<uchar>(i,j)==255.0)   // uchar ?
            {
                prominent_X += j;
                prominent_Y += i;
                countIntersection+=1;
            }
        }
    }

//    std::cout << "prom : " << prominent_X << " " << prominent_Y << "\n";

    if (countIntersection!=0)
    {
        prominent_X = prominent_X/countIntersection;
        prominent_Y = prominent_Y/countIntersection;
    }

    cv::circle(hand_contour_img, cv::Point(prominent_X, prominent_Y), 3, cv::Scalar(255),-1);

    prominent = cv::Point2f(prominent_X, prominent_Y);

//    draw_image(hand_contour_img, "prom point");
}

void poseEstimation::calculate_distance_features()
{
    distance_vector.clear();

    std::vector<cv::Point> largest;
    largest = contours[largest_index];

    for (int i = 0; i < largest.size()-1; ++i)  // graph
    {
        cv::Point pt1 = largest[i];
        cv::Point pt2 = largest[i+1];

        cv::LineIterator it(image, pt1, pt2);

        for (int j = 0; j < it.count; ++j,++it)
        {
            double dist = std::sqrt(std::pow((prominent.x - it.pos().x), 2) + std::pow((prominent.y - it.pos().y), 2)); // ? float double
            distance_vector.push_back(dist);
        }
    }
//        std::cout << distance_vector.size();

    save_to_file("raw/distances.txt", distance_vector);

}

void poseEstimation::sampling()
{
    normal_sum = 0;
    sampled_distance_vector.clear();

    int vec_size = distance_vector.size();
//    std::cout << "vec size : " << vec_size << "\n";
    float div = vec_size/500.0f;  // samples array size

    for(double n : distance_vector)
    {
        normal_sum += n;
    }



    if (div>0)
    {
        float i = 0;
        while(i < vec_size && sampled_distance_vector.size()<500) //???
        {
            int avgPoint = (int) i;
            sampled_distance_vector.push_back(distance_vector[avgPoint]/normal_sum);
            i+=div;
        }
    }
//    std::cout << "samp size : " << sampled_distance_vector.size() << "\n";

//    save_to_file("sampled_dist.txt", sampled_distance_vector);

    for (int j = 0; j < sampled_distance_vector.size(); ++j) {
        if(sampled_distance_vector[j]>1)
        {
            std::cout << "val";
            std::cout << sampled_distance_vector[j];
        }
    }

}



void poseEstimation::save_to_file(std::string filename, std::vector<double> vec)
{
    std::ofstream output_file(filename);
    std::ostream_iterator<double> output_iterator(output_file, "\n");
    std::copy(vec.begin(), vec.end(), output_iterator);

//    std::cout << "written\n";
}

void poseEstimation::minmaxscaler(std::vector<double> vec)
{
    normalized_distance_vector.clear();

    double min=vec[0], max=vec[0];
    int min_index = 0;

    for (int i = 0; i < vec.size(); ++i)
    {
        if(vec[i]<min)
        {
            min = vec[i];
            min_index = i;
            continue;
        }

        else if(vec[i]>max)
        {
            max = vec[i];
            continue;
        }
    }
//    std::cout << "z : " << max << "  " << min << "\n";
//    std::cout << max << "  " <<  min ;

//    min = min;
//    max = max_z;

    if (max == min)
    {
        throw "Invalid vector\n";
    }

    double a = 1/(max-min); // source -> http://stats.stackexchange.com/a/70808/129034
    double b = max - a*max;

    for(double val: vec)
    {
        double scale = a*val + b + 1;

        normalized_distance_vector.push_back(scale);
    }


//    std::cout << normalized_distance_vector.size();

    // rotate at min_index

    std::rotate(normalized_distance_vector.begin(), normalized_distance_vector.begin()+min_index,
                 normalized_distance_vector.end());

    save_to_file("../raw/normalized_dist.txt", normalized_distance_vector);
}

// TODO 1 -> INTERSECTION LINE AND CONTOUR
// TODO 2 -> 500 ARRAY LENGTH