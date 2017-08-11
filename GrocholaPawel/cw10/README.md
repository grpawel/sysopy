# Zadania - Zestaw 10
Celem zadania jest napisanie prostego systemu pozwalaj¹cego na wykonywanie obliczeñ na "klastrze obliczeniowym".

Mamy centralny serwer (nas³uchuj¹cy jednoczeœnie na gnieŸdzie sieciowym i gnieŸdzie lokalnym), z poziomu którego zlecane s¹ obliczenia. Klienci po uruchomieniu przesy³aj¹ do serwera swoj¹ nazwê, a ten sprawdza czy klient o takiej nazwie ju¿ istnieje - jeœli tak, to odsy³a klientowi komunikat ¿e nazwa jest ju¿ w u¿yciu; jeœli nie, to zapamiêtuje klienta. Zlecenia obliczeñ s¹ wpisane bezpoœrednio w konsoli serwera i dotycz¹ czterech podstawowych dzia³añ matematycznych - dodawania, odejmowania, mno¿enia i dzielenia. Serwer tworzy strukturê opisuj¹c¹ zlecenie (dzia³anie, argument 1, argument 2), a nastêpnie przesy³a j¹ do dowolnie wybranego klienta. Ten oblicza wynik zlecenia i odsy³a go do serwera, który wyœwietla wynik na standardowym wyjœciu. Klient przy wy³¹czeniu Ctrl+C powinien wyrejestrowaæ siê z serwera.

W procesie serwera obs³uga wprowadzania zleceñ z terminala i obs³uga sieci powinny byæ zrealizowane w dwóch osobnych w¹tkach. W¹tek obs³uguj¹cy sieæ powinien obs³ugiwaæ gniazdo sieciowe i gniazdo lokalne jednoczeœnie, wykorzystuj¹c w tym celu funkcje do monitorowania wielu deskryptorów (epoll/poll/select). Dodatkowo, osobny w¹tek powinien cyklicznie "pingowaæ" zarejestrowanych klientów, aby zweryfikowaæ ¿e wci¹¿ odpowiadaj¹ na ¿¹dania i jeœli nie - usuwaæ ich z listy klientów.

Mo¿esz przyj¹æ, ¿e iloœæ klientów zarejestrowanych na serwerze jest ograniczona do maksymalnie kilkunastu. Zapewnij mo¿liwoœæ ustalenia dla którego zlecenia wyœwietlony zosta³ wynik (np. umieœæ w serwerze globalny licznik, wyœwietlany i inkrementowany po dodaniu ka¿dego nowego zlecenia i umieszczany jako dodatkowy identyfikator w komunikatach wysy³anych do klientów i odpowiedziach od nich przychodz¹cych) i który klient go obliczy³.

Serwer przyjmuje jako swoje argumenty:

- numer portu TCP/UDP (zale¿nie od zadania),
- œcie¿kê gniazda UNIX.

Klient przyjmuje jako swoje argumenty:

- swoj¹ nazwê (string o z góry ograniczonej d³ugoœci),
- sposób po³¹czenia z serwerem (sieæ lub komunikacja lokalna przez gniazda UNIX),
- adres serwera (adres IPv4 i numer portu lub œcie¿kê do gniazda UNIX serwera).
##Zadanie 1

Komunikacja klientów i serwera odbywa siê z u¿yciem protoko³u strumieniowego.

Wskazówka: na ³¹czu strumieniowym mo¿na zasymulowaæ komunikacjê datagramow¹ korzystaj¹c z TLV (Type-Length-Value) - najpierw wysy³ana jest sta³ej wielkoœci (np. 1 bajt) wartoœæ mówi¹c¹ o typie komunikatu/danych, póŸniej sta³ej wielkoœci (np. 2 bajty) wartoœæ mówi¹c¹ o d³ugoœci przesy³anych danych, a¿ wreszcie przesy³ane s¹ faktyczne dane w iloœci okreœlonej chwilê wczeœniej; mo¿na te¿ np. przechowywaæ typ danych jako pierwszy bajt przesy³anej wartoœci - tak jak robi¹ to struktury opisuj¹ce adresy gniazd. U¿ycie tych technik nie jest wymagane w zadaniu.

## Zadanie 2

Komunikacja klientów i serwera odbywa siê z u¿yciem protoko³u datagramowego.