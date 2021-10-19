  # DOKUMENTACJA - Stacja podlewająca rośliny 
  ## Założenia
    - Prosta w obsłudze
    - Zaawansowana 
    - Konfigurowalna
    - Open source

## Przyszłość

Po zakończeniu prac nad podstawową wersją stacji, tj. pozwalającą jej na autonomiczną pracę, mam w planach napisać dodatkowy program na komputery z systemem windows oraz rozszerzyć możliwości podstawowej stacji do współpracy z tym programem. Aplikacja umożliwiałby dodatkowy podgląd na pracę stacji. 
Pozwoliło by to też na wyprodukowanie wersji bez wyświatlacza i jego kontrolera. Taka wersja miała by wolne dodatkowe piny umożliwiające podpięcie dodatkowych czujników. Możliwa jest również wersja __WiFi/Bluetooth__


 ## Uruchomienie projektu
 ```
    git clone git@github.com:MichalZiolkiewicz/wateringMachine.git
 ```


  # TODO

## **NOWE MENU**

### **Główny ekran**
    * Brak komunikatów
    Temperatura: 21*C
    Wilgotność: 65%
    Roślina 2 - 3 dni temu

   Po kliknięciu pokrętła otwiera się menu główne


 ### **Główne menu**
    -> Rosliny
    Podlej wszystkie * 
    Ustawienia 
    Test
    Czujniki
    Dane
    Cofnij

   Podlewanie wszystkich na raz ignoruje dane wilgotności czujników, nie otwiera tylko wyłączonych zaworów. - Przy wybrani tej opcji pojawi się komunikat 
   "Czy na pewno chcesz to zrobić?" -> NIE   TAK

  Po wejściu w rośliny będą do wyboru od 1-5, po wybraniu konkretnej wyświetli się konkretne menu

 ### **Menu rośliny**
    -> Tryb podlewania: automat
    Wody na cykl: 150ml
    Interwał: 12dni
    Nawodnienie: 20%
    Ostatnio podlano: 9 dni temu
    Podlej ręcznie (OK)
    Resetuj czas po ręcznym podlewaniu: OK
    Cofnij

  ### **Menu czujników** 
    -> Close: zamkniete
    - Water level: OK
    - Temperatura: 21*C
    - Wilgotność: 65%
    - Wilg 1: OK
    - Wilg 2: OK
    - Wilg 3: OK
    - Wilg 4: NIE OK
    - Wilg 5: OK 
  
  ### **Dane**
    Woda uzupełniona 7 dni temu
    R1 - automat - 4 dni temu
    R2 - wyłączona
    R3 - automat - 1 dzien temu


  ## WAŻNE
  - przebudować cały interfejs
 - przerywanie pracy w przypadku otworzenia zbiornika z woda - zawory otwarte while(waterContainerOpen); 
 - po podlaniu ręcznym powinny się resetować czasy w trybie czasowym na 0, albo zrobić menu z innymi opcjami i tam będzie moża wybrać czy resetować czy nie
 - zablokować możliwość zmiany interwałów jeśli zawór jest zamknięty
 - zrobić obsługę menu czujnikiem obrotu
 - zabezpieczenie jeżeli po kilku sekundach nie zmieni się stan czujnika wilgotności to podlewanie się zatrzyma i wyświetli się komunikat o błędzie
   - w ustawieniach dodać opcję - wyłączanie awaryjne w sekundach - po jakim czasie ma się zamknąć zawór jeśli nie wykryje wody
 - Zapisywanie czasu od ostatniego podlewania - w razie resetu arduino czas mógłby się przeciągnoć nawet o 2 tygodnie! 
 - Sumowanie mililitrów wody potrzebnej do podlania rośliny inaczej ma się pompować jak jest jeden zawór otwarty a inaczej jak wszystkie

  ## MNIEJ WAŻNE
 - wygaszanie ekranu co jakiś czas dla oszczędzania energii i komfortu użytkowania
 - poprawić napis zapisano, żeby w innych zakładkach też się tak robiło
 - W ustawieniach dodać możliwość zmiany co ile czasu ma się sprawdzać wilgotność
 - **??** komunikaty led - mruganie na zmianę, kiedy i na jakie kolory
 - **!!** może dodać czujnik wilgotności dth22 - nad zaworami, pokazywałby temperaturę i wilgotność jaka panuje obok kwiatów
 - Animacja z trzema kropkami kiedy otwarte są zawory 
 - wyświetlanie odliczania w dół pod napisem o otwartych zaworach  
 - szacowany czas na ile może wystarczyć zbiornik na podstawie wpisanych ml i czasu co ile się podlewa

## BŁĘDY DO POPRAWY
- przy automatycznym podlewaniu ciągle przeładowuje się menu - tryb czasowy -> ustawienia - wtedy się wczytuje ekran główny na zamknięcie zaworów
- komunikat o braku wody/otwartej klapie nie wyświetla się prawidłowo

## KODY BŁĘDÓW

> - x1 - błąd menu opcji
> - x2 - 
> - x3 - 
> - x4 - 
> - x5 - 


## Linki do dokumentacji elementów i sklepów
 * Arduino Uno
    * [Sklep]()
    * [Dokumentacja]()
 * Pompa 12V
    * [Sklep]()
    * [Dokumentacja]()
 * Przekaźniki
    * [Sklep]()
    * [Dokumentacja]()
 * DTH22
    * [Sklep]()
    * [Dokumentacja]()
 * Elektrozawór 12V
    * [Sklep]()
    * [Dokumentacja]()
 * Zasilacz
    * [Sklep]()
    * [Dokumentacja]()
 * Czujnik poziomu wody
    * [Sklep]()
    * [Dokumentacja]()
 * Czujnik wilgotności
    * [Sklep]()
    * [Dokumentacja]()
 * Czujnik przepływu
    * [Sklep]()
    * [Dokumentacja]()
 * Czujnik obrotu
    * [Sklep]()
    * [Dokumentacja]()
 * Gniazdo zasilania DC
    * [Sklep]()
    * [Dokumentacja]()
 * Kontaktron
    * [Sklep]()
    * [Dokumentacja]()
 * Wyświetlacz LCD 20x4
    * [Sklep]()
    * [Dokumentacja]()

## Obudowa
   - zamknięcie klapy 
      - uszczelka dookoła
      - zatrzask jak w odkurzaczu xiaomi przy złączce/pojemniku 