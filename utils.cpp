//
// Created by cadu3d on 3/29/2026.
//
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "utils.h"

namespace
{
    std::filesystem::path preProcessadasDir()
    {
        return projectRoot() / "output" / "Lab_2" / "preProcessadas";
    }

    std::filesystem::path lab2InputDir()
    {
        return projectRoot() / "input" / "Lab_2";
    }

    std::filesystem::path lab1OutputDir()
    {
        return projectRoot() / "output" / "Lab_1";
    }

    std::filesystem::path outputDir(const std::string& folder)
    {
        if (folder == "preProcessadas")
        {
            return preProcessadasDir();
        }

        return projectRoot() / "output" / folder;
    }

    bool ehExtensaoImagem(const std::filesystem::path& path)
    {
        std::string ext = path.extension().string();
        std::transform(
            ext.begin(),
            ext.end(),
            ext.begin(),
            [](unsigned char c)
            {
                return static_cast<char>(std::tolower(c));
            }
        );

        return ext == ".jpg" || ext == ".jpeg" || ext == ".png";
    }

    bool menorNatural(const std::string& a, const std::string& b)
    {
        size_t i = 0;
        size_t j = 0;

        while (i < a.size() && j < b.size())
        {
            unsigned char ca = static_cast<unsigned char>(a[i]);
            unsigned char cb = static_cast<unsigned char>(b[j]);

            if (std::isdigit(ca) && std::isdigit(cb))
            {
                size_t inicioNumeroA = i;
                size_t inicioNumeroB = j;

                while (inicioNumeroA < a.size() && a[inicioNumeroA] == '0')
                {
                    ++inicioNumeroA;
                }

                while (inicioNumeroB < b.size() && b[inicioNumeroB] == '0')
                {
                    ++inicioNumeroB;
                }

                size_t fimNumeroA = inicioNumeroA;
                size_t fimNumeroB = inicioNumeroB;

                while (fimNumeroA < a.size() && std::isdigit(static_cast<unsigned char>(a[fimNumeroA])))
                {
                    ++fimNumeroA;
                }

                while (fimNumeroB < b.size() && std::isdigit(static_cast<unsigned char>(b[fimNumeroB])))
                {
                    ++fimNumeroB;
                }

                size_t tamanhoNumeroA = fimNumeroA - inicioNumeroA;
                size_t tamanhoNumeroB = fimNumeroB - inicioNumeroB;

                if (tamanhoNumeroA != tamanhoNumeroB)
                {
                    return tamanhoNumeroA < tamanhoNumeroB;
                }

                for (size_t k = 0; k < tamanhoNumeroA; ++k)
                {
                    if (a[inicioNumeroA + k] != b[inicioNumeroB + k])
                    {
                        return a[inicioNumeroA + k] < b[inicioNumeroB + k];
                    }
                }

                i = fimNumeroA;
                j = fimNumeroB;
                continue;
            }

            ca = static_cast<unsigned char>(std::tolower(ca));
            cb = static_cast<unsigned char>(std::tolower(cb));

            if (ca != cb)
            {
                return ca < cb;
            }

            ++i;
            ++j;
        }

        return a.size() < b.size();
    }

    bool menorPathNatural(const std::filesystem::path& a, const std::filesystem::path& b)
    {
        return menorNatural(a.filename().string(), b.filename().string());
    }

}

std::filesystem::path projectRoot()
{
    return PROJECT_ROOT;
}

void mostrarImagem(std::string janela, cv::Mat imageRGB, int seconds)
{
    cv::imshow(janela, imageRGB);
    cv::waitKey(seconds * 1000);
    cv::destroyWindow(janela);
}

std::vector<cv::Mat> buscarImagens()
{
    std::filesystem::path sourceDir = verificarOutput();
    return carregarImagens(sourceDir);
}

std::vector<ImagemCarregada> buscarImagensComNomes()
{
    std::filesystem::path sourceDir = verificarOutput();
    return carregarImagensComNomes(sourceDir);
}

bool temImagem(const std::filesystem::path& folder)
{
    if (!std::filesystem::exists(folder))
    {
        return false;
    }

    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (ehExtensaoImagem(entry.path()))
        {
            return true;
        }
    }

    return false;
}

std::vector<cv::Mat> carregarImagens(const std::filesystem::path& folder)
{
    std::vector<std::filesystem::path> paths;
    std::vector<cv::Mat> imagens;

    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::filesystem::path path = entry.path();

        if (ehExtensaoImagem(path))
        {
            paths.push_back(path);
        }
    }

    std::sort(paths.begin(), paths.end(), menorPathNatural);

    for (const std::filesystem::path& path : paths)
    {
        cv::Mat imagem = cv::imread(path.string(), cv::IMREAD_GRAYSCALE);

        if (!imagem.empty())
        {
            imagens.push_back(imagem);
        }
    }

    return imagens;
}

std::vector<ImagemCarregada> carregarImagensComNomes(const std::filesystem::path& folder)
{
    std::vector<std::filesystem::path> paths;

    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::filesystem::path path = entry.path();

        if (ehExtensaoImagem(path))
        {
            paths.push_back(path);
        }
    }

    std::sort(paths.begin(), paths.end(), menorPathNatural);

    std::vector<ImagemCarregada> imagens;

    for (const std::filesystem::path& path : paths)
    {
        cv::Mat imagem = cv::imread(path.string(), cv::IMREAD_GRAYSCALE);

        if (!imagem.empty())
        {
            imagens.push_back({path.stem().string(), imagem});
        }
    }

    return imagens;
}


std::filesystem::path verificarOutput()
{
    std::filesystem::path outputDir = preProcessadasDir();
    std::filesystem::path sourceDir = lab2InputDir();

    if (!temImagem(outputDir))
    {
        return sourceDir;
    }

    return outputDir;
}

std::string verificarOrigemOutput()
{
    if (!temImagem(preProcessadasDir()))
    {
        return "imagens originais";
    }

    return "imagens pre-processadas";
}

std::filesystem::path buscarImagem()
{

    const std::filesystem::path pathImage = projectRoot() / "input" / "lab_1" / "imagem.jpg";
    return pathImage;

}


std::filesystem::path lab1OutputImagemCinza()
{
    return lab1OutputDir() / "imagem_gray.jpg";
}


cv::Mat gerarImagemCinza()
{
    const std::filesystem::path pathImage = buscarImagem();
    cv::Mat imagemOriginal = cv::imread(pathImage.string(), cv::IMREAD_GRAYSCALE);

    if (imagemOriginal.empty())
    {
        throw std::runtime_error("Nao foi possivel carregar a imagem em: " + pathImage.string());
    }

    const std::filesystem::path outputPath = lab1OutputImagemCinza();
    std::filesystem::create_directories(outputPath.parent_path());
    cv::imwrite(outputPath.string(), imagemOriginal);
    return imagemOriginal;
}

void gravaImagem(cv::Mat result)
{
    std::filesystem::create_directories(preProcessadasDir());
    cv::imwrite((preProcessadasDir() / "result.png").string() , result);
}

void gravaImagem(cv::Mat result, int index)
{
    gravaImagem(result, index, "preProcessadas");
}

void gravaImagem(cv::Mat result, int index, std::string folder)
{
    std::filesystem::path destino = outputDir(folder);
    std::filesystem::create_directories(destino);

    std::filesystem::path outputPath = destino / ("resultado_" + std::to_string(index) + ".png");
    cv::imwrite(outputPath.string(), result);
}

void gravaImagem(cv::Mat result, const std::string& name, std::string folder)
{
    std::filesystem::path destino = outputDir(folder);
    std::filesystem::create_directories(destino);

    std::string outputName = name;

    if (outputName.rfind("resultado_", 0) == 0)
    {
        outputName = outputName.substr(10);
    }

    std::filesystem::path outputPath = destino / ("resultado_" + outputName + ".png");
    cv::imwrite(outputPath.string(), result);
}


void limparOutput(int lab)
{
    limparOutput("Lab_" + std::to_string(lab));
}

void limparOutput(std::string lab)
{
    std::filesystem::path folder = projectRoot() / "output" / lab;
    int removidas = 0;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(folder))
    {
        const std::filesystem::path path = entry.path();
        const std::string ext = path.extension().string();

        if (
            ext == ".jpg" ||
            ext == ".jpeg" ||
            ext == ".png" ||
            ext == ".bmp" ||
            ext == ".tif" ||
            ext == ".tiff"
        )
        {
            std::filesystem::remove(path);
            ++removidas;
        }
    }

    std::cout << "Imagens removidas: " << removidas << std::endl;
}

int limparArquivosComPrefixos(
    const std::filesystem::path& pasta,
    const std::vector<std::string>& prefixos
)
{
    int removidos = 0;

    if (!std::filesystem::exists(pasta))
    {
        return removidos;
    }

    for (const auto& entry : std::filesystem::directory_iterator(pasta))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::string nomeArquivo = entry.path().filename().string();

        for (const std::string& prefixo : prefixos)
        {
            if (nomeArquivo.rfind(prefixo, 0) == 0)
            {
                std::filesystem::remove(entry.path());
                ++removidos;
                break;
            }
        }
    }

    return removidos;
}

void salvarGraficoPercentuais(
    const std::map<std::string, std::vector<double>>& series,
    const std::filesystem::path& destino
)
{
    const int largura = 900;
    const int altura = 500;
    const int margem = 60;
    cv::Mat grafico(altura, largura, CV_8UC3, cv::Scalar(255, 255, 255));
    std::vector<cv::Scalar> cores = {
        cv::Scalar(0, 0, 220),
        cv::Scalar(0, 150, 0),
        cv::Scalar(220, 80, 0),
        cv::Scalar(160, 0, 160),
        cv::Scalar(0, 160, 180)
    };

    cv::line(grafico, cv::Point(margem, altura - margem), cv::Point(largura - margem, altura - margem), cv::Scalar(0, 0, 0), 1);
    cv::line(grafico, cv::Point(margem, margem), cv::Point(margem, altura - margem), cv::Scalar(0, 0, 0), 1);
    cv::putText(grafico, "Incremento", cv::Point(largura / 2 - 45, altura - 15), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 1);
    cv::putText(grafico, "Perda acumulada (%)", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 1);

    for (int i = 0; i <= 100; i += 25)
    {
        int y = altura - margem - static_cast<int>((altura - (2 * margem)) * (i / 100.0));
        cv::line(grafico, cv::Point(margem - 5, y), cv::Point(margem, y), cv::Scalar(0, 0, 0), 1);
        cv::putText(grafico, std::to_string(i), cv::Point(20, y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.45, cv::Scalar(0, 0, 0), 1);
    }

    int indexCor = 0;
    int posicaoLegenda = margem;

    for (const auto& serie : series)
    {
        std::vector<cv::Point> pontos;
        cv::Scalar cor = cores[indexCor % cores.size()];

        for (int i = 0; i < serie.second.size(); ++i)
        {
            int x = margem;

            if (serie.second.size() > 1)
            {
                x += static_cast<int>((largura - (2 * margem)) * (static_cast<double>(i) / (serie.second.size() - 1)));
            }

            double valor = std::clamp(serie.second[i], 0.0, 100.0);
            int y = altura - margem - static_cast<int>((altura - (2 * margem)) * (valor / 100.0));
            pontos.push_back(cv::Point(x, y));

            std::string incremento = "p" + std::to_string(i);
            cv::putText(grafico, incremento, cv::Point(x - 8, altura - margem + 20), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);
        }

        for (int i = 1; i < pontos.size(); ++i)
        {
            cv::line(grafico, pontos[i - 1], pontos[i], cor, 2);
        }

        for (const cv::Point& ponto : pontos)
        {
            cv::circle(grafico, ponto, 4, cor, cv::FILLED);
        }

        cv::line(grafico, cv::Point(largura - 190, posicaoLegenda), cv::Point(largura - 160, posicaoLegenda), cor, 2);
        cv::putText(grafico, serie.first, cv::Point(largura - 150, posicaoLegenda + 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cor, 1);

        posicaoLegenda += 25;
        indexCor++;
    }

    cv::imwrite(destino.string(), grafico);
}
