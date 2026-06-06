#include <iostream>

#include "utils.h"

void runLab3()
{
    int processar = 0;

    while (true)
    {
        std::cout << "\n";
        std::cout << "LAB 03 - Granulometria Morfologica, (0 -> VOLTAR):" << std::endl;
        std::cout << "1 -> Reset (deleta imagens pre-processadas e outputs)" << std::endl;
        std::cout << "2 -> Pre-Processar (ainda nao implementado)" << std::endl;
        std::cout << "3 -> Definir incremento de raio (ainda nao implementado)" << std::endl;
        std::cout << "4 -> Definir iteracoes (ainda nao implementado)" << std::endl;
        std::cout << "5 -> Gerar assinaturas (ainda nao implementado)" << std::endl;
        std::cout << "> ";
        std::cin >> processar;

        if (processar == 0)
        {
            return;
        }

        if (processar == 1)
        {
            limparOutput(3);
            continue;
        }

        std::cout << "Opcao ainda nao implementada." << std::endl;
    }
}
