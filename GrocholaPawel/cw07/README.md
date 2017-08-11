#Zadania - Zestaw 7
#IPC - pamieæ wspólna, semafory
Przydatne funkcje

System V:

<sys/shm.h> <sys/ipc.h> - shmget, shmclt, shmat, shmdt
POSIX:

<sys/mman.h> - shm_open, shm_close, shm_unlink, mmap, munmap
## Problem synchronizacyjny œpi¹cego golibrody z kolejk¹
W ramach æwiczenia 7 nale¿y zaimplementowaæ prawid³owe rozwi¹zanie problemu œpi¹cego golibrody z kolejk¹. Golibroda prowadzi zak³ad fryzjerski z jednym krzes³em w gabinecie (na którym strzy¿e klientów) i N krzes³ami w poczekalni. Zak³ad dzia³a wed³ug nastêpuj¹cego schematu:

- Po przyjœciu do zak³adu klient sprawdza co robi golibroda. Jeœli golibroda œpi, klient budzi go, siada na krzeœle w gabinecie i jest strzy¿ony. Jeœli golibroda strzy¿e innego klienta, nowy klient sprawdza, czy w poczekalni jest wolne krzes³o. Jeœli tak, to klient siada w poczekalni. W przeciwnym razie klient opuszcza zak³ad.
- Golibroda sprawdza, czy w poczekalni oczekuj¹ klienci. Jeœli w poczekalni nie ma klientów, golibroda zasypia. W przeciwnym razie golibroda zaprasza do strzy¿enia klienta, który czeka³ najd³u¿ej. Gdy golibroda skoñczy strzy¿enie, klient opuszcza zak³ad. Golibroda ponownie sprawdza poczekalniê.
Zak³adamy, ¿e golibroda oraz ka¿dy klient to osobne procesy. Liczba krzese³ w poczekalni jest podawana w argumencie wywo³ania golibrody. Proces golibrody pracuje do momentu otrzymania wybranego sygna³u (np. SIGINT).
Klienci s¹ tworzeni przez jeden proces macierzysty (funkcj¹ fork). Ka¿dy klient posiada identyfikator – mo¿e to byæ np. jego numer PID. Proces tworz¹cy klientów przyjmuje dwa argumenty: liczbê klientów do stworzenia oraz liczbê strzy¿eñ S. Ka¿dy klient w pêtli odwiedza zak³ad fryzjerski. Gdy klient zostanie ostrzy¿ony S razy, proces klienta koñczy pracê. Proces macierzysty koñczy pracê po zakoñczeniu wszystkich procesów klientów (mo¿na wówczas uruchomiæ w konsoli kolejn¹ partiê klientów).

Program golibrody wypisuje na ekranie komunikaty o nastêpuj¹cych zdarzeniach:

1. zaœniêcie golibrody,
2. rozpoczêcie strzy¿enia (w komunikacie tym podawany identyfikator strzy¿onego klienta),
3. zakoñczenie strzy¿enia (równie¿ z identyfikatorem strzy¿onego klienta).

Ka¿dy klient wypisuje na ekranie komunikaty o nastêpuj¹cych zdarzeniach:

1. obudzenie golibrody,
2. zajêcie miejsca w poczekalni,
3. opuszczenie zak³adu z powodu braku wolnych miejsc w poczekalni,
4. opuszczenie zak³adu po zakoñczeniu strzy¿enia.
Ka¿dy komunikat golibrody lub klienta powinien zawieraæ znacznik czasowy z dok³adnoœci¹ do mikrosekund (mo¿na tu wykorzystaæ funkcjê clock_gettime z flag¹ CLOCK_MONOTONIC). Ka¿dy komunikat klienta powinien ponadto zawieraæ jego identyfikator.

Synchronizacja procesów musi wykluczaæ zakleszczenia i gwarantowaæ sekwencjê zdarzeñ zgodn¹ ze schematem dzia³ania zak³adu. Niedopuszczalne jest zasypianie procesów klientów lub golibrody funkcjami z rodziny sleep. Szeregowanie klientów do strzy¿enia nale¿y zaimplementowaæ w postaci kolejki FIFO w pamiêci wspólnej. W pamiêci wspólnej mo¿na równie¿ przechowywaæ zmienn¹ zliczaj¹c¹ klientów oczekuj¹cych w poczekalni. Odpowiednie zasoby do synchronizacji i komunikacji powinny byæ tworzone przez proces golibrody. Proces ten jest równie¿ odpowiedzialny za usuniêcie tych zasobów z systemu (przed zakoñczeniem pracy).

## Zadanie 1
Zaimplementuj opisany powy¿ej problem synchronizacyjny wykorzystuj¹c semafory i pamiêæ wspóln¹ z IPC Systemu V.

## Zadanie 2.
Zaimplementuj opisany powy¿ej problem synchronizacyjny wykorzystuj¹c semafory i pamiêæ wspóln¹ z IPC POSIX.