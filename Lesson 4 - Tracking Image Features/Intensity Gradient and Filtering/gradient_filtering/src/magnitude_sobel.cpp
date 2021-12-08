#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

void magnitudeSobel()
{
    // load image from file
    cv::Mat img;
    img = cv::imread("../images/img1gray.png");

    // convert image to grayscale
    cv::Mat imgGray;
    cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

    // apply smoothing using the GaussianBlur() function from the OpenCV
    // ToDo : Add your code here
    cv::Mat blurredImg = imgGray.clone();
    cv::GaussianBlur(imgGray, blurredImg, cv::Size(5, 5), 2);

    // create filter kernels using the cv::Mat datatype both for x and y
    // ToDo : Add your code here
    float sobel_x[9] = {-1, 0, +1,
                        -2, 0, +2, 
                        -1, 0, +1};
    cv::Mat kernel_x = cv::Mat(3, 3, CV_32F, sobel_x);

    float sobel_y[9] = {-1, -2, -1,
                        0, 0, 0, 
                        +1, +2, +1};
    cv::Mat kernel_y = cv::Mat(3, 3, CV_32F, sobel_y);

    // apply filter using the OpenCv function filter2D()
    // ToDo : Add your code here
    cv::Mat filtered_x;
    cv::filter2D(blurredImg, filtered_x, -1, kernel_x, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
    
    cv::Mat filtered_y;
    cv::filter2D(blurredImg, filtered_y, -1, kernel_y, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

    // compute magnitude image based on the equation presented in the lesson 
    // ToDo : Add your code here
    // cv::Mat squared_filtered_x;
    // filtered_x.convertTo(filtered_x, CV_32F);
    // squared_filtered_x.convertTo(squared_filtered_x, CV_32F);
    // cv::pow(filtered_x, 2, squared_filtered_x);

    // cv::Mat squared_filtered_y;
    // filtered_y.convertTo(filtered_y, CV_32F);
    // squared_filtered_y.convertTo(squared_filtered_y, CV_32F);
    // cv::pow(filtered_y, 2, squared_filtered_y);

    // cv::Mat added_squared;
    // added_squared.convertTo(added_squared, CV_32F);
    // cv::add(squared_filtered_x, squared_filtered_y, added_squared);

    // cv::Mat magnitude;
    // magnitude.convertTo(magnitude, CV_32F);
    // cv::sqrt(added_squared, magnitude);

    // magnitude.convertTo(magnitude, CV_8UC1);

    cv::Mat magnitude = imgGray.clone();
    for (int r = 0; r < magnitude.rows; r++)
    {
        for (int c = 0; c < magnitude.cols; c++)
        {
            magnitude.at<unsigned char>(r, c) = sqrt(pow(filtered_x.at<unsigned char>(r, c), 2) +
                                                     pow(filtered_y.at<unsigned char>(r, c), 2));
        }
    }
    

    // show result
    string windowName = "Gaussian Blurring";
    cv::namedWindow(windowName, 1); // create window
    cv::imshow(windowName, magnitude);
    cv::waitKey(0); // wait for keyboard input before continuing
}

int main()
{
    magnitudeSobel();
}