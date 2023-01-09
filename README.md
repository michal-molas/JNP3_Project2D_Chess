# JNP3_Project2D_Chess

Jako projekt zaliczeniowy Direct2D, napisałem szachy dla dwóch graczy. 

Aplikacja działa w trybie pełnoekranowym.

Z wymaganych elementów zaimplementowałem:
- mapy bitowe: czarne i białe fiury (poza pionem) oraz przycisk opuszczenia programu.
- napisy: napisy wyświetlane w przypadku szacha, mata i pata.
- gradienty: gradienty promieniste dla pionów czarnych oraz białych (odwzorowane w programie graficznym w bitmapach pozostałych figur).
- ścieżki: kształt piona.
- prostokąty: pola planszy.

Animacje:
- obrót planszy po ruchu (tak że gra jest widziana z perspektywy gracza rozgrywającego turę).
- obroty figur przy obrocie planszy.
- pulsowanie aktualnie wybranej figury.

Zaimplementowane elementy rozgrywki:
- Wybieranie figury
- Wyświetlanie dozwolonych pól ruchu wybranej figury
- Ruch figurą

Częściowo zaimplementowane elementy rozgrywki:
- Szachowanie działa w większości przypadków, jednak czasem król może wejść na sachowane pole, co nie powinno zachodzić.
- Mat oraz pat. Ze względu na poprzedni punkt zdarza się że mat/pat nie jest zaliczany, ponieważ król może poruszyć się na nielegalne pole

Niezaimplementowane elementy rozgrywki:
- Promocja piona
- Roszada
- En passant (bicie w przelocie)
