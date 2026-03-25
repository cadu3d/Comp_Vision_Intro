#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

void selectExerciceGUI();
void runExercise(int exercise);
void Exercise1();
void Exercise2();
void Exercise3();
void Exercise4();
void Exercise5();
void Exercise6();
void Exercise7();
void Exercise8();

int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    std::cout << "Introducao a Computacao Grafica Lab 1 " "!\n";
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    selectExerciceGUI();
}

void selectExerciceGUI()
{
    int exercise;

    std::cout << "\n";
    std::cout << "Selecione um exercicio de 1 a 8: " << std::endl;;
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
        Exercise4();
        break;
    case 5:
        Exercise5();
        break;
    case 6:
        Exercise6();
        break;
    case 7:
        Exercise7();
        break;
    case 8:
        Exercise8();
        break;
    default:
        std::cout << "Invalid exercise" << std::endl;
        break;
    }
}

//Métodos utilitarios INICIO **************************************************************

void mostrarImagem(std::string janela, cv::Mat imageRGB, int seconds)
{
    cv::imshow(janela, imageRGB);
    cv::waitKey(seconds * 1000);
    cv::destroyWindow(janela);
}

std::string buscarImagem()
{
    std::string pathImage;
    std::string imagem = "imagem.jpg";
    pathImage = "../sourceAssets/" + imagem;
    return pathImage;
}

cv::Mat generateGrayscale()
{
    std::string pathImage = buscarImagem();
    cv::Mat imageGray = cv::imread(pathImage, cv::IMREAD_GRAYSCALE);
    cv::imwrite("../output/imagem_gray.jpg", imageGray);
    std::cout << "A imagem em tons de cinza foi gravada em disco" << std::endl;
    return imageGray;
}

//Exercicios INICIO **********************************************************************

void Exercise1()
{
    std::cout << "Rodando o Exercicio 1" << std::endl;

    std::string pathImage = buscarImagem();

    //Mostrar imagem
    cv::Mat image = cv::imread(pathImage, cv::IMREAD_COLOR);
    mostrarImagem("Mostrar imagem", image, 3);

    //Mostrar Largura, Altura e Numero de Canais
    std::cout << "A imagem tem resolucao de: " << image.rows << " x " << image.cols
        << " e " << image.channels() << " canais. " << std::endl;

    selectExerciceGUI();
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
    mostrarImagem("Colorida", imageRGB, 2);
    mostrarImagem("Tons de Cinza", imageGray, 2);

    selectExerciceGUI();
}


void Exercise3()
{
    std::cout << "Rodando o Exercicio 3" << std::endl;

    //ler e gravar a imagem em tons de cinza
    cv::Mat imageGray = generateGrayscale();

    // capturar coordenada
    int x, y;
    std::cout << "Digite a coordenada y: ";
    std::cin >> y;
    std::cout << "Digite a coordenada x: ";
    std::cin >> x;

    //capturar valor na coordenada e mostrar no console
    int intensidade = imageGray.at<uchar>(y, x);
    std::cout << "intensidade na coordenada (" << y << "," << x << ") = " << intensidade << std::endl;

    selectExerciceGUI();
}

void Exercise4()
{
    std::cout << "Rodando o Exercicio 4" << std::endl;

    //ler e gravar a imagem em tons de cinza
    cv::Mat imageGray = generateGrayscale();

    //inverter a intensidade do pixel
    for (int y = 0; y < 512; ++y)
    {
        for (int x = 0; x < 512; ++x)
        {
            uchar atual = imageGray.at<uchar>(y, x);
            uchar invertido = 255 - atual;
            imageGray.at<uchar>(y, x) = invertido;
        }
    }

    //mostrar imagem invertida
    mostrarImagem("Invertida", imageGray, 2);

    selectExerciceGUI();
}

void Exercise5()
{
    std::cout << "Rodando o Exercicio 5" << std::endl;

    //ler e gravar a imagem em tons de cinza
    cv::Mat imageGray = generateGrayscale();

    //inverter a intensidade do pixel
    for (int y = 0; y < 512; ++y)
    {
        for (int x = 0; x < 512; ++x)
        {
            uchar atual = imageGray.at<uchar>(y, x);

            if (atual > 128)
            {
                imageGray.at<uchar>(y, x) = 255;
            }
            else
            {
                imageGray.at<uchar>(y, x) = 0;
            }
        }
    }

    //mostrar imagem invertida
    mostrarImagem("Preto & Branco", imageGray, 2);

    selectExerciceGUI();
}

void Exercise6()
{
    std::cout << "Rodando o Exercicio 6" << std::endl;

    //Gerar imagem t.cinza e clonar
    cv::Mat imageGray = generateGrayscale();
    cv::Mat image128 = imageGray.clone();
    cv::Mat image64 = imageGray.clone();
    cv::Mat image16 = imageGray.clone();
    cv::Mat image4 = imageGray.clone();

    for (int y = 0; y < 512; ++y)
    {
        for (int x = 0; x < 512; ++x)
        {
            uchar atual = imageGray.at<uchar>(y, x);
            int total = 256;
            int divisor;
            int levels;

            levels = 128;
            divisor = 256 / levels;
            uchar a128 = atual / 2;
            image128.at<uchar>(y, x) = (atual / divisor) * divisor;

            levels = 64;
            divisor = 256 / levels;
            uchar a64 = (atual / divisor) * divisor;
            image64.at<uchar>(y, x) = a64;

            levels = 16;
            divisor = 256 / levels;
            uchar a16 = (atual / divisor) * divisor;
            image16.at<uchar>(y, x) = a16;

            levels = 4;
            divisor = 256 / levels;
            uchar a4 = (atual / divisor) * divisor;
            image4.at<uchar>(y, x) = a4;
        }
    }

    //Mostrar breve comparação
    std::vector<cv::Mat> images = {image128, image64, image16, image4};
    cv::Mat comparcao;
    cv::hconcat(images, comparcao); // all side by side

    mostrarImagem("Preto & Branco", comparcao, 6);


    selectExerciceGUI();
}

void Exercise7()
{
    std::cout << "Rodando o Exercicio 7" << std::endl;

    //ler e gravar a imagem em tons de cinza
    cv::Mat imageGray = generateGrayscale();

    float pixelsAmostra;
    float percentualRed;

    std::cout << "percentual de reducao" << std::endl;
    std::cin >> percentualRed;
    pixelsAmostra = 100 / percentualRed;
    pixelsAmostra = std::round(pixelsAmostra);

    int rows = imageGray.rows / pixelsAmostra;
    int cols = imageGray.cols / pixelsAmostra;
    cv::Mat imagemReduzida = cv::Mat(rows, cols, CV_8UC1);

    std::cout << "amostra pixel: " << pixelsAmostra << ", row col  " << rows << " " << cols << std::endl;
    int vLength = pixelsAmostra * pixelsAmostra;
    std::vector<uchar> v(vLength);


    int index = 0;
    for (int y = 0; y < pixelsAmostra; ++y)
    {
        for (int x = 0; x < pixelsAmostra; ++x)
        {
            v[index++] = imageGray.at<uchar>(y, x);
        }
    }

    int total = 0;
    for (int i = 0; i < vLength; ++i)
    {
        total = total + v[i];
    }
    int average = total / vLength;

    std::cout << "total: " << total << ", average: " << average << std::endl;

    //mostrar imagem invertida
    // mostrarImagem("Preto & Branco", imageGray, 2);

    selectExerciceGUI();
}


void Exercise8()
{
    std::cout << "Rodando o Exercicio 8" << std::endl;
}
