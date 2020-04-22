# Przetwarzanie Rownolegle
## Deadline - 27.04

### Wersje algorytmów:

#### 1. [DONE] Algorytm sekwencyjny:
testowanie każdej liczby z przedziału i sprawdzanie czy jest podzielna przez liczby mniejsze. 
Przedział można ograniczyć do: < pierwsza_liczba_w_kolejnosci, pierwiastek_kwadratowy_z_badanej_liczby>.
##### !!
w treści " Jako potencjalne dzielniki można wykorzystywać liczby pierwsze (jeśli są dostępne ... )" -> rozumiem to tak, że po sprawdzeniu pierwszej liczby jakieś liczby pierwsze już wyliczyliśmy i możemy je zapisywać w pamięci i później z nich skorzystać (??)

#### 2. [DONE] Algorytm sekwencyjny: 
metoda Sita (usuwanie ze zbioru liczb złożonych, liczby które pozostaną w zbiorze, są liczbami pierwszymi). Aby wyznaczyć (a później usunąć) liczby złożone wyznaczamy wielokrotności liczb. Np. wielkorotności liczby 2 - [ 2, 4, 6, 8, ...], liczby 3 - [3, 6, 9, 12, ...] itd. (wielokrotności liczb pierwszych)
##### !!
#### 2.1 [TODO] Algorytm sekwencyjny: 
metoda Sita (usuwanie ze zbioru (<M..N>) liczb złożonych, liczby które pozostaną w zbiorze, są liczbami pierwszymi). Aby wyznaczyć (a później usunąć) liczby złożone wyznaczamy wielokrotności liczb pierwszych z przedziału <2,..,pierwiastek(N)>

#### 3. [DONE] Algorytm równoległy:
metoda Sita - podejście domenowe. Zbiór liczb dzielony jest pomiędzy wątki, każdy z nich wykonuje to samo zadanie - sprawdza które z tych liczb należy wykreślić (są złożone). 

#### 4. [DONE] Algorytm równoległy:
metoda Sita - podejście funkcyjne. Zbiór liczb jest jeden - wspólny. Każdy z wątków sprawda wszystkie liczby używając przydzielonego mu wcześniej dzielnika (i jego wielokrotności).

#### 3&4 Uwagi:
Równoważenie pracy procesorów poprzez (wybrór przy pomocy eksperymentów obliczeniowych):
- odpowiednią liczbę wątków
- odpowiedni sposób przydziału pracy dla wątków:
* wskazanie pracy dla dowolonego wątku (dyrektywy: single, sections, section)
* podział pracy w pętli for (wariant statyczny blokowy, statyczny cykliczny, statyczny dynamiczny)

### Wersje kodu
Używane wersje algorytmów zapisywać jako wersje kodu do konkretnego zadania (próby ulepszenia wyników). Powinny sprawdzać: 
- poprawność wyników
- czas przetwarzania
- pomiar zrównoważenia pracy pomiędzy procesami
Kontrolować zależności pomiędzy numerem eksperymentu, trybem realizacji oraz testowanym kodem.
##### !! Wszystkie kody źródłowe używane w eksperymentach umieścić w sprawozdaniu w części wstępnej (także scharakteryzować: użyte podejście, użyte struktury, dostęp do danych współdzielenie, podział pracy, dominujące operacje arytmetyczne)

### [TODO]
1. Upewnić się, że pojęcia 'domenowe' i 'funkcyjne' nie powinnym być odwrotnie przypasowane do algorytmów

### [DONE] Do 21.04: 
- Beata: (2) Algorytm sekwencyjny - metoda Sita oraz (3) Algorytm równoległy - metoda Sita - podejście domenowe
- Piotr: (1) Algorytm sekwencyjny - testowanie każdej liczby ... oraz (4) Algorytm równoległy - metoda Sita - podejście funkcyjne


