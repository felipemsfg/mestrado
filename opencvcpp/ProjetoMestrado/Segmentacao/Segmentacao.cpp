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

Mat Segmentar(std::string path)
{
    Mat src = imread(path);
    src = GetSquareImage(src, 500);

    //if (src.empty())
    //{
    //    /*cout << "Could not open or find the image!\n" << endl;
    //    cout << "Usage: " << argv[0] << " <Input image>" << endl;*/
    //    return ;
    //}

    //imshow("1 - Original", src);

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            double d = norm(src.at<Vec3b>(i, j));
            //if (src.at<Vec3b>(i, j) >= Vec3b(255, 255, 255))
            //printf("%2f\n", d);
            if (d >= 200)
            {
                src.at<Vec3b>(i, j)[0] = 0;
                src.at<Vec3b>(i, j)[1] = 0;
                src.at<Vec3b>(i, j)[2] = 0;
            }
        }
    }
    //imshow("2 - Black Background Image", src);




    // Create a kernel that we will use to sharpen our image
    Mat kernel = (Mat_<float>(3, 3) <<
        1, 1, 1,
        1, -8, 1,
        1, 1, 1);

    Mat imgLaplacian;
    filter2D(src, imgLaplacian, CV_32F, kernel);
    Mat sharp;
    src.convertTo(sharp, CV_32F);
    Mat imgResult = sharp - imgLaplacian;

    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
    //imshow("3 - Laplaciano", imgResult);

    // Create binary image from source image
    Mat bw;
    cvtColor(src, bw, COLOR_BGR2GRAY);
    threshold(bw, bw, 40, 255, THRESH_BINARY | THRESH_OTSU);
    //imshow("4 - Binary Image", bw);

    // Perform the distance transform algorithm
    Mat dist;
    distanceTransform(bw, dist, DIST_L2, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1.0, NORM_MINMAX);
    //imshow("5 - Distance Transform Image", dist);

    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, 0.4, 1.0, THRESH_BINARY);

    // Dilate a bit the dist image
    Mat kernel1 = Mat::ones(3, 3, CV_8U);
    dilate(dist, dist, kernel1);
    //imshow("6 - Peaks", dist);

    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);

    // Find total markers
    vector<vector<Point> > contours;
    findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // Create the marker image for the watershed algorithm
    Mat markers = Mat::zeros(dist.size(), CV_32S);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(markers, contours, static_cast<int>(i), Scalar(static_cast<int>(i) + 1), -1);
    }
    // Draw the background marker
    circle(markers, Point(5, 5), 3, Scalar(255), -1);
    //imshow("Markers", markers);
    // Perform the watershed algorithm    
    watershed(imgResult, markers);


    Mat mark;
    markers.convertTo(mark, CV_8U);
    bitwise_not(mark, mark);
    //imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
    // image looks like at that point
    // Generate random colors
    vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = theRNG().uniform(0, 256);
        int g = theRNG().uniform(0, 256);
        int r = theRNG().uniform(0, 256);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    // Create the result image
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
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
    // Visualize the final image
    //imshow("Final Result", dst);
    return dst;
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


    std::ifstream file("E:\\Google Drive\\Mestrado\\52 - Base de imagens\\teste\\000-list.txt");
    std::string str;
    while (std::getline(file, str))
    {
        // Process str
        /*std::cout << str << std::endl;*/
        Mat result = Segmentar(str);
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

