#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
using namespace std;
using namespace cv;


cv::Mat GetSquareImage(const cv::Mat& img, int target_width = 500)
{
    int width = img.cols,
        height = img.rows;

    cv::Mat square = cv::Mat::zeros(target_width, target_width, img.type());

    int max_dim = (width >= height) ? width : height;
    float scale = ((float)target_width) / max_dim;
    cv::Rect roi;
    if (width >= height)
    {
        roi.width = target_width;
        roi.x = 0;
        roi.height = height * scale;
        roi.y = (target_width - roi.height) / 2;
    }
    else
    {
        roi.y = 0;
        roi.height = target_width;
        roi.width = width * scale;
        roi.x = (target_width - roi.width) / 2;
    }

    cv::resize(img, square(roi), roi.size());

    return square;
}

void hitmiss(cv::Mat& src, cv::Mat& dst, cv::Mat& kernel)
{
    CV_Assert(src.type() == CV_8U && src.channels() == 1);

    cv::Mat k1 = (kernel == 1) / 255;
    cv::Mat k2 = (kernel == -1) / 255;

    cv::normalize(src, src, 0, 1, cv::NORM_MINMAX);

    cv::Mat e1, e2;
    cv::erode(src, e1, k1, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
    cv::erode(1 - src, e2, k2, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
    if (countNonZero(k2) <= 0) {
        e2 = src;
    }
    dst = e1 & e2;
}

cv::Mat Skeleton(Mat & src) {
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

cv::Mat SkeletonFromFile(std::string path) {
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


Mat Segmentar(std::string path)
{
    int size = 1500;
    
    Mat src = imread(path);
    src = GetSquareImage(src, size);

    /*Rect crop((size / 8)*3, (size / 8)*3, size / 4, size / 4);
    Mat src = origem(crop);*/

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            double d = norm(src.at<Vec3b>(i, j));
            if (d >= 200)
            {
                src.at<Vec3b>(i, j)[0] = 0;
                src.at<Vec3b>(i, j)[1] = 0;
                src.at<Vec3b>(i, j)[2] = 0;
            }
        }
    }
    
    Mat elementoEstruturante = (Mat_<float>(3, 3) <<
        1, 1, 1,
        1, -8, 1,
        1, 1, 1);

    Mat laplaciano;
    filter2D(src, laplaciano, CV_32F, elementoEstruturante);
    Mat sharp;
    src.convertTo(sharp, CV_32F);
    Mat imgResult = sharp - laplaciano;

    imgResult.convertTo(imgResult, CV_8UC3);
    laplaciano.convertTo(laplaciano, CV_8UC3);
    
    // Imagem binária
    Mat bw;
    cvtColor(src, bw, COLOR_BGR2GRAY);
    threshold(bw, bw, 40, 255, THRESH_BINARY | THRESH_OTSU);
    
    // distanceTransform
    Mat dist;
    distanceTransform(bw, dist, DIST_L2, 3);    
    normalize(dist, dist, 0, 1.0, NORM_MINMAX);
    
    // Identificação dos picos
    threshold(dist, dist, 0.4, 1.0, THRESH_BINARY);

    // Dilatação
    Mat elementoEstruturante2 = Mat::ones(3, 3, CV_8U);
    dilate(dist, dist, elementoEstruturante2);
    
    // Identificação dos contornos e marcação dos marcadores
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    vector<vector<Point> > contours;
    findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    Mat markers = Mat::zeros(dist.size(), CV_32S);
    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(markers, contours, static_cast<int>(i), Scalar(static_cast<int>(i) + 1), -1);
    }
    circle(markers, Point(5, 5), 3, Scalar(255), -1);
        
    // Watershed
    watershed(imgResult, markers);
    
    Mat mark;
    markers.convertTo(mark, CV_8U);
    bitwise_not(mark, mark);
    // imshow("mark", mark);

    
    // Gera cores aleatórias
    vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        /*int b = theRNG().uniform(0, 256);
        int g = theRNG().uniform(0, 256);
        int r = theRNG().uniform(0, 256);*/
        // Gera todas as cores brancas para não mudar a etapa de colorir (próxima)
        int b = 255;
        int g = 255;
        int r = 255;
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    
    // Imagem final colorida ou imagem branca com linhas pretas
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i, j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
            {
                dst.at<Vec3b>(i, j) = colors[index - 1];
            }
        }
    }

    Mat greyMat;
    cvtColor(dst, greyMat, cv::COLOR_BGR2GRAY);
    threshold(greyMat, greyMat, 150, 255, THRESH_BINARY );
    
    // Recorto a imagem para pegar somente a área de interesse 
    Rect crop((size / 10)*4.5, (size / 10)*4, (size / 10) * 1.2, (size / 10));
    greyMat = greyMat(crop);

    // Hit or Miss
    Mat kernel = (Mat_<int>(13, 18) <<
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0
        );

    

    //Mat1b kernel = imread("E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\elementoestruturante\\skeleto2.png", IMREAD_GRAYSCALE);
    Mat output_image;
    morphologyEx(greyMat, output_image, MORPH_HITMISS, kernel);

    const int rate = 1;
    kernel = (kernel + 1) * 127;
    kernel.convertTo(kernel, CV_8U);
    resize(kernel, kernel, Size(), rate, rate, INTER_NEAREST);
    // imshow("kernel", kernel);
    resize(greyMat, greyMat, Size(), rate, rate, INTER_NEAREST);
    resize(output_image, output_image, Size(), rate, rate, INTER_NEAREST);
    
    /*Mat skel = Skeleton(greyMat);
    imshow("skel", skel);*/
    return output_image;
    // return skel;
}



int main(int argc, char* argv[])
{
    //Mat src = imread("E:\\Google Drive\\Mestrado\\33 - Watershed - emgucv\\water_coins.jpg");
 /*   string path[6] = {
        "E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\0007275H 2013-09-18.png",
        "E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\0006091H 2015-03-21.png",
        "E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\0014459K 2013-09-25.png",
        "E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\0016892B 2013-07-17.png",
        "E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\0030456K 2017-08-30.png",
        "E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\0045527F 2013-01-23.png"
    };*/

//    Mat skeleton = imread("E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\elementoestruturante\\completo.png");

    //Mat skeleton = SkeletonFromFile("E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\elementoestruturante\\completo.png");
    //imshow("oba", skeleton);
//    imwrite("E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\elementoestruturante\\skeleto.png", skeleton);


    std::ifstream file("E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\000-list.txt");
    std::string str;
    while (std::getline(file, str))
    {       
        // Process str
        /*std::cout << str << std::endl;*/
        Mat result = Segmentar(str);
        //Mat result = Segmentar("E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\elementoestruturante\\completo.png");
        imshow("result " + str, result);      


    }

    waitKey();
    return 0;
}



#include<opencv2/opencv.hpp>
#include<iostream>
//using namespace std;
//using namespace cv;
//int main()
//{
//    Mat img = imread("E:\\Google Drive\\Mestrado\\33 - Watershed - emgucv\\water_coins.jpg");
//    //Mat img = imread("E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\0045527F 2013-01-23.png");
//
//    Mat gray;
//    cvtColor(img, gray, COLOR_BGR2GRAY);
//    imshow("Gray", gray);
//
//    Mat thresh;
//    threshold(gray, thresh, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);
//    imshow("Threshold", thresh);
//
//    // noise removal
//    Mat opening;
//    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
//    morphologyEx(thresh, opening, MORPH_OPEN, kernel, Point(-1,-1), 2);
//    imshow("noise removal", opening);
//
//    Mat sure_bg;
//    dilate(opening, sure_bg, kernel, Point(-1, -1), 3);
//    imshow("Surebg", sure_bg);
//
//    Mat distance;
//    distanceTransform(opening, distance, DIST_L2, 5);
//    normalize(distance, distance, 0, 1.0, NORM_MINMAX);
//    imshow("Distance Transform Image", distance);
//    
//    double max, min;
//    minMaxLoc(distance, &min, &max);
//
//    Mat sure_fg;
//    threshold(distance, sure_fg, max * 0.7, 255, 0);
//    imshow("Threshold Distance", distance);
//
//    Mat unknown;
//    subtract(sure_bg, sure_fg, unknown);
//    imshow("Unknown", unknown);
//
//    /*namedWindow("image", WINDOW_NORMAL);
//    imshow("image", distance);*/
//    waitKey(0);
//    return 0;
//
//}
//// Segmentacao.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
//
//#include <iostream>
//
//int main()
//{
//    std::cout << "Hello World!\n";
//}
//
//// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
//// Debug program: F5 or Debug > Start Debugging menu
//
//// Tips for Getting Started: 
////   1. Use the Solution Explorer window to add/manage files
////   2. Use the Team Explorer window to connect to source control
////   3. Use the Output window to see build output and other messages
////   4. Use the Error List window to view errors
////   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
////   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

