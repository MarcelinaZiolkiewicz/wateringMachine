# wateringMachine

Bardzo lubię żywe roślinki, jednak zawsze zapominam je podlewać albo je przelewam, dlatego wpadłem na pomysł zbudowania i zaprogramowania urządzenia które będzie tego wszystkiego pilnowało za mnie. Zarówno program jak i urządzenie jest w bardzo wczesnej fazie tworzenia, jest to amatorski projekt do amatorskiego zastosowania, jednak chciałbym żeby była jaknajbardziej rozbudowany. Na początku postanowiłem oprzeć całe urządzenie na Arduino Uno jednak ze względu na brakujące port GPIO oraz szybko kończącą się pamięć urządzenia zmieniłem je na Arduino Nano Every.

## Założenia 
- Podlewanie odbywa się z 3 warunków - ręcznie, jeśli jest sucho lub po jakimś z góry określonym czasie
- Każda roślina może być podlewana w innym lub ty,m samym czasie
- Urządznie wyświetla wilgotność i temperaturę w okolicy rozdzielacza - powinno wyświetlać te informacje i w razie przekroczenia ustalonych norm poinformować o tym
- Ilość wody powinna być odmierzana w ml 
- Urządzenie powinno pokazać komunikat o niskim poziomie wody i przypominać się tak nby użytkownik napewno to zauważył - w razie braku wody podlewanie nie zostanie przeprowadzone
- Urządzenie ze względu na typ pompy powinno informować o zamknięciu pojemnika
- Stacja powinna posiadać zabezpieczenie w razie obluzowania węża który zatrzyma w porę podlewanie unikając całkowitego zalania 

### Przyszłość

W przypadku pomyślnego zakończenia pracy nad podstawowymi funkcjami chciałbym dorobić desktopowa aplikacje która łączyła by sie ze stacja za pomoca przewodu usb. Można by za jej pomocą sterować urządzeniem, wybierać predefiniowane ustawienia np. dla konkretnych roślin itd. 

### Obudowa

Wszystkie części potrzebne do złożenia stacji - te samodzielne jak i zmodyfikowane elementy gotowej elektroniki będą możliwe do wydrukowania na drukarce 3D. Kiedy będą gotowe pojawi się tutaj link do thingiverse. Jednak trochę to może potrwać :P 


### Linki do dokumentacji elementów i sklepów
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
