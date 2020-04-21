#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "omp.h"

std::vector<int> parallelFunctional(int minNum, int maxNum) 
{
    int lastNum = (int)sqrt(maxNum);
    std::vector<int> vectorComplex;

#pragma omp		parallel num_threads(4)
    {
#pragma omp		for
        for (int i = 2; i <= lastNum; i++) {
            if (std::count(vectorComplex.begin(), vectorComplex.end(), i)) //ju¿ wiemy, ¿e to liczba z³o¿ona
                break;

            //jest to liczba pierwsza
            for (int m = i + i; m <= maxNum; m += i) //dodajemy wszystkie wielokrotnoœci
            {
#pragma omp critical
                if ((std::count(vectorComplex.begin(), vectorComplex.end(), m)) == false) //nie ma jej jeszcze w vectorComplex
                    vectorComplex.push_back(m);
            }
        }
    }

    std::vector<int> vectorPrime;
    for (int i = minNum; i <= maxNum; i++)
        if (std::count(vectorComplex.begin(), vectorComplex.end(), i) == false) //liczby nie ma w vectorComplex
            vectorPrime.push_back(i);

    printf("Complex\n");
    for (int x : vectorComplex)
        std::cout << x << " ";

    printf("\nPrime\n");
    for (int x : vectorPrime)
        std::cout << x << " ";

        return vectorPrime;
}

int main() 
{
    parallelFunctional(20, 100);
}