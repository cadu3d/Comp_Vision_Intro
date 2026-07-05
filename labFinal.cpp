#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>

#include "filters.h"
#include "utils.h"

std::filesystem::path origemLabFinal(std::string& origemMensagem);
void menuPreProcessamentoFinal();
void detectarCirculos();
void detectarRetas();
std::vector<std::string> carregarClasses(const std::filesystem::path& classesPath);
std::string classeParaSufixo(const std::string& classe);
std::string confiancaParaSufixo(float confianca);
cv::Mat preProcessarImagemOnnx(const cv::Mat& imagem);
std::pair<int, float> predizerClasse(cv::dnn::Net& net, const cv::Mat& blob);
void analisarImagemComIA();
void analisarImagemComIAPreProcessadas();
void analisarImagemComIAContorno();

// Parametros usados na deteccao de circulos.
double houghDpFinal = 1.2;
double houghMinDistFinal = 100;
double houghParam1Final = 120;
double houghParam2Final = 35;
int houghMinRadiusFinal = 80;
int houghMaxRadiusFinal = 256;

// Parametros usados na deteccao de retas.
double houghRhoFinal = 1;
double houghThetaFinal = CV_PI / 180;
int houghThresholdFinal = 25;
double houghMinLineLengthFinal = 25;
double houghMaxLineGapFinal = 90;
double houghAngleThresholdFinal = 10;

double anguloRetaGraus(const cv::Vec4i& reta)
{
    double angulo = std::atan2(reta[3] - reta[1], reta[2] - reta[0]) * 180.0 / CV_PI;

    if (angulo < 0)
    {
        angulo += 180.0;
    }

    if (angulo >= 180.0)
    {
        angulo -= 180.0;
    }

    return angulo;
}

double diferencaAngularGraus(double a, double b)
{
    double diferenca = std::abs(a - b);
    return std::min(diferenca, 180.0 - diferenca);
}

double comprimentoReta(const cv::Vec4i& reta)
{
    double dx = reta[2] - reta[0];
    double dy = reta[3] - reta[1];
    return std::sqrt(dx * dx + dy * dy);
}

cv::Point2d centroReta(const cv::Vec4i& reta)
{
    return cv::Point2d((reta[0] + reta[2]) / 2.0, (reta[1] + reta[3]) / 2.0);
}

double distanciaNormalEntreRetas(const cv::Vec4i& a, const cv::Vec4i& b)
{
    double dx = a[2] - a[0];
    double dy = a[3] - a[1];
    double comprimento = std::sqrt(dx * dx + dy * dy);

    if (comprimento <= 0.0)
    {
        return 0.0;
    }

    cv::Point2d normal(-dy / comprimento, dx / comprimento);
    cv::Point2d diferencaCentros = centroReta(b) - centroReta(a);
    return std::abs(diferencaCentros.x * normal.x + diferencaCentros.y * normal.y);
}

std::vector<cv::Vec4i> filtrarRetasPorAngulo(std::vector<cv::Vec4i> retas, double thresholdAngulo)
{
    constexpr double thresholdDistanciaNormal = 20.0;

    std::sort(
        retas.begin(),
        retas.end(),
        [](const cv::Vec4i& a, const cv::Vec4i& b)
        {
            return comprimentoReta(a) > comprimentoReta(b);
        }
    );

    std::vector<cv::Vec4i> filtradas;

    for (const cv::Vec4i& reta : retas)
    {
        double angulo = anguloRetaGraus(reta);
        bool analogica = false;

        for (const cv::Vec4i& retaAceita : filtradas)
        {
            bool anguloSimilar = diferencaAngularGraus(angulo, anguloRetaGraus(retaAceita)) <= thresholdAngulo;
            bool posicaoSimilar = distanciaNormalEntreRetas(retaAceita, reta) <= thresholdDistanciaNormal;

            if (anguloSimilar && posicaoSimilar)
            {
                analogica = true;
                break;
            }
        }

        if (!analogica)
        {
            filtradas.push_back(reta);
        }
    }

    return filtradas;
}

std::filesystem::path origemLabFinal(std::string& origemMensagem)
{
    const std::filesystem::path preProcessadas = projectRoot() / "output" / "Lab_FINAL" / "preProcessadas";
    origemMensagem = "imagens originais";

    // Usar imagens pre-processadas quando elas existirem.
    if (temImagem(preProcessadas))
    {
        origemMensagem = "imagens pre-processadas";
        return preProcessadas;
    }

    return projectRoot() / "input" / "Lab_FINAL";
}

void menuPreProcessamentoFinal()
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

        if (filtro == "18")
        {
            detectarCirculos();
            continue;
        }

        if (filtro == "19")
        {
            detectarRetas();
            continue;
        }

        // Aplicar o filtro escolhido pelo usuario.
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

void detectarCirculos()
{
    std::string origemMensagem;
    std::filesystem::path origem = origemLabFinal(origemMensagem);
    std::vector<ImagemCarregada> imagens = carregarImagensComNomes(origem);
    std::filesystem::path destino = projectRoot() / "output" / "Lab_FINAL";
    std::filesystem::create_directories(destino);
    int totalDetectado = 0;

    for (int i = 0; i < imagens.size(); ++i)
    {
        const ImagemCarregada& imagemComNome = imagens[i];
        cv::Mat imagemCinza = imagemComNome.imagem.clone();

        // Encontrar os circulos usando a Transformada de Hough.
        std::vector<cv::Vec3f> coordCirculos;
        cv::HoughCircles(
            imagemCinza,
            coordCirculos,
            cv::HOUGH_GRADIENT,
            houghDpFinal,
            houghMinDistFinal,
            houghParam1Final,
            houghParam2Final,
            houghMinRadiusFinal,
            houghMaxRadiusFinal
        );

        // Converter para BGR para desenhar os resultados coloridos.
        cv::Mat result;
        cv::cvtColor(imagemCinza, result, cv::COLOR_GRAY2BGR);

        for (const cv::Vec3f& circulo : coordCirculos)
        {
            cv::Point centro(cvRound(circulo[0]), cvRound(circulo[1]));
            int raio = cvRound(circulo[2]);

            cv::circle(result, centro, raio, cv::Scalar(0, 255, 0), 3);
            cv::circle(result, centro, 3, cv::Scalar(0, 0, 255), -1);
        }

        std::string nomeResultado = imagemComNome.nome;
        if (!coordCirculos.empty())
        {
            nomeResultado += "_CIRCULO";
            ++totalDetectado;
        }

        std::string arquivoResultado = nomeResultado + ".png";
        cv::imwrite((destino / arquivoResultado).string(), result);
        std::cout << imagemComNome.nome << " -> " << arquivoResultado << std::endl;
    }

    std::cout << "Circulos detectados em " << totalDetectado
        << " de " << imagens.size() << " " << origemMensagem << "." << std::endl;
}

void detectarRetas()
{
    std::string origemMensagem;
    std::filesystem::path origem = origemLabFinal(origemMensagem);
    std::vector<ImagemCarregada> imagens = carregarImagensComNomes(origem);
    std::filesystem::path destino = projectRoot() / "output" / "Lab_FINAL";
    std::filesystem::create_directories(destino);
    double thresholdAngulo = houghAngleThresholdFinal;

    std::cout << "Threshold de angulo para remover retas analogas em graus (padrao "
        << houghAngleThresholdFinal << "): ";

    if (!(std::cin >> thresholdAngulo))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        thresholdAngulo = houghAngleThresholdFinal;
    }

    if (thresholdAngulo < 0)
    {
        thresholdAngulo = houghAngleThresholdFinal;
    }

    for (int i = 0; i < imagens.size(); ++i)
    {
        const ImagemCarregada& imagemComNome = imagens[i];
        cv::Mat imagemCinza = imagemComNome.imagem.clone();
        cv::Mat bordas;

        // Gerar as bordas antes de procurar retas.
        cv::Canny(imagemCinza, bordas, 50, 150);

        std::vector<cv::Vec4i> retas;
        cv::HoughLinesP(
            bordas,
            retas,
            houghRhoFinal,
            houghThetaFinal,
            houghThresholdFinal,
            houghMinLineLengthFinal,
            houghMaxLineGapFinal
        );
        retas = filtrarRetasPorAngulo(retas, thresholdAngulo);

        // Converter para BGR para desenhar as retas coloridas.
        cv::Mat result;
        cv::cvtColor(imagemCinza, result, cv::COLOR_GRAY2BGR);

        for (const cv::Vec4i& reta : retas)
        {
            cv::Point inicio(reta[0], reta[1]);
            cv::Point fim(reta[2], reta[3]);
            cv::line(result, inicio, fim, cv::Scalar(0, 0, 255), 2);
        }

        std::string sufixoForma = "_CIRCULO";
        if (retas.size() == 3)
        {
            sufixoForma = "_TRIANGULO";
        }
        else if (retas.size() == 4)
        {
            sufixoForma = "_QUADRADO";
        }

        std::string arquivoResultado = imagemComNome.nome + sufixoForma + ".png";
        cv::imwrite((destino / arquivoResultado).string(), result);
        std::cout << imagemComNome.nome << " -> " << arquivoResultado << std::endl;
    }

    std::cout << "Retas detectadas em " << imagens.size()
        << " " << origemMensagem << "." << std::endl;
}

std::vector<std::string> carregarClasses(const std::filesystem::path& classesPath)
{
    std::ifstream classesFile(classesPath);

    if (!classesFile)
    {
        throw std::runtime_error("Nao foi possivel abrir: " + classesPath.string());
    }

    // Ler uma classe por linha do arquivo.
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

std::string classeParaSufixo(const std::string& classe)
{
    std::string sufixo = classe;
    std::transform(
        sufixo.begin(),
        sufixo.end(),
        sufixo.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(std::toupper(c));
        }
    );

    return sufixo;
}

std::string confiancaParaSufixo(float confianca)
{
    int valor = static_cast<int>(std::round(std::clamp(confianca, 0.0F, 1.0F) * 1000.0F));
    return std::to_string(valor);
}

cv::Mat preProcessarImagemOnnx(const cv::Mat& imagem)
{
    cv::Mat redimensionada;

    if (imagem.cols == 256 && imagem.rows == 256)
    {
        cv::pyrDown(imagem, redimensionada, cv::Size(128, 128));
    }
    else
    {
        cv::resize(imagem, redimensionada, cv::Size(128, 128), 0.0, 0.0, cv::INTER_AREA);
    }

    cv::Mat normalizada;
    // Mesmo preprocessamento do PyTorch: ToTensor() seguido de Normalize(0.5, 0.5).
    redimensionada.convertTo(normalizada, CV_32F, 1.0 / 127.5, -1.0);

    return cv::dnn::blobFromImage(
        normalizada,
        1.0,
        cv::Size(),
        cv::Scalar(),
        false,
        false,
        CV_32F
    );
}

std::pair<int, float> predizerClasse(cv::dnn::Net& net, const cv::Mat& blob)
{
    net.setInput(blob);

    // Rodar o modelo e transformar a saida em probabilidades.
    cv::Mat output = net.forward();
    cv::Mat scores = output.reshape(1, 1);
    cv::Mat probabilidades;
    cv::exp(scores, probabilidades);
    probabilidades /= cv::sum(probabilidades)[0];

    // Veredito por argmax: a classe com a maior probabilidade vence, sem threshold.
    cv::Point classeEncontrada;
    double confianca = 0.0;
    cv::minMaxLoc(probabilidades, nullptr, &confianca, nullptr, &classeEncontrada);

    return std::pair<int, float>(classeEncontrada.x, static_cast<float>(confianca));
}

void analisarImagemComIA(
    const std::filesystem::path& modelPath,
    const std::filesystem::path& classesPath,
    const std::filesystem::path& origem,
    const std::string& descricaoModelo
)
{
    const std::filesystem::path destino = projectRoot() / "output" / "Lab_FINAL";

    if (!temImagem(origem))
    {
        std::cout << "Nenhuma imagem encontrada em: " << origem.string() << std::endl;
        return;
    }

    try
    {
        std::vector<std::string> classes = carregarClasses(classesPath);
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath.string());
        std::vector<ImagemCarregada> imagens = carregarImagensComNomes(origem);
        std::filesystem::create_directories(destino);

        std::cout << "[IA] Modelo: " << descricaoModelo << std::endl;
        std::cout << "[IA] Origem: " << origem.string() << std::endl;

        for (const ImagemCarregada& imagemComNome : imagens)
        {
            cv::Mat imagemPreProcessada = preProcessarImagemOnnx(imagemComNome.imagem);
            std::pair<int, float> resultado = predizerClasse(net, imagemPreProcessada);
            int classeIndex = resultado.first;
            float confianca = resultado.second;

            if (classeIndex < 0 || classeIndex >= static_cast<int>(classes.size()))
            {
                std::cout << imagemComNome.nome
                    << ": o modelo retornou uma classe invalida: " << classeIndex << std::endl;
                continue;
            }

            for (const std::string& classe : classes)
            {
                const std::string prefixoResultado = imagemComNome.nome + "_" + classeParaSufixo(classe) + "_";
                for (const auto& entry : std::filesystem::directory_iterator(destino))
                {
                    if (entry.is_regular_file()
                        && entry.path().extension() == ".png"
                        && entry.path().stem().string().rfind(prefixoResultado, 0) == 0)
                    {
                        std::filesystem::remove(entry.path());
                    }
                }
            }

            std::string classe = classes[classeIndex];
            std::string nomeResultado = imagemComNome.nome + "_" + classeParaSufixo(classe)
                + "_" + confiancaParaSufixo(confianca) + ".png";
            cv::imwrite((destino / nomeResultado).string(), imagemComNome.imagem);

            std::cout << imagemComNome.nome << " -> " << nomeResultado
                << " (confianca: " << confianca << ")" << std::endl;
        }

        std::cout << "Imagens analisadas: " << imagens.size()
            << " em " << destino.string() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Erro ao analisar imagem com IA: " << e.what() << std::endl;
    }
}

void analisarImagemComIA()
{
    const std::filesystem::path preProcessadas = projectRoot() / "output" / "Lab_FINAL" / "preProcessadas";
    const std::filesystem::path inputOriginal = projectRoot() / "input" / "Lab_FINAL";
    const std::filesystem::path origem = temImagem(preProcessadas) ? preProcessadas : inputOriginal;

    analisarImagemComIA(
        projectRoot() / "models" / "handdraw_shapes.onnx",
        projectRoot() / "models" / "classes.txt",
        origem,
        "imagens originais"
    );
}

void analisarImagemComIAPreProcessadas()
{
    const std::filesystem::path preProcessadas = projectRoot() / "output" / "Lab_FINAL" / "preProcessadas";

    analisarImagemComIA(
        projectRoot() / "models" / "handdraw_shapes_preprocessed.onnx",
        projectRoot() / "models" / "classes_preprocessed.txt",
        preProcessadas,
        "imagens pre-processadas"
    );
}

void analisarImagemComIAContorno()
{
    const std::filesystem::path preProcessadas = projectRoot() / "output" / "Lab_FINAL" / "preProcessadas";

    analisarImagemComIA(
        projectRoot() / "models" / "handdraw_shapes_preprocessed_2.onnx",
        projectRoot() / "models" / "classes_preprocessed_2.txt",
        preProcessadas,
        "imagens de contorno"
    );
}

void runLabFinal()
{
    // Usei string porque existe a opcao 00.
    std::string opcao;

    // Menu principal do projeto final.
    std::cout << "\n";
    std::cout << "LAB FINAL:" << std::endl;
    std::cout << "\n";
    std::cout << "0 -> Voltar" << std::endl;
    std::cout << "00 -> Reset" << std::endl;
    std::cout << "1 -> Pre-Processar" << std::endl;
    std::cout << std::endl;
    std::cout << "------------ Analise Morfologica ------" << std::endl;
    std::cout << "2 -> Gerar Centro" << std::endl;
    std::cout << "3 -> Desenhar Grafico" << std::endl;
    std::cout << "4 -> Calcular Max e Min" << std::endl;
    std::cout << "5 -> Calcular Amplitude" << std::endl;
    std::cout << "6 -> Detectar Formas" << std::endl;
    std::cout << std::endl;
    std::cout << "------------ Analise Transf. Hough ------" << std::endl;
    std::cout << "7 -> Detectar Circulos" << std::endl;
    std::cout << "8 -> Detectar Formas com Retas" << std::endl;
    std::cout << std::endl;
    std::cout << "------------ Inteligencia Artificial ----------" << std::endl;
    std::cout << "9 -> Analisar imagem com IA" << std::endl;
    std::cout << "10 -> Analisar imagem com IA - Mascara" << std::endl;
    std::cout << "11 -> Analisar imagem com IA - Contorno" << std::endl;
    std::cout << std::endl;
    std::cout << "> ";
    std::cin >> opcao;

    // Volta para o menu principal do programa.
    if (opcao == "0")
    {
        return;
    }

    // Apaga os arquivos gerados no Lab Final.
    if (opcao == "00")
    {
        limparOutput("Lab_FINAL");
        runLabFinal();
        return;
    }

    // Abre o submenu de filtros e pre-processamento.
    if (opcao == "1")
    {
        menuPreProcessamentoFinal();
        runLabFinal();
        return;
    }

    // Gera o centro das imagens.
    if (opcao == "2")
    {
        Filters::gerarCentroLabFinal();
        runLabFinal();
        return;
    }

    // Desenha o grafico da forma.
    if (opcao == "3")
    {
        Filters::desenharGraficoLabFinal();
        runLabFinal();
        return;
    }

    // Calcula os pontos de maximo e minimo.
    if (opcao == "4")
    {
        Filters::calcularMaxMinLabFinal();
        runLabFinal();
        return;
    }

    // Calcula a amplitude do grafico.
    if (opcao == "5")
    {
        Filters::calcularAmplitudeLabFinal();
        runLabFinal();
        return;
    }

    // Detecta a forma usando os dados do nome do grafico.
    if (opcao == "6")
    {
        Filters::detectarFormasLabFinal();
        runLabFinal();
        return;
    }

    // Detecta circulos pela Transformada de Hough.
    if (opcao == "7")
    {
        detectarCirculos();
        runLabFinal();
        return;
    }

    // Detecta formas usando retas.
    if (opcao == "8")
    {
        detectarRetas();
        runLabFinal();
        return;
    }

    // Analise com IA usando o modelo das imagens originais.
    if (opcao == "9")
    {
        analisarImagemComIA();
        runLabFinal();
        return;
    }

    // Analise com IA usando o modelo das imagens com mascara.
    if (opcao == "10")
    {
        analisarImagemComIAPreProcessadas();
        runLabFinal();
        return;
    }

    // Analise com IA usando o modelo das imagens com contorno.
    if (opcao == "11")
    {
        analisarImagemComIAContorno();
        runLabFinal();
        return;
    }

    std::cout << "Escolha invalida" << std::endl;
    runLabFinal();
}
