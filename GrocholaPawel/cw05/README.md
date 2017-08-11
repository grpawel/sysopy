# Zadania - Zestaw 5
# Potoki nazwane i nienazwane
## Zadanie 1
Nale¿y napisaæ interpreter poleceñ obs³uguj¹cy operator pipe - "|". Interpreter czyta kolejne linie z wejœcia standardowego. Ka¿da linia ma format

prog1 arg1 ... argn1 | prog2 arg1 ... argn2 | ... | progN arg1 ... argnN

Dla takiej linii interpreter powinien uruchomiæ wszystkie N poleceñ w osobnych procesach, zapewniaj¹c przy u¿yciu potoków nienazwanych oraz funkcji dup2, by wyjœcie standardowe procesu k by³o przekierowane do wejœcia standardowego procesu (k+1). Mo¿na za³o¿yæ ograniczenie górne na iloœæ obs³ugiwanych argumentów (co najmniej 3) oraz iloœæ po³¹czonych komend w pojedynczym poleceniu (co najmniej 20). Interpreter powinien umo¿liwiaæ uruchamianie programów znajduj¹cych siê w katalogu na liœcie w zmiennej PATH bez podawania pe³nej œcie¿ki. Po uruchomieniu ci¹gu programów sk³adaj¹cych siê na pojeczyne polecenie (linijkê) interpreter powinien oczekiwaæ na zakoñczenie wszystkich tych programów.

Uwaga: nale¿y u¿yæ pipe/fork/exec, nie popen


## Zadanie 2
W ramach tego zadania stworzony zostanie wizualizator zbioru Mandelbrota. Zbiór ten sk³ada siê z liczb zespolonych c takich, ¿e ci¹g {zn} zdefiniowany poprzez

           z0 = 0, zn+1 = fc(zn),      gdzie fc(z) = z2

ma modu³ ograniczony przez 2, tj. |zn| <= 2 dla ka¿dego n. W praktyce wizualizacja polega na tym, ¿e dla zadanego punktu c sprawdzamy, czy po pewnej ustalonej iloœci iteracji K ten warunek jest spe³niony i kolorujemy punkt c na podstawie tego, po ilu iteracjach warunek przestaje byæ prawd¹ - nazwijmy tê liczbê iters(c). Jeœli np. K = 100 i dla danego c dopiero w 30 iteracji dostajemy zn takie, ¿e |zn| > 2, to iters(c) = 30. Jeœli po K iteracjach warunek pozostaje spe³niony, to przyjmujemy iters(c) = K.

Wizualizacjê bêdziemy wykonywaæ dla punktów w prostok¹tnym obszarze D = { c: -2 < Re(c) < 1, -1 < Im(c) < 1 }

Nale¿y napisaæ dwa programy - master oraz slave - które bêd¹ komunikowaæ siê poprzez potok nazwany (kolejkê FIFO), do której œcie¿kê bêd¹ dostawaæ jako argument wywo³ania. Do potoku pisaæ bêdzie wiele procesów wykonuj¹cych program slave, a czyta³ bêdzie z niej jeden proces master.

#### Slave

Slave przyjmuje trzy argumenty - œcie¿kê do potoku nazwanego i liczby ca³kowite N oraz K. Wykonuje nastêpuj¹ce akcje:

otwiera potok nazwany
generuje N losowych punktów w obszarze D, dla ka¿dego punktu c oblicza wartoœæ iters(c) oraz zapisuje do potoku jedn¹ linijkê zawieraj¹c¹ czêœæ rzeczywist¹ oraz urojon¹ punktu c oraz obliczon¹ wartoœæ iters(c)
zamyka potok nazwany

#### Master

Master przyjmuje dwa argumenty - œcie¿kê do potoku nazwanego i liczbê ca³kowit¹ R. Tworzy on potok o podanej nazwie. Master przechowuje zainicjalizowan¹ zerami tablicê dwuwymiarow¹ T o rozmiarze R x R reprezentuj¹c¹ obraz obszaru D - ka¿dy element tablicy to jeden "piksel" i odpowiada w naturalny sposób pewnemu kawa³kowi obszaru D.

Master czyta z potoku nazwanego kolejne linijki a¿ do momentu gdy zostanie on zamkniêty przez wszystkie procesy do niego pisz¹ce. Ka¿da linijka to informacja o punkcie c = x + iy oraz wartoœci iters(c). Dla ka¿dej oczytanej linijki master zapisuje w tablicy T wartoœæ iters(c) w miejscu odpowiadaj¹cym pikselowi reprezentuj¹cemu kawa³ek D w którym le¿y punkt c.

Po zakoñczeniu czytania master zapisuje zawartoœæ tablicy T do pliku o nazwie data tak, ¿e w ka¿dej linijce znajduj¹ siê oddzielone bia³ymi znakami wartoœci indeksów i, j oraz wartoœæ T[i][j], w kolejnoœci takiej jak w poni¿szym przyk³adzie:

0 0 10
0 1 35
0 2 11
1 0 5
1 1 77

itd. Nastêpnie master uruchamia program Gnuplot przy u¿yciu funkcji popen, i przekazuje na jego wejœcie standardowe nastêpuj¹ce polecenia:

set view map
set xrange [0:X]
set yrange [0:X]
plot 'data' with image

gdzie za X wstawiona ma byæ wartoœæ argumentu R. W efekcie powinna powstaæ wizualizacja zbioru Mandelbrota podobna do poni¿szej:

![mandelbrot](mandelbrot.png)

(R = 600, K = 100, 10 000 000 losowych punktów)

Uwaga: aby okno z wizualizacj¹ nie znika³o od razu, po przes³aniu poleceñ do Gnuplota nale¿y

wyczyœciæ bufor potoku przez który przekazujemy polecenia do Gnuplota
np. oczekiwaæ na znak z klawiatury
Mo¿na u¿yæ np. takiego kodu:

fflush(pipe);
getc(stdin);