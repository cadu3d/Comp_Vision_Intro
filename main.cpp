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
        std::cout << "Selecione um LAB:" << std::endl;
        std::cout << "1 -> Lab1" << std::endl;
        std::cout << "2 -> Lab2" << std::endl;
        std::cout << "3 -> Lab3" << std::endl;
        std::cout << "4 -> Projeto Final" << std::endl;
        std::cout << "-------------------" << std::endl;
        std::cout << "0 -> FECHAR" << std::endl;
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
