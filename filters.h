#ifndef COMP_VISION_INTRO_FILTERS_H
#define COMP_VISION_INTRO_FILTERS_H

#include <filesystem>
#include <opencv2/core.hpp>
#include <string>
#include <vector>

class Filters
{
public:
    static std::string menuPreProcImagem();
    static void preProcImagem(int filtro);
    static void preProcImagem(int filtro, std::string lab);
    static void filtroGaussiano(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroMediana(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroSuavizacao(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroEqualizacao(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroBorda(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroNormalizacaoContraste(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroLimiarOtsu(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroGamma(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroEsqueleto(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroInverter(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroTonsCinza(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroLimparMidia(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroMascara(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroExtrairForma(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void filtroPixelsMedios(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void desenharGraficoDistancias(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void analisarPicosDistancias(const std::vector<cv::Mat>& imagens, std::filesystem::path destino);
    static void gerarCentroLabFinal();
    static void desenharGraficoLabFinal();
    static void calcularMaxMinLabFinal();
    static void calcularAmplitudeLabFinal();
    static void detectarFormasLabFinal();
    static cv::Mat filtroGaussiano(const cv::Mat& imagem, int tamanhoFiltro);
    static cv::Mat filtroMediana(const cv::Mat& imagem, int tamanhoFiltro);
    static cv::Mat filtroSuavizacao(const cv::Mat& imagem, int tamanhoFiltro);
    static cv::Mat filtroEqualizacao(const cv::Mat& imagem);
    static cv::Mat filtroBorda(const cv::Mat& imagem, double limiarBaixo, double limiarAlto);
    static cv::Mat filtroNormalizacaoContraste(const cv::Mat& imagem);
    static cv::Mat filtroLimiarOtsu(const cv::Mat& imagem);
    static cv::Mat filtroGamma(const cv::Mat& imagem, double gamma);
    static cv::Mat filtroEsqueleto(const cv::Mat& imagem);
    static cv::Mat filtroInverter(const cv::Mat& imagem);
    static cv::Mat filtroTonsCinza(const cv::Mat& imagem);
    static cv::Mat filtroLimparMidia(const cv::Mat& imagem);
    static cv::Mat centralizarForma(const cv::Mat& imagem, bool preencherCanvas = false);
    static cv::Mat preencherBordasComPreto(const cv::Mat& imagem);
    static cv::Mat filtroMascara(const cv::Mat& imagem);
    static cv::Mat gerarBordaInterna(const cv::Mat& imagem);
    static cv::Mat filtroExtrairForma(const cv::Mat& imagem);
    static cv::Mat filtroPixelsMedios(const cv::Mat& imagem);
    static std::string nomePreProcessamento(int filtro);
};

#endif //COMP_VISION_INTRO_FILTERS_H
