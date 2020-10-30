#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include "Base.h"
#include "Draw.h"
#include "Hough.h"

using namespace std;
using namespace cv;
using namespace Mestrado;

cv::Mat GetHough(std::string path)
{
    Mat img, gray;
    img = imread(path);
    img = Mestrado::Base::GetSquareImage(img, 1000);
    cvtColor(img, gray, COLOR_BGR2GRAY);
    // smooth it, otherwise a lot of false circles may be detected
    GaussianBlur(gray, gray, Size(9, 9), 2, 2);
    //imshow("gaussian", gray);
    vector<Vec3f> circles;
    HoughCircles(gray, circles, HOUGH_GRADIENT,
        2, gray.rows / 8, 5, 24, 50, 80);

    Point maiorCentro = Point(0, 0);
    int maiorRaio = 0;

    Point maisAoCentro = Point(0, 0);
    int raio = 0;
    int diferenca = img.cols;
    for (size_t i = 0; i < circles.size(); i++)
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);

        if (radius > maiorRaio) {
            maiorCentro = center;
            maiorRaio = radius;
        }

        if (abs(img.cols / 2 - circles[i][0]) < diferenca) {
            diferenca = abs((img.cols / 2) - circles[i][0]);
            raio = radius;
            maisAoCentro = center;
        }

        // draw the circle center
        circle(img, center, 3, Scalar(0, 255, 0), -1, 8, 0);
        // draw the circle outline
        circle(img, center, radius, Scalar(0, 0, 255), 1, 8, 0);
    }

    /*circle(img, maiorCentro, maiorRaio, Scalar(0, 255, 255), 3, 8, 0);
    circle(img, maisAoCentro, raio, Scalar(255, 255, 255), 3, 8, 0);*/

    Mestrado::Draw::DrawLine(img, Point(img.rows / 2, 0), Point(img.rows / 2, img.cols));
    //namedWindow("circles", 1);
    //imshow("circles", img);
    imwrite(path + "circles.png", img);
    return img;
}