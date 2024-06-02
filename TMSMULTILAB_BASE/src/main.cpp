////////////////////////////////////////////////////////
///////      Podstawowa konfiguracja
////////////////////////////////////////////////////////

//#define CPP_EXAMPLE

#define BUFFERSYNC

////////////////////////////////////////////////////////


#include "main.h"
#include "math.h"
//#include <iostream>
using namespace std;


//obiekt regulacji
class ObiektRegulacji {

    double katArmaty;
    double cel;
    unsigned int predkosc;

public:
    ObiektRegulacji() {
        katArmaty = 45;
        cel = 100;
        predkosc = 5;
    }

    void setKat(double kat) {
        if (kat < 0 || kat>90) {}
        else katArmaty = kat;
    }

    double getKat() {
        return katArmaty;
    }

    void setCelReg(double cel) {
        if (cel <= 10 || cel >= 230) {}
        else this->cel = cel;
    }

    double getCelReg() {
        return cel;
    }

    void setPredkosc(unsigned int predkosc) {
        if (predkosc <= 0) {}
        else this->predkosc = predkosc;
    }

    unsigned int getPredkosc() {
        return predkosc;
    }
};
//

//pid
class PID {

    float m_out;
    float m_in;

    float m_Tp, m_Kp, m_Ki, m_Kd;
    float m_sum, m_Smin, m_Smax;
    float m_prevIn;

public:
    PID(float Kp, float Ti, float Td, float Tp, float Smin, float Smax) {
        m_Tp = Tp; m_Kp = Kp;
        m_Ki = Kp * Tp / Ti;
        m_Kd = Kp * Td / Tp;
        m_in = 0; m_prevIn = 0; m_out = 0; m_sum = 0;
        m_Smin = Smin; m_Smax = Smax;
    }

    void setInput(float input) {
        m_in = input;
    }

    void Calculate() {
        float deltaIn = m_in - m_prevIn;
        m_sum += m_in * m_Ki;
        if (m_sum > m_Smax) m_sum = m_Smax;
        if (m_sum < m_Smin) m_sum = m_Smin;
        m_out = m_sum + m_in * m_Kp + deltaIn * m_Kd;
    }

    float getOutput() {
        return m_out;
    }

};
//



unsigned int wysEkranu = 128;
unsigned int szeEkranu = 240;
unsigned int offsetPodloza = 24;
double mX = 240;
double mY = 0.002;

int miejsceLadowania = 0;

unsigned int czestOdswAnimacji = 500;//czestotliwosc odswierzania animacji lotu
unsigned int czasAnimacji = (int)(1000 / czestOdswAnimacji);//wyliczenie czasu animacji
unsigned int preScaleZapisany = 10000;//moment preskalera licznika uzytkownika w ktorym zakonczyla sie analiza
bool koniecAnalizy = false;//ustawiana i restartowana w celu wyznaczenia momentu analizy
bool koniecRysowania = false;//ustawiana i restartowana w celu wyznaczenia momentu rysowania

ObiektRegulacji objReg;
PID regPID(1, 1, 0, 1, 0, 100);

unsigned int prepreScale = 0;
/////////////////////////////


unsigned long* ekran; // Adres obszaru graficznego LCD [8*128*2]
#ifdef TMSLAB_C2000
unsigned char* textEkran; // Adres obszaru tekstowego [40*16/2]
#endif

#ifdef TMSLAB_WIN
unsigned short int* textEkran; // Adres obszaru tekstowego [40*16/2]
extern int (*PartialRefresh)();
char credits[43] = "-         Regulator lotu pocisku         -";  // Tekst wyswietlany w naglowku symulatora
long Timer2IsrPeriod = 1; // okres pracy symulowanego licznika Timer2 podany w przyblizeniu w ms
#endif

int Tim = 0;                // Licznik uzytkownika
unsigned int preScale = 0;  // Preskaler licznika uzytkownika
volatile char EnableRefresh = 0;    //Zezwolenie na odswiezenie zawartosci pamieci graficznej

R_P_LCD_TMSLAB LCD;             // Obiekt obslugujacy LCD
R_P_KEYBOARD_TMSLAB KEYBOARD;   // Obiekt obslugujacy klawiature
R_P_LEDBAR_TMSLAB LEDBAR;       // Obiekt obslugujacy diody LED


#ifdef TMSLAB_C2000
#define M_PI 3.14
#endif

//int i=0;
unsigned char Komentarze[] = "___________________";
int rozmiar = 20;

enum Tryb{
    RYSOWANIE,
    WYKRESY
};

Tryb trybPracy = RYSOWANIE;

int daneZadana[105];
int daneWynik[105];
int daneUchyb[105];

int main()
{
    for(int i=0; i<105;i++)
    {
        daneZadana[i]=0;
        daneWynik[i]=0;
        daneUchyb[i]=0;
    }

    SetUpPeripherials();

#ifdef TMSLAB_C2000
    LCD.LCD_Init(ekran, textEkran);
#endif

#ifdef TMSLAB_WIN
    LCD.LCD_Init(&ekran, &textEkran);
#endif

    KEYBOARD.InitKB(100);

    LEDBAR.InitLedBar();

    InitData();

    EnableInterrupts();


    while (1)
    {
        EnableRefresh = 1;
        LCD.Synchronize();
        EnableRefresh = 0;

#ifdef TMSLAB_WIN
        if (PartialRefresh()) return 0;
#endif
    }
}

#ifdef TMSLAB_C2000

interrupt
void Timer2Isr()
{


#ifdef BUFFERSYNC
    if (EnableRefresh)
        LCD.PartialRefresh();
#endif

    KEYBOARD.PartialRefresh();


    prepreScale++;


    if (prepreScale == 50)
    {
        preScale++;
        prepreScale = 0;

        if (preScale % 1000 == 0) //wywolana co 1[sek] = 1000[ms]
        {
            unsigned char Text[] = "Czas[s] = _____";

                Text[11] = (Tim / 1000 + '0');
                Text[12] = (Tim / 100 - 10 * (Tim / 1000) + '0');
                Text[13] = Tim / 10 - 10 * (Tim / 100) + '0';
                Text[14] = Tim % 10 + '0';

            PrintText(textEkran, Text, 15, 25, 0);

            Tim++;

            if(trybPracy == 1)
                Wykresy();
        }

        /*
        1. (wywolana raz na poczatku) wywo³uj funkcjê analiza i regulacja
            jesli preScale==1 -> koniecRysowania=FALSE + wywo³aj regulacjê + zapisz czas konca regulacji (preScaleZapisany)
        2. co 20[ms] wywo³uj funcjê rysuj
            jesli (preScale-preScaleZapisany)%20==0 && !koniecRysowania -> wywolaj rysowanie
            jesli koniec rysowania zmien zmienna -> koniecRysowania=TRUE
        3. (wywolana raz na koniec) delay na 1000[ms] - przerwa miedzy lotami
        4. (wywolana raz na koniec) reset zmiennej preScale
        */

        if (preScale == 1)
        {
            koniecAnalizy = false;
            koniecRysowania = false;
            //AnalizaIRegulacja(objReg, regPID);
            preScaleZapisany = preScale;
        }

        if (!koniecRysowania && (preScale - preScaleZapisany) % czasAnimacji == 0)
        {
            RysowanieTrajektorii();
        }

        if (koniecRysowania && preScale == (preScaleZapisany + 2000))
        {
            AnalizaIRegulacja();
            preScale = 0;
            if(trybPracy == 0)
                ClearScreen();
            DodajDaneZadana((int)objReg.getCelReg());
        }

        unsigned char Key = KEYBOARD.GetKey();

        if (Key == 6) objReg.setCelReg(objReg.getCelReg() + 0.1); //przycisk 6
        if (Key == 8) objReg.setCelReg(objReg.getCelReg() - 0.1); //przycisk *

        if (Key == 4)  trybPracy=WYKRESY; //przycisk 5
        if (Key == 2) trybPracy=RYSOWANIE; //przycisk C

        unsigned char Text1[] = "Wartosc zadana______";

            for (int j = 0; j < rozmiar; j++)
            {
                Komentarze[j] = Text1[j];
            }

            int celR = (int)objReg.getCelReg();

            Komentarze[17] = (celR / 100 - 10 * (celR / 1000) + '0');
            Komentarze[18] = celR / 10 - 10 * (celR / 100) + '0';
            Komentarze[19] = celR % 10 + '0';

        PrintText(textEkran, Komentarze, 20, 0, 5);
    }

}


unsigned long ADRFTECHED = 0;
interrupt
void NoIsr()
{
    ADRFTECHED = PieCtrlRegs.PIECTRL.bit.PIEVECT;
    asm(" ESTOP0");
}

void EnableInterrupts()
{

    EALLOW;
    //Ustawienie wektorow przerwan
    unsigned long VECTBEG = (unsigned long)&PieVectTable;
    unsigned long VECTLAST = (unsigned long)&PieVectTable
        + sizeof(PieVectTable);
    while (VECTBEG >= VECTLAST)
        *(unsigned long*)VECTBEG++ = (unsigned long)NoIsr;
    PieVectTable.TIMER2_INT = Timer2Isr;

    CpuTimer2Regs.TCR.bit.TIE = 1;
    CpuTimer2Regs.TCR.bit.TRB = 1;

    IER = IER_MASK;//Odblokuj przerwania
    asm(" push ier");
    asm(" pop dbgier");

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    PieCtrlRegs.PIEACK.all = 0xffff;
    EDIS;
    EINT;
}

void SetUpPeripherials()
{
    SetupCoreSystem();
    ekran = (unsigned long*)0x8000;              //[8*128*2]
    textEkran = (unsigned char*)0x8a00;//[40*16/2]
    EALLOW;
    //Okres licznika T2
    CpuTimer2Regs.PRD.all = System_Clk * Timer2ISR_Period;
    EDIS;
}
extern "C"
{
    int _system_pre_init()
    {
        EALLOW;
        WdRegs.WDWCR.all = 0x68;
        EDIS;
        return (1);
    }
}
#endif

#ifdef TMSLAB_WIN
void EnableInterrupts()
{}
void SetUpPeripherials()
{}
void Timer2Isr()
{
    preScale++;


    if (preScale % 1000 == 0) //wywolana co 1[sek] = 1000[ms]
    {
        unsigned char Text[] = "Czas[s] = _____";

            Text[11] = (Tim / 1000 + '0');
            Text[12] = (Tim / 100 - 10 * (Tim / 1000) + '0');
            Text[13] = Tim / 10 - 10 * (Tim / 100) + '0';
            Text[14] = Tim % 10 + '0';

        PrintText(textEkran, Text, 15, 25, 0);

        Tim++;

        if(trybPracy == 1)
            Wykresy();
    }

    /*
    1. (wywolana raz na poczatku) wywo³uj funkcjê analiza i regulacja
        jesli preScale==1 -> koniecRysowania=FALSE + wywo³aj regulacjê + zapisz czas konca regulacji (preScaleZapisany)
    2. co 20[ms] wywo³uj funcjê rysuj
        jesli (preScale-preScaleZapisany)%20==0 && !koniecRysowania -> wywolaj rysowanie
        jesli koniec rysowania zmien zmienna -> koniecRysowania=TRUE
    3. (wywolana raz na koniec) delay na 2000[ms] - przerwa miedzy lotami
    4. (wywolana raz na koniec) reset zmiennej preScale
    */

    if (preScale == 1)
    {
        koniecAnalizy = false;
        koniecRysowania = false;
        //AnalizaIRegulacja(objReg, regPID);
        preScaleZapisany = preScale;
    }

    if (!koniecRysowania && (preScale - preScaleZapisany) % czasAnimacji == 0)
    {
        RysowanieTrajektorii();
    }

    if (koniecRysowania && preScale == (preScaleZapisany + 2000))
    {
        AnalizaIRegulacja();
        preScale = 0;
        if(trybPracy == 0)
            ClearScreen();
        DodajDaneZadana((int)objReg.getCelReg());
    }

    unsigned char Key = KEYBOARD.GetKey();

    if (Key == 6) objReg.setCelReg(objReg.getCelReg() + 0.1);
    if (Key == 4) objReg.setCelReg(objReg.getCelReg() - 0.1);

    if (Key == 7)  trybPracy=WYKRESY;
    if (Key == 9) trybPracy=RYSOWANIE;

    unsigned char Text1[] = "Wartosc zadana______";

        for (int j = 0; j < rozmiar; j++)
        {
            Komentarze[j] = Text1[j];
        }

        int celR = (int)objReg.getCelReg();

        Komentarze[17] = (celR / 100 - 10 * (celR / 1000) + '0');
        Komentarze[18] = celR / 10 - 10 * (celR / 100) + '0';
        Komentarze[19] = celR % 10 + '0';

    PrintText(textEkran, Komentarze, 20, 0, 2);

}
#endif

void InitData()
{
    for (int a = 0; a < (128 * 8); a++)
        ekran[a] = 0;
    for (int a = 0; a < (40 * 16); a++)
        textEkran[a] = ' ';

}
void ClearScreen()
{
    for (int a = 0; a < (128 * 8); a++)
        ekran[a] = 0;
}

double stopnieNaRad(double stopnie)
{
    return (stopnie * M_PI) / 180;
}

double radNaStopnie(double rad)
{
    return (rad * 180) / M_PI;
}

double pozNaRad(double x, int offset, double v) //ta funkcja nie ma sensu
{
    double stopnie = asin(((128 - 2 * offset) / mY + (9.8 * x * x) / 2) / (v * x * mX));
    return stopnie;
}


void AnalizaIRegulacja()
{
    unsigned char Text[] = "AnalizaIRegulacja___";

        for (int j = 0; j < rozmiar; j++)
        {
            Komentarze[j] = Text[j];
        }

    PrintText(textEkran, Komentarze, 20, 0, 0);


    double stopnie = objReg.getKat();

    unsigned char Text1[] = "Stopnie_przedReg____";

        for (int j = 0; j < rozmiar; j++)
        {
            Komentarze[j] = Text1[j];
        }

        Komentarze[17] = ((int)stopnie / 100 - 10 * ((int)stopnie / 1000) + '0');
        Komentarze[18] = (int)stopnie / 10 - 10 * ((int)stopnie / 100) + '0';
        Komentarze[19] = (int)stopnie % 10 + '0';

    PrintText(textEkran, Komentarze, 20, 0, 1);


    double uchyb = objReg.getCelReg() - miejsceLadowania;

    DodajDaneUchyb(uchyb);

    unsigned char Text3[] = "Uchyb_______________";

        for (int j = 0; j < rozmiar; j++)
        {
            Komentarze[j] = Text3[j];
        }

        if(uchyb<0) Komentarze[16] = '-';
        Komentarze[17] = (abs(uchyb) / 100 - 10 * (abs(uchyb) / 1000) + '0');
        Komentarze[18] = abs(uchyb) / 10 - 10 * (abs(uchyb) / 100) + '0';
        Komentarze[19] = abs(uchyb) % 10 + '0';

    PrintText(textEkran, Komentarze, 20, 20, 5);
/*
    regPID.setInput(uchyb);

    regPID.Calculate();

    double zadana = regPID.getOutput();
*/
    //cout<<"\nZadane miejsce ladowania: "<<zadana;

//    stopnie = (int)radNaStopnie(pozNaRad(zadana, offsetPodloza, objReg.getPredkosc()));
/*    if(uchyb<0) stopnie-=10; //proporcje 0-90 na 0-szerokosc ekranu
    else if(uchyb>0) stopnie+=10;
    else stopnie=stopnie;
*/

    //regulacja kata wystrzalu armaty
    stopnie+=uchyb/10;

    //cout<<" Stopnie; "<<stopnie;

    objReg.setKat(stopnie);


    unsigned char Text2[] = "Stopnie_poReg_______";

        for (int j = 0; j < rozmiar; j++)
        {
            Komentarze[j] = Text2[j];
        }

        Komentarze[17] = ((int)stopnie / 100 - 10 * ((int)stopnie / 1000) + '0');
        Komentarze[18] = (int)stopnie / 10 - 10 * ((int)stopnie / 100) + '0';
        Komentarze[19] = (int)stopnie % 10 + '0';

    PrintText(textEkran, Komentarze, 20, 0, 2);

    koniecAnalizy = true;
}

unsigned int t = 0;
unsigned int x = 0;
unsigned int y = 0;

    int ileRysowanie=0;

    void RysowanieTrajektorii()
    {
        unsigned char Text[] = "RysowanieTrajektorii";

            for (int j = 0; j < rozmiar; j++)
            {
                Komentarze[j] = Text[j];
            }

        PrintText(textEkran, Komentarze, 20, 0, 0);


        //rysowanie tla

        int dlugoscA = 10;
        int szerokoscA = 10;

        for (unsigned int w = 0; w < wysEkranu; w++)
        {
            for (unsigned int s = 0; s < szeEkranu; s++)
            {
                //podloze
                if (w <= offsetPodloza)
                    if(trybPracy == 0) SetPixel(ekran, s, (wysEkranu - w - 1));
                //punkt docelowy
                if (abs((int)(w - offsetPodloza)) <= 5 && abs((int)(s - objReg.getCelReg())) <= 4)
                    if(trybPracy == 0) SetPixel(ekran, s, (wysEkranu - w - 1 -3));
                //armata
                /*
                if(s>=5 && s<=(5+dlugoscA) && w >=(5+offsetPodloza) && w<=(5+offsetPodloza+szerokoscA))
                {
                    int xA=s*cos(stopnieNaRad(objReg.getKat()));
                    cout<<xA;
                    int yA=w*sin(stopnieNaRad(objReg.getKat()));
                    cout<<yA;
                    SetPixel(ekran, xA, (wysEkranu-yA));
                    //SetPixel(ekran, s, (wysEkranu-w));
                }
                */
            }
        }

        //z kata obliczyc punkt (x,y) konca armaty

        int xA = dlugoscA * cos(stopnieNaRad(objReg.getKat()));
        //cout<<xA;
        int yA = dlugoscA * sin(stopnieNaRad(objReg.getKat()));
        //cout<<yA;
        //narysowac linie laczaca punkt poczatkowy z (x,y) i linie wyzej, nizej
        for (int dlugosc = 0; dlugosc < dlugoscA; dlugosc++)
        {
            for (int szerokosc = -szerokoscA / 2; szerokosc < szerokoscA / 2; szerokosc++)
            {
                if(trybPracy == 0)
                SetPixel(ekran, (5 + (xA * dlugosc / dlugoscA) + szerokosc), (wysEkranu - (5 + offsetPodloza + (yA * dlugosc / dlugoscA) + szerokosc)));
            }
        }

        //wyliczanie pozycji kuli dla danej chwili
        t = (int)(preScale - preScaleZapisany) / 25; // /25 = korekta zeby trajektoria rysowala sie wolniej

        x = (unsigned int)(0.38 * objReg.getPredkosc() * t * cos(stopnieNaRad(objReg.getKat())));

        y = (unsigned int)(mY * (objReg.getPredkosc() * t * mX * sin(stopnieNaRad(objReg.getKat())) - (9.8 * t * t / 2)) + offsetPodloza);


        if (abs((int)(offsetPodloza - y)) < 3)
        {
            miejsceLadowania = x;

            unsigned char Text[] = "Nowe ladowanie = ___";

                for (int j = 0; j < rozmiar; j++)
                {
                    Komentarze[j] = Text[j];
                }

                Komentarze[17] = (miejsceLadowania / 100 - 10 * (miejsceLadowania / 1000) + '0');
                Komentarze[18] = miejsceLadowania / 10 - 10 * (miejsceLadowania / 100) + '0';
                Komentarze[19] = miejsceLadowania % 10 + '0';

            PrintText(textEkran, Komentarze, 20, 0, 4);
        }

        if (y >= offsetPodloza && y < wysEkranu && x >= 0 && x < szeEkranu) //XD
        {
            if(trybPracy == 0)
                SetPixel(ekran, x, (wysEkranu - y));
        }
        else if (y >= wysEkranu || x >= szeEkranu)
        {
        }
        else
        {
            koniecRysowania = true;
            preScaleZapisany = preScale;

            unsigned char Text[] = "Koniec rysowania____";

                for (int j = 0; j < rozmiar; j++)
                {
                    Komentarze[j] = Text[j];
                }

            PrintText(textEkran, Komentarze, 20, 0, 0);

            DodajDaneWynik(miejsceLadowania);
        }
    }

    void Wykresy()
    {
        ClearScreen();

        //240x128
        for(unsigned int x=1; x<szeEkranu; x++)
        {
            for(unsigned int y=1; y<wysEkranu; y++)
            {
                //wykres lewo (wartosc zadana i uzyskana)
                if(x==5 && y>=wysEkranu-75 && y<wysEkranu-5) SetPixel(ekran, x, y);
                if(y==wysEkranu-5 && x>5 && x<110) SetPixel(ekran, x, y);

                //wykres prawo (uchyb)
                if(x==5+120 && y>=wysEkranu-75 && y<wysEkranu-5) SetPixel(ekran, x, y);
                if(y==wysEkranu-40 && x>5+120 && x<110+120) SetPixel(ekran, x, y);
            }
        }
        for(int i=0; i<105; i++)
        {
            //wykresZadana
            SetPixel(ekran, (i+5), (int)(wysEkranu-(daneZadana[i]/240.0*70+5)));
            //wykresWynik
            SetPixel(ekran, (i+5), (int)(wysEkranu-(daneWynik[i]/240.0*70+5)));

            //wykresUchyb
            SetPixel(ekran, (i+5+120), (int)(wysEkranu-(daneUchyb[i]/240.0*70+40)));
        }
    }


void DodajDaneZadana(int liczba)
{
    for(int i=0; i<104; i++)
    {
        daneZadana[i]=daneZadana[i+1];
    }
    daneZadana[104]=liczba;
}

void DodajDaneWynik(int liczba)
{
    for(int i=0; i<104; i++)
        {
            daneWynik[i]=daneWynik[i+1];
        }
    daneWynik[104]=liczba;
}

void DodajDaneUchyb(int liczba)
{
    for(int i=0; i<104; i++)
        {
            daneUchyb[i]=daneUchyb[i+1];
        }
    daneUchyb[104]=liczba;
}
