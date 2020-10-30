#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include "Draw.h"
using namespace std;
using namespace cv;
using namespace Mestrado;

void DrawLine(cv::Mat img, cv::Point start, cv::Point end)
{
    int thickness = 2;
    int lineType = LINE_8;
    line(img,
        start,
        end,
        Scalar(255, 100, 50),
        thickness,
        lineType);
}