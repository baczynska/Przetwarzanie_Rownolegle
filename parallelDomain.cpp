#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "omp.h"

#define threadsNum 4

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
    1. Podziel pomi�dzy w�tki zbi�r podany na wej�cie algorytmu (na podzbiory):
        2. Wykonuj dla ka�dej liczby ze zbioru (2 ... sqrt(N)):
            1. Je�li liczba jest ju� w zbiorze liczb z�o�onych, we� nast�pn� liczb�
            2. Dodaj wszystkie wielokrotno�ci tej liczby znajduj�ce si� w podzbiorze w�tku
               do GLOBALNEGO zbioru liczb z�o�onych(atomowo)
            3. Je�li wszystkie liczby z przedzia�u okaza�y si� by� z�o�one zako�cz iteracj�
    2. Wszystkie liczby z interesuj�cego nas przedzia�u, kt�rych nie ma w
       zbiorze liczb z�o�onych, s� liczbami pierwszymi

    -Konieczno�� atomowego dodawania liczb do zbioru
    +Nie potrzeba �adnego dodatkowego zbioru z liczbami pierwszymi
    +/-Przewa�nie dodajemy do zbioru tylko wielokrotno�ci liczb pierwszych
     (a nie np wielokrotno�ci czw�rki: 4, 8, 12, 16)
*/
std::vector<int> parallelDomain1(int minNum, int maxNum)
{
    std::vector<int> vectorComplex;
    std::vector < std::vector <int> > subsets = createSubsets(minNum, maxNum, threadsNum);

#pragma omp		parallel num_threads(threadsNum)
    {
        int threadNumber = omp_get_thread_num();
        std::vector <int> threadSubset = subsets[threadNumber];
        int lastNum = (int)sqrt(threadSubset[1]);
        int complexCounter = 0;
        int range = ( threadSubset[1] - threadSubset[0] ) + 1;

        for (int i = 2; i <= lastNum; i++) {
            if (std::count(vectorComplex.begin(), vectorComplex.end(), i)) //ju� wiemy, �e to liczba z�o�ona
                continue;
            //znajdujemy pierwsza wielokrotnosc tej liczby w przedziale watku
            int m = threadSubset[0];
            for (; m % i != 0; m++)
                continue;
            if (m == i)
                m = i + i;
            
            for (; m <= threadSubset[1]; m += i) //dodajemy wszystkie wielokrotno�ci
            {
#pragma omp critical
                if ((std::count(vectorComplex.begin(), vectorComplex.end(), m)) == false) //nie ma jej jeszcze w vectorComplex
                {
                    vectorComplex.push_back(m);
                    complexCounter++;
                    if (complexCounter == range) //dodali�my ju� wszystkie liczby z przedzialu
                    { 
                        i = lastNum + 1;
                        m = maxNum + 1;
                    }
                }
            }
        }
    }

    std::vector<int> vectorPrime;
    for (int i = minNum; i <= maxNum; i++)
        if (std::count(vectorComplex.begin(), vectorComplex.end(), i) == false) //liczby nie ma w vectorComplex
            vectorPrime.push_back(i);

    return vectorPrime;
}

//////////////////////////////////////////////////////////////////

/*
    1. Podziel pomi�dzy w�tki zbi�r podany na wej�cie algorytmu (na podzbiory):
        1. Wykonuj dla ka�dej liczby ze zbioru (2 ... sqrt(N)):
            1. Dodaj wszystkie wielokrotno�ci tej liczby znajduj�ce si� w podzbiorze w�tku
               do LOKALNEGO zbioru liczb z�o�onych w�tku
            2. Je�li wszystkie liczby z przedzia�u okaza�y si� by� z�o�one zako�cz iteracj�
    3. Po��cz wszystkie lokalne zbiory liczb z�o�onych w jeden globalny
    4. Wszystkie liczby z interesuj�cego nas przedzia�u, kt�rych nie ma w
       zbiorze liczb z�o�onych, s� liczbami pierwszymi

    +Brak konieczno�ci atomowego dodawania liczb do zbioru
    +Nie potrzeba �adnego dodatkowego zbioru z liczbami pierwszymi
    -Dodajemy do zbioru tak�e wielokrotno�ci liczb z�o�onych
     (np wielokrotno�ci czw�rki: 4, 8, 12, 16)
*/
std::vector<int> parallelDomain2(int minNum, int maxNum)
{
    std::vector<int> globalComplex;
    std::vector < std::vector <int> > subsets = createSubsets(minNum, maxNum, threadsNum);

#pragma omp		parallel num_threads(threadsNum)
    {
        std::vector<int> localComplex;
        int threadNumber = omp_get_thread_num();
        std::vector <int> threadSubset = subsets[threadNumber];
        int lastNum = (int)sqrt(threadSubset[1]);
        int complexCounter = 0;
        int range = (threadSubset[1] - threadSubset[0]) + 1;

        for (int i = 2; i <= lastNum; i++) {
            if (std::count(localComplex.begin(), localComplex.end(), i)) //ju� wiemy, �e to liczba z�o�ona
                continue;

            //znajdujemy pierwsza wielokrotnosc tej liczby w przedziale watku
            int m = threadSubset[0];
            for (; m % i != 0; m++)
                continue;
            if (m == i)
                m = i + i;

            for (; m <= threadSubset[1]; m += i) //dodajemy wszystkie wielokrotno�ci
            {
                if ((std::count(localComplex.begin(), localComplex.end(), m)) == false) //nie ma jej jeszcze w vectorComplex
                {
                    localComplex.push_back(m);
                    complexCounter++;
                    if (complexCounter == range) //dodali�my ju� wszystkie liczby z przedzialu
                    {
                        i = lastNum + 1;
                        m = maxNum + 1;
                    }
                }
            }
        }

        //��czenie lokalnych zbior�w liczb z�o�onych w jeden globalny
#pragma omp critical
        globalComplex.insert(globalComplex.end(), localComplex.begin(), localComplex.end());
    }

    std::vector<int> vectorPrime;
    for (int i = minNum; i <= maxNum; i++)
        if (std::count(globalComplex.begin(), globalComplex.end(), i) == false) //liczby nie ma w vectorComplex
            vectorPrime.push_back(i);

    return vectorPrime;
}

//////////////////////////////////////////////////////////////////

/*
    1. Znajd� wszystkie liczby pierwsze <= sqrt(upperLimit)
    2. Podziel pomi�dzy w�tki zbi�r podany na wej�cie algorytmu (na podzbiory):
        1. Wykonuj dla ka�dej liczby ze zbioru liczb pierwszych:
            1. Dodaj wszystkie wielokrotno�ci tej liczby znajduj�ce si� w podzbiorze w�tku
               do LOKALNEGO zbioru liczb z�o�onych w�tku
            2. Je�li wszystkie liczby z przedzia�u okaza�y si� by� z�o�one zako�cz iteracj�
    3. Po��cz wszystkie lokalne zbiory liczb z�o�onych w jeden globalny
    4. Wszystkie liczby z interesuj�cego nas przedzia�u, kt�rych nie ma w
       zbiorze liczb z�o�onych, s� liczbami pierwszymi

    +Brak konieczno�ci atomowego dodawania liczb do zbioru
    -Potrzeba wygenerowania zbioru liczb pierwszych 
     przed wykonaniem algorytmu
    +Dodajemy do zbioru tylko wielokrotno�ci liczb pierwszych
     (a nie np wielokrotno�ci czw�rki: 4, 8, 12, 16)
*/
std::vector<int> parallelDomain3(int minNum, int maxNum)
{
    int lastNum = (int)sqrt(maxNum);
    std::vector < std::vector <int> > subsets = createSubsets(minNum, maxNum, threadsNum);
    std::vector<int> globalComplex;

    std::vector <int> startingPrimes = parallelDomain1(2, lastNum);

#pragma omp		parallel num_threads(threadsNum)
    {
        std::vector<int> localComplex;
        int threadNumber = omp_get_thread_num();
        std::vector <int> threadSubset = subsets[threadNumber];
        int lastNum = (int)sqrt(threadSubset[1]);
        int complexCounter = 0;
        int range = (threadSubset[1] - threadSubset[0]) + 1;

        for (int i = 0; i < startingPrimes.size(); i++) {
            int primeNumber = startingPrimes[i];
            if (std::count(localComplex.begin(), localComplex.end(), primeNumber)) //ju� wiemy, �e to liczba z�o�ona
                continue;

            //znajdujemy pierwsza wielokrotnosc tej liczby w przedziale watku
            int m = threadSubset[0];
            for (; m % primeNumber != 0; m++)
                continue;
            if (m == primeNumber)
                m = primeNumber + primeNumber;

            for (; m <= threadSubset[1]; m += primeNumber) //dodajemy wszystkie wielokrotno�ci
            {
                if ((std::count(localComplex.begin(), localComplex.end(), m)) == false) //nie ma jej jeszcze w vectorComplex
                {
                    localComplex.push_back(m);
                    complexCounter++;
                    if (complexCounter == range) //dodali�my ju� wszystkie liczby z przedzialu
                    {
                        i = startingPrimes.size();
                        m = maxNum + 1;
                    }
                }
            }
        }

#pragma omp critical
        globalComplex.insert(globalComplex.end(), localComplex.begin(), localComplex.end());
    }

    std::vector<int> vectorPrime;
    for (int i = minNum; i <= maxNum; i++)
        if (std::count(globalComplex.begin(), globalComplex.end(), i) == false) //liczby nie ma w vectorComplex
            vectorPrime.push_back(i);

    return vectorPrime;
}

//////////////////////////////////////////////////////////////////

int main()
{
    std::vector <int> tmp = parallelDomain1(20, 100);
    //std::vector <int> tmp = parallelDomain2(20, 100);
    //std::vector <int> tmp = parallelDomain3(20, 100);
    printVector(tmp);
}