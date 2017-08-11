# Zadania - zestaw 2
Zapoznaj siê ze sk³adni¹ i operacjami wykonywanymi przez poni¿sze funkcje:

- funkcje operuj¹ce na plikach i katalogach: open, close, read, write, fcntl, stat, fstat, lstat, mkdir, rmdir, opendir, closedir, readdir, rewinddir, nftw, fopen, fclose, getc, putc,
- funkcje i zmienne do obs³ugi b³êdów: perror, errno.
## Zadanie 1. Porównanie wydajnoœci systemowych i bibliotecznych funkcji we/wy
Celem zadania jest napisanie programu porównuj¹cego wydajnoœæ systemowych i bibliotecznych funkcji wejœcia/wyjœcia. Program operowa³ bêdzie na przechowywanej w pliku tablicy rekordów. Pojedynczy rekord bêdzie tablic¹ bajtów o sta³ej wielkoœci. Nazwa pliku, wielkoœæ oraz iloœæ rekordów stanowiæ bêd¹ argumenty wywo³ania programu.

Program udostêpniaæ powinien 3 operacje:

- generate - tworzenie plik z rekordami wype³nionego losow¹ zawartoœci¹ wygenerowan¹ poprzez odczyt pliku /dev/random
- shuffle - losowo permutuje rekordy w pliku (mo¿na wykorzystaæ np. algorytm Fishera-Yates'a)
- sort - sortuje rekordy w pliku u¿ywaj¹c sortowania b¹belkowego. Kluczem do sortowania niech bêdzie wartoœæ pierwszego bajtu rekordu (interpretowanego jako liczba bez znaku - unsigned char)
Sortowanie i permutacja powinny byæ zaimplementowane w dwóch wariantach:

- sys - przy u¿yciu funkcji systemowych: read i write
- lib - przy u¿yciu funkcji biblioteki C: fread i fwrite
(do generowania danych wystarczy jedna wersja).

Rodzaj operacji oraz sposób dostêpu do plików ma byæ wybierany na podstawie argumentu wywo³ania - np. ./program sys shuffle datafile 100 512 powinno losowo permutowaæ dane w pliku datafile przy u¿yciu funkcji systemowych, zak³adaj¹c ¿e zawiera on 100 rekordów wielkoœci 512 bajtów.

Program powinien byæ zaimplementowany tak, by podczas permutowania i sortowania w pamiêci przechowywane by³y jednoczeœnie najwy¿ej dwa rekordy (obie operacje sprowadzaj¹ siê do zamieniania miejscami i/lub porównywania dwóch rekordów).

Dla obu wariantów implementacji przeprowadŸ pomiary czasu u¿ytkownika i czasu systemowego operacji sortowania i permutowania. Testy wykonaj dla nastêpuj¹cych rozmiarów rekordu: 4, 512, 4096 i 8192 bajty. Dla ka¿dego rozmiaru rekordu wykonaj dwa testy ró¿ni¹ce siê liczb¹ rekordów w sortowanym pliku. Liczby rekordów dobierz tak, by czas sortowania mieœci³ siê w przedziale od kilku do kilkudziesiêciu sekund. Porównuj¹c dwa warianty implementacji nale¿y korzystaæ z identycznego pliku do sortowania (po wygenerowaniu, a przed sortowaniem, utwórz jego kopiê). Zmierzone czasy sortowania zestaw w pliku wyniki.txt. Do pliku dodaj komentarz podsumowuj¹cy wnioski z testów.

## Zadanie 2. Operacje na strukturze katalogów
Napisz program przyjmuj¹cy dwa argumenty wywo³ania: œcie¿kê do katalogu w systemie plików, oraz liczbê ca³kowit¹ oznaczaj¹c¹ rozmiar pliku w bajtach. Po uruchomieniu program przeszukuje drzewo katalogów zakorzenione w œcie¿ce przekazanej w pierwszym argumencie i szuka w nim plików zwyczajnych o rozmiarze nie wiêkszym ni¿ wielkoœæ podana jako drugi argument. Dla ka¿dego znalezionego pliku wypisywane s¹ nastêpuj¹ce informacje:

- œcie¿ka bezwzglêdna pliku
- rozmiar w bajtach
- prawa dostêpu do pliku (w formacie u¿ywanym przez ls -l, np. r--r--rw- oznacza prawa odczytu dla wszystkich i zapisu dla w³aœciciela)
- datê ostatniej modyfikacji
Œcie¿ka podana jako argument wywo³ania mo¿e byæ wzglêdna lub bezwzglêdna. Program powinien wyszukiwaæ tylko pliki zwyczajne (bez dowi¹zañ, urz¹dzeñ blokowych, itp). Program nie powinien pod¹¿aæ za dowi¹zaniami symbolicznymi do katalogów.

Program nale¿y zaimplementowaæ w dwóch wariantach:

Korzystaj¹c z funkcji opendir, readdir oraz funkcji z rodziny stat
Korzystaj¹c z funkcji nftw

##Zadanie 3. Ryglowanie plików
Napisz program umo¿liwiaj¹cy w trybie interaktywnym (tekstowym) wykonanie nastêpuj¹cych operacji dla pliku bêd¹cego jego argumentem:

1. ustawienie rygla do odczytu na wybrany znak pliku,
2. ustawienie rygla do zapisu na wybrany znak pliku,
3. wyœwietlenie listy zaryglowanych znaków pliku (z informacj¹ o numerze PID procesu bêd¹cego w³aœcicielem rygla oraz jego typie - odczyt/zapis),
4. zwolnienie wybranego rygla,
5. odczyt (funkcj¹ read) wybranego znaku pliku,
6. zmiana (funkcj¹ write) wybranego znaku pliku.

Operacje 1 oraz 2 powinny byæ dostêpne w dwóch wariantach, ró¿ni¹cych siê zachowaniem w przypadku, gdy ustawienie rygla jest niemo¿liwe ze wzglêdu na rygle za³o¿one przez inne procesy:

- wersja nieblokuj¹ca - próba ustawienia rygla koñczy siê wypisaniem informacji o b³êdzie
- wersja blokuj¹ca - program czeka, a¿ ustawienie rygla bêdzie mo¿liwe

Wybór znaku we wszystkich przypadkach polega na podaniu numeru bajtu w pliku. Do ryglowania nale¿y wykorzystaæ funkcjê fcntl i strukturê flock. Wykonaj eksperymenty uruchamiaj¹c program jednoczeœnie w dwóch terminalach dla tego samego pliku. Próbuj ryglowaæ (do odczytu lub do zapisu) te same znaki pliku i modyfikowaæ (lub odczytywaæ) wybrane znaki pliku wykorzystuj¹c zarówno blokuj¹ce, jak i nieblokuj¹ce wersje operacji. Zwróæ uwagê, ¿e domyœlnie ryglowanie dzia³a w trybie rygli zalecanych (advisory), a nie rygli wymuszanych (mandatory).