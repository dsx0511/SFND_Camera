#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

using namespace std;

cv::Mat PerformNMS(cv::Mat corner_img, cv::Mat vis_img, int minResponse)
{
    // define size of sliding window
    int sw_size = 7;                  // should be odd so we can center it on a pixel and have symmetry in all directions
    int sw_dist = floor(sw_size / 2); // number of pixels to left/right and top/down to investigate

    // create output image
    vector<cv::KeyPoint> keyPoints;

    // loop over all pixels in the corner image
    for (int r = sw_dist; r < corner_img.rows - sw_dist - 1; r++) // rows
    {
        for (int c = sw_dist; c < corner_img.cols - sw_dist - 1; c++) // cols
        {
            // loop over all pixels within sliding window around the current pixel
            int max_val{0}; // keeps track of strongest response
            for (int rs = r - sw_dist; rs <= r + sw_dist; rs++)
            {
                for (int cs = c - sw_dist; cs <= c + sw_dist; cs++)
                {
                    // check wether max_val needs to be updated
                    int new_val = (int) corner_img.at<float>(rs, cs);
                    max_val = max_val < new_val ? new_val : max_val;
                }
            }

            int current_val = (int) corner_img.at<float>(r, c);
            // check wether current pixel is local maximum
            if ((current_val == max_val) && (current_val > minResponse))
            {
                cv::KeyPoint kp(c, r, sw_dist * 2);
                keyPoints.push_back(kp);
            }
                
        }
    }

    cv::Mat result_img;
    cv::drawKeypoints(vis_img, keyPoints, result_img);
  	  
    // visualize results
    std::string windowName = "NMS Result Image";
    cv::namedWindow(windowName, 5);
    cv::imshow(windowName, result_img);
    cv::waitKey(0);
  
    return result_img;
}

void cornernessHarris()
{
    // load image from file
    cv::Mat img;
    img = cv::imread("../images/img1.png");
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY); // convert to grayscale

    // Detector parameters
    int blockSize = 2;     // for every pixel, a blockSize × blockSize neighborhood is considered
    int apertureSize = 3;  // aperture parameter for Sobel operator (must be odd)
    int minResponse = 100; // minimum value for a corner in the 8bit scaled response matrix
    double k = 0.04;       // Harris parameter (see equation for details)

    // Detect Harris corners and normalize output
    cv::Mat dst, dst_norm, dst_norm_scaled;
    dst = cv::Mat::zeros(img.size(), CV_32FC1);
    cv::cornerHarris(img, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT);
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
    cv::convertScaleAbs(dst_norm, dst_norm_scaled);

    // visualize results
    string windowName = "Harris Corner Detector Response Matrix";
    cv::namedWindow(windowName, 4);
    cv::imshow(windowName, dst_norm_scaled);
    cv::waitKey(0);

    // TODO: Your task is to locate local maxima in the Harris response matrix 
    // and perform a non-maximum suppression (NMS) in a local neighborhood around 
    // each maximum. The resulting coordinates shall be stored in a list of keypoints 
    // of the type `vector<cv::KeyPoint>`.
    PerformNMS(dst_norm, dst_norm_scaled, minResponse);

    // vector<cv::KeyPoint> keyPoints;

    // for (int r = 0; r < dst_norm.rows; r++)
    // {
    //     for (int c = 0; c < dst_norm.cols; c++)
    //     {
    //         int response = (int) dst_norm.at<float>(r, c);
    //         if (response > minResponse)
    //         {
    //             cv::KeyPoint new_kp(c, r, 6, -1, response, 0, -1);
    //             // cv::KeyPoint new_kp;
    //             // new_kp.pt = cv::Point2f(r, c);
    //             // new_kp.size = 2 * apertureSize;
    //             // new_kp.response = response;

    //             bool overlap = false;
    //             for(auto kp = keyPoints.begin(); kp != keyPoints.end(); ++kp)
    //             {
    //                 float overlap_area = cv::KeyPoint::overlap(new_kp, *kp);
    //                 if (overlap_area > 0)
    //                 {
    //                     overlap = true;
    //                     if (new_kp.response > (*kp).response)
    //                     {
    //                         *kp = new_kp;
    //                         break;
    //                     }
    //                 }
    //             }

    //             if (!overlap)
    //                 keyPoints.push_back(new_kp);
    //         }
    //     }
    // }

    // cv::Mat dst_with_kp;
    // cv::drawKeypoints(dst_norm_scaled, keyPoints, dst_with_kp, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    // // visualize results
    // string windowName2 = "Harris corner key points.";
    // cv::namedWindow(windowName2, 4);
    // cv::imshow(windowName2, dst_with_kp);
    // cv::waitKey(0);

}

int main()
{
    cornernessHarris();
}