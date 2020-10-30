#pragma once
#include <string>
#include <opencv2/opencv.hpp>
namespace Mestrado
{
    class Base
    {
    public:
        static cv::Mat GetSquareImage(const cv::Mat& img, int target_width = 500);
    };

}