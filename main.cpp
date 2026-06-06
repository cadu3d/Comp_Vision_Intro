#include <clocale>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

void runLab1();
void runLab2();
void runLab3();
void runLabFinal();


void showLabMenu()
{
    int labNumber = 0;

    while (true)
    {
        std::cout << "\n";
        std::cout << "Selecione um LAB de 1 a 3, ou" << std::endl;
        std::cout << "4 para Projeto Final " << std::endl;
        std::cout << "0 para FECHAR" << std::endl;
        std::cout << "> ";
        std::cin >> labNumber;

        switch (labNumber)
        {
        case 0:
            std::cout << "Encerrando o programa." << std::endl;
            return;
        case 1:
            runLab1();
            break;
        case 2:
            runLab2();
            break;
        case 3:
            runLab3();
            break;
        case 4:
            runLabFinal();
            break;
        default:
            std::cout << "Lab inválido." << std::endl;
            break;
        }
    }
}


int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    std::setlocale(LC_ALL, "pt_BR.UTF-8");
    std::cout << "Introdução à Computação Gráfica" << std::endl;
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;

    showLabMenu();
    return 0;
}
