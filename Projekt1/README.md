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

### Uruchomienie przetwarzania
Konfiguracja: Release, x86 </br>
Maksymalna długość przetwarzania (dla dowolnej liczby procesów i wątków): 1 minuta </br>
Zapis wyników w jednej strukturze </br>
Warianty bazujące na różnych podejściach koncepcyjnych ze względu na różne wydajności mogą być testowane dla różnych
instancji </br>
Także, by zweryfikować poprwność: </br>
- wyświetlenie liczb pierwszych na konsoli w wierszach po 10 
- wyświetla liczbę liczb pierwszych w badanym zakresie </br> </br>
Testowane instancje:
- 2..MAX
- MAX/2..MAX
- 2..MAX/2 </br> </br>
Liczba użytych procesorów
- Przetwarzanie sekwencyjne -> jeden procesor
- Przetwarzanie równoległe -> maksymalna liczba procesorów logicznych w systemie
- Przetwarzanie równoległe -> maksymalna liczba procesorów fizycznych w systemie (jeśli różna od liczby logicznych)
- Przetwarzanie równoległe -> połowa liczby procesorów fizycznych (jeśli liczba różna od użytych wcześniej)

### By zminimalizować koszty związane z synchronizacją:
- jedna dyrektywa tworząca wątki #pragma omp parallel
- obecność wyścigu usuwamy za pomocą klauzuli 'nowait'
- minimalizacja unieważnień linii pamięci podręcznej

### Problemy do podjęcia w trakcie przygotowywania kodu:
- lokalność dostępu do danych -> unikać unieważnienia linii pamięci podręcznej
- zrównoważenie obciążeń procesów -> uwzględnić czas wykonywania oraz rozrzut jego wartości (podejście równoległe, podział pracy statyczny oraz dynamiczny) 
- ocena jakości zrównoważenie (w sprawozdaniu)
- rodzaj wykonywanej operacji podstawowej (koncepcja 1 -> dzielenie, koncepcja 2 -> dodawanie)
##### !! (W sprawozdaniu) Określić parametry uruchomień, ocenić prędkość, przyspieszenie oraz wyjaścić przyczynę zaobserwowanych wartości

### Sprawozdanie
#### Punkt 1 
Opis wykorzystanego systemu obliczeniowego

#### Punkt 2
Prezentacja przygotowanych wariantów kodów z wyjaśnieniem przewidywanego przebiegu przetwarzania. Kluczowe elementy podane w sposób skrótowy. </br>
W opisie przebiegu przetwarzania należy uwzględnić: </br>
< już zawarto w docu >

### [TODO]
1. Upewnić się, że pojęcia 'domenowe' i 'funkcyjne' nie powinnym być odwrotnie przypasowane do algorytmów

### [DONE] Do 21.04: 
- Beata: (2) Algorytm sekwencyjny - metoda Sita oraz (3) Algorytm równoległy - metoda Sita - podejście domenowe
- Piotr: (1) Algorytm sekwencyjny - testowanie każdej liczby ... oraz (4) Algorytm równoległy - metoda Sita - podejście funkcyjne


