# Zadania - Zestaw 4
# Zestaw 4. Sygna³y
Rodzaje sygna³ów: SIGINT, SIGQUIT, SIGKILL, SIGTSTP, SIGSTOP, SIGTERM, SIGSEGV, SIGHUP, SIGALARM, SIGCHLD, SIGUSR1, SIGUSR2
Sygna³y czasu rzeczywistego: SIGRTMIN, SIGRTMIN+n, SIGRTMAX
Przydatne polecenia Unix: kill, ps
Przydatne funkcje systemowe: kill, raise, sigqueue, signal, sigaction, sigemptyset, sigfillset, sigaddset, sigdelset, sigismember, sigprocmask, sigpending, pause, sigsuspend

## Zadanie 1
Napisz program wypisuj¹cy kolejn¹ liter¹ alfabetu od A do Z. Po odebraniu sygna³u SIGTSTP program zaczyna wypisywaæ litery wstecz. Po ponownym wys³aniu SIGTSTP program powraca do pierwotnego wypisywania. Program powinien umo¿liwiaæ wielokrotne przechodzenie przez powy¿szy cykl (sygna³em SIGTSTP).

Program powinien równie¿ obs³ugiwaæ sygna³ SIGINT. Po jego odebraniu program wypisuje komunikat "Odebrano sygna³ SIGINT" i koñczy dzia³anie. W kodzie programu, do przechwycenia sygna³ów u¿yj zarówno funkcji signal, jak i sigaction (np. SIGINT odbierz za pomoc¹ signal, a SIGTSTP za pomoc¹ sigaction).

## Zadanie 2
Napisz program, który tworzy N potomków i oczekuje na ich proœby na przejœcie (sygna³y SIGUSR1). Po uzyskaniu K próœb, proces powinien pozwoliæ kontynuowaæ dzia³anie wszystkim procesom, które poprosi³y o przejœcie (wys³aæ im sygna³ pozwolenia na rozpoczêcie pracy) i niezw³ocznie akceptowaæ ka¿d¹ kolejn¹ proœbê. Program powinien wypisaæ listê wszystkich otrzymanych sygna³ów wraz numerem PID potomka oraz kodem zakoñczenia procesu (w przypadku otrzymania sygna³u zakoñczenia pracy potomka).

Program koñczy dzia³anie po zakoñczeniu pracy ostatniego potomka albo po otrzymaniu sygna³u SIGINT (w tym wypadku nale¿y zakoñczyæ wszystkie dzia³aj¹ce procesy potomne).

N i M s¹ argumentami programu.

Zachowanie dzieci powinno byæ nastêpuj¹ce. Ka¿de dziecko najpierw symuluje pracê (uœnie na 0-10 sekund). Nastêpnie wysy³a sygna³ SIGUSR1 proœby o przejœcie, a po uzyskaniu pozwolenia losuje jeden z 32 sygna³ów czasu rzeczywistego  (SIGRTMIN,SIGRTMAX), wysy³a go do rodzica i koñczy dzia³anie, zwracaj¹c wypisan¹ ró¿nicê czasu miêdzy wys³aniem proœby a wys³aniem sygna³u jako kod zakoñczenia procesu.

## Zadanie 3
Napisz program który tworzy proces potomnych i wysy³a do niego L sygna³ów SIGUSR1, a nastêpnie sygna³ zakoñczenia wysy³ania SIGUSR2. Potomek po otrzymaniu sygna³ów SIGUSR1 od rodzica zaczyna je odsy³aæ do procesu macierzystego, a po otrzymaniu SIGUSR2 koñczy pracê.

Proces macierzysty w zale¿noœci od argumentu Type (1,2,3) programu wysy³a sygna³y na trzy ró¿ne sposoby:

- SIGUSR1, SIGUSR2 za pomoc¹ funkcji kill
- SIGUSR1, SIGUSR2 za pomoc¹ funkcji sigqueue
- wybrane 2 sygna³y czasu rzeczywistego za pomoc¹ kill

Program powinien wypisywaæ informacje o:

- liczbie wys³anych sygna³ów do potomka
- liczbie odebranych sygna³ów przez potomka
- liczbie odebranych sygna³ów od potomka
Program koñczy dzia³anie po zakoñczeniu pracy potomka albo po otrzymaniu sygna³u SIGINT (w tym wypadku nale¿y zakoñczyæ proces potomny).

L i Type s¹ argumentami programu.

(*) Zamiast podstawowego wariantu program tworzy w pêtli N procesów potomnych, a nastêpnie wysy³a sygna³y do losowo wybranego potomka (do ka¿dego z nich w inny sposób) Przeanalizuj wyniki z wykorzystaniem funkcji sleep pomiêdzy losowaniem kolejnego potomka, wartoœæ funkcji sleep jest ostatnim argumentem programu, domyœlna wartoœæ T=0 oznacza jego brak. Argumenty programu N, L, T=0.