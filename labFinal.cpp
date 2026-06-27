#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>

#include "filters.h"
#include "utils.h"

std::filesystem::path origemLabFinal(std::string& origemMensagem);
void menuPreProcessamentoFinal();
void detectarCirculos();
void detectarRetas();
std::vector<std::string> carregarClasses();
cv::Mat preProcessarImagemOnnx(const cv::Mat& imagem);
std::pair<int, float> predizerClasse(cv::dnn::Net& net, const cv::Mat& blob);
void analisarImagemComIA();

// Parametros usados na deteccao de circulos.
double houghDpFinal = 1.2;
double houghMinDistFinal = 100;
double houghParam1Final = 120;
double houghParam2Final = 35;
int houghMinRadiusFinal = 80;
int houghMaxRadiusFinal = 256;

// Parametros usados na deteccao de retas.
double houghRhoFinal = 1;
double houghThetaFinal = CV_PI / 180;
int houghThresholdFinal = 40;
double houghMinLineLengthFinal = 40;
double houghMaxLineGapFinal = 60;

std::filesystem::path origemLabFinal(std::string& origemMensagem)
{
    const std::filesystem::path preProcessadas = projectRoot() / "output" / "Lab_FINAL" / "preProcessadas";
    origemMensagem = "imagens originais";

    // Usar imagens pre-processadas quando elas existirem.
    if (temImagem(preProcessadas))
    {
        origemMensagem = "imagens pre-processadas";
        return preProcessadas;
    }

    return projectRoot() / "input" / "Lab_FINAL";
}

void menuPreProcessamentoFinal()
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
            limparOutput("Lab_FINAL");
            continue;
        }

        // Aplicar o filtro escolhido pelo usuario.
        try
        {
            Filters::preProcImagem(std::stoi(filtro), "Lab_FINAL");
        }
        catch (const std::exception&)
        {
            std::cout << "Filtro invalido" << std::endl;
        }
    }
}

void detectarCirculos()
{
    std::string origemMensagem;
    std::filesystem::path origem = origemLabFinal(origemMensagem);
    std::vector<cv::Mat> imagens = carregarImagens(origem);

    for (int i = 0; i < imagens.size(); ++i)
    {
        cv::Mat imagemCinza = imagens[i].clone();

        // Encontrar os circulos usando a Transformada de Hough.
        std::vector<cv::Vec3f> coordCirculos;
        cv::HoughCircles(
            imagemCinza,
            coordCirculos,
            cv::HOUGH_GRADIENT,
            houghDpFinal,
            houghMinDistFinal,
            houghParam1Final,
            houghParam2Final,
            houghMinRadiusFinal,
            houghMaxRadiusFinal
        );

        // Converter para BGR para desenhar os resultados coloridos.
        cv::Mat result;
        cv::cvtColor(imagemCinza, result, cv::COLOR_GRAY2BGR);

        for (const cv::Vec3f& circulo : coordCirculos)
        {
            cv::Point centro(cvRound(circulo[0]), cvRound(circulo[1]));
            int raio = cvRound(circulo[2]);

            cv::circle(result, centro, raio, cv::Scalar(0, 255, 0), 3);
            cv::circle(result, centro, 3, cv::Scalar(0, 0, 255), -1);
        }

        gravaImagem(result, i, "Lab_FINAL");
    }

    std::cout << "Circulos detectados em " << imagens.size()
        << " " << origemMensagem << "." << std::endl;
}

void detectarRetas()
{
    std::string origemMensagem;
    std::filesystem::path origem = origemLabFinal(origemMensagem);
    std::vector<cv::Mat> imagens = carregarImagens(origem);

    for (int i = 0; i < imagens.size(); ++i)
    {
        cv::Mat imagemCinza = imagens[i].clone();
        cv::Mat bordas;

        // Gerar as bordas antes de procurar retas.
        cv::Canny(imagemCinza, bordas, 50, 150);

        std::vector<cv::Vec4i> retas;
        cv::HoughLinesP(
            bordas,
            retas,
            houghRhoFinal,
            houghThetaFinal,
            houghThresholdFinal,
            houghMinLineLengthFinal,
            houghMaxLineGapFinal
        );

        // Converter para BGR para desenhar as retas coloridas.
        cv::Mat result;
        cv::cvtColor(imagemCinza, result, cv::COLOR_GRAY2BGR);

        for (const cv::Vec4i& reta : retas)
        {
            cv::Point inicio(reta[0], reta[1]);
            cv::Point fim(reta[2], reta[3]);
            cv::line(result, inicio, fim, cv::Scalar(0, 0, 255), 2);
        }

        gravaImagem(result, i, "Lab_FINAL");
    }

    std::cout << "Retas detectadas em " << imagens.size()
        << " " << origemMensagem << "." << std::endl;
}

std::vector<std::string> carregarClasses()
{
    const std::filesystem::path classesPath = projectRoot() / "models" / "classes.txt";
    std::ifstream classesFile(classesPath);

    if (!classesFile)
    {
        throw std::runtime_error("Nao foi possivel abrir: " + classesPath.string());
    }

    // Ler uma classe por linha do arquivo.
    std::vector<std::string> classes;
    std::string classe;

    while (std::getline(classesFile, classe))
    {
        if (!classe.empty())
        {
            classes.push_back(classe);
        }
    }

    if (classes.empty())
    {
        throw std::runtime_error("Arquivo de classes vazio: " + classesPath.string());
    }

    return classes;
}

cv::Mat preProcessarImagemOnnx(const cv::Mat& imagem)
{
    // Converter a imagem para o formato esperado pelo modelo ONNX.
    return cv::dnn::blobFromImage(
        imagem,
        1.0 / 255.0,
        cv::Size(128, 128),
        cv::Scalar(),
        false,
        false,
        CV_32F
    );
}

std::pair<int, float> predizerClasse(cv::dnn::Net& net, const cv::Mat& blob)
{
    net.setInput(blob);

    // Rodar o modelo e transformar a saida em probabilidades.
    cv::Mat output = net.forward();
    cv::Mat scores = output.reshape(1, 1);
    cv::Mat probabilidades;
    cv::exp(scores, probabilidades);
    probabilidades /= cv::sum(probabilidades)[0];

    cv::Point classeEncontrada;
    double confianca = 0.0;
    cv::minMaxLoc(probabilidades, nullptr, &confianca, nullptr, &classeEncontrada);

    return std::pair<int, float>(classeEncontrada.x, static_cast<float>(confianca));
}

void analisarImagemComIA()
{
    std::string nomeImagem;

    std::cout << "Digite o nome da imagem em output/Lab_FINAL/preProcessadas: ";
    std::cin >> nomeImagem;

    const std::filesystem::path imagePath = projectRoot() / "output" / "Lab_FINAL" / "preProcessadas" / nomeImagem;
    const std::filesystem::path modelPath = projectRoot() / "models" / "handdraw_shapes.onnx";

    cv::Mat imagem = cv::imread(imagePath.string(), cv::IMREAD_GRAYSCALE);

    if (imagem.empty())
    {
        std::cout << "Nao foi possivel carregar a imagem: " << imagePath.string() << std::endl;
        return;
    }

    try
    {
        std::vector<std::string> classes = carregarClasses();
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath.string());
        cv::Mat imagemPreProcessada = preProcessarImagemOnnx(imagem);
        std::pair<int, float> resultado = predizerClasse(net, imagemPreProcessada);
        int classeIndex = resultado.first;
        float confianca = resultado.second;

        if (classeIndex < 0 || classeIndex >= static_cast<int>(classes.size()))
        {
            std::cout << "O modelo retornou uma classe invalida: " << classeIndex << std::endl;
            return;
        }

        if (confianca < 0.5F)
        {
            std::cout << "Tipo de forma: unknown"
                << " (confianca: " << confianca << ")" << std::endl;
            return;
        }

        std::cout << "Tipo de forma: " << classes[classeIndex]
            << " (confianca: " << confianca << ")" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Erro ao analisar imagem com IA: " << e.what() << std::endl;
    }
}

void runLabFinal()
{
    std::string processar;

    std::cout << "\n";
    std::cout << "LAB FINAL:" << std::endl;
    std::cout << "1 -> Pre-Processar" << std::endl;
    std::cout << "2 -> Detectar Circulos" << std::endl;
    std::cout << "3 -> Detectar Retas" << std::endl;
    std::cout << "4 -> Analisar imagem com IA" << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "0 -> Voltar" << std::endl;
    std::cout << "00 -> Reset (deleta imagens pre-processadas e outputs)" << std::endl;
    std::cin >> processar;

    if (processar == "0")
    {
        return;
    }

    if (processar == "00")
    {
        limparOutput("Lab_FINAL");
        runLabFinal();
        return;
    }

    if (processar == "1")
    {
        menuPreProcessamentoFinal();
        runLabFinal();
        return;
    }

    if (processar == "2")
    {
        detectarCirculos();
        runLabFinal();
        return;
    }

    if (processar == "3")
    {
        detectarRetas();
        runLabFinal();
        return;
    }

    if (processar == "4")
    {
        analisarImagemComIA();
        runLabFinal();
        return;
    }

    std::cout << "Escolha invalida" << std::endl;
    runLabFinal();
}
