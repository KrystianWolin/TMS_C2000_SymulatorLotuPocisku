
#ifdef SIMULATOR

extern unsigned int* KEYS;

class R_P_KEYBOARD_TMSLAB
{
public:
void PartialRefresh();
void InitKB(int Flt);
void ClearKB();
unsigned char GetKey();
unsigned char CheckKey(unsigned char KeyCode);

private:
volatile int* m_ActualKey;
volatile int KBState[4*4];
int m_Col;
int m_Row;
int m_Filter;

};


#else

class R_P_KEYBOARD_TMSLAB
{
public:
void PartialRefresh();
void InitKB(int Flt);
void ClearKB();
unsigned char GetKey();
unsigned char CheckKey(unsigned char KeyCode);
void SetColumn(unsigned char column);
unsigned char GetRows();
unsigned char GetRow(unsigned char row);

private:
volatile int* m_ActualKey;
volatile int KBState[4*4];
int m_Col;
int m_Row;
int m_Filter;

};

#endif
