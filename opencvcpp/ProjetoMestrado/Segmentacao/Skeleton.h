#pragma once
#include <string>
#include<opencv2/opencv.hpp>
namespace Mestrado
{
    class Skeleton
    {
    
    public:
        static cv::Mat GetSkeleton(cv::Mat& src);
        static cv::Mat GetSkeletonFromFile(std::string path);

    };

}