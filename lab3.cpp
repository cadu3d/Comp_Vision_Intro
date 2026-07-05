#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "filters.h"
#include "utils.h"

struct AssinaturaGranulometrica
{
    std::string nome;
    std::string classe;
    std::vector<double> valores;
};

void menuPreProcessamentoLab3();
void configurarGranulometria();
void gerarComparacoesGranulometricas();
void gerarAssinaturasGranulometricas();
void salvarAssinaturasCsv(const std::vector<AssinaturaGranulometrica>& assinaturas);
void salvarComparacaoCsv(const std::map<std::string, std::vector<double>>& mediasPorClasse);
void salvarGraficoAssinatura(const AssinaturaGranulometrica& assinatura);
void salvarGraficoComparacao(const std::map<std::string, std::vector<double>>& mediasPorClasse);
std::string pegarClasseLab3(const std::string& nome);
std::string montarNomeComparacao(const std::string& nome, const std::vector<int>& perdasPercentuais);
bool ehNumeroInteiro(const std::string& texto);
std::vector<std::string> separarTexto(const std::string& texto, char separador);
std::vector<AssinaturaGranulometrica> carregarAssinaturasPelosNomes();
bool ehResultadoGranulometriaLab3(const std::string& nome);
std::filesystem::path origemLab3(std::string& origemMensagem);
std::vector<ImagemCarregada> carregarImagensLab3(const std::filesystem::path& origem);
std::map<std::string, std::vector<double>> calcularMediasPorClasse(
    const std::vector<AssinaturaGranulometrica>& assinaturas
);

int incrementoRaioLab3 = 2;
int iteracoesLab3 = 12;

void runLab3()
{
    std::string processar;

    std::cout << "\n";
    std::cout << "LAB 03 - Granulometria Morfologica:" << std::endl;
    std::cout << "\n";
    std::cout << "0 -> VOLTAR" << std::endl;
    std::cout << "00 -> RESET" << std::endl;
    std::cout << "\n";
    std::cout << "1 -> Pre-Processar" << std::endl;
    std::cout << "2 -> Definir incremento de raio e iteracoes" << std::endl;
    std::cout << "3 -> Gerar comparacoes" << std::endl;
    std::cout << "4 -> Gerar assinaturas" << std::endl;
    std::cin >> processar;

    if (processar == "0")
    {
        return;
    }

    if (processar == "00")
    {
        limparOutput(3);
        runLab3();
        return;
    }

    if (processar == "1")
    {
        menuPreProcessamentoLab3();
        runLab3();
        return;
    }

    if (processar == "2")
    {
        configurarGranulometria();
        runLab3();
        return;
    }

    if (processar == "3")
    {
        gerarComparacoesGranulometricas();
        runLab3();
        return;
    }

    if (processar == "4")
    {
        gerarAssinaturasGranulometricas();
        runLab3();
        return;
    }

    std::cout << "Opcao invalida." << std::endl;
    runLab3();
}

void menuPreProcessamentoLab3()
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
            limparOutput(3);
            continue;
        }

        try
        {
            Filters::preProcImagem(std::stoi(filtro), "Lab_3");
        }
        catch (const std::exception&)
        {
            std::cout << "Filtro invalido" << std::endl;
        }
    }
}

void configurarGranulometria()
{
    std::cout << "Digite o incremento do raio (atual " << incrementoRaioLab3 << "): ";
    std::cin >> incrementoRaioLab3;

    if (incrementoRaioLab3 < 1)
    {
        incrementoRaioLab3 = 1;
    }

    std::cout << "Incremento de raio definido: " << incrementoRaioLab3 << std::endl;
    std::cout << "Digite a quantidade de iteracoes (atual " << iteracoesLab3 << "): ";
    std::cin >> iteracoesLab3;

    if (iteracoesLab3 < 1)
    {
        iteracoesLab3 = 1;
    }

    std::cout << "Iteracoes definidas: " << iteracoesLab3 << std::endl;
}

void gerarComparacoesGranulometricas()
{
    std::filesystem::path destino = projectRoot() / "output" / "Lab_3" / "preProcessadas";
    std::filesystem::create_directories(destino);

    // Remover comparacoes antigas antes de gerar novas.
    limparArquivosComPrefixos(destino, {"comparacao_", "Classe_"});

    std::string origemMensagem;
    std::filesystem::path origem = origemLab3(origemMensagem);
    std::vector<ImagemCarregada> imagens = carregarImagensLab3(origem);

    if (imagens.empty() && origem != projectRoot() / "input" / "Lab_3")
    {
        origemMensagem = "imagens originais";
        imagens = carregarImagensLab3(projectRoot() / "input" / "Lab_3");
    }

    if (imagens.empty())
    {
        std::cout << "Nenhuma imagem encontrada em: " << origem.string() << std::endl;
        return;
    }

    for (const ImagemCarregada& imagem : imagens)
    {
        cv::Mat imagemBase = imagem.imagem.clone();
        cv::Mat imagemAnterior = imagemBase.clone();
        cv::Mat comparacao = imagemBase.clone();
        cv::Mat perdaAcumulada = cv::Mat::zeros(imagemBase.size(), CV_8UC1);
        std::vector<cv::Mat> imagensComparacao;
        std::vector<int> perdasPercentuais;

        // Usar a quantidade de pixels brancos como area de referencia.
        double areaOriginal = static_cast<double>(cv::countNonZero(imagemBase));

        if (areaOriginal <= 0)
        {
            areaOriginal = static_cast<double>(imagemBase.rows * imagemBase.cols);
        }

        imagensComparacao.push_back(imagemBase);
        perdasPercentuais.push_back(0);

        for (int i = 1; i <= iteracoesLab3; ++i)
        {
            int raio = i * incrementoRaioLab3;
            int tamanho = (raio * 2) + 1;

            // Abrir a imagem com um elemento circular do raio atual.
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(tamanho, tamanho));
            cv::Mat imagemAberta;
            cv::Mat perda;

            cv::morphologyEx(imagemBase, imagemAberta, cv::MORPH_OPEN, kernel);
            cv::subtract(imagemAnterior, imagemAberta, perda);

            // Pinta de preto tudo que foi perdido ate esta escala.
            cv::bitwise_or(perdaAcumulada, perda, perdaAcumulada);
            comparacao.setTo(cv::Scalar(0), perda);
            imagensComparacao.push_back(comparacao.clone());
            perdasPercentuais.push_back(
                static_cast<int>(std::round((cv::countNonZero(perdaAcumulada) / areaOriginal) * 100.0))
            );

            imagemAnterior = imagemAberta.clone();
        }

        imagensComparacao.push_back(cv::Mat(imagemBase.size(), imagemBase.type(), cv::Scalar(0)));
        perdasPercentuais.push_back(100);

        cv::Mat tira;
        cv::hconcat(imagensComparacao, tira);

        std::filesystem::path arquivo = destino / (montarNomeComparacao(imagem.nome, perdasPercentuais) + ".png");
        cv::imwrite(arquivo.string(), tira);

        std::cout << "Comparacao gerada: " << imagem.nome << std::endl;
    }

    std::cout << "\nComparacoes geradas para " << imagens.size()
        << " imagens em " << destino.string() << " usando " << origemMensagem << std::endl;
}

void gerarAssinaturasGranulometricas()
{
    std::filesystem::path destino = projectRoot() / "output" / "Lab_3";
    std::vector<AssinaturaGranulometrica> assinaturas = carregarAssinaturasPelosNomes();

    if (assinaturas.empty())
    {
        std::cout << "Nenhuma tira Classe_*_Perda_*.png encontrada em: "
            << (projectRoot() / "output" / "Lab_3" / "preProcessadas").string() << std::endl;
        std::cout << "Execute primeiro: 4 -> Gerar comparacoes" << std::endl;
        return;
    }

    std::filesystem::create_directories(destino);

    for (const AssinaturaGranulometrica& assinatura : assinaturas)
    {
        salvarGraficoAssinatura(assinatura);
        std::cout << "Assinatura gerada: " << assinatura.nome << std::endl;
    }

    std::map<std::string, std::vector<double>> mediasPorClasse = calcularMediasPorClasse(assinaturas);

    salvarAssinaturasCsv(assinaturas);
    salvarComparacaoCsv(mediasPorClasse);
    salvarGraficoComparacao(mediasPorClasse);

    std::cout << "\nAssinaturas lidas dos nomes de " << assinaturas.size()
        << " imagens em " << destino.string() << std::endl;
    std::cout << "Classes comparadas: ";

    for (const auto& classe : mediasPorClasse)
    {
        std::cout << classe.first << " ";
    }

    std::cout << std::endl;
}

std::string pegarClasseLab3(const std::string& nome)
{
    size_t posicao = nome.find('-');

    if (posicao == std::string::npos)
    {
        posicao = nome.find('_');
    }

    if (posicao == std::string::npos)
    {
        return nome;
    }

    return nome.substr(0, posicao);
}

std::string montarNomeComparacao(const std::string& nome, const std::vector<int>& perdasPercentuais)
{
    std::string nomeComparacao = "Classe_" + pegarClasseLab3(nome) + "_Perda";

    for (int perda : perdasPercentuais)
    {
        nomeComparacao += "_" + std::to_string(std::clamp(perda, 0, 100));
    }

    return nomeComparacao + "_" + nome;
}

bool ehNumeroInteiro(const std::string& texto)
{
    if (texto.empty())
    {
        return false;
    }

    for (char caractere : texto)
    {
        if (!std::isdigit(static_cast<unsigned char>(caractere)))
        {
            return false;
        }
    }

    return true;
}

std::vector<std::string> separarTexto(const std::string& texto, char separador)
{
    std::vector<std::string> partes;
    std::stringstream stream(texto);
    std::string parte;

    while (std::getline(stream, parte, separador))
    {
        partes.push_back(parte);
    }

    return partes;
}

std::vector<AssinaturaGranulometrica> carregarAssinaturasPelosNomes()
{
    std::filesystem::path origem = projectRoot() / "output" / "Lab_3" / "preProcessadas";
    std::vector<AssinaturaGranulometrica> assinaturas;

    if (!std::filesystem::exists(origem))
    {
        return assinaturas;
    }

    for (const auto& entry : std::filesystem::directory_iterator(origem))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".png")
        {
            continue;
        }

        std::string nome = entry.path().stem().string();

        if (nome.rfind("Classe_", 0) != 0)
        {
            continue;
        }

        std::vector<std::string> partes = separarTexto(nome, '_');

        if (partes.size() < 4 || partes[2] != "Perda")
        {
            continue;
        }

        AssinaturaGranulometrica assinatura;
        assinatura.nome = nome;
        assinatura.classe = partes[1];

        // Ler os percentuais gravados no nome da tira.
        for (int i = 3; i < partes.size(); ++i)
        {
            if (!ehNumeroInteiro(partes[i]))
            {
                break;
            }

            assinatura.valores.push_back(std::stod(partes[i]));
        }

        if (!assinatura.valores.empty())
        {
            assinaturas.push_back(assinatura);
        }
    }

    std::sort(
        assinaturas.begin(),
        assinaturas.end(),
        [](const AssinaturaGranulometrica& a, const AssinaturaGranulometrica& b)
        {
            return a.nome < b.nome;
        }
    );

    return assinaturas;
}

bool ehResultadoGranulometriaLab3(const std::string& nome)
{
    return nome.rfind("perda_", 0) == 0
        || nome.rfind("tira_perdas_", 0) == 0
        || nome.rfind("comparacao_", 0) == 0
        || nome.rfind("Classe_", 0) == 0;
}

std::filesystem::path origemLab3(std::string& origemMensagem)
{
    std::filesystem::path preProcessadas = projectRoot() / "output" / "Lab_3" / "preProcessadas";

    if (temImagem(preProcessadas))
    {
        origemMensagem = "imagens pre-processadas";
        return preProcessadas;
    }

    origemMensagem = "imagens originais";
    return projectRoot() / "input" / "Lab_3";
}

std::vector<ImagemCarregada> carregarImagensLab3(const std::filesystem::path& origem)
{
    std::vector<ImagemCarregada> imagens = carregarImagensComNomes(origem);
    std::vector<ImagemCarregada> filtradas;

    for (const ImagemCarregada& imagem : imagens)
    {
        if (!ehResultadoGranulometriaLab3(imagem.nome))
        {
            filtradas.push_back(imagem);
        }
    }

    return filtradas;
}

std::map<std::string, std::vector<double>> calcularMediasPorClasse(
    const std::vector<AssinaturaGranulometrica>& assinaturas
)
{
    std::map<std::string, std::vector<double>> mediasPorClasse;
    std::map<std::string, std::vector<int>> totalPorClasse;

    for (const AssinaturaGranulometrica& assinatura : assinaturas)
    {
        if (mediasPorClasse.find(assinatura.classe) == mediasPorClasse.end())
        {
            mediasPorClasse[assinatura.classe] = std::vector<double>(assinatura.valores.size(), 0.0);
            totalPorClasse[assinatura.classe] = std::vector<int>(assinatura.valores.size(), 0);
        }

        if (mediasPorClasse[assinatura.classe].size() < assinatura.valores.size())
        {
            mediasPorClasse[assinatura.classe].resize(assinatura.valores.size(), 0.0);
            totalPorClasse[assinatura.classe].resize(assinatura.valores.size(), 0);
        }

        for (int i = 0; i < assinatura.valores.size(); ++i)
        {
            mediasPorClasse[assinatura.classe][i] += assinatura.valores[i];
            totalPorClasse[assinatura.classe][i]++;
        }
    }

    for (auto& classe : mediasPorClasse)
    {
        for (int i = 0; i < classe.second.size(); ++i)
        {
            int total = totalPorClasse[classe.first][i];

            if (total > 0)
            {
                classe.second[i] /= total;
            }
        }
    }

    return mediasPorClasse;
}

void salvarAssinaturasCsv(const std::vector<AssinaturaGranulometrica>& assinaturas)
{
    std::filesystem::path destino = projectRoot() / "output" / "Lab_3" / "assinaturas.csv";
    std::ofstream arquivo(destino);
    size_t totalPontos = 0;

    for (const AssinaturaGranulometrica& assinatura : assinaturas)
    {
        totalPontos = std::max(totalPontos, assinatura.valores.size());
    }

    arquivo << "imagem,classe";

    for (int i = 0; i < totalPontos; ++i)
    {
        arquivo << ",p" << i;
    }

    arquivo << "\n";

    for (const AssinaturaGranulometrica& assinatura : assinaturas)
    {
        arquivo << assinatura.nome << "," << assinatura.classe;

        for (double valor : assinatura.valores)
        {
            arquivo << "," << std::fixed << std::setprecision(6) << valor;
        }

        arquivo << "\n";
    }
}

void salvarComparacaoCsv(const std::map<std::string, std::vector<double>>& mediasPorClasse)
{
    std::filesystem::path destino = projectRoot() / "output" / "Lab_3" / "comparacao_classes.csv";
    std::ofstream arquivo(destino);
    size_t totalPontos = 0;

    for (const auto& classe : mediasPorClasse)
    {
        totalPontos = std::max(totalPontos, classe.second.size());
    }

    arquivo << "classe";

    for (int i = 0; i < totalPontos; ++i)
    {
        arquivo << ",p" << i;
    }

    arquivo << "\n";

    for (const auto& classe : mediasPorClasse)
    {
        arquivo << classe.first;

        for (double valor : classe.second)
        {
            arquivo << "," << std::fixed << std::setprecision(6) << valor;
        }

        arquivo << "\n";
    }
}

void salvarGraficoAssinatura(const AssinaturaGranulometrica& assinatura)
{
    std::map<std::string, std::vector<double>> serie;
    serie[assinatura.nome] = assinatura.valores;

    std::filesystem::path destino = projectRoot() / "output" / "Lab_3" / ("assinatura_" + assinatura.nome + ".png");
    salvarGraficoPercentuais(serie, destino);
}

void salvarGraficoComparacao(const std::map<std::string, std::vector<double>>& mediasPorClasse)
{
    std::filesystem::path destino = projectRoot() / "output" / "Lab_3" / "comparacao_classes.png";
    salvarGraficoPercentuais(mediasPorClasse, destino);
}
