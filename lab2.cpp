#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "filters.h"
#include "utils.h"

void configurarHough();
void mostrarParametrosHough();
void transformarHough();
void menuPreProcessamentoLab2();

// Parametros que controlam a deteccao de circulos.
double houghDp = 1.2;
double houghMinDist = 100;
double houghParam1 = 120;
double houghParam2 = 35;
int houghMinRadius = 80;
int houghMaxRadius = 250;

void runLab2()
{
    std::string processar;

    std::cout << "\n";
    std::cout << "LAB 02 - TRANSFORMADA DE HOUGH:" << std::endl;
    std::cout << "\n";
    std::cout << "0 -> VOLTAR" << std::endl;
    std::cout << "00 -> RESET" << std::endl;
    std::cout << "\n";
    std::cout << "1 -> Pre-Processar" << std::endl;
    std::cout << "2 -> Configurar parametros de Hough" << std::endl;
    std::cout << "3 -> Aplicar Transformada de Hough" << std::endl;
    std::cin >> processar;

    if (processar == "0")
    {
        return;
    }

    if (processar == "00")
    {
        limparOutput(2);
        runLab2();
        return;
    }

    if (processar == "1")
    {
        menuPreProcessamentoLab2();
        runLab2();
        return;
    }

    if (processar == "2")
    {
        configurarHough();
        runLab2();
        return;
    }

    if (processar == "3")
    {
        transformarHough();
        return;
    }

    std::cout << "Escolha invalida" << std::endl;
    runLab2();
}

void menuPreProcessamentoLab2()
{
    while (true)
    {
        std::string filtro = Filters::menuPreProcImagem();

        if (filtro == "0")
        {
            break;
        }

        if (filtro == "00")
        {
            limparOutput(2);
            continue;
        }

        // Aplicar o filtro escolhido pelo usuario.
        try
        {
            Filters::preProcImagem(std::stoi(filtro));
        }
        catch (const std::exception&)
        {
            std::cout << "Filtro invalido" << std::endl;
        }
    }
}

void mostrarParametrosHough()
{
    std::cout << "dp = " << houghDp << std::endl;
    std::cout << "minDist = " << houghMinDist << std::endl;
    std::cout << "param1 = " << houghParam1 << std::endl;
    std::cout << "param2 = " << houghParam2 << std::endl;
    std::cout << "minRadius = " << houghMinRadius << std::endl;
    std::cout << "maxRadius = " << houghMaxRadius << std::endl;
}

void configurarHough()
{
    std::cout << "\n";
    std::cout << "CONFIGURAR HOUGH CIRCLES" << std::endl;
    std::cout << "\n";

    // Ler os parametros usados pela funcao HoughCircles.
    std::cout << "dp (padrao 1.2): ";
    std::cin >> houghDp;

    std::cout << "minDist - distancia minima entre centros (padrao 100): ";
    std::cin >> houghMinDist;

    std::cout << "param1 - limiar alto do Canny interno (padrao 120): ";
    std::cin >> houghParam1;

    std::cout << "param2 - limiar do acumulador Hough (padrao 35): ";
    std::cin >> houghParam2;

    std::cout << "minRadius - raio minimo (padrao 80): ";
    std::cin >> houghMinRadius;

    std::cout << "maxRadius - raio maximo (padrao 250): ";
    std::cin >> houghMaxRadius;

    std::cout << "\nParametros usados:" << std::endl;
    mostrarParametrosHough();
}

void transformarHough()
{
    std::vector<cv::Mat> imagens = buscarImagens();

    for (int i = 0; i < imagens.size(); ++i)
    {
        cv::Mat imagemCinza = imagens[i].clone();

        // Encontrar os circulos na imagem em tons de cinza.
        std::vector<cv::Vec3f> coordCirculos;
        cv::HoughCircles(
            imagemCinza,
            coordCirculos,
            cv::HOUGH_GRADIENT,
            houghDp,
            houghMinDist,
            houghParam1,
            houghParam2,
            houghMinRadius,
            houghMaxRadius
        );

        // Converter para BGR para desenhar os circulos coloridos.
        cv::Mat result;
        cv::cvtColor(imagemCinza, result, cv::COLOR_GRAY2BGR);

        for (const cv::Vec3f& circulo : coordCirculos)
        {
            cv::Point centro(cvRound(circulo[0]), cvRound(circulo[1]));
            int raio = cvRound(circulo[2]);

            cv::circle(result, centro, raio, cv::Scalar(0, 255, 0), 3);
            cv::circle(result, centro, 3, cv::Scalar(0, 0, 255), -1);
        }

        gravaImagem(result, i, "Lab_2");
    }

    std::cout << "Transformada de Hough aplicada em " << imagens.size()
        << " " << verificarOrigemOutput() << "." << std::endl;

    runLab2();
}
