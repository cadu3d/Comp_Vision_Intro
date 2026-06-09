#include <algorithm>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "filters.h"
#include "utils.h"

namespace
{
    int normalizarTamanhoFiltro(int tamanhoFiltro)
    {
        if (tamanhoFiltro < 1)
        {
            tamanhoFiltro = 1;
        }

        if (tamanhoFiltro % 2 == 0)
        {
            tamanhoFiltro++;
        }

        return tamanhoFiltro;
    }

    void salvarResultado(const cv::Mat& imagem, const std::filesystem::path& destino, int index)
    {
        cv::imwrite((destino / ("resultado_" + std::to_string(index) + ".png")).string(), imagem);
    }

    cv::Mat dilatarContornoPreto(const cv::Mat& imagem, int tamanhoKernel = 3, int iteracoes = 1)
    {
        cv::Mat invertida;
        cv::Mat dilatada;
        cv::Mat result;
        cv::Mat kernel = cv::getStructuringElement(
            cv::MORPH_RECT,
            cv::Size(tamanhoKernel, tamanhoKernel)
        );

        cv::bitwise_not(imagem, invertida);
        cv::dilate(invertida, dilatada, kernel, cv::Point(-1, -1), iteracoes);
        cv::bitwise_not(dilatada, result);

        return result;
    }

    std::vector<cv::Point> encontrarPixelsBorda(const cv::Mat& mascaraForma)
    {
        std::vector<std::vector<cv::Point>> contornos;
        cv::findContours(mascaraForma, contornos, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        if (contornos.empty())
        {
            return {};
        }

        return *std::max_element(
            contornos.begin(),
            contornos.end(),
            [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b)
            {
                return a.size() < b.size();
            }
        );
    }

    void salvarGraficoDistancias(
        const std::vector<double>& distancias,
        const std::filesystem::path& destino
    )
    {
        if (distancias.empty())
        {
            return;
        }

        constexpr int largura = 800;
        constexpr int altura = 400;
        constexpr int margem = 40;

        cv::Mat grafico(altura, largura, CV_8UC3, cv::Scalar(255, 255, 255));
        cv::line(grafico, cv::Point(margem, altura - margem), cv::Point(largura - margem, altura - margem), cv::Scalar(0, 0, 0), 1);
        cv::line(grafico, cv::Point(margem, margem), cv::Point(margem, altura - margem), cv::Scalar(0, 0, 0), 1);

        double maiorDistancia = *std::max_element(distancias.begin(), distancias.end());
        if (maiorDistancia <= 0)
        {
            maiorDistancia = 1.0;
        }

        std::vector<cv::Point> pontos;
        pontos.reserve(distancias.size());

        for (int i = 0; i < distancias.size(); ++i)
        {
            double xRatio = distancias.size() == 1 ? 0.0 : static_cast<double>(i) / (distancias.size() - 1);
            double yRatio = distancias[i] / maiorDistancia;
            int x = margem + static_cast<int>(xRatio * (largura - 2 * margem));
            int y = altura - margem - static_cast<int>(yRatio * (altura - 2 * margem));
            pontos.emplace_back(x, y);
        }

        for (int i = 1; i < pontos.size(); ++i)
        {
            cv::line(grafico, pontos[i - 1], pontos[i], cv::Scalar(255, 0, 0), 1);
        }

        cv::imwrite(destino.string(), grafico);
    }

    cv::Point encontrarCruzamentoPixelsMedios(const cv::Mat& mascaraForma)
    {
        cv::Mat pixelsMediosLinhas = cv::Mat::zeros(mascaraForma.size(), CV_8UC1);

        for (int y = 0; y < mascaraForma.rows; ++y)
        {
            int xMin = mascaraForma.cols;
            int xMax = -1;

            for (int x = 0; x < mascaraForma.cols; ++x)
            {
                if (mascaraForma.at<uchar>(y, x) > 0)
                {
                    xMin = std::min(xMin, x);
                    xMax = std::max(xMax, x);
                }
            }

            if (xMax >= 0)
            {
                int xMeio = (xMin + xMax) / 2;
                pixelsMediosLinhas.at<uchar>(y, xMeio) = 255;
            }
        }

        for (int x = 0; x < mascaraForma.cols; ++x)
        {
            int yMin = mascaraForma.rows;
            int yMax = -1;

            for (int y = 0; y < mascaraForma.rows; ++y)
            {
                if (mascaraForma.at<uchar>(y, x) > 0)
                {
                    yMin = std::min(yMin, y);
                    yMax = std::max(yMax, y);
                }
            }

            if (yMax >= 0)
            {
                int yMeio = (yMin + yMax) / 2;
                if (pixelsMediosLinhas.at<uchar>(yMeio, x) > 0)
                {
                    return cv::Point(x, yMeio);
                }
            }
        }

        return cv::Point(-1, -1);
    }

    cv::Mat criarMascaraPixelsPretos(const cv::Mat& imagem)
    {
        cv::Mat cinza;
        if (imagem.channels() == 1)
        {
            cinza = imagem;
        }
        else
        {
            cv::cvtColor(imagem, cinza, cv::COLOR_BGR2GRAY);
        }

        cv::Mat mascaraForma;
        cv::threshold(cinza, mascaraForma, 127, 255, cv::THRESH_BINARY_INV);
        return mascaraForma;
    }

    void salvarGraficoPixelsMedios(
        const cv::Mat& imagem,
        const std::string& nomeImagem,
        const std::filesystem::path& destinoGraficos
    )
    {
        cv::Mat mascaraForma = criarMascaraPixelsPretos(imagem);
        cv::Point cruzamento = encontrarCruzamentoPixelsMedios(mascaraForma);

        if (cruzamento.x < 0)
        {
            return;
        }

        std::vector<cv::Point> pixelsBorda = encontrarPixelsBorda(mascaraForma);
        std::vector<double> distancias;
        distancias.reserve(pixelsBorda.size());

        for (const cv::Point& pixelBorda : pixelsBorda)
        {
            distancias.push_back(cv::norm(pixelBorda - cruzamento));
        }

        std::filesystem::create_directories(destinoGraficos);
        salvarGraficoDistancias(distancias, destinoGraficos / (nomeImagem + "_GRAPH.png"));
    }
}

std::string Filters::menuPreProcImagem()
{
    std::string filtro;

    std::cout
        << "------------ FILTROS -----------\n"
        << "1 -> Gaussiano\n"
        << "2 -> Mediana\n"
        << "3 -> Suavizacao\n"
        << "4 -> Equalizacao\n"
        << "5 -> Borda\n"
        << "6 -> Normalizacao de Contraste\n"
        << "7 -> Limiar de Otsu\n"
        << "8 -> Gamma\n"
        << "9 -> Esqueleto\n"
        << "\n"
        << "----------- Processamentos ------\n"
        << "10 -> Limpar Midia\n"
        << "11 -> Mascara\n"
        << "12 -> Extrair Forma\n"
        << "13 -> Pixels Medios\n"
        << "\n"
        << "------------------------------------\n"
        << "0 -> Voltar\n"
        << "00 -> Reset\n";
    std::cin >> filtro;

    return filtro;
}

void Filters::preProcImagem(int filtro)
{
    preProcImagem(filtro, "Lab_2");
}

void Filters::preProcImagem(int filtro, std::string lab)
{
    if (filtro == 0)
    {
        return;
    }

    if (filtro < 1 || filtro > 13)
    {
        std::cout << "Filtro invalido" << std::endl;
        return;
    }

    std::filesystem::path destino = projectRoot() / "output" / lab / "preProcessadas";
    std::filesystem::path origem = projectRoot() / "input" / lab;
    std::string origemMensagem = "imagens originais";

    if (temImagem(destino))
    {
        origem = destino;
        origemMensagem = "imagens pre-processadas";
    }

    std::vector<cv::Mat> imagens = carregarImagens(origem);
    std::filesystem::create_directories(destino);

    switch (filtro)
    {
    case 1:
        filtroGaussiano(imagens, destino);
        break;
    case 2:
        filtroMediana(imagens, destino);
        break;
    case 3:
        filtroSuavizacao(imagens, destino);
        break;
    case 4:
        filtroEqualizacao(imagens, destino);
        break;
    case 5:
        filtroBorda(imagens, destino);
        break;
    case 6:
        filtroNormalizacaoContraste(imagens, destino);
        break;
    case 7:
        filtroLimiarOtsu(imagens, destino);
        break;
    case 8:
        filtroGamma(imagens, destino);
        break;
    case 9:
        filtroEsqueleto(imagens, destino);
        break;
    case 10:
        filtroLimparMidia(imagens, destino);
        break;
    case 11:
        filtroMascara(imagens, destino);
        break;
    case 12:
        filtroExtrairForma(imagens, destino);
        break;
    case 13:
    {
        std::vector<ImagemCarregada> imagensComNomes = carregarImagensComNomes(origem);

        std::cout << "[Pixels Medios] Marcando pixels medios por linha e coluna..." << std::endl;
        for (int i = 0; i < imagensComNomes.size(); ++i)
        {
            salvarResultado(filtroPixelsMedios(imagensComNomes[i].imagem), destino, i);
            salvarGraficoPixelsMedios(imagensComNomes[i].imagem, imagensComNomes[i].nome, destino.parent_path());
        }
        break;
    }
    default:
        break;
    }

    std::cout << "\n" << nomePreProcessamento(filtro) << " aplicado em " << imagens.size()
        << " " << origemMensagem << ".\n" << std::endl;
}

cv::Mat Filters::filtroGaussiano(const cv::Mat& imagem, int tamanhoFiltro)
{
    cv::Mat result;
    tamanhoFiltro = normalizarTamanhoFiltro(tamanhoFiltro);
    cv::GaussianBlur(imagem, result, cv::Size(tamanhoFiltro, tamanhoFiltro), 0);
    return result;
}

void Filters::filtroGaussiano(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    int tamanhoFiltro;

    std::cout << "Tamanho do filtro Gaussiano: ";
    std::cin >> tamanhoFiltro;
    tamanhoFiltro = normalizarTamanhoFiltro(tamanhoFiltro);

    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroGaussiano(imagens[i], tamanhoFiltro), destino, i);
    }
}

cv::Mat Filters::filtroMediana(const cv::Mat& imagem, int tamanhoFiltro)
{
    cv::Mat result;
    tamanhoFiltro = normalizarTamanhoFiltro(tamanhoFiltro);
    cv::medianBlur(imagem, result, tamanhoFiltro);
    return result;
}

void Filters::filtroMediana(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    int tamanhoFiltro;

    std::cout << "Tamanho do filtro Mediana: ";
    std::cin >> tamanhoFiltro;
    tamanhoFiltro = normalizarTamanhoFiltro(tamanhoFiltro);

    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroMediana(imagens[i], tamanhoFiltro), destino, i);
    }
}

cv::Mat Filters::filtroSuavizacao(const cv::Mat& imagem, int tamanhoFiltro)
{
    cv::Mat result;
    tamanhoFiltro = normalizarTamanhoFiltro(tamanhoFiltro);
    cv::blur(imagem, result, cv::Size(tamanhoFiltro, tamanhoFiltro));
    return result;
}

void Filters::filtroSuavizacao(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    int tamanhoFiltro;

    std::cout << "Tamanho do filtro Suavizacao: ";
    std::cin >> tamanhoFiltro;
    tamanhoFiltro = normalizarTamanhoFiltro(tamanhoFiltro);

    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroSuavizacao(imagens[i], tamanhoFiltro), destino, i);
    }
}

cv::Mat Filters::filtroEqualizacao(const cv::Mat& imagem)
{
    cv::Mat result;
    cv::equalizeHist(imagem, result);
    return result;
}

void Filters::filtroEqualizacao(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroEqualizacao(imagens[i]), destino, i);
    }
}

cv::Mat Filters::filtroBorda(const cv::Mat& imagem, double limiarBaixo, double limiarAlto)
{
    cv::Mat result;
    cv::Canny(imagem, result, limiarBaixo, limiarAlto);
    return result;
}

void Filters::filtroBorda(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroBorda(imagens[i], 50, 150), destino, i);
    }
}

cv::Mat Filters::filtroNormalizacaoContraste(const cv::Mat& imagem)
{
    cv::Mat result;
    cv::normalize(imagem, result, 0, 255, cv::NORM_MINMAX);
    return result;
}

void Filters::filtroNormalizacaoContraste(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroNormalizacaoContraste(imagens[i]), destino, i);
    }
}

cv::Mat Filters::filtroLimiarOtsu(const cv::Mat& imagem)
{
    cv::Mat result;
    cv::threshold(imagem, result, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    return result;
}

void Filters::filtroLimiarOtsu(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroLimiarOtsu(imagens[i]), destino, i);
    }
}

cv::Mat Filters::filtroGamma(const cv::Mat& imagem, double gamma)
{
    if (gamma <= 0)
    {
        gamma = 1.0;
    }

    cv::Mat imagemFloat;
    cv::Mat resultFloat;
    cv::Mat result;

    imagem.convertTo(imagemFloat, CV_32F, 1.0 / 255.0);
    cv::pow(imagemFloat, gamma, resultFloat);
    resultFloat.convertTo(result, imagem.type(), 255.0);

    return result;
}

void Filters::filtroGamma(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    double gamma;

    std::cout << "Valor de gamma: ";
    std::cin >> gamma;

    if (gamma <= 0)
    {
        gamma = 1.0;
    }

    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroGamma(imagens[i], gamma), destino, i);
    }
}

cv::Mat Filters::filtroEsqueleto(const cv::Mat& imagem)
{
    cv::Mat binaria;
    cv::threshold(imagem, binaria, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    if (cv::countNonZero(binaria) > (binaria.rows * binaria.cols) / 2)
    {
        cv::bitwise_not(binaria, binaria);
    }

    cv::Mat skeleton = cv::Mat::zeros(binaria.size(), CV_8UC1);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::Mat temp;
    cv::Mat eroded;
    cv::Mat img = binaria.clone();

    do
    {
        cv::erode(img, eroded, element);
        cv::dilate(eroded, temp, element);
        cv::subtract(img, temp, temp);
        cv::bitwise_or(skeleton, temp, skeleton);
        img = eroded.clone();
    }
    while (cv::countNonZero(img) > 0);

    cv::bitwise_not(skeleton, skeleton);
    return skeleton;
}

void Filters::filtroEsqueleto(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroEsqueleto(imagens[i]), destino, i);
    }
}

cv::Mat Filters::filtroLimparMidia(const cv::Mat& imagem)
{
    cv::Mat result = filtroGaussiano(imagem, 40);
    result = filtroGamma(result, 26);
    result = filtroLimiarOtsu(result);
    result = centralizarForma(result, true);

    return result;
}

void Filters::filtroLimparMidia(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    std::vector<cv::Mat> resultados = imagens;

    std::cout << "[Limpar Midia] Iniciando filtro Gaussiano (40)..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = filtroGaussiano(imagem, 40);
    }

    std::cout << "[Limpar Midia] Iniciando correcao gamma (26)..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = filtroGamma(imagem, 26);
    }

    std::cout << "[Limpar Midia] Iniciando limiar de Otsu..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = filtroLimiarOtsu(imagem);
    }

    std::cout << "[Limpar Midia] Centralizando forma (crop + resize + padding 256x256)..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = centralizarForma(imagem, true);
    }

    std::cout << "[Limpar Midia] Salvando resultados..." << std::endl;
    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(resultados[i], destino, i);
    }
}

cv::Mat Filters::centralizarForma(const cv::Mat& imagem, bool preencherCanvas)
{
    constexpr int larguraCanvas = 256;
    constexpr int alturaCanvas = 256;
    constexpr int tamanhoMaximoForma = 240;

    if (imagem.empty())
    {
        return imagem.clone();
    }

    cv::Mat mascaraPretos;
    std::vector<cv::Point> pixelsPretos;

    cv::inRange(imagem, cv::Scalar(0), cv::Scalar(0), mascaraPretos);
    cv::findNonZero(mascaraPretos, pixelsPretos);

    cv::Mat forma = imagem;

    if (pixelsPretos.empty())
    {
        forma = imagem.clone();
    }
    else
    {
        cv::Rect bordaForma = cv::boundingRect(pixelsPretos);
        forma = imagem(bordaForma).clone();
    }

    if (preencherCanvas || forma.cols > larguraCanvas || forma.rows > alturaCanvas)
    {
        int larguraMaxima = preencherCanvas ? tamanhoMaximoForma : larguraCanvas;
        int alturaMaxima = preencherCanvas ? tamanhoMaximoForma : alturaCanvas;
        double escala = std::min(
            static_cast<double>(larguraMaxima) / forma.cols,
            static_cast<double>(alturaMaxima) / forma.rows
        );

        cv::resize(forma, forma, cv::Size(), escala, escala, cv::INTER_NEAREST);
    }

    cv::Mat canvas(alturaCanvas, larguraCanvas, imagem.type(), cv::Scalar(255));
    int x = (larguraCanvas - forma.cols) / 2;
    int y = (alturaCanvas - forma.rows) / 2;

    forma.copyTo(canvas(cv::Rect(x, y, forma.cols, forma.rows)));

    return canvas;
}

cv::Mat Filters::preencherBordasComPreto(const cv::Mat& imagem)
{
    if (imagem.empty())
    {
        return imagem.clone();
    }

    cv::Mat result = imagem.clone();
    const cv::Scalar preto(0);

    for (int x = 0; x < result.cols; ++x)
    {
        if (result.at<uchar>(0, x) == 255)
        {
            cv::floodFill(result, cv::Point(x, 0), preto);
        }

        if (result.at<uchar>(result.rows - 1, x) == 255)
        {
            cv::floodFill(result, cv::Point(x, result.rows - 1), preto);
        }
    }

    for (int y = 0; y < result.rows; ++y)
    {
        if (result.at<uchar>(y, 0) == 255)
        {
            cv::floodFill(result, cv::Point(0, y), preto);
        }

        if (result.at<uchar>(y, result.cols - 1) == 255)
        {
            cv::floodFill(result, cv::Point(result.cols - 1, y), preto);
        }
    }

    return result;
}

cv::Mat Filters::filtroMascara(const cv::Mat& imagem)
{
    cv::Mat result = imagem.clone();
    result = preencherBordasComPreto(result);
    cv::bitwise_not(result, result);
    result = centralizarForma(result, true);

    return result;
}

void Filters::filtroMascara(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    std::vector<cv::Mat> resultados = imagens;

    std::cout << "[Mascara] Preenchendo bordas com preto..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = preencherBordasComPreto(imagem);
    }

    std::cout << "[Mascara] Invertendo imagem..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        cv::bitwise_not(imagem, imagem);
    }

    std::cout << "[Mascara] Centralizando forma (crop + resize + padding 256x256)..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = centralizarForma(imagem, true);
    }

    std::cout << "[Mascara] Salvando resultados..." << std::endl;
    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(resultados[i], destino, i);
    }
}

cv::Mat Filters::filtroPixelsMedios(const cv::Mat& imagem)
{
    if (imagem.empty())
    {
        return imagem.clone();
    }

    cv::Mat mascaraForma = criarMascaraPixelsPretos(imagem);

    cv::Mat result;
    if (imagem.channels() == 1)
    {
        cv::cvtColor(imagem, result, cv::COLOR_GRAY2BGR);
    }
    else
    {
        result = imagem.clone();
    }

    const cv::Vec3b verde(0, 255, 0);

    for (int y = 0; y < mascaraForma.rows; ++y)
    {
        int xMin = mascaraForma.cols;
        int xMax = -1;

        for (int x = 0; x < mascaraForma.cols; ++x)
        {
            if (mascaraForma.at<uchar>(y, x) > 0)
            {
                xMin = std::min(xMin, x);
                xMax = std::max(xMax, x);
            }
        }

        if (xMax >= 0)
        {
            int xMeio = (xMin + xMax) / 2;
            result.at<cv::Vec3b>(y, xMeio) = verde;
        }
    }

    for (int x = 0; x < mascaraForma.cols; ++x)
    {
        int yMin = mascaraForma.rows;
        int yMax = -1;

        for (int y = 0; y < mascaraForma.rows; ++y)
        {
            if (mascaraForma.at<uchar>(y, x) > 0)
            {
                yMin = std::min(yMin, y);
                yMax = std::max(yMax, y);
            }
        }

        if (yMax >= 0)
        {
            int yMeio = (yMin + yMax) / 2;
            result.at<cv::Vec3b>(yMeio, x) = verde;
        }
    }

    cv::Point cruzamento = encontrarCruzamentoPixelsMedios(mascaraForma);
    if (cruzamento.x >= 0)
    {
        cv::circle(result, cruzamento, 3, cv::Scalar(0, 0, 255), cv::FILLED);
    }

    return result;
}

void Filters::filtroPixelsMedios(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    std::cout << "[Pixels Medios] Marcando pixels medios por linha e coluna..." << std::endl;

    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(filtroPixelsMedios(imagens[i]), destino, i);
        salvarGraficoPixelsMedios(imagens[i], "resultado_" + std::to_string(i), destino.parent_path());
    }
}

cv::Mat Filters::gerarBordaInterna(const cv::Mat& imagem)
{
    if (imagem.empty())
    {
        return imagem.clone();
    }

    cv::Mat preenchida = imagem.clone();
    cv::floodFill(preenchida, cv::Point(0, 0), cv::Scalar(0));

    cv::Mat borda = filtroBorda(preenchida, 50, 150);
    cv::bitwise_not(borda, borda);

    return borda;
}

cv::Mat Filters::filtroExtrairForma(const cv::Mat& imagem)
{
    cv::Mat result = filtroGaussiano(imagem, 40);
    result = filtroGamma(result, 16);
    result = filtroLimiarOtsu(result);
    result = centralizarForma(result, true);
    result = gerarBordaInterna(result);
    result = dilatarContornoPreto(result);
    result = centralizarForma(result, true);

    return result;
}

void Filters::filtroExtrairForma(const std::vector<cv::Mat>& imagens, std::filesystem::path destino)
{
    std::vector<cv::Mat> resultados = imagens;

    std::cout << "[Extrair Forma] Iniciando filtro Gaussiano (40)..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = filtroGaussiano(imagem, 40);
    }

    std::cout << "[Extrair Forma] Iniciando correcao gamma (16)..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = filtroGamma(imagem, 16);
    }

    std::cout << "[Extrair Forma] Iniciando limiar de Otsu..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = filtroLimiarOtsu(imagem);
    }

    std::cout << "[Extrair Forma] Centralizando forma (crop + resize + padding 256x256)..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = centralizarForma(imagem, true);
    }

    std::cout << "[Extrair Forma] Iniciando geracao de borda interna..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = gerarBordaInterna(imagem);
    }

    std::cout << "[Extrair Forma] Iniciando dilatacao da borda interna..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = dilatarContornoPreto(imagem);
    }

    std::cout << "[Extrair Forma] Centralizando forma pos-dilatacao (crop + resize + padding 256x256)..." << std::endl;
    for (cv::Mat& imagem : resultados)
    {
        imagem = centralizarForma(imagem, true);
    }

    std::cout << "[Extrair Forma] Salvando resultados..." << std::endl;
    for (int i = 0; i < imagens.size(); ++i)
    {
        salvarResultado(resultados[i], destino, i);
    }
}

std::string Filters::nomePreProcessamento(int filtro)
{
    switch (filtro)
    {
    case 1: return "Filtro Gaussiano";
    case 2: return "Filtro Mediana";
    case 3: return "Suavizacao";
    case 4: return "Equalizacao";
    case 5: return "Bordas";
    case 6: return "Normalizacao de Contraste";
    case 7: return "Limiar de Otsu";
    case 8: return "Gamma";
    case 9: return "Esqueleto";
    case 10: return "Limpar Midia";
    case 11: return "Mascara";
    case 12: return "Extrair Forma";
    case 13: return "Pixels Medios";
    default: return "Filtro invalido";
    }
}
