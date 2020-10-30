#pragma once
#pragma once
#include <string>
#include<opencv2/opencv.hpp>

namespace Mestrado
{
    class TratarFundo
    {
    public:
        static cv::Mat RemoverFundo(std::string path);        
    };
}