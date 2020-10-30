#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include "Skeleton.h"
using namespace std;
using namespace cv;
using namespace Mestrado;

cv::Mat GetSkeleton(cv::Mat& src) {
    cv::threshold(src, src, 127, 255, cv::THRESH_BINARY);
    cv::Mat skel(src.size(), CV_8UC1, cv::Scalar(0));
    cv::Mat temp;
    cv::Mat eroded;

    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

    bool done;
    do
    {
        cv::erode(src, eroded, element);
        cv::dilate(eroded, temp, element); // temp = open(img)
        cv::subtract(src, temp, temp);
        cv::bitwise_or(skel, temp, skel);
        eroded.copyTo(src);

        done = (cv::countNonZero(src) == 0);
    } while (!done);

    return skel;
}

cv::Mat GetSkeletonFromFile(std::string path) {
    Mat src = imread(path);

    cvtColor(src, src, cv::COLOR_BGR2GRAY);
    threshold(src, src, 127, 255, THRESH_BINARY);
    Mat skel(src.size(), CV_8UC1, cv::Scalar(0));
    Mat temp;
    Mat eroded;

    Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

    bool done;
    do
    {
        cv::erode(src, eroded, element);
        cv::dilate(eroded, temp, element); // temp = open(img)
        cv::subtract(src, temp, temp);
        cv::bitwise_or(skel, temp, skel);
        eroded.copyTo(src);

        done = (cv::countNonZero(src) == 0);
    } while (!done);

    return skel;
}