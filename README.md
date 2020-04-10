# Przetwarzanie Rownolegle

### Wersje algorytmów:

1. Algorytm sekwencyjny - testowanie każdej liczby z przedziału i sprawdzanie czy jest podzielna przez liczby mniejsze. 
Przedział można ograniczyć do: < pierwsza_liczba_w_kolejnosci, pierwiastek_kwadratowy_z_badanej_liczby>.

2. Algorytm sekwencyjny - metoda Sita (usuwanie ze zbioru liczb złożonych, liczby które pozostaną w zbiorze, są liczbami pierwszymi). Aby wyznaczyć (a później usunąć) liczby złożone wyznaczamy wielokrotności liczb. Np. wielkorotności liczby 2 - [ 2, 4, 6, 8, ...], liczby 3 - [3, 6, 9, 12, ...] itd. (wielokrotności liczb pierwszych)

3. Algorytm równoległy - metoda Sita - podejście domenowe. Zbiór liczb dzielony jest pomiędzy wątki, każdy z nich wykonuje to samo zadanie - sprawdza które z tych liczb należy wykreślić (są złożone). 

4. Algorytm równoległy - metoda Sita - podejście funkcyjne. Zbiór liczb jest jeden - wspólny. Każdy z wątków sprawda wszystkie liczby używając przydzielonego mu wcześniej dzielnika (i jego wielokrotności).

### Do 21.04: 
- Beata: (2) Algorytm sekwencyjny - metoda Sita oraz (3) Algorytm równoległy - metoda Sita - podejście domenowe
- Piotr: (1) Algorytm sekwencyjny - testowanie każdej liczby ... oraz (4) Algorytm równoległy - metoda Sita - podejście funkcyjne

## Deadline - 27.04
