#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include "Altura.h"
using namespace std;
using namespace cv;
using namespace Mestrado;

int Altura::CalcAltura(std::string path) {
    Mat src = imread(path);
    return src.rows;
}

int Altura::PegarAltura() {
    std::string imageFilePath = "E:\\Code\\felipemsfg.github.com\\mestrado\\opencvcpp\\ProjetoMestrado\\x64\\Debug\\imagens\\media-distancia.txt";
    std::string saveResultFolder = "E:\\Code\\felipemsfg.github.com\\mestrado\\opencvcpp\\ProjetoMestrado\\x64\\Debug\\imagens\\result\\";
    std::ifstream file(imageFilePath);
    std::string str;
    int count = 0;
    int alturaTotal = 0;
    int menorAltura = 10000;
    int maiorAltura = 0;
    while (std::getline(file, str))
    {
        int altura = CalcAltura(str);
        alturaTotal = altura + alturaTotal;
        if (altura > maiorAltura) {
            maiorAltura = altura;
        }
        if (altura < menorAltura) {
            menorAltura = altura;
        }
        count++;
    }

    int media = alturaTotal / count;
    return media;
}