#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

#include "filters.h"
#include "utils.h"

struct HoughParams
{
    double dp = 1.2;
    double minDist = 100;
    double param1 = 120;
    double param2 = 35;
    int minRadius = 80;
    int maxRadius = 256;
};

struct HoughLineParams
{
    double rho = 1;
    double theta = CV_PI / 180;
    int threshold = 40;
    double minLineLength = 40;
    double maxLineGap = 60;
};

std::filesystem::path origemLabFinal(std::string& origemMensagem)
{
    const std::filesystem::path preProcessadas = projectRoot() / "output" / "Lab_FINAL" / "preProcessadas";
    origemMensagem = "imagens originais";

    if (temImagem(preProcessadas))
    {
        origemMensagem = "imagens pre-processadas";
        return preProcessadas;
    }

    return projectRoot() / "input" / "Lab_FINAL";
}

void detectarCirculos()
{
    const HoughParams houghParams;
    std::string origemMensagem;
    std::filesystem::path origem = origemLabFinal(origemMensagem);
    std::vector<cv::Mat> imagens = carregarImagens(origem);

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

        gravaImagem(result, i, "Lab_FINAL");
    }

    std::cout << "Circulos detectados em " << imagens.size()
        << " " << origemMensagem << "." << std::endl;
}

void detectarRetas()
{
    const HoughLineParams houghParams;
    std::string origemMensagem;
    std::filesystem::path origem = origemLabFinal(origemMensagem);
    std::vector<cv::Mat> imagens = carregarImagens(origem);

    for (int i = 0; i < imagens.size(); ++i)
    {
        cv::Mat imagemCinza = imagens[i].clone();
        cv::Mat bordas;
        std::vector<cv::Vec4i> retas;

        cv::Canny(imagemCinza, bordas, 50, 150);
        cv::HoughLinesP(
            bordas,
            retas,
            houghParams.rho,
            houghParams.theta,
            houghParams.threshold,
            houghParams.minLineLength,
            houghParams.maxLineGap
        );

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

    cv::Mat output = net.forward();
    cv::Mat scores = output.reshape(1, 1);
    cv::Mat probabilities;
    cv::exp(scores, probabilities);
    probabilities /= cv::sum(probabilities)[0];

    cv::Point classIdPoint;
    double confidence = 0.0;
    cv::minMaxLoc(probabilities, nullptr, &confidence, nullptr, &classIdPoint);

    return {classIdPoint.x, static_cast<float>(confidence)};
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
        auto [classeIndex, confianca] = predizerClasse(net, imagemPreProcessada);

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

    while (true)
    {
        std::cout << "\n";
        std::cout << "LAB FINAL:" << std::endl;
        std::cout << "1 -> Pre-Processar" << std::endl;
        std::cout << "2 -> Detectar Circulos" << std::endl;
        std::cout << "3 -> Detectar Retas" << std::endl;
        std::cout << "4 -> Analisar imagem com IA" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "0 -> Voltar" << std::endl;
        std::cout << "00 -> Reset (deleta imagens pre-processadas e outputs)" << std::endl;
        std::cout << "> ";
        std::cin >> processar;

        if (processar == "0")
        {
            return;
        }

        if (processar == "00")
        {
            limparOutput("Lab_FINAL");
        }

        if (processar == "1")
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

        if (processar == "2")
        {
            detectarCirculos();
        }

        if (processar == "3")
        {
            detectarRetas();
        }

        if (processar == "4")
        {
            analisarImagemComIA();
        }
    }
}
