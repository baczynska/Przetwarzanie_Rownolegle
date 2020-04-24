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

/*
    1. Podziel pomi�dzy w�tki nast�puj�ce iteracje:
            1. We� kolejn� liczb� z zakresu (2 ... sqrt(N))
            2. Je�li jest ju� w zbiorze liczb z�o�onych, to koniec iteracji
            3. Dodaj wszystkie interesuj�ce nas wielokrotno�ci tej liczby
               do GLOBALNEGO zbioru liczb z�o�onych (atomowo)
        2. Wszystkie liczby z interesuj�cego nas przedzia�u, kt�rych nie ma w
           zbiorze liczb z�o�onych, s� liczbami pierwszymi

        -Konieczno�� atomowego dodawania liczb do zbioru
        +Nie potrzeba �adnego dodatkowego zbioru z liczbami pierwszymi
        +/- Przewa�nie dodajemy do zbioru wielokrotno�ci liczb pierwszych
         (a nie np wielokrotno�ci czw�rki: 4, 8, 12, 16)
*/
std::vector<int> parallelFunctional1(int minNum, int maxNum)
{
    int lastNum = (int)sqrt(maxNum);
    std::vector<bool> primeOrComplex;
    for (int i = 2; i <= maxNum; i++)
        primeOrComplex.push_back(PRIME);

#pragma omp		parallel num_threads(threadsNum)
    {
#pragma omp		for
        for (int divider = 2; divider <= lastNum; divider++)
        {
            if (primeOrComplex[divider - 2] == COMPLEX)
                continue;

            for (int multiple = divider + divider; multiple <= maxNum; multiple += divider) //usuwamy wszystkie wielokrotno�ci
            {
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
    1. Podziel pomi�dzy w�tki nast�puj�ce iteracje:
        1. We� kolejn� liczb� z zakresu (2 ... sqrt(N))
        2. Dodaj wszystkie interesuj�ce nas wielokrotno�ci tej liczby 
           do LOKALNEGO zbioru liczb z�o�onych w�tku
    3. Po��cz wszystkie lokalne zbiory liczb z�o�onych w jeden globalny
    4. Wszystkie liczby z interesuj�cego nas przedzia�u, kt�rych nie ma w
       zbiorze liczb z�o�onych, s� liczbami pierwszymi

    +Brak konieczno�ci atomowego dodawania liczb do zbioru
    +Nie potrzeba �adnego dodatkowego zbioru z liczbami pierwszymi
    -Dodajemy do zbioru tak�e wielokrotno�ci liczb z�o�onych
     (np wielokrotno�ci czw�rki: 4, 8, 12, 16)
*/
std::vector<int> parallelFunctional2(int minNum, int maxNum)
{
    int lastNum = (int)sqrt(maxNum);
    std::vector<bool> primeOrComplex;
    for (int i = 2; i <= maxNum; i++)
        primeOrComplex.push_back(PRIME);

#pragma omp		parallel num_threads(threadsNum)
    {
        std::vector<int> localComplex;

#pragma omp		for
        for (int divider = 2; divider <= lastNum; divider++)
        {
            //jest to liczba pierwsza
            for (int multiple = divider + divider; multiple <= maxNum; multiple += divider) //dodajemy wszystkie wielokrotno�ci
                localComplex.push_back(multiple);
        }

        //��czenie lokalnych zbior�w liczb z�o�onych w jeden globalny
        for (int i = 0; i < localComplex.size(); i++)
        {
            int numberIndex = localComplex[i] - 2;
#pragma omp critical
            primeOrComplex[numberIndex] = COMPLEX;
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
    1. Znajd� wszystkie liczby pierwsze <= sqrt(upperLimit)
    2. Podziel pomi�dzy w�tki nast�puj�ce iteracje:
        1. We� kolejn� liczb� ze zbioru liczb pierwszych
        2. Dodaj wszystkie interesuj�ce nas wielokrotno�ci tej liczby 
           do LOKALNEGO zbioru liczb z�o�onych w�tku
    3. Po��cz wszystkie lokalne zbiory liczb z�o�onych w jeden globalny
    4. Wszystkie liczby z interesuj�cego nas przedzia�u, kt�rych nie ma w
       zbiorze liczb z�o�onych, s� liczbami pierwszymi

    +Brak konieczno�ci atomowego dodawania liczb do zbioru
    -Potrzeba wygenerowania zbioru liczb pierwszych 
     przed wykonaniem algorytmu
    +Dodajemy do zbioru tylko wielokrotno�ci liczb pierwszych
     (a nie np wielokrotno�ci czw�rki: 4, 8, 12, 16)
*/
std::vector<int> parallelFunctional3(int minNum, int maxNum)
{
    int lastNum = (int)sqrt(maxNum);
    std::vector<bool> primeOrComplex;
    for (int i = 2; i <= maxNum; i++)
        primeOrComplex.push_back(PRIME);

    std::vector <int> startingPrimes = parallelFunctional1(2, lastNum);
#pragma omp		parallel num_threads(threadsNum)
    {
        std::vector<int> localComplex;
#pragma omp		for
        for (int i = 0; i < startingPrimes.size(); i++) 
        {
            int primeNumber = startingPrimes[i];
            for (int multiple = primeNumber + primeNumber; multiple <= maxNum; multiple += primeNumber) //dodajemy wszystkie wielokrotno�ci
                localComplex.push_back(multiple);
        }

        //��czenie lokalnych zbior�w liczb z�o�onych w jeden globalny
        for (int i = 0; i < localComplex.size(); i++)
        {
            int numberIndex = localComplex[i] - 2;
#pragma omp critical
            primeOrComplex[numberIndex] = COMPLEX;
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
    //std::vector <int> tmp = parallelFunctional1(20, 100);
    //std::vector <int> tmp = parallelFunctional2(20, 100);
    std::vector <int> tmp = parallelFunctional3(20, 100);
    printVector(tmp);
}