#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

void userInput();
void runExercise(int exercise);
void Exercise1();
void Exercise2();
void Exercise3();
void Exercise4();
void Exercise5();
void Exercise6();

int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    std::cout << "Introducao a Computacao Grafica Lab 1 " "!\n";
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    userInput();
}

void userInput()
{
    int exercise;

    std::cout << "\n";
    std::cout << "Selecione um exercicio de 1 a 8" << std::endl;
    std::cin >> exercise;
    runExercise(exercise);
}

void runExercise(int exercise)
{
    switch (exercise)
    {
    case 1:
        Exercise1();
        break;
    case 2:
        Exercise2();
        break;
    case 3:
        Exercise3();
        break;
    case 4:
        Exercise3();
        break;
    case 5:
        Exercise3();
        break;
    case 6:
        Exercise3();
        break;
    case 7:
        Exercise3();
        break;
    case 8:
        Exercise3();
        break;
    default:
        std::cout << "Invalid exercise" << std::endl;
        break;
    }
}

void mostrarImagem(std::string janela, cv::Mat imageRGB)
{
    cv::imshow(janela, imageRGB);
    cv::waitKey(2500);
    cv::destroyWindow(janela);
}

std::string buscarImagem()
{
    std::string pathImage;
    std::string imagem = "imagem.jpg";
    pathImage = "../sourceAssets/" + imagem;
    return pathImage;
}

void Exercise1()
{
    std::cout << "Rodando o Exercicio 1" << std::endl;

    std::string pathImage = buscarImagem();

    //Mostrar imagem
    cv::Mat image = cv::imread(pathImage, cv::IMREAD_COLOR);
    mostrarImagem("Mostrar imagem", image);

    //Mostrar Largura, Altura e Numero de Canais
    std::cout << "A imagem tem resolucao de: " << image.rows << " x " << image.cols
        << " e " << image.channels() << " canais. " << std::endl;

    userInput();
}


void Exercise2()
{
    std::cout << "Rodando o Exercicio 2" << std::endl;

    std::string pathImage = buscarImagem();

    //Armazenar imagem na varialvel
    cv::Mat imageRGB = cv::imread(pathImage, cv::IMREAD_COLOR);

    //Converter para grayscale e gravar em disco
    //duvida: usar IMREAD_GRAYSCALE OU RGB2GRAY?
    cv::Mat imageGray;
    cv::cvtColor(imageRGB, imageGray, cv::COLOR_RGB2GRAY);
    cv::imwrite("../output/imagem_gray.jpg", imageGray);
    std::cout << "A imagem em tons de cinza foi gravada em disco" << std::endl;

    //Mostrar imagem colorida e convertida para tons de cinza
    mostrarImagem("Colorida", imageRGB);
    mostrarImagem("Tons de Cinza", imageGray);

    userInput();
}

void Exercise3()
{
    std::cout << "Rodando o Exercicio 3" << std::endl;

    std::string pathImage = buscarImagem();

    cv::Mat imageGray = cv::imread(pathImage, cv::IMREAD_GRAYSCALE);
    cv::imwrite("../output/imagem_gray.jpg", imageGray);
    std::cout << "A imagem em tons de cinza foi gravada em disco" << std::endl;

    int x, y;
    std::cout << "Digite a coordenada x: ";
    std::cin >> x;

    std::cout << "Digite a coordenada y: ";
    std::cin >> y;

}

void Exercise4()
{
    std::cout << "Rodando o Exercicio 4" << std::endl;
}

void Exercise5()
{
    std::cout << "Rodando o Exercicio 5" << std::endl;
}

void Exercise6()
{
    std::cout << "Rodando o Exercicio 6" << std::endl;
}

void Exercise7()
{
    std::cout << "Rodando o Exercicio 7" << std::endl;
}

void Exercise8()
{
    std::cout << "Rodando o Exercicio 8" << std::endl;
}
