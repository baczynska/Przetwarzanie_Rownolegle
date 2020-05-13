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
        if(i % 10 == 9) //co 10-ta liczba
            printf("\n");
    }
    printf("\nprime numbers count: %d\n", vec.size());
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

std::vector < std::vector <bool> > createPOCD(std::vector < std::vector <int> > subsets)
{
    std::vector < std::vector <bool> > result;

    for (int i = 0; i < threadsNum; i++)
    {
        int lowerSubsetLimit = subsets[i][0];
        int upperSubsetLimit = subsets[i][1];

        std::vector<bool> primeOrComplex;
        for (int i = lowerSubsetLimit; i <= upperSubsetLimit; i++)
            primeOrComplex.push_back(PRIME);

        result.push_back(primeOrComplex);
    }

    return result;
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
    std::vector < std::vector <int> > subsets = createSubsets(minNum, maxNum, threadsNum);
    std::vector < std::vector <bool> > primeOrComplexDivided = createPOCD(subsets);

#pragma omp		parallel num_threads(threadsNum)
    {
        int threadNumber = omp_get_thread_num();
        std::vector <int> threadSubset = subsets[threadNumber];
        int lowerSubsetLimit = threadSubset[0];
        int upperSubsetLimit = threadSubset[1];
        int lastSubsetNumber = (int)sqrt(upperSubsetLimit);

        for (int divider = 2; divider <= lastSubsetNumber; divider++)
        {
            //znajdujemy pierwsza wielokrotnosc tej liczby w przedziale watku
            int multiple = lowerSubsetLimit;
            for (; multiple % divider != 0; multiple++)
                continue;
            if (multiple == divider)
                multiple = divider + divider;

            for (; multiple <= upperSubsetLimit; multiple += divider) //dodajemy wszystkie wielokrotnoœci
                primeOrComplexDivided[threadNumber][multiple - lowerSubsetLimit] = COMPLEX;
        }
    }

    std::vector <bool> primeOrComplex;
    primeOrComplex.reserve(maxNum - minNum); 
    for(int i = 0; i < primeOrComplexDivided.size(); i++)
        primeOrComplex.insert(primeOrComplex.end(), primeOrComplexDivided[i].begin(), primeOrComplexDivided[i].end());


    std::vector <int> primeNumbers;
    for (int i = minNum - 2; i < primeOrComplex.size(); i++)
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
    std::vector <int> tmp = parallelDomain2(2, 100);
    printVector(tmp);
}