#include <iostream>

void runLab3()
{
    int processar = 0;

    while (true)
    {
        std::cout << "\n";
        std::cout << "LAB 03 - Granulometria Morfologica, (0 -> VOLTAR):" << std::endl;
        std::cout << "1 -> Pre-Processar (ainda nao implementado)" << std::endl;
        std::cout << "2 -> Definir incremento de raio (ainda nao implementado)" << std::endl;
        std::cout << "3 -> Definir iteracoes (ainda nao implementado)" << std::endl;
        std::cout << "4 -> Gerar assinaturas (ainda nao implementado)" << std::endl;
        std::cout << "> ";
        std::cin >> processar;

        if (processar == 0)
        {
            return;
        }

        std::cout << "Opcao ainda nao implementada." << std::endl;
    }
}
