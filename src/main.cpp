#include <iostream>
#include <mpfr.h>
#include "MPcalc.h"

int main()
{
    std::cout << "---ULTIMATE CALCULATOR---" << std::endl << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  -  2048 Decimal Digits" << std::endl;
    std::cout << "  -  REALLY BIG numbers" << std::endl << std::endl;
    std::cout << "WARNING: pi and e are estimates, do not rely on them being 100% correct" << std::endl << std::endl;
    std::cout << "Operators: ";
    for(unsigned int i = 0; i < calculator::constants::opCount; i++)
    {
        if(i < calculator::constants::opCount-1)
            std::cout << calculator::constants::operators[i] << ", ";
        else
            std::cout << calculator::constants::operators[i] << ".";
    }
    std::cout << std::endl << std::endl;

    for(;;)
    {
        std::string input;
        std::cout << "Enter Equation: ";
        std::getline(std::cin, input);
        std::cout << calculator::calculate(input.c_str(), input.size()) << std::endl << std::endl;
    }
}
