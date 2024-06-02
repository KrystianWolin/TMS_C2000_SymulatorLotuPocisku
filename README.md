# TMS_C2000_SymulatorLotuPocisku

## Założenia dotyczące modelu obiektu i regulatora.
Stworzenie modelu obiektu symulującego układ oparty na rzucie ukośnym. Regulator musi 
sterować kątem armaty by wystrzelony pocisk trafiał w cel (zerowy uchyb).

## Założenia związane z GUI
Wyświetlanie w czasie rzeczywistym trajektorii lotu pocisku, pokazywanie zmiennych kluczowych 
dla działania układu w czasie rzeczywistym nad trajektorią. Graficzne pokazanie miejsca, w którym 
znajduje się cel oraz miejsca wylotowego pocisku(armata). Wyświetlanie wykresów pokazujących 
zmianę w czasie wartości zadanej, miejsca lądowania pocisku jak i uchybu.

## Schemat blokowy układu regulacji z symulowanym obiektem
![Diagram regulator drawio](https://github.com/KrystianWolin/TMS_C2000_SymulatorLotuPocisku/assets/129780873/d78a3f45-18bf-4cb8-be56-4015049b65c5)

## Opis funkcjonalności oprogramowania
Oprogramowanie ma za zadanie symulację wystrzału z armaty oraz regulacji kąta na podstawie 
miejsca poprzedniego lądowania. Program wizualizuje powyższy proces poprzez rysowanie 
trajektorii oraz rysowanie wykresów.

## Opis i obsługa interfejsu użytkownika
![20231128_124423](https://github.com/KrystianWolin/TMS_C2000_SymulatorLotuPocisku/assets/129780873/cfa2cb52-f390-46c2-8172-14ebb7280229)
W górnej części ekranu widoczne są zmienne układu takie jak wyjście z regulatora w obecnej oraz 
poprzedniej iteracji, czas, miejsce lądowania, wartość zadana oraz uchyb. Dzięki podłączonej 
klawiaturze numerycznej jesteśmy w stanie zmieniać parametry układu oraz wyświetlane 
informacje: za pomocą przycisków ‘*’ oraz ‘6’ możemy zmieniać miejsce lądowania; przejście w 
tryb pracy” Rysowanie wykresów” odbywa się poprzez naciśniecie przycisku ‘5’.

![20231128_124035](https://github.com/KrystianWolin/TMS_C2000_SymulatorLotuPocisku/assets/129780873/ef28d147-f8c0-4cca-b21c-65ea31af8469)
Na ekranie rysowania wykresów widzimy te same zmienne co na ekranie trajektorii, w dolnej 
części ekranu po lewej stronie widzimy wykres wartości zadanej oraz wartości osiągniętej
natomiast po prawej widzimy wykres uchybu. Aby przejść do trybu rysowania wykresów należy 
wcisnąć przycisk ‘C’.
