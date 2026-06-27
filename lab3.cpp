#include <iostream>

#include "utils.h"

void runLab3()
{
    int processar;

    std::cout << "\n";
    std::cout << "LAB 03 - Granulometria Morfologica, (0 -> VOLTAR):" << std::endl;
    std::cout << "1 -> Reset (deleta imagens pre-processadas e outputs)" << std::endl;
    std::cout << "2 -> Pre-Processar (ainda nao implementado)" << std::endl;
    std::cout << "3 -> Definir incremento de raio (ainda nao implementado)" << std::endl;
    std::cout << "4 -> Definir iteracoes (ainda nao implementado)" << std::endl;
    std::cout << "5 -> Gerar assinaturas (ainda nao implementado)" << std::endl;
    std::cin >> processar;

    switch (processar)
    {
    case 0:
        return;
    case 1:
        // Limpar os arquivos gerados pelo Lab 3.
        limparOutput(3);
        runLab3();
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        std::cout << "Opcao ainda nao implementada." << std::endl;
        runLab3();
        break;
    default:
        std::cout << "Opcao invalida." << std::endl;
        runLab3();
        break;
    }
}
