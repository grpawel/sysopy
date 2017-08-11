#Zadania - Zestaw 9
##Opis problemu
Jednym z najczêœciej spotykanych problemów synchronizacji jest "Problem czytelników i pisarzy". Grupa czytelników czyta dane z okreœlonego miejsca (np. rekordu w bazie danych) a pisarze go zapisuj¹. Ten sam obiekt mo¿e czytaæ wiêcej ni¿ jeden czytelnik. W przeciwieñstwie do nich, pisarze musz¹ posiadaæ wy³¹czny dostêp do obiektu dzielonego. Równoczeœnie z pisarzem dostêpu do zasobu nie mo¿e otrzymaæ ani inny pisarz, ani czytelnik, gdy¿ mog³oby to spowodowaæ b³êdy.
Problem ten posiada kilka wariantów:
1. Wariant faworyzuj¹cy czytelników
Czytelnicy nie maj¹ obowi¹zku czekania na otrzymanie dostêpu do zasobu, jeœli w danym momencie nie otrzyma³ go pisarz. Poniewa¿ pisarz mo¿e otrzymaæ tylko dostêp wy³¹czny, musi czekaæ na opuszczenie zasobu przez wszystkie inne procesy. Jeœli czytelnicy przybywaj¹ odpowiednio szybko, mo¿e dojœæ do zag³odzenia pisarza: w tej sytuacji bêdzie on w nieskoñczonoœæ czeka³ na zwolnienie zasobu przez wci¹¿ nap³ywaj¹cych nowych czytelników.
2. Wariant faworyzuj¹cy pisarzy
Czytelnicy nie mog¹ otrzymaæ dostêpu do zasobu, je¿eli oczekuje na niego pisarz. W tej sytuacji oczekuj¹cy pisarz otrzymuje dostêp najwczeœniej, jak to jest mo¿liwe, czyli zaraz po opuszczeniu zasobu przez ostatni proces, który przyby³ przed nim. W tym wariancie mo¿e dojœæ do zag³odzenia oczekuj¹cych czytelników.
3. Wariant z kolejk¹ FIFO lub z priorytetami, minimalizuj¹cy ryzyko zag³odzenia.
## Zadanie
Zaimplementuj wielow¹tkowy program realizuj¹cy poprawne rozwi¹zanie problemu czytelników i pisarzy, spe³niaj¹ce nastêpuj¹ce za³o¿enia:

- jest wiele w¹tków czytelników i wiele w¹tków pisarzy
- dane umieszczone w pamiêci wspólnej, jest to tablica liczb ca³kowitych
- pisarz cyklicznie modyfikuje w sposób losowy wybrane liczby w tablicy (losuje iloœæ liczb do modyfikacji, ich pozycje w tablicy oraz wartoœci tych liczb)
- czytelnik uruchamiany jest z jednym argumentem - dzielnik i znajduje w tablicy wszystkie liczby, które siê przez niego dziel¹ bez reszty, wykonuj¹c cyklicznie operacjê przeszukiwania tablicy
- pisarz raportuje na standardowym wyjœciu wykonanie swojej operacji (modyfikacji tablicy), w wersji opisowej programu (opcja -i) wypisuje indeks i wpisan¹ wartoœæ
- analogiczny raport wykonuje czytelnik, dodaj¹c dodatkowo wynik operacji: iloœæ znalezionych liczb spe³niaj¹cych warunek, a w wersji opisowej programu (opcja -i) podaje indeksy i wartoœci znalezionych liczb.

Nale¿y wykonaæ dwie wersje rozwi¹zania.

a) rozwi¹zanie wykorzystuj¹ce do synchronizacji semafory nienazwane standardu POSIX (zgodne z typem rozwi¹zañ problemu wspó³bie¿nego stosuj¹cego semafory)

b) rozwi¹zanie wykorzystuj¹ce do synchronizacji muteks i zmienne warunkowe (zgodne z typem rozwi¹zañ problemu wspó³bie¿nego stosuj¹cego monitor)

Dla ka¿dej z wersji wybierz inny wariant rozwi¹zania, jeden z mo¿liwoœci¹ zag³odzenia czytelnika albo pisarza, a drugi np z kolejk¹ FIFO, minimalizuj¹cymi to ryzyko.

UWAGA! Nale¿y uwzglêdniæ mo¿liwoœæ jednoczesnego uruchamiania wielu w¹tków pisarzy.