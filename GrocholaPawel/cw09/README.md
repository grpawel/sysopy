#Zadania - Zestaw 9
##Opis problemu
Jednym z najcz�ciej spotykanych problem�w synchronizacji jest "Problem czytelnik�w i pisarzy". Grupa czytelnik�w czyta dane z okre�lonego miejsca (np. rekordu w bazie danych) a pisarze go zapisuj�. Ten sam obiekt mo�e czyta� wi�cej ni� jeden czytelnik. W przeciwie�stwie do nich, pisarze musz� posiada� wy��czny dost�p do obiektu dzielonego. R�wnocze�nie z pisarzem dost�pu do zasobu nie mo�e otrzyma� ani inny pisarz, ani czytelnik, gdy� mog�oby to spowodowa� b��dy.
Problem ten posiada kilka wariant�w:
1. Wariant faworyzuj�cy czytelnik�w
Czytelnicy nie maj� obowi�zku czekania na otrzymanie dost�pu do zasobu, je�li w danym momencie nie otrzyma� go pisarz. Poniewa� pisarz mo�e otrzyma� tylko dost�p wy��czny, musi czeka� na opuszczenie zasobu przez wszystkie inne procesy. Je�li czytelnicy przybywaj� odpowiednio szybko, mo�e doj�� do zag�odzenia pisarza: w tej sytuacji b�dzie on w niesko�czono�� czeka� na zwolnienie zasobu przez wci�� nap�ywaj�cych nowych czytelnik�w.
2. Wariant faworyzuj�cy pisarzy
Czytelnicy nie mog� otrzyma� dost�pu do zasobu, je�eli oczekuje na niego pisarz. W tej sytuacji oczekuj�cy pisarz otrzymuje dost�p najwcze�niej, jak to jest mo�liwe, czyli zaraz po opuszczeniu zasobu przez ostatni proces, kt�ry przyby� przed nim. W tym wariancie mo�e doj�� do zag�odzenia oczekuj�cych czytelnik�w.
3. Wariant z kolejk� FIFO lub z priorytetami, minimalizuj�cy ryzyko zag�odzenia.
## Zadanie
Zaimplementuj wielow�tkowy program realizuj�cy poprawne rozwi�zanie problemu czytelnik�w i pisarzy, spe�niaj�ce nast�puj�ce za�o�enia:

- jest wiele w�tk�w czytelnik�w i wiele w�tk�w pisarzy
- dane umieszczone w pami�ci wsp�lnej, jest to tablica liczb ca�kowitych
- pisarz cyklicznie modyfikuje w spos�b losowy wybrane liczby w tablicy (losuje ilo�� liczb do modyfikacji, ich pozycje w tablicy oraz warto�ci tych liczb)
- czytelnik uruchamiany jest z jednym argumentem - dzielnik i znajduje w tablicy wszystkie liczby, kt�re si� przez niego dziel� bez reszty, wykonuj�c cyklicznie operacj� przeszukiwania tablicy
- pisarz raportuje na standardowym wyj�ciu wykonanie swojej operacji (modyfikacji tablicy), w wersji opisowej programu (opcja -i) wypisuje indeks i wpisan� warto��
- analogiczny raport wykonuje czytelnik, dodaj�c dodatkowo wynik operacji: ilo�� znalezionych liczb spe�niaj�cych warunek, a w wersji opisowej programu (opcja -i) podaje indeksy i warto�ci znalezionych liczb.

Nale�y wykona� dwie wersje rozwi�zania.

a) rozwi�zanie wykorzystuj�ce do synchronizacji semafory nienazwane standardu POSIX (zgodne z typem rozwi�za� problemu wsp�bie�nego stosuj�cego semafory)

b) rozwi�zanie wykorzystuj�ce do synchronizacji muteks i zmienne warunkowe (zgodne z typem rozwi�za� problemu wsp�bie�nego stosuj�cego monitor)

Dla ka�dej z wersji wybierz inny wariant rozwi�zania, jeden z mo�liwo�ci� zag�odzenia czytelnika albo pisarza, a drugi np z kolejk� FIFO, minimalizuj�cymi to ryzyko.

UWAGA! Nale�y uwzgl�dni� mo�liwo�� jednoczesnego uruchamiania wielu w�tk�w pisarzy.