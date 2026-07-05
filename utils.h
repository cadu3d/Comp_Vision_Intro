//
// Created by cadu3d on 3/29/2026.
//

#ifndef COMP_VISION_INTRO_UTILS_H
#define COMP_VISION_INTRO_UTILS_H

#include <filesystem>
#include <map>
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
std::filesystem::path projectRoot();
bool temImagem(const std::filesystem::path& folder);
std::filesystem::path buscarImagem();
std::vector<cv::Mat> buscarImagens();
std::vector<cv::Mat> carregarImagens(const std::filesystem::path& folder);
std::vector<ImagemCarregada> buscarImagensComNomes();
std::vector<ImagemCarregada> carregarImagensComNomes(const std::filesystem::path& folder);
std::string pegarNome();
cv::Mat gerarImagemCinza();
std::filesystem::path lab1OutputImagemCinza();
void gravaImagem(cv::Mat result);
void gravaImagem(cv::Mat result, int index);
void gravaImagem(cv::Mat result, int index, std::string folder);
void gravaImagem(cv::Mat result, const std::string& name, std::string folder);
std::filesystem::path verificarOutput();
std::string verificarOrigemOutput();
void limparOutput(int lab);
void limparOutput(std::string lab);
int limparArquivosComPrefixos(
    const std::filesystem::path& pasta,
    const std::vector<std::string>& prefixos
);
void salvarGraficoPercentuais(
    const std::map<std::string, std::vector<double>>& series,
    const std::filesystem::path& destino
);

#endif //COMP_VISION_INTRO_UTILS_H
