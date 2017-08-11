# Zadanie - zestaw 3
# Tworzenie procesów. Œrodowisko procesu, sterowanie procesami.
## Zadanie 1. Prosty interpreter zadañ wsadowych
W ramach æwiczenia nale¿y napisaæ prosty interpreter zadañ wsadowych. Interpreter przyjmuje w argumencie nazwê pliku zawieraj¹cego zadanie wsadowe i wykonuje wszystkie polecenia z tego pliku. Plik z zadaniem wsadowym ma œciœle okreœlon¹ postaæ:

1. Linie zaczynaj¹ce siê znakiem # oznaczaj¹ pracê ze zmiennymi œrodowiskowymi. Ka¿da taka linia ma jedn¹ z dwóch postaci:

- \#NazwaZmiennej WartoscZmiennej
Oznacza definicjê zmiennej œrodowiskowej. Od momentu jej wyst¹pienia, w œrodowisku poleceñ wykonywanych przez interpreter powinna istnieæ zmienna NazwaZmiennej o wartoœci WartoscZmiennej. Jeœli taka zmienna istnieje ju¿ w œrodowisku, jej wartoœæ powinna zostaæ zmieniona na WartoscZmiennej.
Na przyk³ad, linia postaci:
\#TestFile ./ala.txt
powinna spowodowaæ dodanie do œrodowiska zmiennej TestFile o wartoœci ./ala.txt

Uwaga: zmienne œrodowiskowe, z którymi zosta³ uruchomiony interpreter powinny byæ dostêpne równie¿ dla wykonywanych poleceñ.

- #NazwaZmiennej
Oznacza usuniêcie zmiennej NazwaZmiennej ze œrodowiska. Polecenie powinno zakoñczyæ siê powodzeniem równie¿ wówczas, gdy w œrodowisku nie ma zmiennej NazwaZmiennej (wówczas œrodowisko siê nie zmienia).
Na przyk³ad, linia postaci:
#TestFile
powinna spowodowaæ usuniêcie ze œrodowiska zmiennej TestFile (jeœli taka zmienna jest w œrodowisku).

2. Linie zaczynaj¹c siê znakiem innym, ni¿ # oznaczaj¹ polecenia do wykonania. Ka¿de polecenie ma postaæ:

NazwaProgramu arg1 arg2 ...

i oznacza, ¿e nale¿y wykonaæ program o nazwie NazwaProgramu z argumentami: arg1, arg2, ...
Na przyk³ad, linia postaci:
ls -l
powinna spowodowaæ wykonanie programu ls z argumentem -l. Lista argumentów mo¿e byæ pusta - wówczas program wykonywany jest bez argumentów. W zadaniu mo¿na równie¿ przyj¹æ górne ograniczenie na liczbê argumentów.

Interpreter musi wykonywaæ polecenia w osobnych procesach. W tym celu, po odczytaniu polecenia do wykonania interpreter tworzy nowy proces potomny. Proces potomny natychmiast wykonuje odpowiedni¹ funkcjê z rodziny exec, która spowoduje uruchomienie wskazanego programu z odpowiednimi argumentami. Uwaga: proces potomny powinien uwzglêdniaæ zawartoœæ zmiennej œrodowiskowej PATH - polecenie do wykonania nie musi obejmowaæ œcie¿ki do uruchamianego programu, jeœli program ten znajduje siê w katalogu wymienionym w zmiennej PATH.
Po stworzeniu procesu potomnego, proces interpretera czeka na jego zakoñczenie i odczytuje status zakoñczenia. Jeœli proces zakoñczy³ siê ze statusem 0 interpreter przystêpuje do wykonania kolejnej linii pliku wsadowego. W przeciwnym wypadku interpreter wyœwietla komunikat o b³êdzie i koñczy pracê. Komunikat ten powinien wskazywaæ, które polecenie z pliku wsadowego zakoñczy³o siê b³êdem. Zak³adamy, ¿e polecenia z pliku wsadowego nie oczekuj¹ na ¿adne wejœcie z klawiatury. Mog¹ natomiast wypisywaæ wyjœcie na ekran.

Na potrzeby demonstracji zadania przygotuj równie¿ program, który przyjmuje w argumencie nazwê zmiennej œrodowiskowej a nastêpnie odczytuje ze œrodowiska i wypisuje na ekranie jej aktualn¹ wartoœæ. Program powinien równie¿ wypisywaæ stosowny komunikat, jeœli w œrodowisku nie ma zmiennej o wskazanej nazwie.

## Zadanie 2. Zasoby procesów
Zmodyfikuj program z Zadania 1 tak, aby ka¿de polecenie wykonywane przez interpreter mia³o na³o¿one pewne twarde ograniczenie na dostêpny czas procesora oraz rozmiar pamiêci wirtualnej. Wartoœci tych ograniczeñ (odpowiednio w sekundach i megabajtach) powinny byæ przekazywane jako drugi i trzeci argument wywo³ania interpretera (pierwszym argumentem jest nazwa pliku wsadowego). Ograniczenia powinny byæ nak³adane przez proces potomny, bezpoœrednio przed wywo³aniem funkcji z rodziny exec. W tym celu proces potomny powinien u¿yæ funkcji setrlimit. Zak³adamy, ¿e wartoœci nak³adanych ograniczeñ s¹ ni¿sze (t.j. bardziej restrykcyjne) ni¿ ograniczenia, które system operacyjny narzuca na u¿ytkownika uruchamiaj¹cego interpreter.

Zaimplementuj w interpreterze raportowanie zu¿ycia zasobów systemowych dla ka¿dego wykonywanego polecenia (a wiêc linii pliku wsadowego zaczynaj¹cych siê znakiem innym, ni¿ #). Interpreter powinien w szczególnoœci raportowaæ czas u¿ytkownika i czas systemowy. Realizuj¹c t¹ czêœæ zadania zwróæ uwagê na funkcjê getrusage i flagê RUSAGE_CHILDREN.

Na potrzeby demonstracji zadania napisz prosty program, który przekracza narzucone limity na zasoby systemowe. Program mo¿e np. wykonywaæ  pêtlê nieskoñczon¹ lub alokowaæ (i zapisywaæ) znaczn¹ iloœæ pamiêci operacyjnej.