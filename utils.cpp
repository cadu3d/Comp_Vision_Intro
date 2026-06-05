//
// Created by cadu3d on 3/29/2026.
//
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "utils.h"

namespace
{
    std::filesystem::path projectRoot()
    {
        return PROJECT_ROOT;
    }

    std::filesystem::path preProcessadasDir()
    {
        return projectRoot() / "output" / "preProcessadas";
    }

    std::filesystem::path lab2InputDir()
    {
        return projectRoot() / "input" / "Lab_2";
    }

    std::filesystem::path outputDir(const std::string& folder)
    {
        if (folder == "preProcessadas")
        {
            return preProcessadasDir();
        }

        if (folder == "transformadasHough")
        {
            return projectRoot() / "output" / "Lab_2" / "transformadasHough";
        }

        return projectRoot() / "output" / folder;
    }

    bool temImagem(const std::filesystem::path& folder)
    {
        if (!std::filesystem::exists(folder))
        {
            return false;
        }

        for (const auto& entry : std::filesystem::directory_iterator(folder))
        {
            std::string ext = entry.path().extension().string();

            if (ext == ".jpg" || ext == ".jpeg" || ext == ".png")
            {
                return true;
            }
        }

        return false;
    }
}

std::vector<cv::Mat> carregarImagens(const std::filesystem::path& folder);
std::vector<ImagemCarregada> carregarImagensComNomes(const std::filesystem::path& folder);

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

std::vector<cv::Mat> carregarImagens(const std::filesystem::path& folder)
{
    std::vector<cv::Mat> imagens;

    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::filesystem::path path = entry.path();
        std::string ext = path.extension().string();

        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png")
        {
            cv::Mat imagem = cv::imread(path.string(), cv::IMREAD_GRAYSCALE);

            if (!imagem.empty())
            {
                imagens.push_back(imagem);
            }
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
        std::string ext = path.extension().string();

        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png")
        {
            paths.push_back(path);
        }
    }

    std::sort(paths.begin(), paths.end());

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
        std::cout << "Pre Processando imagens originais, no dir: " + sourceDir.string() << std::endl;
        return sourceDir;
    }

    std::cout << "Pre Processando imagens ja existentes, no dir: " + outputDir.string() << std::endl;
    return outputDir;
}

std::filesystem::path buscarImagem()
{

    const std::filesystem::path pathImage = projectRoot() / "input" / "lab_1" / "imagem.jpg";
    return pathImage;

}


cv::Mat gerarImagemCinza()
{
    const std::filesystem::path pathImage = buscarImagem();
    cv::Mat imagemOriginal = cv::imread(pathImage.string(), cv::IMREAD_GRAYSCALE);

    if (imagemOriginal.empty())
    {
        throw std::runtime_error("Nao foi possivel carregar a imagem em: " + pathImage.string());
    }

    const std::filesystem::path outputPath = projectRoot() / "output" / "imagem_gray.jpg";
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


void limparOutput()
{
    const std::vector<std::filesystem::path> foldersParaLimpar = {
        preProcessadasDir(),
        projectRoot() / "output" / "Lab_2" / "transformadasHough"
    };

    int removidas = 0;

    for (const std::filesystem::path& folder : foldersParaLimpar)
    {
        if (!std::filesystem::exists(folder))
        {
            continue;
        }

        for (const auto& entry : std::filesystem::directory_iterator(folder))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

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
    }

    std::cout << "Imagens removidas: " << removidas << std::endl;
}
