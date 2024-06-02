
#ifndef main_CL
#define main_CL


#include "setup.h"
#include "stdlib.h"
#include "R_P_LCD_TMSLAB.h"
#include "R_P_KEYBOARD_TMSLAB.h"
#include "R_P_LEDBAR_TMSLAB.h"
//#include "square.h"
#include "stdio.h"

#ifdef TMSLAB_C2000
#include "F2837xD_device.h"
#include "systemInit.h"
#endif



void SetUpPeripherials();
void EnableInterrupts();
void InitData();
void ClearScreen();
void DrawPixels(int Key);


class ObiektRegulacji;
class PID;


double stopnieNaRad(double stopnie);
double radNaStopnie(double rad);
double pozNaRad(double x, int offset, double v);
void AnalizaIRegulacja();
void RysowanieTrajektorii();
void Wykresy();
void DodajDaneZadana(int liczba);
void DodajDaneWynik(int liczba);
void DodajDaneUchyb(int liczba);

#endif
