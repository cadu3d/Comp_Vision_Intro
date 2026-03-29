//
// Created by cadu3d on 3/29/2026.
//
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "utils.h"

namespace
{
    std::filesystem::path projectRoot()
    {
        return PROJECT_ROOT;
    }
}

void mostrarImagem(std::string janela, cv::Mat imageRGB, int seconds)
{
    cv::imshow(janela, imageRGB);
    cv::waitKey(seconds * 1000);
    cv::destroyWindow(janela);
}

std::string buscarImagem()
{
    const std::filesystem::path pathImage = projectRoot() / "sourceAssets" / "imagem.jpg";
    return pathImage.string();
}

cv::Mat gerarImagemCinza()
{
    const std::string pathImage = buscarImagem();
    cv::Mat imagemOriginal = cv::imread(pathImage, cv::IMREAD_GRAYSCALE);

    if (imagemOriginal.empty())
    {
        throw std::runtime_error("Nao foi possivel carregar a imagem em: " + pathImage);
    }

    const std::filesystem::path outputPath = projectRoot() / "output" / "imagem_gray.jpg";
    cv::imwrite(outputPath.string(), imagemOriginal);
    return imagemOriginal;
}
