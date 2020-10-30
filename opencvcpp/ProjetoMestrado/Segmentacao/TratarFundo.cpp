#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include "TratarFundo.h"
using namespace std;
using namespace cv;
using namespace Mestrado;

cv::Mat RemoverFundo(std::string path)
{
    //int size = 1000;
    Mat elementoEstruturante = (Mat_<float>(3, 3) <<
        1, 1, 1,
        1, -8, 1,
        1, 1, 1);

    Mat src = imread(path);
    //src = GetSquareImage(src, size);

    Mat src2;
    src.copyTo(src2);

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            double d = norm(src.at<Vec3b>(i, j));
            if (d < 50)
            {
                src.at<Vec3b>(i, j)[0] = 0;
                src.at<Vec3b>(i, j)[1] = 0;
                src.at<Vec3b>(i, j)[2] = 0;
            }
            else {
                src.at<Vec3b>(i, j)[0] = 255;
                src.at<Vec3b>(i, j)[1] = 255;
                src.at<Vec3b>(i, j)[2] = 255;
            }
        }
    }

    int menorLinha = src.rows;
    int maiorLinha = 0;
    int menorColuna = src.cols;
    int maiorColuna = 0;

    Mat temp;
    erode(src, temp, elementoEstruturante);
    erode(temp, temp, elementoEstruturante);

    for (int i = 0; i < temp.rows; i++) {
        for (int j = 0; j < temp.cols; j++) {
            double d = norm(temp.at<Vec3b>(i, j));
            if (d < 50)
            {
                src.at<Vec3b>(i, j)[0] = 0;
                src.at<Vec3b>(i, j)[1] = 0;
                src.at<Vec3b>(i, j)[2] = 0;
            }
            else {
                if (i < menorLinha) {
                    menorLinha = i;
                }
                if (i > maiorLinha) {
                    maiorLinha = i;
                }
                if (j < menorColuna) {
                    menorColuna = j;
                }
                if (j > maiorColuna) {
                    maiorColuna = j;
                }
                src.at<Vec3b>(i, j)[0] = 255;
                src.at<Vec3b>(i, j)[1] = 255;
                src.at<Vec3b>(i, j)[2] = 255;
            }
        }
    }

    Mat matCroped;
    cvtColor(src, matCroped, cv::COLOR_BGR2GRAY);
    threshold(matCroped, matCroped, 150, 255, THRESH_BINARY);
    Rect crop(menorColuna - 10, menorLinha - 10, maiorColuna + 10 - menorColuna, maiorLinha + 10 - menorLinha);
    src = src(crop);

    matCroped = src2(crop);

    // imshow("croped" + std::to_string(count), matCroped);

    return matCroped;
}