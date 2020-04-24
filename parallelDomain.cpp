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

//////////////////////////////////////////////////////////////////

/*
    1. Podziel pomiêdzy w¹tki zbiór podany na wejœcie algorytmu (na podzbiory):
        2. Wykonuj dla ka¿dej liczby ze zbioru (2 ... sqrt(N)):
            1. Jeœli liczba jest ju¿ w zbiorze liczb z³o¿onych, weŸ nastêpn¹ liczbê
            2. Dodaj wszystkie wielokrotnoœci tej liczby znajduj¹ce siê w podzbiorze w¹tku
               do GLOBALNEGO zbioru liczb z³o¿onych(atomowo)
            3. Jeœli wszystkie liczby z przedzia³u okaza³y siê byæ z³o¿one zakoñcz iteracjê
    2. Wszystkie liczby z interesuj¹cego nas przedzia³u, których nie ma w
       zbiorze liczb z³o¿onych, s¹ liczbami pierwszymi

    -Koniecznoœæ atomowego dodawania liczb do zbioru
    +Nie potrzeba ¿adnego dodatkowego zbioru z liczbami pierwszymi
    +/-Przewa¿nie dodajemy do zbioru tylko wielokrotnoœci liczb pierwszych
     (a nie np wielokrotnoœci czwórki: 4, 8, 12, 16)
*/
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

/*
    1. Podziel pomiêdzy w¹tki zbiór podany na wejœcie algorytmu (na podzbiory):
        1. Wykonuj dla ka¿dej liczby ze zbioru (2 ... sqrt(N)):
            1. Dodaj wszystkie wielokrotnoœci tej liczby znajduj¹ce siê w podzbiorze w¹tku
               do LOKALNEGO zbioru liczb z³o¿onych w¹tku
            2. Jeœli wszystkie liczby z przedzia³u okaza³y siê byæ z³o¿one zakoñcz iteracjê
    3. Po³¹cz wszystkie lokalne zbiory liczb z³o¿onych w jeden globalny
    4. Wszystkie liczby z interesuj¹cego nas przedzia³u, których nie ma w
       zbiorze liczb z³o¿onych, s¹ liczbami pierwszymi

    +Brak koniecznoœci atomowego dodawania liczb do zbioru
    +Nie potrzeba ¿adnego dodatkowego zbioru z liczbami pierwszymi
    -Dodajemy do zbioru tak¿e wielokrotnoœci liczb z³o¿onych
     (np wielokrotnoœci czwórki: 4, 8, 12, 16)
*/
std::vector<int> parallelDomain2(int minNum, int maxNum)
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

        std::vector<bool> localPrimeOrComplex;
        for (int i = lowerSubsetLimit; i <= upperSubsetLimit; i++)
            localPrimeOrComplex.push_back(PRIME);

        for (int divider = 2; divider <= lastSubsetNumber; divider++)
        {
            //znajdujemy pierwsza wielokrotnosc tej liczby w przedziale watku
            int multiple = lowerSubsetLimit;
            for (; multiple % divider != 0; multiple++)
                continue;
            if (multiple == divider)
                multiple = divider + divider;

            for (; multiple <= upperSubsetLimit; multiple += divider) //dodajemy wszystkie wielokrotnoœci
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

/*
    1. ZnajdŸ wszystkie liczby pierwsze <= sqrt(upperLimit)
    2. Podziel pomiêdzy w¹tki zbiór podany na wejœcie algorytmu (na podzbiory):
        1. Wykonuj dla ka¿dej liczby ze zbioru liczb pierwszych:
            1. Dodaj wszystkie wielokrotnoœci tej liczby znajduj¹ce siê w podzbiorze w¹tku
               do LOKALNEGO zbioru liczb z³o¿onych w¹tku
            2. Jeœli wszystkie liczby z przedzia³u okaza³y siê byæ z³o¿one zakoñcz iteracjê
    3. Po³¹cz wszystkie lokalne zbiory liczb z³o¿onych w jeden globalny
    4. Wszystkie liczby z interesuj¹cego nas przedzia³u, których nie ma w
       zbiorze liczb z³o¿onych, s¹ liczbami pierwszymi

    +Brak koniecznoœci atomowego dodawania liczb do zbioru
    -Potrzeba wygenerowania zbioru liczb pierwszych 
     przed wykonaniem algorytmu
    +Dodajemy do zbioru tylko wielokrotnoœci liczb pierwszych
     (a nie np wielokrotnoœci czwórki: 4, 8, 12, 16)
*/
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
    //std::vector <int> tmp = parallelDomain1(20, 100);
    //std::vector <int> tmp = parallelDomain2(20, 100);
    std::vector <int> tmp = parallelDomain3(2, 3000000);
    //printVector(tmp);
}