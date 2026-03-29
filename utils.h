//
// Created by cadu3d on 3/29/2026.
//

#ifndef COMP_VISION_INTRO_UTILS_H
#define COMP_VISION_INTRO_UTILS_H

#include <opencv2/core.hpp>
#include <string>

void mostrarImagem(std::string janela, cv::Mat imageRGB, int seconds);
std::string buscarImagem();
cv::Mat gerarImagemCinza();

#endif //COMP_VISION_INTRO_UTILS_H