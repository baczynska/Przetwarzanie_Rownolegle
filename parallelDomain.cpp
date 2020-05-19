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
        //printf("%d ", vec[i]);
        std::cout << vec[i] << " ";
        if (i % 10 == 9) //co 10-ta liczba
            //printf("\n");
            std::cout << std::endl;
    }
    //printf("\nprime numbers count: %d\n", vec.size());
    std::cout << std::endl << "prime numbers count: " << vec.size() << std::endl;
}

std::vector < std::vector <int> > createSubsets(int lowerLimit, int upperLimit, int subsetsNumber) 
{
    int range = (upperLimit - lowerLimit) / subsetsNumber;
    std::vector < std::vector <int> > subsets;
    std::vector <int> singleSubset;

    int nextNumber = lowerLimit;
    for (int i = 0; i < subsetsNumber - 1; i++) 
    {
        singleSubset = { nextNumber, nextNumber + range - 1};
        subsets.push_back(singleSubset);
        nextNumber = nextNumber + range;
    }
    singleSubset = { nextNumber, upperLimit };
    subsets.push_back(singleSubset);

    return subsets;
}

//////////////////////////////////////////////////////////////////

std::vector<int> parallelDomain1(int minNum, int maxNum)
{
    std::vector<bool> primeOrComplex;
    for (int i = 2; i <= maxNum; i++)
        primeOrComplex.push_back(PRIME);
    std::vector < std::vector <int> > subsets = createSubsets(minNum, maxNum, threadsNum);

#pragma omp		parallel num_threads(threadsNum)
    {
        int threadNumber = omp_get_thread_num();
        std::vector <int> threadSubset = subsets[threadNumber];
        int lowerSubsetLimit = threadSubset[0];
        int upperSubsetLimit = threadSubset[1];
        int lastSubsetNumber = (int)sqrt(upperSubsetLimit);

        for (int divider = 2; divider <= lastSubsetNumber; divider++)
        {
            if (primeOrComplex[divider - 2] == COMPLEX)
                continue;

            //znajdujemy pierwsza wielokrotnosc tej liczby w przedziale watku
            int multiple = lowerSubsetLimit;
            for (; multiple % divider != 0; multiple++)
                continue;
            if (multiple == divider)
                multiple = divider + divider;
            
            for (; multiple <= upperSubsetLimit; multiple += divider) //dodajemy wszystkie wielokrotnoœci
            {
                if (primeOrComplex[multiple - 2] == PRIME)
#pragma omp critical
                    primeOrComplex[multiple - 2] = COMPLEX;
            }
        }
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

std::vector<int> parallelDomain2(int minNum, int maxNum)
{
    //CREATE GLOBAL SUBSETS//
    std::vector < std::vector <int> > subsets = createSubsets(minNum, maxNum, threadsNum);

    std::vector <bool> subset0;
    std::vector <bool> subset1;
    std::vector <bool> subset2;
    std::vector <bool> subset3;
    std::vector <bool> subset4;
    std::vector <bool> subset5;
    std::vector <bool> subset6;
    std::vector <bool> subset7;

#pragma omp		parallel num_threads(threadsNum)
    {
        int threadNumber = omp_get_thread_num();
        std::vector <int> threadSubset = subsets[threadNumber];
        int lowerSubsetLimit = threadSubset[0];
        int upperSubsetLimit = threadSubset[1];
        int subsetRange = upperSubsetLimit - lowerSubsetLimit + 1;
        int lastSubsetNumber = (int)sqrt(upperSubsetLimit);
        std::vector <bool> subset(subsetRange, PRIME);
        
        for (int divider = 2; divider <= lastSubsetNumber; divider++)
        {
            //znajdujemy pierwsza wielokrotnosc tej liczby w przedziale watku
            int multiple = lowerSubsetLimit;
            for (; multiple % divider != 0; multiple++)
                continue;
            if (multiple == divider)
                multiple = divider + divider;

            for (; multiple <= upperSubsetLimit; multiple += divider) //dodajemy wszystkie wielokrotnoœci
                subset[multiple - lowerSubsetLimit] = COMPLEX;
        }

        switch (threadNumber)
        {
        case 0:
            subset0 = subset;
            break;
        case 1:
            subset1 = subset;
            break;
        case 2:
            subset2 = subset;
            break;
        case 3:
            subset3 = subset;
            break;
        case 4:
            subset4 = subset;
            break;
        case 5:
            subset5 = subset;
            break;
        case 6:
            subset6 = subset;
            break;
        case 7:
            subset7 = subset;
            break;
        }
    }

    std::vector <bool> primeOrComplex;
    primeOrComplex.reserve(maxNum - minNum); 

    if (threadsNum == 0)
        goto primeOrComplexCreated;
    primeOrComplex.insert(primeOrComplex.end(), subset0.begin(), subset0.end());

    if (threadsNum == 1)
        goto primeOrComplexCreated;
    primeOrComplex.insert(primeOrComplex.end(), subset1.begin(), subset1.end());

    if (threadsNum == 2)
        goto primeOrComplexCreated;
    primeOrComplex.insert(primeOrComplex.end(), subset2.begin(), subset2.end());

    if (threadsNum == 3)
        goto primeOrComplexCreated;
    primeOrComplex.insert(primeOrComplex.end(), subset3.begin(), subset3.end());

    if (threadsNum == 4)
        goto primeOrComplexCreated;
    primeOrComplex.insert(primeOrComplex.end(), subset4.begin(), subset4.end());

    if (threadsNum == 5)
        goto primeOrComplexCreated;
    primeOrComplex.insert(primeOrComplex.end(), subset5.begin(), subset5.end());

    if (threadsNum == 6)
        goto primeOrComplexCreated;
    primeOrComplex.insert(primeOrComplex.end(), subset6.begin(), subset6.end());

    if (threadsNum == 7)
        goto primeOrComplexCreated;
    primeOrComplex.insert(primeOrComplex.end(), subset7.begin(), subset7.end());

primeOrComplexCreated:  

    std::vector <int> primeNumbers;
    for (int i = 0; i < primeOrComplex.size(); i++)
    {
        if (primeOrComplex[i] == PRIME)
            primeNumbers.push_back(i + 2);
    }

    return primeNumbers;
}

//////////////////////////////////////////////////////////////////

std::vector<int> parallelDomain3(int minNum, int maxNum)
{
    std::vector<bool> primeOrComplex;
    for (int i = 2; i <= maxNum; i++)
        primeOrComplex.push_back(PRIME);
    std::vector < std::vector <int> > subsets = createSubsets(minNum, maxNum, threadsNum);

    int lastNum = (int)sqrt(maxNum);
    std::vector <int> startingPrimes = parallelDomain1(2, lastNum);

#pragma omp		parallel num_threads(threadsNum)
    {
        int threadNumber = omp_get_thread_num();
        std::vector <int> threadSubset = subsets[threadNumber];
        int lowerSubsetLimit = threadSubset[0];
        int upperSubsetLimit = threadSubset[1];
        int lastSubsetNumber = (int)sqrt(upperSubsetLimit);

        std::vector<bool> localPrimeOrComplex;
        for (int i = lowerSubsetLimit; i <= upperSubsetLimit; i++)
            localPrimeOrComplex.push_back(PRIME);

        for (int i = 0; i < startingPrimes.size(); i++) 
        {
            int primeNumber = startingPrimes[i];

            //znajdujemy pierwsza wielokrotnosc tej liczby w przedziale watku
            int multiple = lowerSubsetLimit;
            for (; multiple % primeNumber != 0; multiple++)
                continue;
            if (multiple == primeNumber)
                multiple = primeNumber + primeNumber;

            for (; multiple <= upperSubsetLimit; multiple += primeNumber) //dodajemy wszystkie wielokrotnoœci
                localPrimeOrComplex[multiple - lowerSubsetLimit] = COMPLEX;
        }

        //³¹czenie lokalnych zbiorów liczb z³o¿onych w jeden globalny
        for (int i = 0; i < localPrimeOrComplex.size(); i++)
        {
            if (localPrimeOrComplex[i] == COMPLEX)
            {
                int numberIndex = i + lowerSubsetLimit - 2;
#pragma omp critical
                primeOrComplex[numberIndex] = COMPLEX;
            }
        }
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
    std::vector <int> tmp = parallelDomain2(2, 300000000);
    //printVector(tmp);
}