#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include "structIO.hpp"

using namespace std;

void matchDescriptors(cv::Mat &imgSource, cv::Mat &imgRef, vector<cv::KeyPoint> &kPtsSource, vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource, cv::Mat &descRef,
                      vector<cv::DMatch> &matches, string descriptorType, string matcherType, string selectorType)
{

    // configure matcher
    bool crossCheck = false;
    cv::Ptr<cv::DescriptorMatcher> matcher;

    if (matcherType.compare("MAT_BF") == 0)
    {

        int normType = descriptorType.compare("DES_BINARY") == 0 ? cv::NORM_HAMMING : cv::NORM_L2;
        matcher = cv::BFMatcher::create(normType, crossCheck);
        cout << "BF matching cross-check=" << crossCheck;
    }
    else if (matcherType.compare("MAT_FLANN") == 0)
    {
        if (descSource.type() != CV_32F)
        { // OpenCV bug workaround : convert binary descriptors to floating point due to a bug in current OpenCV implementation
            descSource.convertTo(descSource, CV_32F);
            descRef.convertTo(descRef, CV_32F);
        }

        //... TODO : implement FLANN matching
        cout << "FLANN matching";

        // matcher = cv::FlannBasedMatcher::create();
        matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    }

    // perform matching task
    if (selectorType.compare("SEL_NN") == 0)
    { // nearest neighbor (best match)

        double t = (double)cv::getTickCount();
        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << " (NN) with n=" << matches.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;
    }
    else if (selectorType.compare("SEL_KNN") == 0)
    { // k nearest neighbors (k=2)

        // TODO : implement k-nearest-neighbor matching
        int k = 2;
        vector<vector<cv::DMatch>> knnMatches;

        double t = (double)cv::getTickCount();
        matcher->knnMatch(descSource, descRef, knnMatches, k);

        // TODO : filter matches using descriptor distance ratio test
        for (int i = 0; i < knnMatches.size(); i++)
        {
            vector<cv::DMatch> knnMatch = knnMatches[i];

            float distance1 = knnMatch[0].distance;
            float distance2 = knnMatch[1].distance;

            float ratio = distance1 < distance2 ? distance1 / distance2 : distance2 / distance1;

            if (ratio <= 0.8)
            {
                matches.push_back(knnMatch[0]);
            }
        }
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << " (kNN) with n=" << matches.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;

        float discardRate = ((float) knnMatches.size() - matches.size()) / ((float) knnMatches.size());
        cout << " kNN discarded " << discardRate * 100 << "% matches." << endl;
    }

    // visualize results
    cv::Mat matchImg = imgRef.clone();
    cv::drawMatches(imgSource, kPtsSource, imgRef, kPtsRef, matches,
                    matchImg, cv::Scalar::all(-1), cv::Scalar::all(-1), vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    string windowName = "Matching keypoints between two camera images (best 50)";
    cv::namedWindow(windowName, 7);
    cv::imshow(windowName, matchImg);
    cv::waitKey(0);
}

int main()
{
    cv::Mat imgSource = cv::imread("../images/img1gray.png");
    cv::Mat imgRef = cv::imread("../images/img2gray.png");

    vector<cv::KeyPoint> kptsSource, kptsRef; 
    // readKeypoints("../dat/C35A5_KptsSource_BRISK_large.dat", kptsSource);
    // readKeypoints("../dat/C35A5_KptsRef_BRISK_large.dat", kptsRef);

    // readKeypoints("../dat/C35A5_KptsSource_BRISK_small.dat", kptsSource);
    // readKeypoints("../dat/C35A5_KptsRef_BRISK_small.dat", kptsRef); 

    readKeypoints("../dat/C35A5_KptsSource_SIFT.dat", kptsSource);
    readKeypoints("../dat/C35A5_KptsRef_SIFT.dat", kptsRef);

    cv::Mat descSource, descRef; 
    // readDescriptors("../dat/C35A5_DescSource_BRISK_large.dat", descSource);
    // readDescriptors("../dat/C35A5_DescRef_BRISK_large.dat", descRef);

    // readDescriptors("../dat/C35A5_DescSource_BRISK_small.dat", descSource);
    // readDescriptors("../dat/C35A5_DescRef_BRISK_small.dat", descRef);

    readDescriptors("../dat/C35A5_DescSource_SIFT.dat", descSource);
    readDescriptors("../dat/C35A5_DescRef_SIFT.dat", descRef);

    vector<cv::DMatch> matches;
    string matcherType = "MAT_BF"; // MAT_BF, MAT_FLANN
    string descriptorType = "DES_HOG"; // DES_BINARY, DES_HOG
    string selectorType = "SEL_NN"; // SEL_NN, SEL_KNN
    matchDescriptors(imgSource, imgRef, kptsSource, kptsRef, descSource, descRef, matches, descriptorType, matcherType, selectorType);
     
}
