

#define LCD_STATE_RESET 0
#define LCD_STATE_RD 1
#define LCD_STATE_WR 2
#define LCD_STATE_IDLE 3

#define EnterCritical

#define LeaveCritical

void Wait_NOPs(unsigned int nop_cnt);
//---------------------------------------------------------
#define LCD_CMD_TEXT_HOME				0x0040
#define LCD_CMD_TEXT_AREA				0x0041

#define LCD_CMD_GRAPH_HOME				0x0042
#define LCD_CMD_GRAPH_AREA				0x0043

#define LCD_CMD_SET_OFFSET				0x0022
#define LCD_CMD_SET_ADR_PTR				0x0024

#define LCD_CMD_SET_CURSOR_PTR			0x0021
#define LCD_CMD_CURSOR_PATT_8			0x00A7

#define LCD_CMD_SET_AUTO_WRITE			0x00B0
#define LCD_CMD_SET_AUTO_READ			0x00B1
#define LCD_CMD_RESET_AUTO_WRITE		0x00B2

#define LCD_CMD_DATA_WRITE_INC_ADP		0x00C0
#define LCD_CMD_DATA_READ_INC_ADP		0x00C1
#define LCD_CMD_DATA_WRITE_DEC_ADP		0x00C2
#define LCD_CMD_DATA_READ_DEC_ADP		0x00C3
#define LCD_CMD_DATA_WRITE_STOP_ADP		0x00C4
#define LCD_CMD_DATA_READ_STOP_ADP		0x00C5

#define LCD_CMD_OR_MODE					0x0080
#define LCD_CMD_AND_MODE				0x0083
#define LCD_CMD_EXOR_MODE				0x0081

#define LCD_CMD_TEXT_ON_GRAPH_OFF		0x0094
#define LCD_CMD_TEXT_OFF_GRAPH_ON		0x009b
#define LCD_CMD_TEXT_ON_GRAPH_ON		0x009F

#define LCD_CMD_CURSOR_ON_BLINK_ON		0x0093
#define LCD_CMD_DISPLAY_OFF				0x0090
//---------------------------------------------------------
#define LCD_Normal_Ready				0x0003
#define LCD_Auto_Read_Ready				0x0004
#define LCD_Auto_Write_Ready			0x0008
#define LCD_TEXT_SIZE					(40*16)

#define LCD_RAM_TEXT_HOME				0x0100
#define LCD_RAM_GRAPH_HOME				0x0800

#define LCD_TEXT_AREA					40
#define LCD_GRAPH_AREA					40
#define LCD_EXT_CHAR_GEN				0x4000
