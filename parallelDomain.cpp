#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "omp.h"

#define threadsNum 8

#define PRIME 1
#define COMPLEX 0

//////////////////////////////////////////////////////////////////

void printVector(std::vector <int> vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        printf("%d ", vec[i]);
        if (i % 10 == 9) //co 10-ta liczba
            printf("\n");
    }
    printf("\nprime numbers count: %d\n", vec.size());
}

//////////////////////////////////////////////////////////////////

std::vector<int> createStartingPrimes(int minNum, int maxNum) {

    int lastNum = (int)sqrt(maxNum);
    std::vector<bool> primeOrComplex;
    for (int i = 2; i <= maxNum; i++)
        primeOrComplex.push_back(PRIME);

    for (int divider = 2; divider <= lastNum; divider++)
    {
        if (primeOrComplex[divider - 2] == COMPLEX)
            continue;

        for (int multiple = divider + divider; multiple <= maxNum; multiple += divider) //usuwamy wszystkie wielokrotnoœci
            primeOrComplex[multiple - 2] = COMPLEX;
    }

    std::vector <int> primeNumbers;
    for (int i = minNum - 2; i < primeOrComplex.size(); i++)
    {
        if (primeOrComplex[i] == PRIME)
            primeNumbers.push_back(i + 2);
    }

    return primeNumbers;
}

std::vector<int> parallelDomain(int minNum, int maxNum)
{
    int lastNum = (int)sqrt(maxNum);
    int range = (maxNum - minNum) + 1;
    std::vector <int> startingPrimes = createStartingPrimes(2, lastNum);

    std::vector <bool> primeOrComplex0;
    std::vector <bool> primeOrComplex1;
    std::vector <bool> primeOrComplex2;
    std::vector <bool> primeOrComplex3;
    std::vector <bool> primeOrComplex4;
    std::vector <bool> primeOrComplex5;
    std::vector <bool> primeOrComplex6;
    std::vector <bool> primeOrComplex7;

#pragma omp		parallel num_threads(threadsNum)
    {
        int threadNumber = omp_get_thread_num();
        std::vector<bool> localPrimeOrComplex(range, PRIME);

#pragma omp		for schedule(dynamic, 10)
        for (int i = 0; i < startingPrimes.size(); i++)
        {
            int divider = startingPrimes[i];
            //znajdujemy pierwsza wielokrotnosc tej liczby w przedziale
            int multiple = minNum;
            for (; multiple % divider != 0; multiple++)
                continue;
            if (multiple == divider)
                multiple = divider + divider;

            for (; multiple <= maxNum; multiple += divider) //dodajemy wszystkie wielokrotności
                localPrimeOrComplex[multiple - minNum] = COMPLEX;
        }

        switch (threadNumber)
        {
        case 0:
            primeOrComplex0 = localPrimeOrComplex;
            break;
        case 1:
            primeOrComplex1 = localPrimeOrComplex;
            break;
        case 2:
            primeOrComplex2 = localPrimeOrComplex;
            break;
        case 3:
            primeOrComplex3 = localPrimeOrComplex;
            break;
        case 4:
            primeOrComplex4 = localPrimeOrComplex;
            break;
        case 5:
            primeOrComplex5 = localPrimeOrComplex;
            break;
        case 6:
            primeOrComplex6 = localPrimeOrComplex;
            break;
        case 7:
            primeOrComplex7 = localPrimeOrComplex;
            break;
        }
    }

    //łączenie lokalnych zbiorów liczb złożonych w jeden globalny
    std::vector <bool> primeOrComplex;
    switch (threadsNum)
    {
    case 1:
        for (int i = 0; i < range; i++)
            primeOrComplex.push_back(primeOrComplex0[i]);
        break;
    case 2:
        for (int i = 0; i < range; i++)
            primeOrComplex.push_back(
                primeOrComplex0[i] *
                primeOrComplex1[i]);
        break;
    case 3:
        for (int i = 0; i < range; i++)
            primeOrComplex.push_back(
                primeOrComplex0[i] *
                primeOrComplex1[i] *
                primeOrComplex2[i]);
        break;
    case 4:
        for (int i = 0; i < range; i++)
            primeOrComplex.push_back(
                primeOrComplex0[i] *
                primeOrComplex1[i] *
                primeOrComplex2[i] *
                primeOrComplex3[i]);
        break;
    case 5:
        for (int i = 0; i < range; i++)
            primeOrComplex.push_back(
                primeOrComplex0[i] *
                primeOrComplex1[i] *
                primeOrComplex2[i] *
                primeOrComplex3[i] *
                primeOrComplex4[i]);
        break;
    case 6:
        for (int i = 0; i < range; i++)
            primeOrComplex.push_back(
                primeOrComplex0[i] *
                primeOrComplex1[i] *
                primeOrComplex2[i] *
                primeOrComplex3[i] *
                primeOrComplex4[i] *
                primeOrComplex5[i]);
        break;
    case 7:
        for (int i = 0; i < range; i++)
            primeOrComplex.push_back(
                primeOrComplex0[i] *
                primeOrComplex1[i] *
                primeOrComplex2[i] *
                primeOrComplex3[i] *
                primeOrComplex4[i] *
                primeOrComplex5[i] *
                primeOrComplex6[i]);
        break;
    case 8:
        for (int i = 0; i < range; i++)
            primeOrComplex.push_back(
                primeOrComplex0[i] *
                primeOrComplex1[i] *
                primeOrComplex2[i] *
                primeOrComplex3[i] *
                primeOrComplex4[i] *
                primeOrComplex5[i] *
                primeOrComplex6[i] *
                primeOrComplex7[i]);
        break;
    }

    std::vector <int> primeNumbers;
    for (int i = minNum - 2; i < primeOrComplex.size(); i++)
    {
        if (primeOrComplex[i] == PRIME)
            primeNumbers.push_back(i + 2);
    }

    return primeNumbers;
}

//////////////////////////////////////////////////////////////////

int main()
{
    std::vector <int> tmp = parallelDomain(2, 300000000); //7.191
    //printVector(tmp);
}
