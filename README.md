# Išmanioji automobilių stovėjimo aikštelė

## Projekto aprašymas

Šio projekto tikslas -- sukurti nedidelį išmaniosios automobilių
stovėjimo aikštelės prototipą naudojant **Arduino Uno** ir **Tinkercad
Circuits** simuliavimo aplinką.

Sistema stebi dvi parkavimo vietas, nustato, ar jos užimtos, parodo
laisvų vietų skaičių LCD ekrane ir valdo įvažiavimo užtvarą naudojant
servo variklį. Papildomai naudojami LED indikatoriai ir garsinis
signalas.

Projektas sukurtas kaip robotikos laboratorinio darbo prototipas,
kuriame derinami jutikliai, valdiklis, išvesties įrenginiai ir
programinė valdymo logika.

## Pagrindinės funkcijos

-   Dviejų parkavimo vietų užimtumo nustatymas.
-   Automobilio aptikimas prie įvažiavimo.
-   Laisvų parkavimo vietų skaičiavimas.
-   Laisvų vietų rodymas 16x2 LCD ekrane.
-   Žalių LED indikatorių valdymas:
    -   LED įjungtas -- vieta laisva.
    -   LED išjungtas -- vieta užimta.
-   Automatinis servo užtvaro atidarymas, kai yra laisvų vietų.
-   Užtvaro uždarymas po nustatyto laiko arba automobiliui pasitraukus
    nuo įėjimo jutiklio.
-   Garsinis signalas įleidžiant automobilį arba tada, kai aikštelė
    pilna.

## Naudoti komponentai

  ------------------------------------------------------------------------
  Komponentas                                 Kiekis Paskirtis
  --------------------- ---------------------------- ---------------------
  Arduino Uno                                      1 Pagrindinis sistemos
                                                     valdiklis

  HC-SR04 ultragarsinis                            3 Automobilio ir
  jutiklis                                           parkavimo vietų
                                                     aptikimas

  Servo variklis                                   1 Įvažiavimo užtvaro
                                                     valdymas

  LCD 16x2 ekranas                                 1 Informacijos apie
                                                     aikštelę rodymas

  Žalias LED                                       2 Parkavimo vietų
                                                     būsenos indikacija

  Buzzer'is                                        1 Garsinis sistemos
                                                     signalas

  Rezistoriai                           Pagal schemą LED srovės ribojimas

  Breadboard ir                         Pagal schemą Grandinės sujungimas
  jungiamieji laidai                                 
  ------------------------------------------------------------------------

## Arduino kontaktų priskyrimas

### Ultragarsiniai jutikliai

  Jutiklis              TRIG   ECHO
  ------------------- ------ ------
  Įvažiavimas             D9     D8
  Parkavimo vieta 1      D11    D10
  Parkavimo vieta 2      D13    D12

### Kiti komponentai

  Komponentas        Arduino kontaktas
  ---------------- -------------------
  Servo signalas                    D7
  Buzzer'is                         D2
  Žalias LED 1                      D5
  Žalias LED 2                      D3

### LCD ekranas

LCD ekranas valdomas naudojant `LiquidCrystal` biblioteką:

``` cpp
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
```

Naudojami kontaktai:

  LCD signalas     Arduino kontaktas
  -------------- -------------------
  RS                              A0
  E                               A1
  D4                              A2
  D5                              A3
  D6                              A4
  D7                              A5

## Veikimo principas

1.  Arduino paleidimo metu sukonfigūruoja jutiklius, LED, buzzer'į,
    servo variklį ir LCD ekraną.
2.  Servo užtvaras nustatomas į uždarytą padėtį.
3.  Ultragarsiniai jutikliai matuoja atstumą iki objektų.
4.  Jei atstumas iki parkavimo vietos yra mažesnis nei nustatyta riba,
    vieta laikoma užimta.
5.  Programa suskaičiuoja laisvas vietas.
6.  LED indikatoriai atnaujinami pagal vietų užimtumą.
7.  LCD ekrane rodomas laisvų vietų skaičius arba pranešimas, kad
    aikštelė pilna.
8.  Jei automobilis aptinkamas prie įėjimo ir yra laisvų vietų, sistema:
    -   parodo pasveikinimo pranešimą;
    -   sugeneruoja garsinį signalą;
    -   atidaro servo užtvarą.
9.  Užtvaras uždaromas, kai praeina minimalus atidarymo laikas ir
    automobilis nebeaptinkamas, arba kai pasiekiamas maksimalus
    atidarymo laikas.

## Naudojama aptikimo riba

Programoje nustatyta tokia aptikimo riba:

``` cpp
const int detectionDistance = 15;
```

Tai reiškia, kad objektas laikomas aptiktu, kai išmatuotas atstumas yra
mažesnis nei 15 cm.

Ši reikšmė gali būti keičiama pagal simuliacijos sąlygas ir jutiklių
išdėstymą.

## Programinės įrangos sprendimai

Programoje naudojami keli svarbūs sprendimai:

### 1. Funkcija `getDistance()`

Ši funkcija siunčia impulsą HC-SR04 jutiklio TRIG kontaktui ir matuoja
atsako trukmę ECHO kontakte. Pagal gautą trukmę apskaičiuojamas atstumas
centimetrais.

### 2. Parkavimo vietų būsenos

Kiekviena vieta turi atskirą būseną:

``` cpp
bool occupied1;
bool occupied2;
```

Pagal šias būsenas apskaičiuojamas laisvų vietų skaičius.

### 3. Užtvaro būsenos

Užtvarui naudojamos dvi pagrindinės būsenos:

-   `GATE_CLOSED` -- užtvaras uždarytas.
-   `GATE_OPEN` -- užtvaras atidarytas.

Užtvaro laikas valdomas naudojant `millis()`, todėl programa gali tęsti
kitus veiksmus nelaukdama `delay(3000)` pabaigos.

### 4. LCD atnaujinimas

LCD ekranas atnaujinamas tik tada, kai pasikeičia rodomas tekstas. Tai
sumažina nereikalingą ekrano perrašymą ir gali padėti išvengti
mirgėjimo.

## Testavimo scenarijai

  -----------------------------------------------------------------------
  Testas                              Tikėtinas rezultatas
  ----------------------------------- -----------------------------------
  Abi vietos laisvos                  LCD rodo 2 laisvas vietas, abu LED
                                      įjungti

  Užimta pirma vieta                  LCD rodo 1 laisvą vietą, pirmas LED
                                      išjungtas

  Užimta antra vieta                  LCD rodo 1 laisvą vietą, antras LED
                                      išjungtas

  Abi vietos užimtos                  LCD rodo „PARKING FULL", abu LED
                                      išjungti

  Automobilis aptinkamas, yra laisvų  Buzzer'is supypsi, užtvaras
  vietų                               atsidaro

  Automobilis aptinkamas, vietų nėra  Užtvaras lieka uždarytas ir
                                      sugeneruojamas įspėjamasis signalas

  Automobilis pasitraukia nuo įėjimo  Sistema gali būti paruošta kitam
                                      automobiliui
  -----------------------------------------------------------------------

## Apribojimai

Šis projektas yra simuliacinis prototipas, todėl turi tam tikrų
apribojimų:

-   Naudojamas tik vienas įėjimo jutiklis, todėl sistema negali
    patikimai nustatyti viso automobilio pravažiavimo pro užtvarą.
-   Parkavimo vietų būsenos nustatomos pagal vieną atstumo matavimą ir
    nustatytą ribą.
-   Sistema nenumato atskiro išvažiavimo vartų mechanizmo.
-   Nėra ilgalaikio automobilių skaičiaus saugojimo atmintyje.
-   Simuliacijos rezultatai gali skirtis nuo realios fizinės grandinės
    veikimo.

## Galimi ateities patobulinimai

Ateityje projektą būtų galima patobulinti šiais būdais:

1.  Pridėti antrą įėjimo jutiklį, kad būtų galima tiksliau nustatyti
    automobilio judėjimo kryptį.
2.  Įdiegti atskirą išvažiavimo vartų sistemą.
3.  Naudoti daugiau parkavimo vietų.
4.  Pridėti raudonus LED užimtoms vietoms.
5.  Išsaugoti parkavimo vietų būsenas EEPROM atmintyje.
6.  Pridėti mygtuką rankiniam užtvaro valdymui.
7.  Naudoti būsenų filtravimą, kad trumpalaikiai klaidingi jutiklio
    matavimai nepakeistų vietos būsenos.
8.  Pridėti avarinį užtvaro sustabdymą.
9.  Sukurti išsamesnį statistikos rodymą, pavyzdžiui, įvažiavusių
    automobilių skaičių.

## Išvada

Sukurtas išmaniosios automobilių stovėjimo aikštelės prototipas sujungia
jutiklių duomenų nuskaitymą, Arduino programavimą, servo variklio
valdymą, LCD informacijos pateikimą, LED indikaciją ir garsinius
signalus.

Projektas parodo, kaip jutiklių duomenys gali būti naudojami
automatiniam sprendimų priėmimui. Sukurta sistema gali būti toliau
plečiama, pridedant daugiau parkavimo vietų, patikimesnį automobilio
judėjimo nustatymą ir papildomas saugos funkcijas.

## Autorius
-   Vardas, pavardė: Kostas Stelmokas
-   Dalykas: Įvadas į robotiką
-   Platforma: tinkercad.com
-   Valdiklis: Arduino Uno R3
