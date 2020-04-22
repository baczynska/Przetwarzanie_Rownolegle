#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "omp.h"

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
    1. Podziel pomiêdzy w¹tki nastêpuj¹ce iteracje:
            1. WeŸ kolejn¹ liczbê z zakresu (2 ... sqrt(N))
            2. Jeœli jest ju¿ w zbiorze liczb z³o¿onych, to koniec iteracji
            3. Dodaj wszystkie interesuj¹ce nas wielokrotnoœci tej liczby
               do GLOBALNEGO zbioru liczb z³o¿onych (atomowo)
        2. Wszystkie liczby z interesuj¹cego nas przedzia³u, których nie ma w
           zbiorze liczb z³o¿onych, s¹ liczbami pierwszymi

        -Koniecznoœæ atomowego dodawania liczb do zbioru
        +Nie potrzeba ¿adnego dodatkowego zbioru z liczbami pierwszymi
        +/- Przewa¿nie dodajemy do zbioru wielokrotnoœci liczb pierwszych
         (a nie np wielokrotnoœci czwórki: 4, 8, 12, 16)
*/
std::vector<int> parallelFunctional1(int minNum, int maxNum)
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

    return vectorPrime;
}

//////////////////////////////////////////////////////////////////

/*
    1. Podziel pomiêdzy w¹tki nastêpuj¹ce iteracje:
        1. WeŸ kolejn¹ liczbê z zakresu (2 ... sqrt(N))
        2. Dodaj wszystkie interesuj¹ce nas wielokrotnoœci tej liczby 
           do LOKALNEGO zbioru liczb z³o¿onych w¹tku
    3. Po³¹cz wszystkie lokalne zbiory liczb z³o¿onych w jeden globalny
    4. Wszystkie liczby z interesuj¹cego nas przedzia³u, których nie ma w
       zbiorze liczb z³o¿onych, s¹ liczbami pierwszymi

    +Brak koniecznoœci atomowego dodawania liczb do zbioru
    +Nie potrzeba ¿adnego dodatkowego zbioru z liczbami pierwszymi
    -Dodajemy do zbioru tak¿e wielokrotnoœci liczb z³o¿onych
     (np wielokrotnoœci czwórki: 4, 8, 12, 16)
*/
std::vector<int> parallelFunctional2(int minNum, int maxNum)
{
    int lastNum = (int)sqrt(maxNum);
    std::vector <int> globalComplex;

#pragma omp		parallel num_threads(4)
    {
        std::vector<int> localComplex;

#pragma omp		for
        for (int i = 2; i <= lastNum; i++) {
            if (std::count(localComplex.begin(), localComplex.end(), i)) //ju¿ wiemy, ¿e to liczba z³o¿ona
                break;

            //jest to liczba pierwsza
            for (int m = i + i; m <= maxNum; m += i) //dodajemy wszystkie wielokrotnoœci
                if ((std::count(localComplex.begin(), localComplex.end(), m)) == false) //nie ma jej jeszcze w vectorComplex
                    localComplex.push_back(m);
        }

        //³¹czenie lokalnych zbiorów liczb z³o¿onych w jeden globalny
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
    1. ZnajdŸ wszystkie liczby pierwsze <= sqrt(upperLimit)
    2. Podziel pomiêdzy w¹tki nastêpuj¹ce iteracje:
        1. WeŸ kolejn¹ liczbê ze zbioru liczb pierwszych
        2. Dodaj wszystkie interesuj¹ce nas wielokrotnoœci tej liczby 
           do LOKALNEGO zbioru liczb z³o¿onych w¹tku
    3. Po³¹cz wszystkie lokalne zbiory liczb z³o¿onych w jeden globalny
    4. Wszystkie liczby z interesuj¹cego nas przedzia³u, których nie ma w
       zbiorze liczb z³o¿onych, s¹ liczbami pierwszymi

    +Brak koniecznoœci atomowego dodawania liczb do zbioru
    -Potrzeba wygenerowania zbioru liczb pierwszych 
     przed wykonaniem algorytmu
    +Dodajemy do zbioru tylko wielokrotnoœci liczb pierwszych
     (a nie np wielokrotnoœci czwórki: 4, 8, 12, 16)
*/
std::vector<int> parallelFunctional3(int minNum, int maxNum)
{
    int lastNum = (int)sqrt(maxNum);
    std::vector<int> globalComplex;

    std::vector <int> startingPrimes = parallelFunctional1(2, lastNum);
#pragma omp		parallel num_threads(4)
    {
        std::vector<int> localComplex;
#pragma omp		for
        for (int i = 0; i < startingPrimes.size(); i++) 
        {
            int primeNumber = startingPrimes[i];
            for (int m = primeNumber + primeNumber; m <= maxNum; m += primeNumber) //dodajemy wszystkie wielokrotnoœci
                if ((std::count(localComplex.begin(), localComplex.end(), m)) == false) //nie ma jej jeszcze w vectorComplex
                    localComplex.push_back(m);
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
    //std::vector <int> tmp = parallelFunctional1(20, 100);
    //std::vector <int> tmp = parallelFunctional2(20, 100);
    std::vector <int> tmp = parallelFunctional3(20, 100);
    printVector(tmp);
}