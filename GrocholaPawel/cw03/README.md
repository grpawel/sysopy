# Zadanie - zestaw 3
# Tworzenie proces�w. �rodowisko procesu, sterowanie procesami.
## Zadanie 1. Prosty interpreter zada� wsadowych
W ramach �wiczenia nale�y napisa� prosty interpreter zada� wsadowych. Interpreter przyjmuje w argumencie nazw� pliku zawieraj�cego zadanie wsadowe i wykonuje wszystkie polecenia z tego pliku. Plik z zadaniem wsadowym ma �ci�le okre�lon� posta�:

1. Linie zaczynaj�ce si� znakiem # oznaczaj� prac� ze zmiennymi �rodowiskowymi. Ka�da taka linia ma jedn� z dw�ch postaci:

- \#NazwaZmiennej WartoscZmiennej
Oznacza definicj� zmiennej �rodowiskowej. Od momentu jej wyst�pienia, w �rodowisku polece� wykonywanych przez interpreter powinna istnie� zmienna NazwaZmiennej o warto�ci WartoscZmiennej. Je�li taka zmienna istnieje ju� w �rodowisku, jej warto�� powinna zosta� zmieniona na WartoscZmiennej.
Na przyk�ad, linia postaci:
\#TestFile ./ala.txt
powinna spowodowa� dodanie do �rodowiska zmiennej TestFile o warto�ci ./ala.txt

Uwaga: zmienne �rodowiskowe, z kt�rymi zosta� uruchomiony interpreter powinny by� dost�pne r�wnie� dla wykonywanych polece�.

- #NazwaZmiennej
Oznacza usuni�cie zmiennej NazwaZmiennej ze �rodowiska. Polecenie powinno zako�czy� si� powodzeniem r�wnie� w�wczas, gdy w �rodowisku nie ma zmiennej NazwaZmiennej (w�wczas �rodowisko si� nie zmienia).
Na przyk�ad, linia postaci:
#TestFile
powinna spowodowa� usuni�cie ze �rodowiska zmiennej TestFile (je�li taka zmienna jest w �rodowisku).

2. Linie zaczynaj�c si� znakiem innym, ni� # oznaczaj� polecenia do wykonania. Ka�de polecenie ma posta�:

NazwaProgramu arg1 arg2 ...

i oznacza, �e nale�y wykona� program o nazwie NazwaProgramu z argumentami: arg1, arg2, ...
Na przyk�ad, linia postaci:
ls -l
powinna spowodowa� wykonanie programu ls z argumentem -l. Lista argument�w mo�e by� pusta - w�wczas program wykonywany jest bez argument�w. W zadaniu mo�na r�wnie� przyj�� g�rne ograniczenie na liczb� argument�w.

Interpreter musi wykonywa� polecenia w osobnych procesach. W tym celu, po odczytaniu polecenia do wykonania interpreter tworzy nowy proces potomny. Proces potomny natychmiast wykonuje odpowiedni� funkcj� z rodziny exec, kt�ra spowoduje uruchomienie wskazanego programu z odpowiednimi argumentami. Uwaga: proces potomny powinien uwzgl�dnia� zawarto�� zmiennej �rodowiskowej PATH - polecenie do wykonania nie musi obejmowa� �cie�ki do uruchamianego programu, je�li program ten znajduje si� w katalogu wymienionym w zmiennej PATH.
Po stworzeniu procesu potomnego, proces interpretera czeka na jego zako�czenie i odczytuje status zako�czenia. Je�li proces zako�czy� si� ze statusem 0 interpreter przyst�puje do wykonania kolejnej linii pliku wsadowego. W przeciwnym wypadku interpreter wy�wietla komunikat o b��dzie i ko�czy prac�. Komunikat ten powinien wskazywa�, kt�re polecenie z pliku wsadowego zako�czy�o si� b��dem. Zak�adamy, �e polecenia z pliku wsadowego nie oczekuj� na �adne wej�cie z klawiatury. Mog� natomiast wypisywa� wyj�cie na ekran.

Na potrzeby demonstracji zadania przygotuj r�wnie� program, kt�ry przyjmuje w argumencie nazw� zmiennej �rodowiskowej a nast�pnie odczytuje ze �rodowiska i wypisuje na ekranie jej aktualn� warto��. Program powinien r�wnie� wypisywa� stosowny komunikat, je�li w �rodowisku nie ma zmiennej o wskazanej nazwie.

## Zadanie 2. Zasoby proces�w
Zmodyfikuj program z Zadania 1 tak, aby ka�de polecenie wykonywane przez interpreter mia�o na�o�one pewne twarde ograniczenie na dost�pny czas procesora oraz rozmiar pami�ci wirtualnej. Warto�ci tych ogranicze� (odpowiednio w sekundach i megabajtach) powinny by� przekazywane jako drugi i trzeci argument wywo�ania interpretera (pierwszym argumentem jest nazwa pliku wsadowego). Ograniczenia powinny by� nak�adane przez proces potomny, bezpo�rednio przed wywo�aniem funkcji z rodziny exec. W tym celu proces potomny powinien u�y� funkcji setrlimit. Zak�adamy, �e warto�ci nak�adanych ogranicze� s� ni�sze (t.j. bardziej restrykcyjne) ni� ograniczenia, kt�re system operacyjny narzuca na u�ytkownika uruchamiaj�cego interpreter.

Zaimplementuj w interpreterze raportowanie zu�ycia zasob�w systemowych dla ka�dego wykonywanego polecenia (a wi�c linii pliku wsadowego zaczynaj�cych si� znakiem innym, ni� #). Interpreter powinien w szczeg�lno�ci raportowa� czas u�ytkownika i czas systemowy. Realizuj�c t� cz�� zadania zwr�� uwag� na funkcj� getrusage i flag� RUSAGE_CHILDREN.

Na potrzeby demonstracji zadania napisz prosty program, kt�ry przekracza narzucone limity na zasoby systemowe. Program mo�e np. wykonywa�  p�tl� niesko�czon� lub alokowa� (i zapisywa�) znaczn� ilo�� pami�ci operacyjnej.