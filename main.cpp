#include <iostream>
#include <opencv2/core.hpp>

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
void ChooseExercise(int exercise);
void Exercise1(int exercise);
void Exercise2(int exercise);
void Exercise3(int exercise);
void Exercise4(int exercise);
void Exercise5(int exercise);
void Exercise6(int exercise);

int main()
{
    int exercise;
    auto lang = "C++";

    std::cout << "Hello and welcome to Visual Computing Lab" << lang << "!\n";
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    std::cout << "Selecione um exercicio de 1 a 8" << std::endl;
    std::cin >> exercise;

    ChooseExercise(exercise);
}

void ChooseExercise(int exercise)
{
    switch (exercise)
    {
    case 1:
        std::cout << "Rodando o Exercício 1" << std::endl;
        break;
    case 2:
        std::cout << "Rodando o Exercício 2" << std::endl;
        break;
    case 3:
        std::cout << "Rodando o Exercício 3" << std::endl;
        break;
    case 4:
        std::cout << "Rodando o Exercício 4" << std::endl;
        break;
    case 5:
        std::cout << "Rodando o Exercício 5" << std::endl;
        break;
    case 6:
        std::cout << "Rodando o Exercício 6" << std::endl;
        break;
    case 7:
        std::cout << "Rodando o Exercício 7" << std::endl;
        break;
    case 8:
        std::cout << "Rodando o Exercício 8" << std::endl;
        break;
    default:
        std::cout << "Invalid exercise" << std::endl;
        break;
    }
}


void Exercise1(int exercise)
{
    std::cout << "Run Exercise " << exercise << std::endl;
}

void Exercise2(int exercise1)
{
    std::cout << "Run Exercise " << exercise1 << std::endl;
}


void Exercise3(int exercise1)
{
    std::cout << "Run Exercise " << exercise1 << std::endl;
}

void Exercise4(int exercise1)
{
    std::cout << "Run Exercise " << exercise1 << std::endl;
}

void Exercise5(int exercise1)
{
    std::cout << "Run Exercise " << exercise1 << std::endl;
}

void Exercise6(int exercise1)
{
    std::cout << "Run Exercise " << exercise1 << std::endl;
}

void Exercise7(int exercise1)
{
    std::cout << "Run Exercise " << exercise1 << std::endl;
}

void Exercise8(int exercise1)
{
    std::cout << "Run Exercise " << exercise1 << std::endl;
}
