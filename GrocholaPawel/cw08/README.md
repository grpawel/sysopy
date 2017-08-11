#Zadania - Zestaw 8
##Zadanie 1
Napisz program przyjmuj�cy cztery argumenty: ilo�� w�tk�w, nazw� pliku, ilo�� rekord�w czytanych przez w�tek w pojedynczym odczycie i s�owo do wyszukania w rekordzie. Plik zawiera rekordy o sta�ym rozmiarze 1024 bajt�w. Rekord sk�ada si� z pola id (int) oraz z pola text (pozosta�y rozmiar rekordu). Program otwiera plik przekazany w argumencie, po czym tworzy zadan� ilo�� w�tk�w. W�tki czytaj� naprzemiennie funkcj� read ustalone ilo�ci rekord�w z otwartego pliku. W�tki powinny przechowywa� czytane rekordy pliku w swoich danych w�asnych. W polach text rekord�w w�tki poszukuj� s�owa przekazanego przez parametr. Po odszukaniu zadanego s�owa w�tek powinien wypisa� na ekran sw�j identyfikator (TID) i id rekordu.

Stw�rz trzy wersje programu. W wersji pierwszej, po odszukaniu s�owa w�tek anuluje asynchronicznie wszystkie pozosta�e w�tki i ko�czy prac�. W wersji drugiej w�tek, kt�ry odszuka� napis r�wnie� anuluje pozosta�e w�tki, lecz anulowanie jest synchroniczne - punktem anulowania w�tku jest zako�czenie przetwarzania wczytanej ilo�ci rekord�w do danych prywatnych. W wersji trzeciej wszystkie w�tki powinny by� od��czone a warunkiem zako�czenia w�tku jest odczytanie wszystkich rekord�w pliku.

##Zadanie 2
a) Przeprowad� testy dotycz�ce zachowania si� w�tk�w w reakcji na nadchodz�ce sygna�y. Uwzgl�dnij nast�puj�ce sygna�y: SIGUSR1, SIGTERM, SIGKILL, SIGSTOP. Poddaj odpowiednim modyfikacjom program u�yty w Zadaniu 1.

Wy�lij te sygna�y do ca�ego procesu (kill) oraz do poszczeg�lnych w�tk�w (kill/pthread_kill). Opisz w pliku �zad2.raport� rezultaty poni�szych dzia�a�, bior�c pod uwag�, kt�re watki zosta�y zatrzymane lub zabite i jakie komunikaty zosta�y wypisane. Odrzu� te dzia�ania, kt�re ze wzgl�du na w�a�ciwo�ci poszczeg�lnych sygna��w s� niemo�liwe do wykonania. 
Oto operacje do przetestowania wynik�w:

1. wys�anie sygna�u do procesu, gdy �aden w�tek nie ma zamaskowanego tego sygna�u
2. wys�anie sygna�u do procesu, gdy g��wny w�tek programu ma zamaskowany ten sygna�, a wszystkie pozosta�e w�tki nie,
3. wys�anie sygna�u do procesu, gdy wszystkie w�tki maj� zainstalowan� niestandardow� procedur� obs�ugi przerwania, kt�ra wypisuje informacj� o nadej�ciu tego sygna�u oraz PID i TID danego w�tku
4. wys�anie sygna�u do w�tku z zamaskowanym tym sygna�em
5. wys�anie sygna�u do w�tku, w kt�rym zmieniona jest procedura obs�ugi sygna�u, jak przedstawiono w punkcie 3
Zaznacz, je�li s� jakie� r�nice w zale�no�ci od rodzaju sygna�u.

b) Spraw, by w jednym z w�tk�w nast�powa�o dzielenie przez zero i by by� wygenerowany odpowiedni sygna�. Jak reaguje na niego w�tek, kt�ry spowodowa� t� sytuacj�, a jak inne w�tki wchodz�ce w sk�ad tego samego procesu? Umie�� odpowied� w pliku zad2.raport . Zaznacz dla jakiego rodzaju systemu operacyjnego i wersji j�dra by�y przeprowadzane testy i jaki to by� komputer.