#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include "RegionOfInterest.h"
using namespace std;
using namespace cv;
using namespace Mestrado;

void ROI::CriarImagesDaAreaDeInteresse(int alturaMedia) {
    std::string imageFilePath = "E:\\Code\\felipemsfg.github.com\\mestrado\\opencvcpp\\ProjetoMestrado\\x64\\Debug\\imagens\\imagens-sem-fundo.txt";
    std::string saveResultFolder = "E:\\Code\\felipemsfg.github.com\\mestrado\\opencvcpp\\ProjetoMestrado\\x64\\Debug\\area_de_interesse";
    std::ifstream file(imageFilePath);
    std::string str;
    while (std::getline(file, str))
    {
        CortarAreaDeInteresse(str, saveResultFolder, alturaMedia);       
    }
}

void ROI::CortarAreaDeInteresse(std::string str, std::string saveResultFolder, int alturaMedia) {
    Mat src = imread(str);
    Mat matRoi;
    Rect roi;
    roi.x = (src.cols / 2) - src.cols * 0.3 ;
    roi.y = alturaMedia - (alturaMedia * 0.25);
    roi.width = src.cols * 0.6;
    roi.height = alturaMedia * 0.55;
    matRoi = src(roi);

    imwrite(str + "_roi.png", matRoi);
}