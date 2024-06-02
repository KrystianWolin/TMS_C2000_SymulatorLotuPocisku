#ifdef SIMULATOR
class R_P_LCD_TMSLAB
{
public:
volatile long m_cntRefresh;                             // Etap auktualizacji
volatile int m_vsync;                                   // Flaga synchronizacji pionowej
void PartialRefresh();                                  // Procedura cyklicznego aktualizowania ekranu LCD
void Synchronize();										                  // Oczekuje zakonczenia aktualizacji wyswietlacza
void LCD_Command(unsigned int uiData);                  // Wys�anie polecenia do LCD
void LCD_Write_2Bytes(unsigned int uiData);             // Wys�anie s�owa danych do LCD
void LCD_Write_1Bytes(unsigned int uiData);             // Wys�anie bajtu danych do LCD
void LCD_Init( unsigned long **Map, unsigned short int** TMap);  // Inicjalizacja wy�wietlacza LCD Map-adres pami�ci graficznejm TMap-adres pami�ci tekstowej

private:
long *g1[2];
int g2[6];
};

#else

#define LCD_NS_TIME_ACCESS 100

class R_P_LCD_TMSLAB
{
public:
volatile long m_cntRefresh;                             // Etap auktualizacji
volatile int m_vsync;                                   // Flaga synchronizacji pionowej
void PartialRefresh();                                  // Procedura cyklicznego aktualizowania ekranu LCD
void Synchronize();										// Oczekuje zakonczenia aktualizacji wyswietlacza
void LCD_Command(unsigned int uiData);                  // Wys�anie polecenia do LCD
void LCD_Write_2Bytes(unsigned int uiData);             // Wys�anie s�owa danych do LCD
void LCD_Write_1Bytes(unsigned int uiData);             // Wys�anie bajtu danych do LCD
void LCD_Init(unsigned long *Map,unsigned char* TMap);  // Inicjalizacja wy�wietlacza LCD Map-adres pami�ci graficznejm TMap-adres pami�ci tekstowej

private:
unsigned long *g_ulMemoryMap;
unsigned char *g_ulTextMap;
unsigned int g_iLmod2;
unsigned int g_cLmod1;
unsigned int g_uiLicznik;
volatile int m_allow;
volatile int m_check_auto;
volatile int g_scStatus;

inline void LCD_Chip_Enable();
unsigned char Read_Port();
void Write_Port(unsigned long data);
void LCD_Command_Mode_Set();
inline void LCD_Data_Mode_Set();
void LCD_Set_State(unsigned char state);
//---------------------------------------------------------
void LCD_Write_Command(unsigned char data);
unsigned char LCD_Read_Status();
void LCD_Write_Data(unsigned char data);
unsigned char LCD_Read_Data();
};
#endif
