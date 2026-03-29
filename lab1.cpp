#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <clocale>

void selExercicioGUI();
void runExercicio(int Exercicio);
void Exercicio1();
void Exercicio2();
void Exercicio3();
void Exercicio4();
void Exercicio5();
void Exercicio6();
void Exercicio7();
void Exercicio8();

void runLab1()
{
    std::cout << "Introdução à Computação Gráfica - Lab 1!\n";
    selExercicioGUI();
}

void selExercicioGUI()
{
    int Exercicio;

    std::cout << "\n";
    std::cout << "Selecione um exercício de 1 a 8, (0 - VOLTAR): " << std::endl;;
    std::cin >> Exercicio;

    if (Exercicio == 0)
    {
        return;
    }

    runExercicio(Exercicio);
}

void runExercicio(int Exercicio)
{
    switch (Exercicio)
    {
    case 1:
        Exercicio1();
        break;
    case 2:
        Exercicio2();
        break;
    case 3:
        Exercicio3();
        break;
    case 4:
        Exercicio4();
        break;
    case 5:
        Exercicio5();
        break;
    case 6:
        Exercicio6();
        break;
    case 7:
        Exercicio7();
        break;
    case 8:
        Exercicio8();
        break;
    default:
        std::cout << "Exercício Inválido" << std::endl;
        selExercicioGUI();
        break;
    }
}

//Métodos utilitários INICIO **************************************************************

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

cv::Mat gerarImagemCinza()
{
    std::string pathImage = buscarImagem();
    cv::Mat imagemOriginal = cv::imread(pathImage, cv::IMREAD_GRAYSCALE);
    cv::imwrite("../output/imagem_gray.jpg", imagemOriginal);
    return imagemOriginal;
}

//Exercícios INICIO **********************************************************************

void Exercicio1()
{
    std::cout << "Rodando o Exercício 1" << std::endl;

    std::string pathImage = buscarImagem();

    //Mostrar imagem
    cv::Mat image = cv::imread(pathImage, cv::IMREAD_COLOR);
    mostrarImagem("Mostrar imagem", image, 3);

    //Mostrar Largura, Altura e Número de Canais
    std::cout << "A imagem tem resolução de: " << image.rows << " x " << image.cols
        << " e " << image.channels() << " canais. " << std::endl;

    selExercicioGUI();
}

void Exercicio2()
{
    std::cout << "Rodando o Exercício 2" << std::endl;

    std::string pathImage = buscarImagem();

    //Armazenar imagem na variável
    cv::Mat imageRGB = cv::imread(pathImage, cv::IMREAD_COLOR);

    //Converter para grayscale e gravar em disco
    //dúvida: usar IMREAD_GRAYSCALE OU RGB2GRAY?
    cv::Mat imagemOriginal;
    cv::cvtColor(imageRGB, imagemOriginal, cv::COLOR_RGB2GRAY);
    cv::imwrite("../output/imagem_gray.jpg", imagemOriginal);
    std::cout << "A imagem em tons de cinza foi gravada em disco" << std::endl;

    //Mostrar imagem colorida e convertida para tons de cinza
    mostrarImagem("Colorida", imageRGB, 2);
    mostrarImagem("Tons de Cinza", imagemOriginal, 2);

    selExercicioGUI();
}


void Exercicio3()
{
    std::cout << "Rodando o Exercício 3" << std::endl;

    //ler e gravar a imagem em tons de cinza
    cv::Mat imagemOriginal = gerarImagemCinza();

    // capturar coordenada
    int x, y;
    std::cout << "Digite a coordenada do pixel em y: ";
    std::cin >> y;
    std::cout << "Digite a coordenada do pixel em x: ";
    std::cin >> x;

    //capturar valor na coordenada e mostrar no console
    int intensidade = imagemOriginal.at<uchar>(y, x);
    std::cout << "intensidade na coordenada (" << y << "," << x << ") = " << intensidade << std::endl;

    selExercicioGUI();
}

void Exercicio4()
{
    std::cout << "Rodando o Exercício 4" << std::endl;

    //ler e gravar a imagem em tons de cinza
    cv::Mat imagemOriginal = gerarImagemCinza();

    //inverter a intensidade do pixel
    for (int y = 0; y < 512; ++y)
    {
        for (int x = 0; x < 512; ++x)
        {
            uchar atual = imagemOriginal.at<uchar>(y, x);
            uchar invertido = 255 - atual;
            imagemOriginal.at<uchar>(y, x) = invertido;
        }
    }

    //mostrar imagem invertida
    mostrarImagem("Invertida", imagemOriginal, 2);

    selExercicioGUI();
}

void Exercicio5()
{
    std::cout << "Rodando o Exercício 5" << std::endl;

    //ler e gravar a imagem em tons de cinza
    cv::Mat imagemOriginal = gerarImagemCinza();

    //inverter a intensidade do pixel
    for (int y = 0; y < 512; ++y)
    {
        for (int x = 0; x < 512; ++x)
        {
            uchar atual = imagemOriginal.at<uchar>(y, x);

            if (atual > 128)
            {
                imagemOriginal.at<uchar>(y, x) = 255;
            }
            else
            {
                imagemOriginal.at<uchar>(y, x) = 0;
            }
        }
    }

    //mostrar imagem invertida
    mostrarImagem("Preto & Branco", imagemOriginal, 2);

    selExercicioGUI();
}

void Exercicio6()
{
    std::cout << "Rodando o Exercício 6" << std::endl;

    //Gerar imagem t.cinza e clonar
    cv::Mat imagemOriginal = gerarImagemCinza();
    cv::Mat image128 = imagemOriginal.clone();
    cv::Mat image64 = imagemOriginal.clone();
    cv::Mat image16 = imagemOriginal.clone();
    cv::Mat image4 = imagemOriginal.clone();

    for (int y = 0; y < 512; ++y)
    {
        for (int x = 0; x < 512; ++x)
        {
            uchar atual = imagemOriginal.at<uchar>(y, x);
            int divisor;
            int levels;

            levels = 128;
            divisor = 256 / levels;
            image128.at<uchar>(y, x) = (atual / divisor) * divisor;

            levels = 64;
            divisor = 256 / levels;
            image64.at<uchar>(y, x) = (atual / divisor) * divisor;

            levels = 16;
            divisor = 256 / levels;
            image16.at<uchar>(y, x) = (atual / divisor) * divisor;

            levels = 4;
            divisor = 256 / levels;
            image4.at<uchar>(y, x) = (atual / divisor) * divisor;
        }
    }

    //Mostrar breve comparação
    std::vector<cv::Mat> images = {image128, image64, image16, image4};
    cv::Mat comparcao;
    cv::hconcat(images, comparcao); // all side by side

    mostrarImagem("Comparação: imagem com 128, 64, 16 e 4 níveis de intensidade", comparcao, 6);


    selExercicioGUI();
}

int calcularMedia(cv::Mat imageTarget, int inicioY, int inicioX, int amostra)
{
    int total = 0;

    for (int y = inicioY; y < inicioY + amostra; ++y)
    {
        for (int x = inicioX; x < inicioX + amostra; ++x)
        {
            total += imageTarget.at<uchar>(y, x);
        }
    }

    return total / (amostra * amostra);
}

int preencheMedia(cv::Mat imageTarget, int inicioY, int inicioX, int amostra, int valor)
{
    int total = 0;

    for (int y = inicioY; y < inicioY + amostra; ++y)
    {
        for (int x = inicioX; x < inicioX + amostra; ++x)
        {
            imageTarget.at<uchar>(y, x) = valor;
        }
    }

    return total / (amostra * amostra);
}

void Exercicio7()
{
    std::cout << "Rodando o Exercício 7" << std::endl;

    //ler e gravar a imagem em tons de cinza
    cv::Mat imageOrig = gerarImagemCinza();

    //Obter tamanho da amostra
    int amostra;
    float percentualRed;
    std::cout << "Digite o percentual de redução (%)" << std::endl;
    std::cin >> percentualRed;
    amostra = 100 / (100 - percentualRed);
    amostra = std::round(amostra);

    //criar imagem reduzida
    int imagemRedRows = imageOrig.rows / amostra;
    int imagemRedCols = imageOrig.cols / amostra;
    cv::Mat imagemRed = cv::Mat(imagemRedRows, imagemRedCols, CV_8UC1);

    //criar imagem ampliada
    int imagemAmpRows = imageOrig.rows;
    int imagemAmpCols = imageOrig.cols;
    cv::Mat imagemAmp = cv::Mat(imagemAmpRows, imagemAmpCols, CV_8UC1);


    //preencher a imagem reduzida com as médias de cada amostra
    for (int y = 0; y < imagemRedRows; ++y)
    {
        for (int x = 0; x < imagemRedCols; ++x)
        {
            int inicioY = y * amostra;
            int inicioX = x * amostra;

            //enviar amostra para calcular a média
            int media = calcularMedia(imageOrig, inicioY, inicioX, amostra);
            imagemRed.at<uchar>(y, x) = media;

            //preenche imagem ampliada
            preencheMedia(imagemAmp, inicioY, inicioX, amostra, media);
        }
    }

    /*Mostrar imagens
    mostrarImagem("Imagem Original", imageOrig, 2);
    mostrarImagem("Imagem Reduzida", imagemRed, 2);
    mostrarImagem("Imagem Reduzida", imagemAmp, 2);
    */

    //Mostrar breve comparação
    std::vector<cv::Mat> images = {imageOrig, imagemAmp};
    cv::Mat comparcao;
    cv::hconcat(images, comparcao); // all side by side
    mostrarImagem("Preto & Branco", comparcao, 6);

    selExercicioGUI();
}

void Exercicio8()
{
    std::cout << "Rodando o Exercício 8" << std::endl;

    //Gerar imagem t.cinza
    cv::Mat imagemOriginal = gerarImagemCinza();

    //Obter tamanho da amostra
    int amostra;
    float percentualRed = 30;
    amostra = 100 / percentualRed;
    amostra = std::round(amostra);

    //criar imagem reduzida
    int imagemRedRows = imagemOriginal.rows / amostra;
    int imagemRedCols = imagemOriginal.cols / amostra;
    cv::Mat imagemRed = cv::Mat(imagemRedRows, imagemRedCols, CV_8UC1);

    //criar imagem ampliada
    cv::Mat imagemAmp = cv::Mat(512, 512, CV_8UC1);


    //preencher a imagem reduzida com as médias de cada amostra
    for (int y = 0; y < imagemRedRows; ++y)
    {
        for (int x = 0; x < imagemRedCols; ++x)
        {
            int inicioY = y * amostra;
            int inicioX = x * amostra;

            //enviar amostra para calcular a média
            int media = calcularMedia(imagemOriginal, inicioY, inicioX, amostra);
            imagemRed.at<uchar>(y, x) = media;

            //preenche imagem ampliada
            preencheMedia(imagemAmp, inicioY, inicioX, amostra, media);
        }
    }

    //Criar imagens para reduzir tons de cinza
    cv::Mat image20 = imagemOriginal.clone();
    cv::Mat imagemFinal = imagemAmp.clone();

    //Reduzir tons de cinza nas imagens
    for (int y = 0; y < 512; ++y)
    {
        for (int x = 0; x < 512; ++x)
        {
            uchar atual = imagemOriginal.at<uchar>(y, x);
            int divisor;
            int levels;

            levels = 20;
            divisor = 256 / levels;
            image20.at<uchar>(y, x) = (atual / divisor) * divisor;

            uchar atual2 = imagemAmp.at<uchar>(y, x);
            levels = 20;
            divisor = 256 / levels;
            imagemFinal.at<uchar>(y, x) = (atual2 / divisor) * divisor;

        }
    }

    //Mostrar breve comparação
    std::vector<cv::Mat> images = {imagemAmp, image20, imagemFinal};
    cv::Mat comparcao;
    cv::hconcat(images, comparcao); // all side by side
    mostrarImagem("Comparação", comparcao, 6);

    //Comentário sobre as diferenças
    std::cout << " -> A imagem de baixa resolução apresenta os contornos prejudicados (serrilhados), porém a qualidade dos preenchimentos/superfícies permanece inalterada." << std::endl;
    std::cout << " -> A imagem com poucos tons de cinza apresenta os contornos inalterados, porém a qualidade dos preenchimentos/superfícies demonstra efeitos de camadas." << std::endl;
    std::cout << " -> A imagem de baixa resolução com poucos níveis de cinza apresenta os contornos prejudicados (serrilhados) e preenchimentos/superfícies com efeitos de camadas e serrilhamento entre as mesmas." << std::endl;

    selExercicioGUI();

}
