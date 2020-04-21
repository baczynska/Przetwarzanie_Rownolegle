// ConsoleApplication2.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

std::vector<int> seqSieve(int minNum, int maxNum) {

    printf("\nDla zakresu\n");
    std::cout << minNum << " **** " << maxNum << " ";

    int lastNum = (int)sqrt(maxNum);
    int arrSize = maxNum - minNum + 1;

    std::vector<int> vectorComplex;
    std::vector<int> vectorPrime;
    
    for (int i = 2; i <= lastNum; i++) {
        if (std::count(vectorComplex.begin(), vectorComplex.end(), i)) {
            // już wiemy, że to liczba złożona
        }
        else {
            // jest to liczba pierwsza
            if(i>= minNum)
                vectorPrime.push_back(i);
            for (int m = (i + 1); m <= maxNum; m++) {
                if ((m % i == 0) && (m>=minNum)) {
                    // wiemy, że jest to liczba złożona podzielna przez naszą liczbę pirwszą i
                    if ((std::count(vectorComplex.begin(), vectorComplex.end(), m)) == false) {
                        vectorComplex.push_back(m);
                    }
                }
            }
        }
        
    }

    for (int i = (lastNum + 1); i <= maxNum; i++) {
        if (((std::count(vectorComplex.begin(), vectorComplex.end(), i)) == false) && i>=minNum) {
            vectorPrime.push_back(i);
        }
    }

    printf("Complex\n");
    for (int x : vectorComplex)
        std::cout << x << " ";

    printf("\nPrime\n");
    for (int x : vectorPrime)
        std::cout << x << " ";

    return vectorPrime;
}

std::vector<int> parSieveDomain(int minNum, int maxNum) {

    
    int numbers = maxNum - minNum + 1;
    std::vector<int> primes;
    

#pragma omp parallel
    {
        int numThreads = omp_get_num_threads();
        int range = (int)numbers / numThreads;
        
        int threadNum = omp_get_thread_num();
        int firstOne, lastOne;

        firstOne = minNum + threadNum * range;
        lastOne = firstOne + (range - 1);

        printf("\n\nNumer watku\n");
        std::cout << threadNum << ' ';

        std::vector<int> myPrimes = seqSieve(minNum + (threadNum * range), (minNum + (threadNum * range) + (range - 1)));
    
        primes.insert(std::end(primes), std::begin(myPrimes), std::end(myPrimes));

    }

    

    return primes;
}

int main()
{
    //std::vector<int> finalPrimes = seqSieve(11, 18);

    std::vector<int> finalPrimes = parSieveDomain(3, 18);

    printf("\n\nFinal Primes\n");
    for (int x : finalPrimes)
        std::cout << x << " ";


    printf("\nGoodbye World\n");
}

// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln
