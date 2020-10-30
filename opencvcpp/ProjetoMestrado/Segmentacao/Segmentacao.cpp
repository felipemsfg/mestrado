#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include "Base.h"
#include "Altura.h"
#include "RegionOfInterest.h"
#include "Skeleton.h"
#include "TratarFundo.h"
#include "Hough.h"

using namespace std;
using namespace cv;
using namespace Mestrado;


std::string random_string(size_t length)
{
    auto randchar = []() -> char
    {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
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

Mat Segmentar(std::string path, bool color, bool doCrop, bool cropDst, std::string resultPath, bool saveResult, int count)
{
    int size = 1000;
    
    Mat src = imread(path);
    src = Mestrado::Base::GetSquareImage(src, size);
    //imshow("original" + std::to_string(count), src);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            double d = norm(src.at<Vec3b>(i, j));
            if (d >= 200)
            {
                src.at<Vec3b>(i, j)[0] = 0;
                src.at<Vec3b>(i, j)[1] = 0;
                src.at<Vec3b>(i, j)[2] = 0;
            }
            /*else {
                src.at<Vec3b>(i, j)[0] = 255;
                src.at<Vec3b>(i, j)[1] = 255;
                src.at<Vec3b>(i, j)[2] = 255;
            }*/
        }
    }

    //imshow("originaleditada" + std::to_string(count), src);
    
    Mat elementoEstruturante = (Mat_<float>(3, 3) << 
        1, 1, 1, 
        1, -8, 1, 
        1, 1, 1);

    Mat laplaciano;
    Mat sharp;
    filter2D(src, laplaciano, CV_32F, elementoEstruturante);    
    src.convertTo(sharp, CV_32F);
    //imshow("sharp" + std::to_string(count), sharp);
    imwrite(path + "_sharp.png", sharp);
    Mat imgResult = sharp - laplaciano;
    //Hough(path + "_sharp.png");


    imgResult.convertTo(imgResult, CV_8UC3);
    //imshow("sharp-laplaciano" + std::to_string(count), imgResult);
    imwrite(path + "_sharp-laplaciano.png", imgResult);
    laplaciano.convertTo(laplaciano, CV_8UC3);
    //imshow("laplaciano" + std::to_string(count), laplaciano);
    
    // dilate(laplaciano, laplaciano, elementoEstruturante);

    imwrite(path + "_laplaciano.png", laplaciano);


    Mat inverted = ~laplaciano;
    //imshow("inverted" + std::to_string(count), inverted);
    imwrite(path + "_inverted.png", inverted);
    
    Mestrado::Hough::GetHough(path + "_laplaciano.png");
        
    //imshow("Hough Circle Transform Demo", src);


    // Imagem binária
    Mat bw;
    cvtColor(imgResult, bw, COLOR_BGR2GRAY);
    threshold(bw, bw, 40, 255, THRESH_BINARY | THRESH_OTSU);
    //imshow("imagem binaria" + std::to_string(count), bw);
    imwrite(path + "_img-binaria.png", bw);
    

    // Dilatação
    Mat fechamento;
    Mat elementoEstruturante3 = Mat::ones(3, 3, CV_8U);
    morphologyEx(bw, fechamento,MORPH_CLOSE, elementoEstruturante);

    /*dilate(bw, dilatacao, elementoEstruturante3);
    erode(dilatacao, dilatacao, elementoEstruturante3);
    erode(dilatacao, dilatacao, elementoEstruturante3);
    dilate(dilatacao, dilatacao, elementoEstruturante3);*/
    //imshow("fechamento" + std::to_string(count), fechamento);
    imwrite(path + "_fechamento.png", fechamento);

    // distanceTransform
    Mat dist;
    distanceTransform(fechamento, dist, DIST_L2, 3);
    normalize(dist, dist, 0, 1.0, NORM_MINMAX);
    
    // Identificação dos picos
    threshold(dist, dist, 0.4, 1.0, THRESH_BINARY);

    // Dilatação
    Mat elementoEstruturante2 = Mat::ones(3, 3, CV_8U);
    dilate(dist, dist, elementoEstruturante);
    dilate(dist, dist, elementoEstruturante);
    //imshow("dilatacao dist" + std::to_string(count), dist);
    imwrite(path + "_dilatacao_dist.png", dist);

    //erode(dist, dist, elementoEstruturante);
    //imshow("erosao", dist);

    // Identificação dos contornos e marcação dos marcadores
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    dilate(dist_8u, dist_8u, elementoEstruturante2);
    // imshow("dist_8u" + std::to_string(count), dist_8u);

    vector<vector<Point> > contours;
    findContours(dist_8u, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
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
       
    // Gera cores aleatórias
    vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b=0;
        int g=0;
        int r=0;

        if (color == true) {
            b = theRNG().uniform(0, 256);
            g = theRNG().uniform(0, 256);
            r = theRNG().uniform(0, 256);
        }
        else {
            // Gera todas as cores brancas para não mudar a etapa de colorir (próxima)
            b = 255;
            g = 255;
            r = 255;
        }
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
   /* if (doCrop == true) {
        Rect crop((size / 10) * 2.5, (size / 10) * 2.5, (size / 10) * 5, (size / 10) * 5);
        greyMat = greyMat(crop);
    }*/

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

    //if (cropDst == true) {
    //    Rect cropDstRec((size / 10) * 2.5, (size / 10) * 2.5, (size / 10) * 5, (size / 10) * 5);
    //    dst = dst(cropDstRec);
    //}

    //imshow("watershed" + std::to_string(count), dst);
    imwrite(path + "_watershed.png", dst);

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

    if (saveResult == true) {
        std::string fileNamePrefix = random_string(14);
        imwrite(resultPath + fileNamePrefix + "_watershed.png", dst);
        imwrite(resultPath + fileNamePrefix + "_hitormiss.png", output_image);
    }
    
    return output_image;
    // return skel;
}

int main(int argc, char* argv[])
{
    // GetAlturaMedia
    // int altura = Mestrado::Altura::PegarAltura();
    int altura = 1345;
    Mestrado::ROI::CriarImagesDaAreaDeInteresse(altura);

    //std::string imageFilePath = "E:\\Code\\felipemsfg.github.com\\mestrado\\opencvcpp\\ProjetoMestrado\\x64\\Debug\\imagens\\000-list.txt";
    //std::string imageFilePath = "E:\\Code\\felipemsfg.github.com\\mestrado\\opencvcpp\\ProjetoMestrado\\x64\\Debug\\imagens\\media-distancia.txt";
    //std::string saveResultFolder = "E:\\Code\\felipemsfg.github.com\\mestrado\\opencvcpp\\ProjetoMestrado\\x64\\Debug\\imagens\\result\\";
    //std::ifstream file(imageFilePath);
    //std::string str;
    //int count = 0;
    //int alturaTotal = 0;
    //while (std::getline(file, str))
    //{       
    //    int altura = Altura(str);
    //    alturaTotal = altura + alturaTotal;
    //    // Mat result = Segmentar(str, true, false, true, saveResultFolder, true, count);
    //    // Mat result = RemoverFundo(str);
    //    //Mat result = Hough(str);

    //    //imshow("hit or miss " + std::to_string(count), result);
    //    
    //    // imwrite(str + "_hit-or-miss.png", result);
    //    cout << std::to_string(count) + " - " + std::to_string(altura) + "\n";
    //    count++;
    //}
    //int media = alturaTotal / count;
    //cout << "Media da altura: " + std::to_string(media) + " - " + str + "\n";

    //waitKey();
    return 0;
}



