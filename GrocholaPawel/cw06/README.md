#Zadania - Zestaw 6
#IPC - kolejki komunikatów
Przydatne funkcje:

System V:

<sys/msg.h> <sys/ipc.h> - msgget, msgctl, msgsnd, msgrcv, ftok

POSIX:

<mqueue.h> - mq_open, mq_send, mq_receive, mq_getattr, mq_setattr, mq_close, mq_unlink, mq_notify

## Zadanie 1. System V
Napisz prosty program typu klient-serwer, w którym komunikacja zrealizowana jest za pomoc¹ kolejek komunikatów - jedna, na zlecenia klientów dla serwera, druga, prywatna, na odpowiedzi.
Serwer po uruchomieniu tworzy now¹ kolejkê komunikatów systemu V. Za pomoc¹ tej kolejki klienci bêd¹ wysy³aæ komunikaty do serwera. Wysy³ane zlecenia maj¹ zawieraæ rodzaj zlecenia jako rodzaj komunikatu oraz informacjê od którego klienta zosta³y wys³ane (PID procesu klienta), w odpowiedzi rodzajem komunikatu ma byæ informacja identyfikuj¹ca czekaj¹cego na ni¹ klienta.
Klient bezpoœrednio po uruchomieniu tworzy kolejkê z unikalnym kluczem IPC  i wysy³a jej klucz komunikatem do serwera. Po otrzymaniu takiego komunikatu, serwer otwiera kolejkê klienta, przydziela klientowi identyfikator (np. numer w kolejnoœci zg³oszeñ) i odsy³a ten identyfikator do klienta (komunikacja w kierunku serwer->klient odbywa siê za pomoc¹ kolejki klienta). Po otrzymaniu identyfikatora, klient rozpoczyna wysy³anie zleceñ do serwera (w pêtli).

Rodzaje zleceñ

- Us³uga echa:
Klient wysy³a ci¹g znaków. Serwer odsy³a ten sam ci¹g z powrotem. Klient po odebraniu wysy³a go na standardowe wyjœcie.
- Us³uga wersalików:
Klient wysy³a ci¹g znaków. Serwer zamienia ma³e litery na du¿e i odsy³a zmieniony ci¹g z powrotem. Klient po odebraniu wysy³a go na standardowe wyjœcie.
- Us³uga czasu:
Po odebraniu takiego zlecenia serwer wysy³a do klienta datê i godzinê w postaci ³añcucha znaków. Klient po odebraniu informacji wysy³a j¹ na standardowe wyjœcie.
- Nakaz zakoñczenia:
Po odebraniu takiego zlecenia serwer zakoñczy dzia³anie, jak tylko opró¿ni siê kolejka zleceñ (zostan¹ wykonane wszystkie pozosta³e zlecenia).
Klient nie czeka na odpowiedŸ.

Poszczególne rodzaje komunikatów nale¿y identyfikowaæ za pomoc¹ typów komunikatów systemu V. Klucze dla kolejek maj¹ byæ generowane na podstawie œcie¿ki $HOME. Ma³e liczby do wygenerowania kluczy oraz rodzaje komunikatów maj¹ byæ zdefiniowane we wspólnym pliku nag³ówkowym. Dla uproszczenia mo¿na za³o¿yæ, ¿e d³ugoœæ komunikatu (lub maksymalna d³ugoœæ ³añcucha znaków przy us³udze echa) jest ograniczona pewn¹ sta³¹ (jej definicja powinna znaleŸæ siê w pliku nag³ówkowym).
Klient i serwer nale¿y napisaæ w postaci osobnych programów (nie korzystamy z funkcji fork). Serwer musi byæ w stanie pracowaæ z wieloma klientami naraz. Przed zakoñczeniem pracy ka¿dy proces powinien usun¹æ kolejkê któr¹ utworzy³ (patrz IPC_RMID oraz funkcja atexit). Dla uproszczenia mo¿na przyj¹æ, ¿e serwer przechowuje informacje o klientach w statycznej tablicy (rozmiar tablicy ogranicza liczbê klientów, którzy mog¹ siê zg³osiæ do serwera).

## Zadanie 2. POSIX
Zrealizuj zadanie alogiczne do Zadania 1 wykorzystuj¹c kolejki komunikatów POSIX.
Kolejka klienta powinna mieæ losow¹ nazwê zgodn¹ z wymaganiami stawianymi przez POSIX. Na typ komunikatu mo¿na zarezerwowaæ pierwszy bajt jego treœci. Przed zakoñczeniem pracy klient wysy³a do serwera komunikat informuj¹cy, ¿e serwer powinien zamkn¹æ po swojej stronie kolejkê klienta. Nastêpnie klient zamyka i usuwa swoj¹ kolejkê. Serwer przed zakoñczeniem pracy zamyka wszystkie otwarte kolejki i usuwa kolejkê, któr¹ utworzy³.