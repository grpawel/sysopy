#Zadania - Zestaw 8
##Zadanie 1
Napisz program przyjmuj¹cy cztery argumenty: iloœæ w¹tków, nazwê pliku, iloœæ rekordów czytanych przez w¹tek w pojedynczym odczycie i s³owo do wyszukania w rekordzie. Plik zawiera rekordy o sta³ym rozmiarze 1024 bajtów. Rekord sk³ada siê z pola id (int) oraz z pola text (pozosta³y rozmiar rekordu). Program otwiera plik przekazany w argumencie, po czym tworzy zadan¹ iloœæ w¹tków. W¹tki czytaj¹ naprzemiennie funkcj¹ read ustalone iloœci rekordów z otwartego pliku. W¹tki powinny przechowywaæ czytane rekordy pliku w swoich danych w³asnych. W polach text rekordów w¹tki poszukuj¹ s³owa przekazanego przez parametr. Po odszukaniu zadanego s³owa w¹tek powinien wypisaæ na ekran swój identyfikator (TID) i id rekordu.

Stwórz trzy wersje programu. W wersji pierwszej, po odszukaniu s³owa w¹tek anuluje asynchronicznie wszystkie pozosta³e w¹tki i koñczy pracê. W wersji drugiej w¹tek, który odszuka³ napis równie¿ anuluje pozosta³e w¹tki, lecz anulowanie jest synchroniczne - punktem anulowania w¹tku jest zakoñczenie przetwarzania wczytanej iloœci rekordów do danych prywatnych. W wersji trzeciej wszystkie w¹tki powinny byæ od³¹czone a warunkiem zakoñczenia w¹tku jest odczytanie wszystkich rekordów pliku.

##Zadanie 2
a) PrzeprowadŸ testy dotycz¹ce zachowania siê w¹tków w reakcji na nadchodz¹ce sygna³y. Uwzglêdnij nastêpuj¹ce sygna³y: SIGUSR1, SIGTERM, SIGKILL, SIGSTOP. Poddaj odpowiednim modyfikacjom program u¿yty w Zadaniu 1.

Wyœlij te sygna³y do ca³ego procesu (kill) oraz do poszczególnych w¹tków (kill/pthread_kill). Opisz w pliku „zad2.raport” rezultaty poni¿szych dzia³añ, bior¹c pod uwagê, które watki zosta³y zatrzymane lub zabite i jakie komunikaty zosta³y wypisane. Odrzuæ te dzia³ania, które ze wzglêdu na w³aœciwoœci poszczególnych sygna³ów s¹ niemo¿liwe do wykonania. 
Oto operacje do przetestowania wyników:

1. wys³anie sygna³u do procesu, gdy ¿aden w¹tek nie ma zamaskowanego tego sygna³u
2. wys³anie sygna³u do procesu, gdy g³ówny w¹tek programu ma zamaskowany ten sygna³, a wszystkie pozosta³e w¹tki nie,
3. wys³anie sygna³u do procesu, gdy wszystkie w¹tki maj¹ zainstalowan¹ niestandardow¹ procedurê obs³ugi przerwania, która wypisuje informacjê o nadejœciu tego sygna³u oraz PID i TID danego w¹tku
4. wys³anie sygna³u do w¹tku z zamaskowanym tym sygna³em
5. wys³anie sygna³u do w¹tku, w którym zmieniona jest procedura obs³ugi sygna³u, jak przedstawiono w punkcie 3
Zaznacz, jeœli s¹ jakieœ ró¿nice w zale¿noœci od rodzaju sygna³u.

b) Spraw, by w jednym z w¹tków nastêpowa³o dzielenie przez zero i by by³ wygenerowany odpowiedni sygna³. Jak reaguje na niego w¹tek, który spowodowa³ tê sytuacjê, a jak inne w¹tki wchodz¹ce w sk³ad tego samego procesu? Umieœæ odpowiedŸ w pliku zad2.raport . Zaznacz dla jakiego rodzaju systemu operacyjnego i wersji j¹dra by³y przeprowadzane testy i jaki to by³ komputer.