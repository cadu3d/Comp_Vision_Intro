#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "filters.h"
#include "utils.h"

void transformadaHough();

struct HoughParams
{
    double dp = 1.2;
    double minDist = 100;
    double param1 = 120;
    double param2 = 34;
    int minRadius = 80;
    int maxRadius = 250;
};

HoughParams houghParams;

void configurarHough();
void mostrarParametrosHough(const HoughParams& params);

void runLab2()
{
    int processar;

    std::cout << "\n";
    std::cout << "LAB 02 - TRANSFORMADA DE HOUGH, (0 -> VOLTAR): " << std::endl;
    std::cout << "1 -> Reset (deleta imagens pre-processadas e outputs)" << std::endl;
    std::cout << "2 -> Pre-Processar" << std::endl;
    std::cout << "3 -> Configurar parametros de Hough" << std::endl;
    std::cout << "4 -> Aplicar Transformada de Hough" << std::endl;
    std::cin >> processar;

    switch (processar)
    {
    case 0:
        return;
    case 1:
        limparOutput(2);
        runLab2();
        break;
    case 2:
    {
        int filtro = Filters::menuPreProcImagem();
        Filters::preProcImagem(filtro);
        runLab2();
        break;
    }
    case 3:
        configurarHough();
        runLab2();
        break;
    case 4:
        transformadaHough();
        break;
    default:
        std::cout << "Escolha invalida" << std::endl;
        runLab2();
        break;
    }
}

void mostrarParametrosHough(const HoughParams& params)
{
    std::cout << "dp = " << params.dp << std::endl;
    std::cout << "minDist = " << params.minDist << std::endl;
    std::cout << "param1 = " << params.param1 << std::endl;
    std::cout << "param2 = " << params.param2 << std::endl;
    std::cout << "minRadius = " << params.minRadius << std::endl;
    std::cout << "maxRadius = " << params.maxRadius << std::endl;
}

void configurarHough()
{
    std::cout << "\n";
    std::cout << "CONFIGURAR HOUGH CIRCLES" << std::endl;
    std::cout << "\n";

    std::cout << "dp (padrao 1.2): ";
    std::cin >> houghParams.dp;

    std::cout << "minDist - distancia minima entre centros (padrao 100): ";
    std::cin >> houghParams.minDist;

    std::cout << "param1 - limiar alto do Canny interno (padrao 120): ";
    std::cin >> houghParams.param1;

    std::cout << "param2 - limiar do acumulador Hough (padrao 34): ";
    std::cin >> houghParams.param2;

    std::cout << "minRadius - raio minimo (padrao 80): ";
    std::cin >> houghParams.minRadius;

    std::cout << "maxRadius - raio maximo (padrao 250): ";
    std::cin >> houghParams.maxRadius;

    std::cout << "\nParametros usados:" << std::endl;
    mostrarParametrosHough(houghParams);
}

void transformadaHough()
{
    std::vector<cv::Mat> imagens = buscarImagens();

    for (int i = 0; i < imagens.size(); ++i)
    {
        cv::Mat imagemCinza = imagens[i].clone();

        std::vector<cv::Vec3f> coordCirculos;
        cv::HoughCircles(
            imagemCinza,
            coordCirculos,
            cv::HOUGH_GRADIENT,
            houghParams.dp,
            houghParams.minDist,
            houghParams.param1,
            houghParams.param2,
            houghParams.minRadius,
            houghParams.maxRadius
        );

        cv::Mat result;
        cv::cvtColor(imagemCinza, result, cv::COLOR_GRAY2BGR);

        for (const cv::Vec3f& circle : coordCirculos)
        {
            cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
            int radius = cvRound(circle[2]);

            cv::circle(result, center, radius, cv::Scalar(0, 255, 0), 3);
            cv::circle(result, center, 3, cv::Scalar(0, 0, 255), -1);
        }

        gravaImagem(result, i, "Lab_2");
    }

    std::cout << "Transformada de Hough aplicada em " << imagens.size()
        << " " << verificarOrigemOutput() << "." << std::endl;

    runLab2();
}

