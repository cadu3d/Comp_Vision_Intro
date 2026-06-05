//
// Created by cadu3d on 3/29/2026.
//

#ifndef COMP_VISION_INTRO_UTILS_H
#define COMP_VISION_INTRO_UTILS_H

#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

struct ImagemCarregada
{
    std::string nome;
    cv::Mat imagem;
};

void mostrarImagem(std::string janela, cv::Mat imageRGB, int seconds);
std::filesystem::path buscarImagem();
std::vector<cv::Mat> buscarImagens();
std::vector<ImagemCarregada> buscarImagensComNomes();
std::string pegarNome();
cv::Mat gerarImagemCinza();
void gravaImagem(cv::Mat result);
void gravaImagem(cv::Mat result, int index);
void gravaImagem(cv::Mat result, int index, std::string folder);
void gravaImagem(cv::Mat result, const std::string& name, std::string folder);
std::filesystem::path verificarOutput();
void limparOutput();

inline cv::Mat preProcessarImagem(const cv::Mat& imagem, int filtro)
{
    cv::Mat result;

    switch (filtro)
    {
    case 1:
        cv::GaussianBlur(imagem, result, cv::Size(9, 9), 0);
        break;
    case 2:
        cv::medianBlur(imagem, result, 5);
        break;
    case 3:
        cv::blur(imagem, result, cv::Size(5, 5));
        break;
    case 4:
        cv::equalizeHist(imagem, result);
        break;
    case 5:
        cv::Canny(imagem, result, 80, 160);
        break;
    default:
        result = imagem.clone();
        break;
    }

    return result;
}

inline std::string nomePreProcessamento(int filtro)
{
    switch (filtro)
    {
    case 1: return "Filtro Gaussiano";
    case 2: return "Filtro Mediana";
    case 3: return "Suavizacao";
    case 4: return "Equalizacao";
    case 5: return "Bordas";
    default: return "Filtro invalido";
    }
}

#endif //COMP_VISION_INTRO_UTILS_H
