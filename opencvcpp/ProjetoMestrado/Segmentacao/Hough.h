#pragma once
#pragma once
#pragma once
#include <string>
#include <opencv2/opencv.hpp>

namespace Mestrado
{
    class Hough
    {       
    public:
        static cv::Mat GetHough(std::string path);
    };
}