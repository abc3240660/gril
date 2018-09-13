/*********************************************************************
*          Portions COPYRIGHT 2013 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDEMO.c
Purpose     : Several GUIDEMO routines
----------------------------------------------------------------------
*/

/**
  ******************************************************************************
  * @file    GUIDEMO.c
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    15-November-2013
  * @brief   Several GUIDEMO routines
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "GUIDEMO.h"
#include "common.h"
#include "ec11key.h"
#include "os.h"
#include "control.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn19;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn38;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn42;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn48;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn58;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn77;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn144;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn20;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn25;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn28;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn34;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn38;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn44;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn47;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn51;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn57;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn60;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn76;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn79;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn89;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn95;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn114;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn134;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn120;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdMedCn30;

#define UI_CONFITRM_CANCEL_STARTUP       		0
#define UI_CONFITRM_CANCEL_APLPLY        		1
#define UI_CONFITRM_CANCEL_ONLY_USE_FOR  		2
#define UI_CONFITRM_CANCEL_ONLY_USE_BYPASS	3
#define UI_CONFITRM_CANCEL_SHUTDOWN      		4
#define UI_CONFITRM_CANCEL_RESET         		5

#define UI_BYPASS_CANCEL_SHUTDOWN        		6

#define UI_CANCEL_FEED_MODE              		7

#define UI_EXIT_FEED_MODE                		8
#define UI_EXIT_SHUTDOWN                 		9
#define UI_EXIT_FLAME_ERROR              		10
#define UI_EXIT_SENSOR                   		11
#define UI_EXIT_OVER_TEMP                		12
#define UI_EXIT_FEED_NOT                 		13

#define UI_COMPLETE_SHUTDOWN             		14


/*******************************************************************
*
*       Static variables
*
********************************************************************
*/
// 多边形绘制向上箭头
static const GUI_POINT _aPointArrowUp[] = {
  {  0,  0 },
  { 8, 10 },
  { 2, 10 },
  { 2, 20 },
  {-2, 20 },
  {-2, 10 },
  {-8, 10 },
};
// 多边形绘制向下箭头
static const GUI_POINT _aPointArrowDown[] = {
  {  0,  0 },
  { -8, -10 },
  { -2, -10 },
  { -2, -20 },
  { 2, -20 },
  { 2, -10 },
  { 8, -10 },
};

// 多边形绘制向右箭头
static const GUI_POINT _aPointArrowRight[] = {
  {  0,  0 },
  { -20, -8 },
  { -20, -2 },
  { -40, -2 },
  { -40, 2 },
  { -20, 2 },
  { -20, 8 },
};

static const GUI_POINT _aPointArrowProgressLeft[] = {
  { 0,   0},// X:0 Y:0
  { 10, -5},// X:right Y:up 
  { 10,  5},// X:right Y:down
};

static const GUI_POINT _aPointArrowProgressRight[] = {
  { 0,   0},// X:0 Y:0
  { -10, 5},// X:right Y:up 
  { -10, -5},// X:right Y:down
};

// 多边形绘制圆圈四周的右上角位置的箭头
static const GUI_POINT _aPointArrowRightUp[] = {
  {  0,  0  },
  {  -10, 10},
  {  -15, -5  },
};

// 多边形绘制圆圈四周的右下角位置的箭头
static const GUI_POINT _aPointArrowRightDown[] = {
  {  0,  0  },
  {  -15, 5},
  {  -10, -10  },
};

// <屏幕操作说明温度.docx>中所述的1.1界面的所有项目
static char * SubMenuTextAll[] =
{
	"HIGH",// 500°
	"495°F",
	"490°F",
	"185°F",
	"180°F",
	"175°F",
	"HIGH SMOKE",// 160°
	"LOW SMOKE",// 160°
	"FEED",
	"SHUTDOWN",
	"- - -",
	"SETUP",
	"EXIT",
};
/*
HIGH HIGH
475  495
450  490
425  485// center
400  480
375  475

175  195
200  190
225  185 // center
200  180
175  175
*/
static char * SubMenuTextAllC[] =
{
	"HIGH",// 500°
	"495°C",
	"490°C",
	"185°C",// N times
	"180°C",
	"175°C",
	"HIGH SMOKE",// 160°
	"LOW SMOKE",// 160°
	"FEED",
	"SHUTDOWN",
	"- - -",
	"SETUP",
	"EXIT",
};

static char *MainMenuTextAll[][2]=
{
	"SET TEMP", 	"NIGHT MODE",
	"FEED",				"UNITS//°F",
	"SHUTDOWN",   "RESET",
	"SETUP",      "ABOUT",
	"HELP",       "EXIT",
	"EXIT",       ""
};

// <屏幕操作说明温度.docx>中所述的2.2界面的所有项目
static char * SetupText[] =
{
	"FACTORY RESET",
	"TEMPERATURE UNIT:F",
	"INVERT SCREEN",
	"COLOR",
	"CUSTOMER SERVICE",
	"EXIT",
};

static char * SetupTextC[] =
{
	"FACTORY RESET",
	"TEMPERATURE UNIT:C",
	"INVERT SCREEN",
	"COLOR",
	"CUSTOMER SERVICE",
	"EXIT",
};

// <屏幕操作说明温度.docx>中所述的2.4界面的所有项目
static const char * SetupColorText[] =
{
	"BLUE",
	"GREEN",
	"RED",
	"BLACK",
	"WHITE",
	"EXIT",
};

// <屏幕操作说明温度.docx>中所述的2.3界面的所有项目
static const char * SetupServiceText[] =
{
	"Camp Chef",
	"1-800-650-2433",
	"M-Th 7 am to 5 pm MST",
	"support@CampChef.com",
	"www.CampChef.com",
	"EXIT",
};

// <屏幕操作说明温度.docx>中所述的1.2界面的所有项目
static const char * HelpText[] =
{
	"Turn knob left or right to",
	"highlight menu.",
	"Press knob to select.",
	"EXIT",
};

// <屏幕操作说明温度.docx>中所述的3.3界面的所有项目
static const char * ShutdownModeText[] =
{
	"SHUTDOWN MODE",
	"15:00 REMAINING",
	"EXIT",
};

// <屏幕操作说明温度.docx>中所述的4.3界面的所有项目
static const char * FeedModeText[] =
{
	"FEED MODE",
	"08:00 REMAINING",
	"EXIT",
};

// <屏幕操作说明温度.docx>中所述的5.3界面的所有项目
static const char * StartupModeText[] =
{
	"STARTUP MODE",
	"10:00 REMAINING",
	"EXIT",
};

static const char * FactoryResetText[] =
{
	"FACTORY RESET",
	"YES",
	"NO",
};

// <屏幕操作说明温度.docx>中所述的4.2界面的所有项目
static const char * FeedHelpText[] =
{
	"Only use \"FEED\" if hopper",
	"and auger are empty.",
	"(also for first startup cycle)",
	"Push and hold knob for three",
	"seconds to begin.",
	"EXIT",
};

// <屏幕操作说明温度.docx>中所述的1.1界面中所有温度选项
// 选择成功后，该值会成为RUN界面的Target值
const short baseTempMax[] =
{
	500, // HIGH
	400,
	375,
	350,
	325,
	300,
	275,
	250,
	225,
	200,
	175,
	500, // HIGH SMOKE
	160, // LOW SMOKE
};

// <屏幕操作说明温度.docx>中所述的1.1界面的X坐标信息
// 一共有5个显示位置，通过旋钮切换共有18个选择
const short SubMenuPosArray[13][5] = 
{
	{0,  0,  140,145,145},
	{0,  150,130,145,145},
	{150,145,130,145,145},
	{145,145,130,145,145},
	{145,145,130,145,103},
	{145,145,130,103,107},
	{145,145,78, 107,155},
	{145,103,82, 155,115},
	{103,107,140,115,159},
	{107,155,85, 159,147},
	{155,115,153,147,158},
	{115,159,130,158,0  },
	{159,147,150,0,  0  },
};

// <屏幕操作说明温度.docx>中所述的3.3界面的X坐标信息
// 一共有5个显示位置，通过旋钮切换共有3个选择
const short ShutdownModePosArray[3][5] = 
{
	{0,  0,  90, 135,225},
	{0,  130,90, 220,0  },
	{130,130,210,0,  0  },
};

// <屏幕操作说明温度.docx>中所述的4.3界面的X坐标信息
// 一共有5个显示位置，通过旋钮切换共有3个选择
const short FeedModePosArray[3][5] = 
{
	{0,  0,  150, 135,225},
	{0,  170,90, 220,0  },
	{170,130,210,0,  0  },
};

// <屏幕操作说明温度.docx>中所述的5.3界面的X坐标信息
// 一共有5个显示位置，通过旋钮切换共有3个选择
const short StartupModePosArray[3][5] = 
{
	{0,  0,  150, 135,225},
	{0,  145,90, 215,0  },
	{145,130,210, 0,  0  },
};

const short FactoryResetPosArray[3][5] = 
{
	{0,  0,  150, 135,225},
	{0,  140,210, 225,0  },
	{140,220,220, 0,  0  },
};

// <屏幕操作说明温度.docx>中所述的2.2界面的X坐标信息
// 一共有7个显示位置，通过旋钮切换共有4个选择
const short SetupPosArray[6][7] = 
{
	{0,  0,  0,  105, 100,140,200},
	{0,  0,  140,58, 140,200,115},
	{0,  140,100,105, 200,115,215},
	{140,100,140,190,115,215,0  },
	{100,140,200,65, 215,0,  0  },
	{140,200,115,205,0,  0,  0  },
};

// <屏幕操作说明温度.docx>中所述的2.4界面的X坐标信息
// 一共有6个显示位置，通过旋钮切换共有6个选择
const short SetupColorPosArray[6][6] = 
{
	{0,  0,  202,205,222,205},
	{0,  220,190,222,205,205},
	{213,205,213,205,205,220},
	{205,222,190,205,215,0  },
	{222,205,185,218,0,  0  },
	{205,205,206,0,  0,  0  },
};

// 进度条橙色
#define USER_GRILL_COLOR   0x00285AEF

#define USER_COLOR1_RED   	0x002A2AA8
#define USER_COLOR2_RED   	0x00A0B0DE
#define USER_COLOR2_BLACK  	0x00262A2D
#define USER_COLOR3_GRAY   	0x00454343
#define USER_COLOR4_GRAY   	0x0073716F
#define USER_COLOR5_GRAY   	0x00818181
#define USER_COLOR6_GRAY   	0x00C6C4C2
#define USER_COLOR7_GRAY   	0x00E8E7E6

EVENT_VAL g_event_val_new;
EVENT_VAL g_event_val_last;

TEMP_VAL g_temp_val_new;
TEMP_VAL g_temp_val_last;

// 0-ok 1-FLAME ERROR 2-SENSOR ERROR
int g_fatal_error = 0;

// 0-end 1-start
int g_startup_mode = 0;
int g_run_mode = 0;
int g_feed_mode = 0;
int g_shutdown_mode = 0;

// RUN模式倒计时，时间设置以及使能
int g_run_timer_setting = 480;// 480 seconds

// 0-OK 1-温度棒异常
int g_temp1_error = 0;
int g_temp2_error = 0;
int g_temp3_error = 0;
int g_temp4_error = 0;
int g_temp5_error = 0;

// default 50%
int g_smoke_val_percent = 50;

// 表示是否初始化
// 0-未初始化, 1-已初始化
int g_factory_reseted = 0;

// 进入startup或者run标志
// 0-run, 1-startup mode
int g_startup_enable = 1;

// 蓝色字体的颜色是可以切换的
GUI_COLOR font_color = GUI_BLACK;

// 背景色可在黑白之间切换，默认为黑色
GUI_COLOR bk_color = GUI_WHITE;

// 被选择的项目要高亮显示的颜色
GUI_COLOR highlight_color = GUI_WHITE;

int g_temp_center = 0;

extern EC11_STA ec11_int_event;
extern u16 step_left;
extern u16 step_right;

void GUIDEMO_SetupUI(void);
void GUIDEMO_SetupColorUI(void);
void GUIDEMO_SubMenu(void);
void GUIDEMO_MainUI(void);
void GUIDEMO_SetupServiceUI(void);
void GUIDEMO_HelpUI(void);
void GUIDEMO_ShutdownHelpUI(void);
void GUIDEMO_ShutdownModeUI(void);
void GUIDEMO_FeedHelpUI(void);
void GUIDEMO_FeedModeUI(void);
void GUIDEMO_LowSmokeUI(void);
void GUIDEMO_StartupModeUI(void);
void GUIDEMO_OtherTempModeUI(void);
void GUIDEMO_AfterLogo(void);

extern const unsigned char gImage_p1_leftdown[1388];
extern const unsigned char gImage_p1_rightdown[1388];

extern u16 g_startup_mode_counter;
extern u16 g_run_mode_counter_mins;// minutes
extern u16 g_run_mode_counter_sec;// seconds
extern u16 g_feed_mode_counter;
extern u16 g_shutdown_mode_counter;

extern void image_display(u16 x,u16 y,u8 * imgx);
extern void GUIDEMO_TempMenu(void);
extern void GUIDEMO_SmokeMenu(void);
extern void GUIDEMO_ModeMenu(void);

int g_temp_hasbig = 1;
int g_temp_update_enable = 0;

void GUIDEMO_FactoryResetUI(void)
{
	OS_ERR err;
	int time_remain = 100;
	int menu_index = 2;
	int menu_index_last = 0;
	EC11_STA ec_sta = EC11_IDLE;
	
	int menu_index_pos1 = 0;
	int menu_index_pos2 = 0;
	int menu_index_pos3 = 0;
	int menu_index_pos4 = 0;
	int menu_index_pos5 = 0;
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	
	do
	{	
		menu_index_pos1 = 0;
		menu_index_pos2 = 0;
		menu_index_pos3 = 0;
		menu_index_pos4 = 0;
		menu_index_pos5 = 0;
		
		// menu switch
		if (menu_index != menu_index_last) {
			if (menu_index >= 3) {
				menu_index_pos1 = menu_index - 2;
			}
			
			if (menu_index >= 2) {
				menu_index_pos2 = menu_index - 1;
			}
			
			menu_index_pos3 = menu_index;
			
			if (menu_index <= 2) {
				menu_index_pos4 = menu_index + 1;
			}

			if (menu_index <= 1) {
				menu_index_pos5 = menu_index + 2;
			}
			
			GUI_SetColor(bk_color);
			GUI_SetBkColor(bk_color);
	
			// display text
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			GUI_DispStringAt("                                                    ", 90, 40);
			if (menu_index_pos1 > 0) {
				GUI_DispStringAt(FactoryResetText[menu_index_pos1-1], FactoryResetPosArray[menu_index-1][0], 40);
			}
			GUI_DispStringAt("                                                    ", 90, 90);
			if (menu_index_pos2 > 0) {
				GUI_DispStringAt(FactoryResetText[menu_index_pos2-1], FactoryResetPosArray[menu_index-1][1], 90);
			}

			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRoundedRect(125, 135, 355, 200, 10);
	
			GUI_SetColor(highlight_color);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontTradeGothicCn58);
			GUI_DispStringAt("              ", 140, 140);
			if (menu_index_pos3 > 0) {
				GUI_DispStringAt(FactoryResetText[menu_index_pos3-1], FactoryResetPosArray[menu_index-1][2], 140);
			}
			
			GUI_SetColor(font_color);
			GUI_SetBkColor(bk_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			GUI_DispStringAt("                                    ", 90, 200);
			if (menu_index_pos4 > 0) {
				GUI_DispStringAt(FactoryResetText[menu_index_pos4-1], FactoryResetPosArray[menu_index-1][3], 200);
			}
			GUI_DispStringAt("                                ", 90, 240);
			if (menu_index_pos5 > 0) {
				GUI_DispStringAt(FactoryResetText[menu_index_pos5-1], FactoryResetPosArray[menu_index-1][4], 240);
			}
		}
		
		menu_index_last = menu_index;
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			if (menu_index > 2) {
				menu_index--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			if (menu_index < 3) {
				menu_index++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT == ec_sta) {
			if (2 == menu_index) {
				ec_sta = EC11_IDLE;
			}
		} else if (EC11_BUT_LONG == ec_sta) {
			if (3 == menu_index) {
				ec_sta = EC11_IDLE;
			}
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			time_remain = 100;
		}
		
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);
	} while(EC11_IDLE == ec_sta);
	
//	if (EC11_BUT == ec_sta) {
		if (2 == menu_index) {
			bk_color = GUI_WHITE;
			font_color = GUI_BLACK;
			highlight_color = GUI_WHITE;
			g_factory_reseted = 1;
			
			g_temp_val_new.temp_unit = 0;// 华氏度
			
			g_startup_mode = 0;
			g_feed_mode = 0;
			g_shutdown_mode = 0;
			g_run_mode = 0;
			
			g_startup_mode_counter = 0;
			g_run_mode_counter_mins = 0;
			g_run_mode_counter_sec = 0;
			g_feed_mode_counter = 0;
			g_shutdown_mode_counter = 0;
			
			MOT = Control_OFF;		
			HOT = Control_OFF;
			FAN = Control_OFF;
				
			g_event_val_new.force_flush = 1;
			g_event_val_new.menu_index = 11;
			
			step_left = 0;
			step_right = 0;
			ec11_int_event = EC11_IDLE;
			GUIDEMO_SubMenu();
		} else if (3 == menu_index) {
			GUIDEMO_SetupUI();
		}
//	}
}

void GUIDEMO_FeedCmpUI(void)
{
	EC11_STA ec_sta = EC11_IDLE;

	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	GUI_SetBkColor(bk_color);
	
	GUI_SetColor(font_color);
	GUI_SetBkColor(bk_color);
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	GUI_DispStringAt("FEED COMPLETE", 130, 130);
	
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRoundedRect(320, 235, 440, 312, 10);
	
	GUI_SetBkColor(USER_COLOR1_RED);
	GUI_SetColor(highlight_color);
	GUI_SetFont(&GUI_FontTradeGothicCn77);
	GUI_DispStringAt("EXIT", 333, 235);
	
	do
	{
		MOT = Control_OFF;		//防止FEED MODE结束没有关闭控制器
		HOT = Control_OFF;
		FAN = Control_OFF;
		
		ec_sta = EC11_KEY_Scan(0);
		
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		g_event_val_new.force_flush = 1;
		g_event_val_new.menu_index = 11;
		
		GUIDEMO_SubMenu();
	}
}

void GUIDEMO_ShutdownCmpUI(void)
{
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	
	GUI_SetColor(font_color);
	GUI_SetBkColor(bk_color);
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	GUI_DispStringAt("SHUTDOWN COMPLETE", 78, 130);
	GUI_DispStringAt("CYCLE POWER TO RESET", 70, 170);

	do
	{
		MOT = Control_OFF;		//防止关机模式结束后没有关闭3个控制器
		HOT = Control_OFF;
		FAN = Control_OFF;

		delay_ms(100);

	} while(1);
}

// <屏幕操作说明温度.docx>中所述的7.3/8.2界面的所有项目
void GUIDEMO_OtherTempModeUI(void)
{
	int progress_offset = 0;
	int progress_offset_last = 0;
	int smoke_setting = 50;// 50%
	int smoke_setting_last = 0;// 50%
	int time_remain = 100;
	char dispStr[64] = "";	
	EC11_STA ec_sta = EC11_IDLE;
	TEMP_VAL temp_val = g_temp_val_new;

	if (g_smoke_val_percent != 0) {
		smoke_setting = g_smoke_val_percent;
	}
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	GUI_SetBkColor(bk_color);
	
	// display text
	GUI_SetColor(font_color);
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	if (0 == g_temp_val_new.temp_unit) {
		sprintf(dispStr, "Target %.3d°F", temp_val.target_val);
	} else {
		sprintf(dispStr, "Target %.3d°C", TEMP_F2C(temp_val.target_val));
	}
	GUI_DispStringAt(dispStr, 30, 10);
	GUI_DispStringAt("Smoke Select:", 30, 60);

	GUI_DrawCircle(310,50,35);
	GUI_DrawArc(315,50,50,50,-30,30);
	GUI_FillPolygon (&_aPointArrowRightUp[0], 3, 363, 20);
	GUI_FillPolygon (&_aPointArrowRightDown[0], 3, 365, 75);
	
	GUI_SetColor(USER_COLOR4_GRAY);
	GUI_FillRect(100, 110, 247, 140);
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRect(247, 110, 400, 140);
	
	image_display(100, 110, (u8*)gImage_p1_leftdown);
	image_display(380, 110, (u8*)gImage_p1_rightdown);
	
	GUI_SetColor(font_color);
	GUI_DispStringAt("Start:", 30, 180);
	
	GUI_DrawEllipse(210,200,12,30);
	GUI_DrawArc(200,200,50,50,-35,35);
	GUI_DrawLine(210,170,240,170);
	GUI_DrawLine(210,230,240,230);
	GUI_FillPolygon (&_aPointArrowRight[0], 7, 180, 200);
	
	GUI_SetFont(&GUI_FontTradeGothicCn38);
	GUI_DispStringAt("Push", 130, 150);
	
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	GUI_DispStringAt("3 Sec", 270, 180);
	
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRoundedRect(320, 235, 440, 312, 10);
	
	GUI_SetColor(highlight_color);
	GUI_SetBkColor(USER_COLOR1_RED);
	GUI_SetFont(&GUI_FontTradeGothicCn77);
	GUI_DispStringAt("EXIT", 333, 235);
	
	do
	{		
		if (smoke_setting_last != smoke_setting) {
			progress_offset = 225 - (225 * smoke_setting / 100);
			GUI_SetColor(USER_COLOR4_GRAY);
			
			if (progress_offset > 215) {
				progress_offset = 215;
			}
				
			if (progress_offset_last != progress_offset) {
				GUI_SetColor(USER_COLOR4_GRAY);
				GUI_FillRect(125, 110, 135+progress_offset, 140);
				GUI_SetColor(USER_COLOR1_RED);
				GUI_FillRect(135+progress_offset, 110, 370, 140);
				
				GUI_SetColor(GUI_WHITE);
				
				GUI_DrawLine(135+progress_offset, 110, 135+progress_offset, 140);
				GUI_FillPolygon (&_aPointArrowProgressLeft[0], 3, 135+progress_offset-20, 125);
				GUI_FillPolygon (&_aPointArrowProgressRight[0], 3, 135+progress_offset+20, 125);
			}
		}
		
		smoke_setting_last = smoke_setting;
		
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_RIGHT == ec_sta) {
			if (smoke_setting < 100) {
				smoke_setting +=5;
			}
			time_remain = 100;
			ec_sta = EC11_IDLE;
		} else if (EC11_LEFT == ec_sta) {
			if (smoke_setting > 0) {
				smoke_setting -=5;
			}
			time_remain = 100;
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	g_smoke_val_percent = smoke_setting;
	
	if ((EC11_BUT == ec_sta) || (0 == time_remain)) {
		g_event_val_new.force_flush = 1;
		g_event_val_new.menu_index = 11;
		
		GUIDEMO_SubMenu();
	} else if (EC11_BUT_LONG == ec_sta) {
		if (1 == g_startup_enable) {
			GUIDEMO_StartupModeUI();
		} else {
			GUIDEMO_MainUI();
		}
	}
}

// <屏幕操作说明温度.docx>中所述的5.3界面的所有项目
void GUIDEMO_StartupModeUI(void)
{
	OS_ERR err;
	char dispStr[64] = "";
	int time_remain = 1 * 60 * 10;// 10 minutes
	int menu_index = 2;
	int menu_index_last = 0;
	EC11_STA ec_sta = EC11_IDLE;
	
	int menu_index_pos1 = 0;
	int menu_index_pos2 = 0;
	int menu_index_pos3 = 0;
	int menu_index_pos4 = 0;
	int menu_index_pos5 = 0;
	
	#if FOR_DEBUG_USE
		time_remain /= 60;
	#endif
	
	g_startup_mode = 1;
	g_run_mode = 0;
	g_run_mode_counter_mins = 0;
	
	// can enter startup mode only once
	g_startup_enable = 0;
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	
	do
	{
		sprintf(dispStr,"%.2d:%.2d REMAINING", time_remain/600, (time_remain%600)/10);
		
		menu_index_pos1 = 0;
		menu_index_pos2 = 0;
		menu_index_pos3 = 0;
		menu_index_pos4 = 0;
		menu_index_pos5 = 0;
		
		// menu switch
		if (menu_index != menu_index_last) {
			if (menu_index >= 3) {
				menu_index_pos1 = menu_index - 2;
			}
			
			if (menu_index >= 2) {
				menu_index_pos2 = menu_index - 1;
			}
			
			menu_index_pos3 = menu_index;
			
			if (menu_index <= 2) {
				menu_index_pos4 = menu_index + 1;
			}

			if (menu_index <= 1) {
				menu_index_pos5 = menu_index + 2;
			}
			
			GUI_SetColor(bk_color);
			GUI_SetBkColor(bk_color);
	
			// display text
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			GUI_DispStringAt("                                                    ", 90, 40);
			if (menu_index_pos1 > 0) {
				if (menu_index_pos1 == 2) {
					GUI_DispStringAt(dispStr, StartupModePosArray[menu_index-1][0], 40);
				} else {
					GUI_DispStringAt(StartupModeText[menu_index_pos1-1], StartupModePosArray[menu_index-1][0], 40);
				}
			}
			GUI_DispStringAt("                                                    ", 90, 90);
			if (menu_index_pos2 > 0) {
				if (menu_index_pos2 == 2) {
					GUI_DispStringAt(dispStr, StartupModePosArray[menu_index-1][1], 90);
				} else {
					GUI_DispStringAt(StartupModeText[menu_index_pos2-1], StartupModePosArray[menu_index-1][1], 90);
				}
			}

			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRoundedRect(25, 135, 455, 200, 10);
	
			GUI_SetColor(highlight_color);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontTradeGothicCn58);
			GUI_DispStringAt("                                ", 70, 140);
			if (menu_index_pos3 > 0) {
				if (menu_index_pos3 == 2) {
					GUI_DispStringAt(dispStr, StartupModePosArray[menu_index-1][2], 140);
				} else {
					GUI_DispStringAt(StartupModeText[menu_index_pos3-1], StartupModePosArray[menu_index-1][2], 140);
				}
			}
			
			GUI_SetColor(font_color);
			GUI_SetBkColor(bk_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			GUI_DispStringAt("                                    ", 90, 200+5);
			if (menu_index_pos4 > 0) {
				if (menu_index_pos4 == 2) {
					GUI_DispStringAt(dispStr, StartupModePosArray[menu_index-1][3], 200+5);
				} else {
					GUI_DispStringAt(StartupModeText[menu_index_pos4-1], StartupModePosArray[menu_index-1][3], 200+5);
				}
			}
			GUI_DispStringAt("                                ", 90, 240);
			if (menu_index_pos5 > 0) {
				if (menu_index_pos5 == 2) {
					GUI_DispStringAt(dispStr, StartupModePosArray[menu_index-1][4], 240);
				} else {
					GUI_DispStringAt(StartupModeText[menu_index_pos5-1], StartupModePosArray[menu_index-1][4], 240);
				}
			}
			
			// for update time remaining only
			if (2 == menu_index) {
				GUI_SetColor(highlight_color);
				GUI_SetFont(&GUI_FontTradeGothicCn58);
			} else if (3 == menu_index) {
				GUI_SetColor(font_color);
				GUI_SetFont(&GUI_FontTradeGothicCn42);
			}
		} else {
			if (0 == time_remain%10) {
				if (2 == menu_index) {
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_DispStringAt(dispStr, StartupModePosArray[menu_index-1][2], 140);
				} else if (3 == menu_index) {
					GUI_DispStringAt(dispStr, StartupModePosArray[menu_index-1][1], 90);
				}
			}
		}
		
		menu_index_last = menu_index;
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			if (menu_index > 2) {
				menu_index--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			if (menu_index < 3) {
				menu_index++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT == ec_sta) {
			if (2 == menu_index) {
				ec_sta = EC11_IDLE;
			}
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			g_run_mode=1;//执行完点火模式才能进入运行模式
			break;
		}
		
		if (2 == menu_index) {// draw big font(GUI_FontTradeGothicCn58) need more time, so delay less time
			OSTimeDlyHMSM(0,0,0,90,OS_OPT_TIME_PERIODIC,&err);
		} else if (3 == menu_index) {
			OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);
		}
	} while(EC11_IDLE == ec_sta);
	
	g_startup_mode = 0;
	
	if (EC11_BUT == ec_sta) {
		g_event_val_new.force_flush = 1;
		
		GUIDEMO_SubMenu();
	}
	if (0 == time_remain) {
		GUIDEMO_MainUI();
	}
}

// <屏幕操作说明温度.docx>中所述的5.2界面的所有项目
void GUIDEMO_LowSmokeUI(void)
{
	char dispStr[64] = "";
	int time_remain = 100;
	EC11_STA ec_sta = EC11_IDLE;
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	GUI_SetBkColor(bk_color);
	
	// display text
	GUI_SetColor(font_color);
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	if (0 == g_temp_val_new.temp_unit) {
		GUI_DispStringAt("Target: 160°F", 30, 10);
	} else {
		sprintf(dispStr, "%.3d°C", TEMP_F2C(160));
		GUI_DispStringAt(dispStr, 30, 10);
	}
	
	GUI_DispStringAt("Smoke: 50%", 30, 60);

	GUI_SetColor(USER_COLOR4_GRAY);
	GUI_FillRect(100, 110, 247, 140);
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRect(247, 110, 400, 140);
	
	image_display(100, 110, (u8*)gImage_p1_leftdown);
	image_display(380, 110, (u8*)gImage_p1_rightdown);
	
	GUI_SetColor(font_color);
	GUI_DispStringAt("Start:", 30, 180);
	
	GUI_DrawEllipse(210,200,12,30);
	GUI_DrawArc(200,200,50,50,-35,35);
	GUI_DrawLine(210,170,240,170);
	GUI_DrawLine(210,230,240,230);
	GUI_FillPolygon (&_aPointArrowRight[0], 7, 180, 200);
	
	GUI_SetFont(&GUI_FontTradeGothicCn38);
	GUI_DispStringAt("Push", 130, 150);
	
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	GUI_DispStringAt("3 Sec", 270, 180);
	
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRoundedRect(320, 235, 440, 312, 10);
	
	GUI_SetColor(highlight_color);
	GUI_SetBkColor(USER_COLOR1_RED);
	GUI_SetFont(&GUI_FontTradeGothicCn77);
	GUI_DispStringAt("EXIT", 333, 235);
	
	do
	{
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if ((EC11_BUT == ec_sta) || (0 == time_remain)) {
		g_event_val_new.force_flush = 1;
		g_event_val_new.menu_index = 11;
		
		GUIDEMO_SubMenu();
	} else if (EC11_BUT_LONG == ec_sta) {
		if (1 == g_startup_enable) {
			GUIDEMO_StartupModeUI();
		} else {
			GUIDEMO_MainUI();
		}
	}
}

// <屏幕操作说明温度.docx>中所述的4.3界面的所有项目
void GUIDEMO_FeedModeUI(void)
{
	OS_ERR err;
	char dispStr[64] = "";
	int time_remain = 1 * 60 * 10;// 8 minutes
	int menu_index = 2;
	int menu_index_last = 0;
	EC11_STA ec_sta = EC11_IDLE;
	
	int menu_index_pos1 = 0;
	int menu_index_pos2 = 0;
	int menu_index_pos3 = 0;
	int menu_index_pos4 = 0;
	int menu_index_pos5 = 0;
	
	#if FOR_DEBUG_USE
		time_remain /= 60;
	#endif
	
	g_feed_mode = 1;
	g_run_mode = 0;
	g_run_mode_counter_mins = 0;
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	
	do
	{
		sprintf(dispStr,"%.2d:%.2d REMAINING", time_remain/600, (time_remain%600)/10);

		menu_index_pos1 = 0;
		menu_index_pos2 = 0;
		menu_index_pos3 = 0;
		menu_index_pos4 = 0;
		menu_index_pos5 = 0;
		
		// menu switch
		if (menu_index != menu_index_last) {
			if (menu_index >= 3) {
				menu_index_pos1 = menu_index - 2;
			}
			
			if (menu_index >= 2) {
				menu_index_pos2 = menu_index - 1;
			}
			
			menu_index_pos3 = menu_index;
			
			if (menu_index <= 2) {
				menu_index_pos4 = menu_index + 1;
			}

			if (menu_index <= 1) {
				menu_index_pos5 = menu_index + 2;
			}
			
			GUI_SetColor(bk_color);
			GUI_SetBkColor(bk_color);
	
			// display text
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			GUI_DispStringAt("                                                    ", 90, 40);
			if (menu_index_pos1 > 0) {
				if (menu_index_pos1 == 2) {
					GUI_DispStringAt(dispStr, FeedModePosArray[menu_index-1][0], 40);
				} else {
					GUI_DispStringAt(FeedModeText[menu_index_pos1-1], FeedModePosArray[menu_index-1][0], 40);
				}
			}
			GUI_DispStringAt("                                                    ", 90, 90);
			if (menu_index_pos2 > 0) {
				if (menu_index_pos2 == 2) {
					GUI_DispStringAt(dispStr, FeedModePosArray[menu_index-1][1], 90);
				} else {
					GUI_DispStringAt(FeedModeText[menu_index_pos2-1], FeedModePosArray[menu_index-1][1], 90);
				}
			}

			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRoundedRect(25, 135, 455, 200, 10);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(highlight_color);
			GUI_SetFont(&GUI_FontTradeGothicCn58);
			GUI_DispStringAt("                                ", 70, 140);
			if (menu_index_pos3 > 0) {
				if (menu_index_pos3 == 2) {
					GUI_DispStringAt(dispStr, FeedModePosArray[menu_index-1][2], 140);
				} else {
					GUI_DispStringAt(FeedModeText[menu_index_pos3-1], FeedModePosArray[menu_index-1][2], 140);
				}
			}
			
			GUI_SetColor(font_color);
			GUI_SetBkColor(bk_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			GUI_DispStringAt("                                    ", 90, 200+5);
			if (menu_index_pos4 > 0) {
				if (menu_index_pos4 == 2) {
					GUI_DispStringAt(dispStr, FeedModePosArray[menu_index-1][3], 200+5);
				} else {
					GUI_DispStringAt(FeedModeText[menu_index_pos4-1], FeedModePosArray[menu_index-1][3], 200+5);
				}
			}
			GUI_DispStringAt("                                ", 90, 240);
			if (menu_index_pos5 > 0) {
				if (menu_index_pos5 == 2) {
					GUI_DispStringAt(dispStr, FeedModePosArray[menu_index-1][4], 240);
				} else {
					GUI_DispStringAt(FeedModeText[menu_index_pos5-1], FeedModePosArray[menu_index-1][4], 240);
				}
			}
			
			// for update time remaining only
			if (2 == menu_index) {
				GUI_SetColor(highlight_color);
				GUI_SetFont(&GUI_FontTradeGothicCn58);
			} else if (3 == menu_index) {
				GUI_SetColor(font_color);
				GUI_SetFont(&GUI_FontTradeGothicCn42);
			}
		} else {
			if (0 == time_remain%10) {
				if (2 == menu_index) {
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_DispStringAt(dispStr, FeedModePosArray[menu_index-1][2], 140);
				} else if (3 == menu_index) {
					GUI_DispStringAt(dispStr, FeedModePosArray[menu_index-1][1], 90);
				}
			}
		}
		
		menu_index_last = menu_index;
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHTj
		if (EC11_LEFT == ec_sta) {
			if (menu_index > 2) {
				menu_index--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			if (menu_index < 3) {
				menu_index++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT == ec_sta) {
			if (2 == menu_index) {
				ec_sta = EC11_IDLE;
			}
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		if (2 == menu_index) {// draw big font(GUI_FontTradeGothicCn58) need more time, so delay less time
			OSTimeDlyHMSM(0,0,0,90,OS_OPT_TIME_PERIODIC,&err);
		} else if (3 == menu_index) {
			OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);
		}
	} while(EC11_IDLE == ec_sta);
	
	g_feed_mode = 0;
	
	if (EC11_BUT == ec_sta) {
		g_event_val_new.force_flush = 1;
		g_event_val_new.menu_index = 11;
		
		GUIDEMO_SubMenu();
	}
	
	if (0 == time_remain) {
		GUIDEMO_FeedCmpUI();
	}
}

// <屏幕操作说明温度.docx>中所述的4.2界面的所有项目
void GUIDEMO_FeedHelpUI(void)
{
	int time_remain = 100;
	EC11_STA ec_sta = EC11_IDLE;

	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	GUI_SetBkColor(bk_color);
	
	// display text
	GUI_SetColor(font_color);
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	GUI_DispStringAt(FeedHelpText[0], 30, 10);
	GUI_DispStringAt(FeedHelpText[1], 30, 55);
	GUI_DispStringAt(FeedHelpText[2], 30, 100);
	GUI_DispStringAt(FeedHelpText[3], 30, 145);
	GUI_DispStringAt(FeedHelpText[4], 30, 190);
	
	GUI_DrawEllipse(100,270,12,30);
	GUI_DrawArc(90,270,50,50,-35,35);
	GUI_DrawLine(100,240,130,240);
	GUI_DrawLine(100,300,130,300);
	GUI_FillPolygon (&_aPointArrowRight[0], 7, 80, 270);
	
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRoundedRect(320, 235, 440, 312, 10);
	
	GUI_SetColor(highlight_color);
	GUI_SetBkColor(USER_COLOR1_RED);
	GUI_SetFont(&GUI_FontTradeGothicCn77);
	GUI_DispStringAt(HelpText[3], 333, 235);
	
	do
	{
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if ((EC11_BUT == ec_sta) || (0 == time_remain)) {
		g_event_val_new.force_flush = 1;
		
		GUIDEMO_SubMenu();
	} else if (EC11_BUT_LONG == ec_sta) {
		GUIDEMO_FeedModeUI();
	}
}

// <屏幕操作说明温度.docx>中所述的3.3界面的所有项目
void GUIDEMO_ShutdownModeUI(void)
{
	OS_ERR err;
	int time_remain = 1 * 60 * 10;// 15 minutes
	int menu_index = 2;
	int menu_index_last = 0;
	char dispStr[64] = "";
	EC11_STA ec_sta = EC11_IDLE;
	
	int menu_index_pos1 = 0;
	int menu_index_pos2 = 0;
	int menu_index_pos3 = 0;
	int menu_index_pos4 = 0;
	int menu_index_pos5 = 0;	
	
	#if FOR_DEBUG_USE
		time_remain /= 60;
	#endif
	
	g_shutdown_mode = 1;
	g_run_mode = 0;
	g_run_mode_counter_mins = 0;
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	
	do
	{
		sprintf(dispStr,"%.2d:%.2d REMAINING", time_remain/600, (time_remain%600)/10);
		
		menu_index_pos1 = 0;
		menu_index_pos2 = 0;
		menu_index_pos3 = 0;
		menu_index_pos4 = 0;
		menu_index_pos5 = 0;
		
		// menu switch
		if (menu_index != menu_index_last) {
			if (menu_index >= 3) {
				menu_index_pos1 = menu_index - 2;
			}
			
			if (menu_index >= 2) {
				menu_index_pos2 = menu_index - 1;
			}
			
			menu_index_pos3 = menu_index;
			
			if (menu_index <= 2) {
				menu_index_pos4 = menu_index + 1;
			}

			if (menu_index <= 1) {
				menu_index_pos5 = menu_index + 2;
			}
			
			GUI_SetColor(bk_color);
			GUI_SetBkColor(bk_color);
			
			// display text
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			GUI_DispStringAt("                                                    ", 90, 40);
			if (menu_index_pos1 > 0) {
				if (menu_index_pos1 == 2) {
					GUI_DispStringAt(dispStr, ShutdownModePosArray[menu_index-1][0], 40);
				} else {
					GUI_DispStringAt(ShutdownModeText[menu_index_pos1-1], ShutdownModePosArray[menu_index-1][0], 40);
				}
			}
			GUI_DispStringAt("                                                    ", 90, 90);
			if (menu_index_pos2 > 0) {
				if (menu_index_pos2 == 2) {
					GUI_DispStringAt(dispStr, ShutdownModePosArray[menu_index-1][1], 90);
				} else {
					GUI_DispStringAt(ShutdownModeText[menu_index_pos2-1], ShutdownModePosArray[menu_index-1][1], 90);
				}
			}

			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRoundedRect(25, 135, 455, 200, 10);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(highlight_color);
			GUI_SetFont(&GUI_FontTradeGothicCn58);
			GUI_DispStringAt("                                ", 70, 140);
			if (menu_index_pos3 > 0) {
				if (menu_index_pos3 == 2) {
					GUI_DispStringAt(dispStr, ShutdownModePosArray[menu_index-1][2], 140);
				} else {
					GUI_DispStringAt(ShutdownModeText[menu_index_pos3-1], ShutdownModePosArray[menu_index-1][2], 140);
				}
			}
			
			GUI_SetColor(font_color);
			GUI_SetBkColor(bk_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			GUI_DispStringAt("                                    ", 90, 200+5);
			if (menu_index_pos4 > 0) {
				if (menu_index_pos4 == 2) {
					GUI_DispStringAt(dispStr, ShutdownModePosArray[menu_index-1][3], 200+5);
				} else {
					GUI_DispStringAt(ShutdownModeText[menu_index_pos4-1], ShutdownModePosArray[menu_index-1][3], 200+5);
				}
			}
			GUI_DispStringAt("                                ", 90, 240);
			if (menu_index_pos5 > 0) {
				if (menu_index_pos5 == 2) {
					GUI_DispStringAt(dispStr, ShutdownModePosArray[menu_index-1][4], 240);
				} else {
					GUI_DispStringAt(ShutdownModeText[menu_index_pos5-1], ShutdownModePosArray[menu_index-1][4], 240);
				}
			}
			
			// for update time remaining only
			if (2 == menu_index) {
				GUI_SetColor(highlight_color);
				GUI_SetFont(&GUI_FontTradeGothicCn58);
			} else if (3 == menu_index) {
				GUI_SetColor(font_color);
				GUI_SetFont(&GUI_FontTradeGothicCn42);
			}
		} else {
			if (0 == time_remain%10) {
				if (2 == menu_index) {
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_DispStringAt(dispStr, ShutdownModePosArray[menu_index-1][2], 140);
				} else if (3 == menu_index) {
					GUI_DispStringAt(dispStr, ShutdownModePosArray[menu_index-1][1], 90);
				}
			}
		}
		
		menu_index_last = menu_index;
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			if (menu_index > 2) {
				menu_index--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			if (menu_index < 3) {
				menu_index++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT == ec_sta) {
			if (2 == menu_index) {
				ec_sta = EC11_IDLE;
			}
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		if (2 == menu_index) {// draw big font(GUI_FontTradeGothicCn58) need more time, so delay less time
			OSTimeDlyHMSM(0,0,0,90,OS_OPT_TIME_PERIODIC,&err);
		} else if (3 == menu_index) {
			OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);
		}
	} while(EC11_IDLE == ec_sta);
	
	g_shutdown_mode = 0;
	
	if (EC11_BUT == ec_sta) {
		g_event_val_new.force_flush = 1;
		g_event_val_new.menu_index = 11;
		
		GUIDEMO_SubMenu();
	}
	
	if (0 == time_remain) {
		GUIDEMO_ShutdownCmpUI();
	}
}

// <屏幕操作说明温度.docx>中所述的3.2界面的所有项目
void GUIDEMO_ShutdownHelpUI(void)
{
	int time_remain = 100;
	EC11_STA ec_sta = EC11_IDLE;

	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);	
	GUI_SetBkColor(bk_color);
	
	// display text
	GUI_SetColor(font_color);
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	GUI_DispStringAt("Push and hold knob for three", 30, 25);
	GUI_DispStringAt("seconds to begin shutdown.", 30, 70);
	
	GUI_DrawEllipse(100,270,12,30);
	GUI_DrawArc(90,270,50,50,-35,35);
	GUI_DrawLine(100,240,130,240);
	GUI_DrawLine(100,300,130,300);
	GUI_FillPolygon (&_aPointArrowRight[0], 7, 80, 270);
	
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRoundedRect(320, 235, 440, 312, 10);
	
	GUI_SetColor(highlight_color);
	GUI_SetBkColor(USER_COLOR1_RED);
	GUI_SetFont(&GUI_FontTradeGothicCn77);
	GUI_DispStringAt(HelpText[3], 333, 235);

	do
	{
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			time_remain = 100;
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if ((EC11_BUT == ec_sta) || (0 == time_remain)) {
		g_event_val_new.force_flush = 1;
		
		GUIDEMO_SubMenu();
	} else if (EC11_BUT_LONG == ec_sta) {
		GUIDEMO_ShutdownModeUI();
	}
}

// <屏幕操作说明温度.docx>中所述的1.2界面的所有项目
void GUIDEMO_HelpUI(void)
{
	int time_remain = 100;
	EC11_STA ec_sta = EC11_IDLE;

	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	GUI_SetBkColor(bk_color);
	
	// display text
	GUI_SetColor(font_color);
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	GUI_DispStringAt(HelpText[0], 20, 10);
	GUI_DispStringAt(HelpText[1], 20, 50);
	GUI_DrawCircle(90,150,35);
	GUI_DrawArc(95,150,50,50,-30,30);
	GUI_FillPolygon (&_aPointArrowRightUp[0], 3, 143, 120);
	GUI_FillPolygon (&_aPointArrowRightDown[0], 3, 145, 175);
	
	GUI_DrawEllipse(100,270,12,30);
	GUI_DrawArc(90,270,50,50,-35,35);
	GUI_DrawLine(100,240,130,240);
	GUI_DrawLine(100,300,130,300);
	GUI_FillPolygon (&_aPointArrowRight[0], 7, 80, 270);
	
	GUI_DispStringAt(HelpText[2], 30, 190);
	
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRoundedRect(320, 235, 440, 312, 10);
	
	GUI_SetBkColor(USER_COLOR1_RED);
	GUI_SetColor(highlight_color);
	GUI_SetFont(&GUI_FontTradeGothicCn77);
	GUI_DispStringAt(HelpText[3], 333, 235);
	
	do
	{
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if ((EC11_BUT == ec_sta) || (0 == time_remain)) {
		g_event_val_new.force_flush = 1;
		
		GUIDEMO_SubMenu();
	}
}

// <屏幕操作说明温度.docx>中所述的2.3界面的所有项目
void GUIDEMO_SetupServiceUI(void)
{
	int time_remain = 300;
	EC11_STA ec_sta = EC11_IDLE;

	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	GUI_SetBkColor(bk_color);
	
	// display text
	GUI_SetColor(font_color);
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	GUI_DispStringAt(SetupServiceText[0], 160+15, 20-10);
	GUI_DispStringAt(SetupServiceText[1], 115+15, 65-10);
	GUI_DispStringAt(SetupServiceText[2], 60+15, 110-10);
	GUI_DispStringAt(SetupServiceText[3], 45+15, 160-10);
	GUI_DispStringAt(SetupServiceText[4], 78+15, 205-10);
	
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRoundedRect(180, 250-10, 310, 315, 10);
	
	GUI_SetColor(highlight_color);
	GUI_SetBkColor(USER_COLOR1_RED);
	GUI_SetFont(&GUI_FontTradeGothicCn77);
	GUI_DispStringAt(SetupServiceText[5], 197, 255-15);
	
	do
	{
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if ((EC11_BUT == ec_sta) || (0 == time_remain)) {
		GUIDEMO_SubMenu();
	}
}

// <屏幕操作说明温度.docx>中所述的2.4界面的所有项目
void GUIDEMO_SetupColorUI(void)
{
	int time_remain = 100;
	int menu_index = 3;
	int menu_index_last = 0;	
	EC11_STA ec_sta = EC11_IDLE;
	
	int menu_index_pos1 = 0;
	int menu_index_pos2 = 0;
	int menu_index_pos3 = 0;
	int menu_index_pos4 = 0;
	int menu_index_pos5 = 0;
	int menu_index_pos6 = 0;
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	
	do
	{
		menu_index_pos1 = 0;
		menu_index_pos2 = 0;
		menu_index_pos3 = 0;
		menu_index_pos4 = 0;
		menu_index_pos5 = 0;
		menu_index_pos6 = 0;
		
		
		// menu switch
		if (menu_index != menu_index_last) {
			if (menu_index >= 3) {
				menu_index_pos1 = menu_index - 2;
			}
			
			if (menu_index >= 2) {
				menu_index_pos2 = menu_index - 1;
			}
			
			menu_index_pos3 = menu_index;
			
			if (menu_index <= 5) {
				menu_index_pos4 = menu_index + 1;
			}

			if (menu_index <= 4) {
				menu_index_pos5 = menu_index + 2;
			}
			
			if (menu_index <= 3) {
				menu_index_pos6 = menu_index + 3;
			}
			
			GUI_SetColor(bk_color);
			GUI_FillRect(0,0,480,320);
			GUI_SetBkColor(bk_color);
			
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRoundedRect(30, 105, 450, 170, 10);
	
			// display text
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			if (menu_index_pos1 > 0) {
				GUI_DispStringAt(SetupColorText[menu_index_pos1-1], SetupColorPosArray[menu_index-1][0]-15, 20);
			}
			if (menu_index_pos2 > 0) {
				GUI_DispStringAt(SetupColorText[menu_index_pos2-1], SetupColorPosArray[menu_index-1][1]-15, 60);
			}
			GUI_SetColor(highlight_color);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontTradeGothicCn58);
			if (menu_index_pos3 > 0) {
					GUI_DispStringAt(SetupColorText[menu_index_pos3-1], SetupColorPosArray[menu_index-1][2]-15, 110);
			}
			GUI_SetColor(font_color);
			GUI_SetBkColor(bk_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			if (menu_index_pos4 > 0) {
					GUI_DispStringAt(SetupColorText[menu_index_pos4-1], SetupColorPosArray[menu_index-1][3]-15, 170);
			}
			if (menu_index_pos5 > 0) {
					GUI_DispStringAt(SetupColorText[menu_index_pos5-1], SetupColorPosArray[menu_index-1][4]-15, 210);
			}
			if (menu_index_pos6 > 0) {
					GUI_DispStringAt(SetupColorText[menu_index_pos6-1], SetupColorPosArray[menu_index-1][5]-15, 250);
			}
		}
		
		menu_index_last = menu_index;
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			if (menu_index > 1) {
				menu_index--;
			}
			time_remain = 100;
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			if (menu_index < 6) {
				menu_index++;
			}
			time_remain = 100;
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT == ec_sta) {	
			if (menu_index != 6) {
				ec_sta = EC11_IDLE;
				if (1 == menu_index) {
					font_color = GUI_CYAN;
				} else if (2 == menu_index) {
					font_color = GUI_GREEN;
				} else if (3 == menu_index) {
					font_color = GUI_RED;
				} else if (4 == menu_index) {
					if (bk_color != GUI_BLACK) {
						font_color = GUI_BLACK;
					}
				} else if (5 == menu_index) {
					if (bk_color != GUI_WHITE) {
						font_color = GUI_WHITE;
					}
				}
				// force to flush
				menu_index_last = 0;
			}
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}

		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if ((EC11_BUT == ec_sta) || (0 == time_remain)) {
		GUIDEMO_SubMenu();
	}
}

// <屏幕操作说明温度.docx>中所述的2.2界面的所有项目
void GUIDEMO_SetupUI(void)
{
	int time_remain = 100;
	int menu_index = 3;
	int menu_index_last = 0;
	char ** pSetupText = NULL;
	EC11_STA ec_sta = EC11_IDLE;
	
	int menu_index_pos1 = 0;
	int menu_index_pos2 = 0;
	int menu_index_pos3 = 0;
	int menu_index_pos4 = 0;
	int menu_index_pos5 = 0;
	int menu_index_pos6 = 0;
	int menu_index_pos7 = 0;
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	
	do
	{
		menu_index_pos1 = 0;
		menu_index_pos2 = 0;
		menu_index_pos3 = 0;
		menu_index_pos4 = 0;
		menu_index_pos5 = 0;
		menu_index_pos6 = 0;
		menu_index_pos7 = 0;
		
		if (0 == g_temp_val_new.temp_unit) {
			pSetupText = SetupText;
		} else {
			pSetupText = SetupTextC;
		}
		
		// menu switch
		if (menu_index != menu_index_last) {
			if (menu_index >= 4) {
				menu_index_pos1 = menu_index - 3;
			}
			
			if (menu_index >= 3) {
				menu_index_pos2 = menu_index - 2;
			}
			
			if (menu_index >= 2) {
				menu_index_pos3 = menu_index - 1;
			}
			
			menu_index_pos4 = menu_index;

			if (menu_index <= 5) {
				menu_index_pos5 = menu_index + 1;
			}
			
			if (menu_index <= 4) {
				menu_index_pos6 = menu_index + 2;
			}

			if (menu_index <= 3) {
				menu_index_pos7 = menu_index + 3;
			}
			
			GUI_SetColor(bk_color);
			GUI_FillRect(0,0,480,320);
			GUI_SetBkColor(bk_color);
			
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRoundedRect(10, 132, 470, 195, 10);
	
			// display text
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			if (menu_index_pos1 > 0) {
				GUI_DispStringAt(pSetupText[menu_index_pos1-1], SetupPosArray[menu_index-1][0], 20);
			}
			if (menu_index_pos2 > 0) {
				GUI_DispStringAt(pSetupText[menu_index_pos2-1], SetupPosArray[menu_index-1][1], 55);
			}
			if (menu_index_pos3 > 0) {
					GUI_DispStringAt(pSetupText[menu_index_pos3-1], SetupPosArray[menu_index-1][2], 90);
			}
			GUI_SetColor(highlight_color);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontTradeGothicCn58);
			if (menu_index_pos4 > 0) {
					GUI_DispStringAt(pSetupText[menu_index_pos4-1], SetupPosArray[menu_index-1][3], 135);
			}
			
			GUI_SetColor(font_color);
			GUI_SetBkColor(bk_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			if (menu_index_pos5 > 0) {
					GUI_DispStringAt(pSetupText[menu_index_pos5-1], SetupPosArray[menu_index-1][4], 196);
			}
			if (menu_index_pos6 > 0) {
					GUI_DispStringAt(pSetupText[menu_index_pos6-1], SetupPosArray[menu_index-1][5], 235);
			}
			if (menu_index_pos7 > 0) {
					GUI_DispStringAt(pSetupText[menu_index_pos7-1], SetupPosArray[menu_index-1][6], 270);
			}
			g_event_val_last.menu_index = menu_index;
		}
		
		menu_index_last = menu_index;
		ec_sta = EC11_KEY_Scan(0);
		
		// skip EC11_LEFT and EC11_RIGHT
		if (EC11_LEFT == ec_sta) {
			if (menu_index > 1) {
				menu_index--;
			}
			time_remain = 100;
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {
			if (menu_index < 6) {
				menu_index++;
			}
			time_remain = 100;
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT == ec_sta) {
			if (1 == menu_index) {
				//bk_color = GUI_WHITE;
				//font_color = GUI_BLACK;
			} else if (2 == menu_index) {
				time_remain = 100;
				ec_sta = EC11_IDLE;
				if (0 == g_temp_val_new.temp_unit) {
					g_temp_val_new.temp_unit = 1;// 摄氏度					
				} else {
					g_temp_val_new.temp_unit = 0;// 华氏度
				}
				// force to flush
				menu_index_last = 0;
			} else if (3 == menu_index) {
				time_remain = 100;
				ec_sta = EC11_IDLE;
				if (GUI_BLACK == bk_color) {
					bk_color = GUI_WHITE;
					font_color = GUI_BLACK;
					highlight_color = GUI_WHITE;
				} else {
					bk_color = GUI_BLACK;
					font_color = GUI_WHITE;
					highlight_color = GUI_BLACK;
				}
				// force to flush
				menu_index_last = 0;
			}
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		if (0 == time_remain) {
			break;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		if (1 == menu_index) {
			GUIDEMO_FactoryResetUI();
		} else if (4 == menu_index) {
			GUIDEMO_SetupColorUI();
		} else if (5 == menu_index) {
			GUIDEMO_SetupServiceUI();
		} else if (6 == menu_index) {
			g_event_val_new.force_flush = 1;
			
			GUIDEMO_SubMenu();
		}
	} else {
		if (0 == time_remain) {
			g_event_val_new.force_flush = 1;
			
			GUIDEMO_SubMenu();
		}
	}
}

void CalcTempVal(int center_val, int val_array[], int gap)
{
	if (500 == center_val) {
		val_array[0] = 0;
		val_array[1] = 0;
		val_array[2] = center_val;
		val_array[3] = center_val - gap;
		val_array[4] = center_val - gap*2;
	} else if ((center_val >= (500-gap)) && (center_val < 500)) {// [475,500) or [495,500)
		val_array[0] = 0;
		val_array[1] = 500;
		val_array[2] = center_val;
		val_array[3] = center_val - gap;
		val_array[4] = center_val - gap*2;
	} else if ((center_val > (500-gap*2)) && (center_val < (500-gap))) {// (450,475) or (490,495)(useless)
		val_array[0] = 500;
		val_array[1] = center_val + gap;
		val_array[2] = center_val;
		val_array[3] = center_val - gap;
		val_array[4] = center_val - gap*2;
	} else if ((center_val >= (175+gap*2)) && (center_val <= (500-gap*2))) {// [225,450] or [185,490]
		val_array[0] = center_val + gap*2;
		val_array[1] = center_val + gap;
		val_array[2] = center_val;
		val_array[3] = center_val - gap;
		val_array[4] = center_val - gap*2;
	} else if ((center_val > (175+gap)) && (center_val < (175+gap*2))) {// (200,225) or (180,185)(useless)
		val_array[0] = center_val + gap*2;
		val_array[1] = center_val + gap;
		val_array[2] = center_val;
		val_array[3] = center_val - gap;
		val_array[4] = 175;
	} else if ((center_val > 175) && (center_val <= (175+gap))) {// (175,200] or (175,180]
		val_array[0] = center_val + gap*2;
		val_array[1] = center_val + gap;
		val_array[2] = center_val;
		val_array[3] = 175;
		val_array[4] = 0;
	} else if (175 == center_val) {
		val_array[0] = center_val + gap*2;
		val_array[1] = center_val + gap;
		val_array[2] = center_val;
		val_array[3] = 0; 
		val_array[4] = 0;
	} else if (170 == center_val) {
		gap = 5;
		val_array[0] = center_val + gap*2;
		val_array[1] = center_val + gap;
		val_array[2] = 0;
		val_array[3] = 0; 
		val_array[4] = 0;
	} else if (165 == center_val) {
		gap = 5;
		val_array[0] = center_val + gap*2;
		val_array[1] = 0;
		val_array[2] = 0;
		val_array[3] = 0; 
		val_array[4] = 0;
	} else if (center_val <= 160) {
		val_array[0] = 0;
		val_array[1] = 0;
		val_array[2] = 0;
		val_array[3] = 0; 
		val_array[4] = 0;
	}
}

void center_initial(int *pCenter, int menu_index)
{
	if (7 == menu_index) {
		*pCenter = 170;
	} else if (8 == menu_index) {
		*pCenter = 165;
	} else if (9 == menu_index) {
		*pCenter = 160;
	} else if (10 == menu_index) {
		*pCenter = 155;
	} else if (11 == menu_index) {
		*pCenter = 150;
	} else if (12 == menu_index) {
		*pCenter = 145;
	} else if (13 == menu_index) {
		*pCenter = 140;
	}
}

void menu_index_initial(int *pMenu_index, int center_val, int val_array[])
{
	if (170 == center_val) {
		*pMenu_index = 7;
	} else if (165 == center_val) {
		*pMenu_index = 8;
	} else if (160 == center_val) {
		*pMenu_index = 9;
	} else if (155 == center_val) {
		*pMenu_index = 10;
	} else if (150 == center_val) {
		*pMenu_index = 11;
	} else if (145 == center_val) {
		*pMenu_index = 12;
	} else if (140 == center_val) {
		*pMenu_index = 13;
	} else {
		if (500 == val_array[2]) {
			*pMenu_index = 1;
		}
		if (500 == val_array[1]) {
			*pMenu_index = 2;
		}
		if (500 == val_array[0]) {
			*pMenu_index = 3;
		}
		if ((val_array[0]&&val_array[1]&&val_array[2]&&val_array[3]&&val_array[4]) != 0) {
			if (val_array[0] != 500) {
				*pMenu_index = 4;
			}
		}
		if (175 == val_array[3]) {
			*pMenu_index = 5;
		}
		if (175 == val_array[2]) {
			*pMenu_index = 6;
		}
	}
}

void GUIDEMO_SubMenu_old(void)
{
    int i = 0;
	int pen_size = 0;
	int time_remain = 100;
	int progress_offset = 0;
	int tmep_val_gap = 5;
	int temp_val_center = 355;
	int temp_val_array[5] = {0};
	int temp_val_center_last = 0;
	char dispStr[64] = "";
	char ** pSubMenuTextAll = NULL;
	EC11_STA ec_sta = EC11_IDLE;
	EVENT_VAL evt_val = g_event_val_new;
	TEMP_VAL temp_val = g_temp_val_new;
	
	// 5个显示位置所要显示内容在SubMenuTextAll
	// 字符串数组中的序号，从1开始
	int menu_index_pos1 = 0;
	int menu_index_pos2 = 0;
	int menu_index_pos3 = 0;
	int menu_index_pos4 = 0;
	int menu_index_pos5 = 0;
	
	int temp1_error_last = 0xFF;
	int temp2_error_last = 0xFF;
	int temp3_error_last = 0xFF;
	int temp4_error_last = 0xFF;
	int temp5_error_last = 0xFF;
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	
	GUI_SetBkColor(bk_color);
	GUI_SetColor(font_color);
	// 多边形绘制
	GUI_FillPolygon (&_aPointArrowUp[0], 7, 185-40, 10);
	GUI_FillPolygon (&_aPointArrowDown[0], 7, 185-40, 268);
	
	GUI_SetColor(USER_COLOR5_GRAY);
	GUI_SetFont(&GUI_FontTradeGothicCn19);
	GUI_DispStringAt("ONE", 305+10+3, 175-92);
	GUI_DispStringAt("TWO", 305+90+10+3, 175-92);
	GUI_DispStringAt("THREE", 302-10+12+5, 175+92-92+10);
	GUI_DispStringAt("FOUR", 302+90+10+3, 175+92-92+10);
	
	pen_size = GUI_GetPenSize();
	GUI_SetPenSize(2);
	
	GUI_DrawLine(280, 20, 465, 20);
	GUI_DrawLine(280, 225,465, 225);
	GUI_DrawLine(280, 20, 280, 225);
	GUI_DrawLine(465, 20, 465, 225);
	
	GUI_DrawLine(373, 20, 373, 225);
	GUI_DrawLine(280, 123, 465, 123);
	GUI_SetPenSize(pen_size);
	
	GUI_SetColor(USER_COLOR4_GRAY);
	GUI_FillRect(15, 270, 58, 312);
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRect(422, 270, 465, 312);
	image_display(25, 275, (u8*)gImage_p1_leftdown);
	image_display(435, 275, (u8*)gImage_p1_rightdown);
	progress_offset = 364 - (364 * g_smoke_val_percent) / 100;
	
	if (progress_offset <= 20) {
		progress_offset = 20;
	}
	if (progress_offset >= 344) {
		progress_offset = 344;
	}
	
	GUI_SetColor(USER_COLOR4_GRAY);
	GUI_FillRect(58, 270, 58+progress_offset, 312);
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRect(58+progress_offset, 270, 422, 312);
	
	GUI_SetColor(GUI_WHITE);
	GUI_DrawLine(58+progress_offset, 275, 58+progress_offset, 307);
	GUI_FillPolygon (&_aPointArrowProgressLeft[0], 3, 58+progress_offset-20, 291);
	GUI_FillPolygon (&_aPointArrowProgressRight[0], 3, 58+progress_offset+20, 291);
	
	// 被选中的项目，外围画框
	GUI_SetColor(USER_COLOR1_RED);
	GUI_SetBkColor(USER_COLOR1_RED);
	GUI_FillRoundedRect(10, 115, 275, 175, 10);	

	EC11_Clear();

	if (g_temp_center != 0) {
		temp_val_center = g_temp_center;
	}
	
	center_initial(&temp_val_center, evt_val.menu_index);

	do
	{
		menu_index_pos1 = 0;
		menu_index_pos2 = 0;
		menu_index_pos3 = 0;
		menu_index_pos4 = 0;
		menu_index_pos5 = 0;
		
		for (i=0; i<5; i++) {
			temp_val_array[i] = 0;
		}

		ec_sta = EC11_IDLE;
		evt_val = g_event_val_new;
		temp_val = g_temp_val_new;
				
		if (0 == temp_val.temp_unit) {
			pSubMenuTextAll = SubMenuTextAll;
		} else {
			pSubMenuTextAll = SubMenuTextAllC;
		}
	
		if (temp_val_center > 500) {
			temp_val_center = 500;
		}
		if (temp_val_center < 140) {
			temp_val_center = 140;
		}

		CalcTempVal(temp_val_center, temp_val_array, tmep_val_gap);
		menu_index_initial(&evt_val.menu_index, temp_val_center, temp_val_array);

		// 根据旋钮左右旋转，动态计算5个显示位置，所要显示的字符串内容
		// 旋钮旋转时才更新UI
		if ((evt_val.menu_index != g_event_val_last.menu_index) || evt_val.force_flush) {
			g_event_val_new.force_flush = 0;
			if (evt_val.menu_index >= 3) {
				menu_index_pos1 = evt_val.menu_index - 2;
			}
			
			if (evt_val.menu_index >= 2) {
				menu_index_pos2 = evt_val.menu_index - 1;
			}
			
			menu_index_pos3 = evt_val.menu_index;
			
			if (evt_val.menu_index <= 12) {
				menu_index_pos4 = evt_val.menu_index + 1;
			}

			if (evt_val.menu_index <= 11) {
				menu_index_pos5 = evt_val.menu_index + 2;
			}
			
			GUI_SetBkColor(bk_color);

			// display text
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			GUI_DispStringAt("                                ", 10, 38);
			if (menu_index_pos1 > 0) {
				if ((evt_val.menu_index > 3) && (evt_val.menu_index < 9)) {
					if (0 == g_temp_val_new.temp_unit) {
						sprintf(dispStr, "%.3d°F", temp_val_array[0]);
					} else {
						sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val_array[0]));
					}
					if (500 == temp_val_array[0]) {
						sprintf(dispStr, "%s", pSubMenuTextAll[0]);
					}
				} else {
					sprintf(dispStr, "%s", pSubMenuTextAll[menu_index_pos1-1]);
				}
				GUI_DispStringAt(dispStr, SubMenuPosArray[evt_val.menu_index-1][0]-40, 38);
			}
			GUI_DispStringAt("                                ", 10, 73);
			if (menu_index_pos2 > 0) {
				if ((evt_val.menu_index > 2) && (evt_val.menu_index < 8)) {
					if (0 == g_temp_val_new.temp_unit) {
						sprintf(dispStr, "%.3d°F", temp_val_array[1]);
					} else {
						sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val_array[1]));
					}
					if (500 == temp_val_array[1]) {
						sprintf(dispStr, "%s", pSubMenuTextAll[0]);
					}
				} else {
					sprintf(dispStr, "%s", pSubMenuTextAll[menu_index_pos2-1]);
				}
				GUI_DispStringAt(dispStr, SubMenuPosArray[evt_val.menu_index-1][1]-40, 73);
			}

			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(highlight_color);
			GUI_SetFont(&GUI_FontTradeGothicCn58);
			
			GUI_DispStringAt("                      ", 16, 115);
			if (menu_index_pos3 > 0) {
				if ((evt_val.menu_index > 1) && (evt_val.menu_index < 7)) {
					if (0 == g_temp_val_new.temp_unit) {
						sprintf(dispStr, "%.3d°F", temp_val_center);
					} else {
						sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val_center));
					}
					if (500 == temp_val_center) {
						sprintf(dispStr, "%s", pSubMenuTextAll[0]);
					}
				} else {
					sprintf(dispStr, "%s", pSubMenuTextAll[menu_index_pos3-1]);
				}
				GUI_DispStringAt(dispStr, SubMenuPosArray[evt_val.menu_index-1][2]-40, 116);
			}
			
			GUI_SetBkColor(bk_color);
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			GUI_DispStringAt("                                ", 10, 176);
			if (menu_index_pos4 > 0) {
				if ((evt_val.menu_index > 0) && (evt_val.menu_index < 6)) {
					if (0 == g_temp_val_new.temp_unit) {
						sprintf(dispStr, "%.3d°F", temp_val_array[3]);
					} else {
						sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val_array[3]));
					}
				} else {
					sprintf(dispStr, "%s", pSubMenuTextAll[menu_index_pos4-1]);
				}
				GUI_DispStringAt(dispStr, SubMenuPosArray[evt_val.menu_index-1][3]-40, 176);
			}
			GUI_DispStringAt("                                ", 10, 210);
			if (menu_index_pos5 > 0) {
				if ((evt_val.menu_index > 0) && (evt_val.menu_index < 5)) {
					if (0 == g_temp_val_new.temp_unit) {
						sprintf(dispStr, "%.3d°F", temp_val_array[4]);
					} else {
						sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val_array[4]));
					}
				} else {
					sprintf(dispStr, "%s", pSubMenuTextAll[menu_index_pos5-1]);
				}
				GUI_DispStringAt(dispStr, SubMenuPosArray[evt_val.menu_index-1][4]-40, 210);
			}
			
			g_event_val_last.menu_index = evt_val.menu_index;
		}
		
		temp_val_center_last = temp_val_center;
		
		if ((temp_val_center >= 175) && (temp_val_center <= 500)) {
			g_temp_center = temp_val_center;
		}
		
		if (0 == time_remain%10) {
			GUI_SetBkColor(bk_color);		
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn38);
			if (0 == g_temp1_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d°F", temp_val.temp1);
				} else {
					sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val.temp1));
				}
				GUI_DispStringAt(dispStr, 290+5, 125-92);
			} else {// 如果温度棒故障，则显示”-“
				if (temp1_error_last != g_temp1_error) {
					GUI_DispStringAt("          ", 290+5, 125-92-14+10);
					GUI_DispStringAt("_ _ _", 290+10, 125-92-14+10);
				}
			}
			temp1_error_last = g_temp1_error;
			if (0 == g_temp2_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d°F", temp_val.temp2);
				} else {
					sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val.temp2));
				}
				GUI_DispStringAt(dispStr, 290+92+5, 125-92);
			} else {
				if (temp2_error_last != g_temp2_error) {
					GUI_DispStringAt("          ", 290+92+5, 125-92-14+10);
					GUI_DispStringAt("_ _ _", 290+92+10, 125-92-14+10);
				}
			}
			temp2_error_last = g_temp2_error;
			if (0 == g_temp3_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d°F", temp_val.temp3);
				} else {
					sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val.temp3));
				}
				GUI_DispStringAt(dispStr, 290+5, 125+92-92+10);
			} else {
				if (temp3_error_last != g_temp3_error) {
					GUI_DispStringAt("          ", 290+5, 125+92-92-14+10+10);
					GUI_DispStringAt("_ _ _", 290+10, 125+92-92-14+10+10);
				}
			}
			temp3_error_last = g_temp3_error;
			if (0 == g_temp4_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d°F", temp_val.temp4);
				} else {
					sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val.temp4));
				}
				GUI_DispStringAt(dispStr, 290+92+5, 125+92-92+10);
			} else {
				if (temp4_error_last != g_temp4_error) {
					GUI_DispStringAt("          ", 290+92+5, 125+92-92-14+10+10);
					GUI_DispStringAt("_ _ _", 290+92+10, 125+92-92-14+10+10);
				}
			}
			temp4_error_last = g_temp4_error;
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if (temp_val_center < 500) {// menu_index = 1
				tmep_val_gap = 5;
				temp_val_center += 5;
				g_event_val_new.force_flush = 1;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if (temp_val_center > 140) {// menu_index = 13
				tmep_val_gap = 5;
				temp_val_center -= 5;
				g_event_val_new.force_flush = 1;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_LEFT_FAST == ec_sta) {// UP
			if (temp_val_center < 500) {
				if (temp_val_center < 175) {
					tmep_val_gap = 5;
					temp_val_center += 5;
				} else {
					tmep_val_gap = BIG_GAP;
					temp_val_center += BIG_GAP;
				}
				g_event_val_new.force_flush = 1;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT_FAST == ec_sta) {// DN
			if (temp_val_center > 140) {
				if (temp_val_center < 175) {// HS LS FEED SHUTD --- SETUP EXIT
					tmep_val_gap = 5;
					temp_val_center -= 5;
				} else {
					tmep_val_gap = BIG_GAP;
					temp_val_center -= BIG_GAP;
					if (temp_val_center < 175) {
						temp_val_center = 170;
					}
				}
				g_event_val_new.force_flush = 1;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;// 忽略该消息
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;// 忽略该消息
		}
		
		time_remain--;
		if (0 == time_remain) {
			time_remain = 100;
		}
		
		#if FOR_DEBUG_USE
		if (g_fatal_error != 0) {
		#else
		if ((g_fatal_error != 0) || (g_temp5_error != 0)) {
		#endif
			GUI_SetBkColor(bk_color);
			GUI_SetColor(bk_color);
			GUI_FillRect(15, 270, 465, 312);
			delay_ms(1000);

			GUI_SetColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			while(1) {
				if (1 == g_fatal_error) {
					GUI_DispStringAt("FLAME ERROR", 50, 275);
				} else {
					GUI_DispStringAt("SENSOR ERROR", 50, 275);
				}
				
				delay_ms(1000);
				
				GUI_DispStringAt("                              ", 50, 275);
				
				delay_ms(1000);
			}
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	// 退出该界面，跳转到下一个界面
	if (EC11_BUT == ec_sta) {
		g_event_val_new.force_flush = 1;
		if (12 == evt_val.menu_index) {
			GUIDEMO_SetupUI();
		} else if (11 == evt_val.menu_index) {
			GUIDEMO_HelpUI();
		} else if (10 == evt_val.menu_index) {
			GUIDEMO_ShutdownHelpUI();
		} else if (9 == evt_val.menu_index) {
			GUIDEMO_FeedHelpUI();
		} else if (8 == evt_val.menu_index) {
			GUIDEMO_LowSmokeUI();
		} else if (7 == evt_val.menu_index) {
			GUIDEMO_LowSmokeUI();
		} else if ((evt_val.menu_index >= 1)&&(evt_val.menu_index <= 6)) {
			g_temp_val_new.target_val = temp_val_center;
			GUIDEMO_OtherTempModeUI();
		} else {// 13 EXIT
			GUIDEMO_MainUI();
		}
	}
}

void GUIDEMO_SubMenu(void)
{
    int i = 0;
	int pen_size = 0;
	int time_remain = 100;
	int tmep_val_gap = 5;
	int temp_val_center = 355;
	int temp_val_center_last = 0;
	char dispStr[64] = "";
	EC11_STA ec_sta = EC11_IDLE;
	EVENT_VAL evt_val = g_event_val_new;
	TEMP_VAL temp_val = g_temp_val_new;
	
	int temp1_error_last = 0xFF;
	int temp2_error_last = 0xFF;
	int temp3_error_last = 0xFF;
	int temp4_error_last = 0xFF;
	int temp5_error_last = 0xFF;
	
	GUI_SetBkColor(bk_color);
	GUI_FillRect(0,0,480,320);
#if 0	
	GUI_SetColor(USER_COLOR1_RED);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn134);
	GUI_DispStringAt("215",      50, 55);

	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
	GUI_DispStringAt("SET TEMP",  70, 180);
#endif
	pen_size = GUI_GetPenSize();
	GUI_SetPenSize(10);
	GUI_SetColor(USER_COLOR7_GRAY);
	GUI_DrawEllipse(130,133,118,118);
	GUI_SetColor(USER_COLOR1_RED);
	GUI_DrawArc(130,133,118,118,-30,30);
	GUI_SetPenSize(pen_size);

	GUI_SetColor(USER_COLOR7_GRAY);
	GUI_SetBkColor(USER_COLOR7_GRAY);
	GUI_FillRect(260, 10, 360, 130);
	GUI_FillRect(362, 10, 462, 130);

	GUI_FillRect(260, 132, 360, 252);
	GUI_FillRect(362, 132, 462, 252);
#if 0
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn60);
	GUI_DispStringAt("120",      273, 100-80);
	GUI_DispStringAt("117",      273+102, 100-80);
	GUI_DispStringAt("119",      273, 100+110-60);
	GUI_DispStringAt("116",      273+102, 100+110-60);
#endif
	GUI_SetColor(USER_COLOR1_RED);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
	GUI_DispStringAt("ONE",      295, 165-80);
	GUI_DispStringAt("TWO",      295+102-5, 165-80);
	GUI_DispStringAt("THREE",      285, 165+110-60);
	GUI_DispStringAt("FOUR",      290+102, 165+110-60);
	
	EC11_Clear();

	if (g_temp_center != 0) {
		temp_val_center = g_temp_center;
	}

	do
	{
		ec_sta = EC11_IDLE;
		evt_val = g_event_val_new;
		temp_val = g_temp_val_new;
		
		if (temp_val_center_last != temp_val_center) {
			g_event_val_new.force_flush = 0;
		
			GUI_SetBkColor(bk_color);
			GUI_SetColor(bk_color);
			GUI_FillRect(0, 0, 255, 462);
			
			GUI_SetColor(USER_COLOR1_RED);
			
			if (0 == g_temp_val_new.temp_unit) {
				sprintf(dispStr, "%.3d", temp_val_center);
			} else {
				sprintf(dispStr, "%.3d", TEMP_F2C(temp_val_center));
			}
			if (150 == temp_val_center) {
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn79);
				GUI_DispStringAt("LOW",      75, 45);
				GUI_DispStringAt("SMOKE",    35, 110);
				
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
				GUI_DispStringAt("160 SET TEMP",  58, 185);
			} else if (155 == temp_val_center) {
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn79);
				GUI_DispStringAt("HIGH",     68, 45);
				GUI_DispStringAt("SMOKE",    35, 110);

				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
				GUI_DispStringAt("220 SET TEMP",  58, 185);
			} else if (500 == temp_val_center) {
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn114);
				GUI_DispStringAt("HIGH",     40, 65);

				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
				GUI_DispStringAt("SET TEMP",  70, 180);
			} else {
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn134);
				GUI_DispStringAt(dispStr,      50, 55);

				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
				GUI_DispStringAt("SET TEMP",  70, 180);				
			}
			
			pen_size = GUI_GetPenSize();
			GUI_SetPenSize(10);
			GUI_SetColor(USER_COLOR2_RED);
			GUI_DrawEllipse(130,133,118,118);
			GUI_SetColor(USER_COLOR1_RED);
			if (495 == temp_val_center) {
				GUI_DrawArc(130,133,118,118,(90-2-(temp_val_center-150)*360/350),90);
			} else {
				GUI_DrawArc(130,133,118,118,(90-5-(temp_val_center-150)*360/350),90);
			}
			GUI_SetPenSize(pen_size);
			
			g_event_val_last.menu_index = evt_val.menu_index;
		}
		
		temp_val_center_last = temp_val_center;
		
		if (0 == time_remain%10) {
			GUI_SetBkColor(USER_COLOR7_GRAY);		
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn60);
			if (0 == g_temp1_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d", temp_val.temp1);
				} else {
					sprintf(dispStr, "%.3d", TEMP_F2C(temp_val.temp1));
				}
				GUI_DispStringAt(dispStr, 273, 100-80);
			} else {// 如果温度棒故障，则显示”-“
				if (temp1_error_last != g_temp1_error) {
					GUI_DispStringAt("   ", 273, 100-80);
					GUI_DispStringAt("---", 273+10, 100-80);
				}
			}
			temp1_error_last = g_temp1_error;
			if (0 == g_temp2_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d", temp_val.temp2);
				} else {
					sprintf(dispStr, "%.3d", TEMP_F2C(temp_val.temp2));
				}
				GUI_DispStringAt(dispStr, 273+102, 100-80);
			} else {
				if (temp2_error_last != g_temp2_error) {
					GUI_DispStringAt("   ", 273+102, 100-80);
					GUI_DispStringAt("---", 273+102+10, 100-80);
				}
			}
			temp2_error_last = g_temp2_error;
			if (0 == g_temp3_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d", temp_val.temp3);
				} else {
					sprintf(dispStr, "%.3d", TEMP_F2C(temp_val.temp3));
				}
				GUI_DispStringAt(dispStr, 273, 100+110-60);
			} else {
				if (temp3_error_last != g_temp3_error) {
					GUI_DispStringAt("   ", 273, 100+110-60);
					GUI_DispStringAt("---", 273+10, 100+110-60);
				}
			}
			temp3_error_last = g_temp3_error;
			if (0 == g_temp4_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d", temp_val.temp4);
				} else {
					sprintf(dispStr, "%.3d", TEMP_F2C(temp_val.temp4));
				}
				GUI_DispStringAt(dispStr, 273+102, 100+110-60);
			} else {
				if (temp4_error_last != g_temp4_error) {
					GUI_DispStringAt("   ", 273+102, 100+110-60);
					GUI_DispStringAt("---", 273+102+10, 100+110-60);
				}
			}
			temp4_error_last = g_temp4_error;
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if (temp_val_center < 500) {// menu_index = 1
				tmep_val_gap = 5;
				temp_val_center += 5;
				g_event_val_new.force_flush = 1;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if (temp_val_center > 150) {// menu_index = 13
				tmep_val_gap = 5;
				temp_val_center -= 5;
				g_event_val_new.force_flush = 1;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_LEFT_FAST == ec_sta) {// UP
			if (temp_val_center < 500) {
				tmep_val_gap = BIG_GAP;
				temp_val_center += BIG_GAP;
				
				g_event_val_new.force_flush = 1;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT_FAST == ec_sta) {// DN
			if (temp_val_center > 150) {
				tmep_val_gap = BIG_GAP;
				temp_val_center -= BIG_GAP;
				
				g_event_val_new.force_flush = 1;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;// 忽略该消息
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;// 忽略该消息
		}
		
		if (temp_val_center > 500) {
			temp_val_center = 500;
		}
		if (temp_val_center < 150) {
			temp_val_center = 150;
		}
				
		time_remain--;
		if (0 == time_remain) {
			time_remain = 100;
		}
		
		#if FOR_DEBUG_USE
		if (g_fatal_error != 0) {
		#else
		if ((g_fatal_error != 0) || (g_temp5_error != 0)) {
		#endif
			GUI_SetBkColor(bk_color);
			GUI_SetColor(bk_color);
			GUI_FillRect(15, 270, 465, 312);
			delay_ms(1000);

			GUI_SetColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			while(1) {
				if (1 == g_fatal_error) {
					GUI_DispStringAt("FLAME ERROR", 50, 275);
				} else {
					GUI_DispStringAt("SENSOR ERROR", 50, 275);
				}
				
				delay_ms(1000);
				
				GUI_DispStringAt("                              ", 50, 275);
				
				delay_ms(1000);
			}
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	// 退出该界面，跳转到下一个界面
	if (EC11_BUT == ec_sta) {
		g_event_val_new.force_flush = 1;
		if (12 == evt_val.menu_index) {
			GUIDEMO_SetupUI();
		} else if (11 == evt_val.menu_index) {
			GUIDEMO_HelpUI();
		} else if (10 == evt_val.menu_index) {
			GUIDEMO_ShutdownHelpUI();
		} else if (9 == evt_val.menu_index) {
			GUIDEMO_FeedHelpUI();
		} else if (8 == evt_val.menu_index) {
			GUIDEMO_LowSmokeUI();
		} else if (7 == evt_val.menu_index) {
			GUIDEMO_LowSmokeUI();
		} else if ((evt_val.menu_index >= 1)&&(evt_val.menu_index <= 6)) {
			g_temp_val_new.target_val = temp_val_center;
			GUIDEMO_OtherTempModeUI();
		} else {// 13 EXIT
			GUIDEMO_MainUI();
		}
	}
}
/*
清除屏幕某一区域
*/
void clear_screen(x0,y0,x1,y1)
{
	GUI_SetColor(GUI_WHITE);
	GUI_FillRect(x0,y0,x1,y1);
}
void DrawAboutUI(char *strGrill_pin, char *strNet, char *strBlueTooth, char *strFW)
{
	int i;
	char dispStr[64] = "";
	char *item[]={
		"ONE",
		"TWO",
	  "THREE",
	  "FOUR"
	};
	GUI_RECT  MiddleBlank_RECT={277,175,465,204};
	//添加上部文字
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn38);
	GUI_DispStringAt("ABOUT", 277, 26);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
	sprintf(dispStr, "GRILL PIN // %s", strGrill_pin);
	GUI_DispStringAt(dispStr, 277, 60);
	sprintf(dispStr, "NETWORK // %s", strNet);
	GUI_DispStringAt(dispStr, 277, 86);
	sprintf(dispStr, "BLUETOOTH // %s", strBlueTooth);
	GUI_DispStringAt(dispStr, 277, 110);
	sprintf(dispStr, "FIRMWARE // %s", strFW);
	GUI_DispStringAt(dispStr, 277, 136);
	//中部灰色条
	GUI_SetColor(USER_COLOR3_GRAY);	
	GUI_FillRect(MiddleBlank_RECT.x0, MiddleBlank_RECT.y0, MiddleBlank_RECT.x1, MiddleBlank_RECT.y1);	
	GUI_SetBkColor(USER_COLOR3_GRAY);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("SIGNAL STRENGTH", 307, 183);
	

	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_BLACK);
	GUI_DispStringAt("CONNECT", 294, 280);
	GUI_DispStringAt("CONNECT", 387, 280);
	
	GUI_SetColor(USER_COLOR1_RED);
	GUI_DrawLine(372, 217, 372, 295);

}

/*
Draw4Temp_Big:画右侧界面，带4个温度框
int *temp：4个温度值
int has_big；是否有上面的那个big温度。1；有big温度 0：没有big温度。
int big；如果has_big=1，这个是big温度值
int update：更新的内容 0：只画4个温度框 1：画框里面的温度值 2：画上面的big温度值
*/
/*1:all draw 0:draw temp only*/

void Draw4Temp_Big(int *temp, int has_big, int big, int update)
{
	int i;
	char dispStr[64] = "";
	char *item[]={
		"ONE",
		"TWO",
	  "THREE",
	  "FOUR"
	};
	
	GUI_RECT  rect4temp_no_big[4]={{260, 10, 360, 130},
																{362, 10, 462, 130},
																{260, 132, 360, 252},
																{362, 132, 462, 252}};
	GUI_RECT  rect4temp_with_big[4]={{260, 90, 360, 198},
																	 {362, 90, 462, 198},
																	 {260, 200, 360, 308},
																	 {362, 200, 462, 308}};
	GUI_RECT *pRect;
	
	g_temp_hasbig = has_big;
	
	if(has_big)//根据是否有big温度选择坐标
		pRect = rect4temp_with_big;
	else
		pRect = rect4temp_no_big;
	switch(update)
	{
		case 0://画4个框
			GUI_SetColor(USER_COLOR7_GRAY);
			for(i=0;i<4;i++)
			{
				GUI_FillRect(pRect[i].x0, pRect[i].y0, pRect[i].x1, pRect[i].y1);
			}
			
			GUI_SetBkColor(USER_COLOR7_GRAY);
			GUI_SetColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
			for(i=0;i<4;i++)
			{
				GUI_DispStringAt(item[i],(pRect[i].x1-pRect[i].x0)/2 + pRect[i].x0 -20, (pRect[i].y1-pRect[i].y0)/2 + pRect[i].y0+20);
			}
			break;
		case 1://更新4温度
			GUI_SetBkColor(USER_COLOR7_GRAY);
			GUI_SetColor(GUI_BLACK);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn60);
			for(i=0;i<4;i++)
			{
				sprintf(dispStr, "%d", temp[i]);
				GUI_DispStringAt(dispStr,(pRect[i].x1-pRect[i].x0)/2 + pRect[i].x0 -20, (pRect[i].y1-pRect[i].y0)/2 + pRect[i].y0-45);
			}
			//break;
		/*case 2:*///更新big温度
			GUI_SetBkColor(GUI_WHITE);
			GUI_SetColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn89);
			
			sprintf(dispStr, "%d", temp[4]);
			GUI_DispStringAt(dispStr,(pRect[0].x1-pRect[0].x0)/2 + pRect[0].x0, pRect[0].y0-90);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
			GUI_DispStringAt("°F",460, 10);
			break;
		default:
			break;
	}
}
/*
DrawArcScale 函数画圆
int degree:要显示的度数0~360°。外面要按照比例转成弧度传进来
int temp_or_smoke：1：画红色的temp设定 0：画黑色的smoke
*/
void DrawArcScale(int degree, int temp_or_smoke)///*1: temp 0: smoke*/
{
  int x0;
  int y0;
  int i;
  char dispStr[64] = "";
	GUI_POINT pos={130,133};//圆心点
	int r=118;//圆半径
	x0 = pos.x;
  y0 = pos.y;
	clear_screen(x0-r,y0-r,x0+r,y0+r);//清除圆区域的屏幕
	GUI_SetBkColor(GUI_WHITE);
  GUI_SetPenSize( 8 );
	if(temp_or_smoke==1)
		GUI_SetColor( GUI_LIGHTRED ); 
	else
		GUI_SetColor( USER_COLOR7_GRAY ); 
	GUI_SetBkColor( GUI_WHITE ); 
  GUI_DrawArc( x0,y0,r, r, 0, 360 );//画一个浅色的正圆
  //GUI_Delay(1000);
	if(temp_or_smoke==1)
	{//画temp界面
		GUI_SetColor( GUI_RED ); 
		GUI_DrawArc( x0,y0,r, r,90 - degree, 90 );

		//添加文字
		GUI_SetBkColor( GUI_WHITE ); 
		GUI_SetColor( USER_COLOR1_RED ); 
		if(degree<=6)
		{
			//low smoke
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn76);
			GUI_DispStringAt("LOW",x0-55, y0-90);
			GUI_DispStringAt("SMOKE",x0-90, y0-25);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
			sprintf(dispStr, "%.3d SET TEMP", 160);
			GUI_DispStringAt(dispStr,x0-70, y0+50);
		}else if(degree<=12)
		{
			//high smoke
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn76);
			GUI_DispStringAt("HIGH",x0-55, y0-90);
			GUI_DispStringAt("SMOKE",x0-90, y0-25);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
			sprintf(dispStr, "%.3d SET TEMP", 220);
			GUI_DispStringAt(dispStr,x0-70, y0+50);
		}else if(degree<=6*59)
		{
			//160~450
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn120);
			sprintf(dispStr, "%d", (degree/6-3)*5+160);
			GUI_DispStringAt(dispStr,x0-70, y0-65);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
			GUI_DispStringAt("SET TEMP",x0-50, y0+50);
		}else{
			//HIGH
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn76);
			GUI_DispStringAt("HIGH",x0-60, y0-38);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
			GUI_DispStringAt("SET TEMP",x0-50, y0+50);
		}
		GUI_SetColor( GUI_BLACK ); 
		GUI_DispStringAt("°F",x0-r+5, y0-r+5);
	}else{//画smoke界面
		GUI_SetColor( GUI_BLACK );
		GUI_DrawArc( x0,y0,r, r,90 - degree, 90 );
		//添加文字
		GUI_SetBkColor( GUI_WHITE ); 
		GUI_SetColor( GUI_BLACK ); 
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn120);
		sprintf(dispStr, "%d", (degree/36));
		if((degree/36)==10)
			GUI_DispStringAt(dispStr,x0-50, y0-65);
		else
			GUI_DispStringAt(dispStr,x0-25, y0-65);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
		GUI_DispStringAt("SET SMOKE",x0-53, y0+50);
	}
}
/*
DrawTimer 函数花底部的timer灰色条
int time1:小时
int time2:分钟
int just_update_time:是否只更新时间，0：整个条都重新画，1：只更新时间
*/
void DrawTimer(int time1,int time2, int just_update_time)
{
	char dispStr[64] = "";
	GUI_RECT  bottom={10, 268, 470, 308};//底部深灰色框
	GUI_POINT bottom_text_pos={175, 275};//底部timer文字位置
	
	GUI_SetColor(GUI_WHITE);
	GUI_FillRect(bottom.x0,bottom.y0,bottom.x1,bottom.y1);
	if(!just_update_time)//只更新时间
	{
		//底部灰色条
		GUI_SetColor(USER_COLOR3_GRAY);
		GUI_FillRect(bottom.x0, bottom.y0, bottom.x1, bottom.y1);
	}
	//更新时间
	GUI_SetBkColor(USER_COLOR3_GRAY);
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdMedCn30);
	sprintf(dispStr,"TIMER // %d:%d",time1, time2);
	GUI_DispStringAt(dispStr, bottom_text_pos.x, bottom_text_pos.y);
}
/*
DrawLeft6ItemsUpUI:画左侧有6个选项的菜单项
int sel_new:选择的ITEM(0~5)
int sel_last:旧的选择的ITEM(0~5)
int update：更新内容选择0：画整个6菜单项，并选中在sel_new上 1：只更新选择项，即sel_new画红色，sel_last画灰色，这个用于旋转按钮时菜单的切换
int ui：界面文字选择共有两种(0~1)  0:主界面的6个  1：setup的5个+最后一个灰色，为1时选择sel_new sel_last的范围是0~4
*/
void DrawLeft6ItemsUpUI(int sel_new,int sel_last,int update, int ui)
{
	int i;

	GUI_RECT left_menu[6]={{10, 10,  243,  58},
										{10, 60,  243, 108},
										{10, 110, 243, 158},
										{10, 160, 243, 208},
										{10, 210, 243, 258},
										{10, 260, 243, 308}};
	GUI_POINT Text[]={{30, 12},
										{30, 12+50},
										{30, 12+50*2},
										{30, 12+50*3},
										{30, 12+50*4},
										{30, 12+50*5}};

  if(!update)
	{
		//画6个条
		GUI_SetColor(USER_COLOR7_GRAY);
		for(i=0;i<6;i++)
			GUI_FillRect(left_menu[i].x0, left_menu[i].y0,  left_menu[i].x1, left_menu[i].y1);
		//添加文字
		GUI_SetBkColor(USER_COLOR7_GRAY);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
		GUI_SetColor(GUI_BLACK);
		for(i=0;i<6;i++)
			GUI_DispStringAt(MainMenuTextAll[i][ui],Text[i].x, Text[i].y);
		
		//默认选择sel_new
		GUI_SetColor(USER_COLOR1_RED);
		GUI_FillRect(left_menu[sel_new].x0, left_menu[sel_new].y0,  left_menu[sel_new].x1, left_menu[sel_new].y1);	
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt(MainMenuTextAll[sel_new][ui],  Text[sel_new].x, Text[sel_new].y);	
	}else{
		//还原sel_last选择项为灰色
		GUI_SetColor(USER_COLOR7_GRAY);
		GUI_FillRect(left_menu[sel_last].x0, left_menu[sel_last].y0,  left_menu[sel_last].x1, left_menu[sel_last].y1);	
		GUI_SetBkColor(USER_COLOR7_GRAY);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt(MainMenuTextAll[sel_last][ui],  Text[sel_last].x, Text[sel_last].y);	
		//更新sel_new选择项为红色
		GUI_SetColor(USER_COLOR1_RED);
		GUI_FillRect(left_menu[sel_new].x0, left_menu[sel_new].y0,  left_menu[sel_new].x1, left_menu[sel_new].y1);	
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt(MainMenuTextAll[sel_new][ui],  Text[sel_new].x, Text[sel_new].y);	
	}
		
}
/*
画shutdown complete界面
*/
void DrawCompleteUI(void)
{
	char dispStr[64] = "";
	GUI_POINT logo_pos={105,28};
	GUI_RECT logo={10, 10,  243,  308};
	GUI_RECT Item_rect = {10, 260, 243, 308};
	char *Item_str={"EXIT"};
	GUI_POINT Item_pos={50, 12+50*5};//exit
	
	//画左侧界面
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
	GUI_SetColor(USER_COLOR7_GRAY);
	//logo
	//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_power);
	GUI_SetBkColor(USER_COLOR1_RED);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("SHUTDOWN",		    50, 90);
	GUI_DispStringAt("COMPLETE",		    65, 152);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
	GUI_DispStringAt("CYCLE POWER",		    87, 190);
}
/*
	DrawExitUI:画左侧底部只有一个exit选项的左侧界面
	int time1：小时
	int time2：分钟
	int ui_sel：界面选择（参考common.h中的宏定义）可选值为：(宏定义的名字根据界面第一行的文字定义的)
	#define UI_EXIT_FEED_MODE                		8
	#define UI_EXIT_SHUTDOWN                 		9
	#define UI_EXIT_FLAME_ERROR              		10
	#define UI_EXIT_SENSOR                   		11
	#define UI_EXIT_OVER_TEMP                		12
	#define UI_EXIT_FEED_NOT                 		13
	*/
void DrawExitUI(int time1, int time2,int ui_sel)
{	
	char dispStr[64] = "";
	GUI_POINT logo_pos={105,28};
	GUI_RECT logo={10, 10,  243,  258};
	GUI_RECT Item_rect = {10, 260, 243, 308};
	char *Item_str={"EXIT"};
	GUI_POINT Item_pos={50, 12+50*5};//exit
	
			//画左侧界面
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
			GUI_SetColor(USER_COLOR7_GRAY);
			switch(ui_sel)
			{
				case UI_EXIT_FEED_MODE:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_warning);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("FEED MODE",		    45, 90);
					GUI_DispStringAt("COMPLETE",		    50, 152);
					break;
				case UI_EXIT_SHUTDOWN :
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_power);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn38);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("Apply Changes", 25, 90);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
					sprintf(dispStr,"%d:%.2d",time1, time2);
					GUI_DispStringAt(dispStr,		 90, 151);
					break;
				case UI_EXIT_FLAME_ERROR:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_warning);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("FLAME ERROR", 13, 86);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
					GUI_DispStringAt("FLAME HAS EXIINGUISHED.", 30, 129);
					GUI_DispStringAt("CLEAN GRILL BEFORE USE", 30, 159);
					break;
				case UI_EXIT_SENSOR:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_warning);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("SENSOR", 72, 88);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
					GUI_DispStringAt("INTERNAL THERMOMETER", 32, 129);
					GUI_DispStringAt("HAS FAILED.CLEAN OR", 42, 158);
					GUI_DispStringAt("REPLACE BEFORE USE.", 42, 188);
					break;
				case UI_EXIT_OVER_TEMP:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_warning);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("OVER TEMP", 45, 87);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
					GUI_DispStringAt("NOEMAL OPERATING", 42, 131);
					GUI_DispStringAt("TEMPERATURE HAS BEEN", 12, 159);
					GUI_DispStringAt("EXCEEDED.CLEAN GRILL.", 23, 189);
					GUI_DispStringAt("BEFORE USE.", 75, 215);
					break;
				case UI_EXIT_FEED_NOT:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_warning);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("FEED NOT AVAILABLE WHEN", 10, 90);
					GUI_DispStringAt("GRILL IS OPERATION", 33, 127);
					break;
				default:
					break;
			}
			

			//选中项画红色
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(Item_rect.x0, Item_rect.y0,  Item_rect.x1, Item_rect.y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringAt(Item_str,Item_pos.x, Item_pos.y);
}
/*
DrawCancelUI:画左侧底部只有一个cancel选项的左侧界面
int time1：小时
int time2：分钟
*/
void DrawCancelUI(int time1, int time2)
{
	GUI_POINT logo_pos={105,28};
	char dispStr[64] = "";
	GUI_RECT logo={10, 10,  243,  258};
	GUI_RECT Item_rect={10, 260, 243, 308};
	char *Item_str="CANCEL";
	GUI_POINT Item_pos={50, 12+50*5};//cancel
			//画左侧界面
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
			GUI_SetColor(USER_COLOR7_GRAY);
			GUI_FillRect(Item_rect.x0, Item_rect.y0,  Item_rect.x1, Item_rect.y1);

			//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_warning);	
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringAt("FEED MODE",		    45, 90);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			sprintf(dispStr,"%d:%.2d",time1, time2);
			GUI_DispStringAt(dispStr,		 80, 140);
			
			//GUI_DispStringAt("logo",		 30, 12);//logo 
		
			//选中项画红色
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(Item_rect.x0, Item_rect.y0,  Item_rect.x1, Item_rect.y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringAt(Item_str,Item_pos.x, Item_pos.y);
}
/*
DrawBypassCancelUI:画左侧底部有bypass cancel选项的左侧界面
int sel：当前选择项0~1
int time1：小时 
int time2：分钟 
int update：更新选择 0：画整个界面 1：只更新选择项，旋转更新界面可以选为1
int ui_sel：界面文字选择，可选择为一下宏：
#define UI_BYPASS_CANCEL_SHUTDOWN        		6
*/
void DrawBypassCancelUI(int sel, int time1, int time2, int update, int ui_sel)
{
	GUI_POINT logo_pos={105,28};
	char dispStr[64] = "";
	GUI_RECT logo={10, 10,  243,  208};
	GUI_RECT Item_rect[] = {{10, 210, 243, 258},
									        {10, 260, 243, 308}};
	char *Item_str[]={"BYPASS","CANCEL"};
	GUI_POINT Item_pos[]={{50, 12+50*4},//bypass
										    {50, 12+50*5}};//cancel
	switch(update){
		case 0://画左侧界面
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
			GUI_SetColor(USER_COLOR7_GRAY);
			GUI_FillRect(Item_rect[0].x0, Item_rect[0].y0,  Item_rect[0].x1, Item_rect[0].y1);
			GUI_FillRect(Item_rect[1].x0, Item_rect[1].y0,  Item_rect[1].x1, Item_rect[1].y1);
	
			switch(ui_sel)
			{
				case UI_BYPASS_CANCEL_SHUTDOWN:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_power);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("STARTUP",		    65, 90);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
					sprintf(dispStr,"%d:%.2d",time1, time2);
					GUI_DispStringAt(dispStr,		 80, 140);
					break;
				default:
					break;
			}
			//GUI_DispStringAt("logo",		 30, 12);//logo 
			break;
		case 1://更新选择项
			//选中项画红色
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(Item_rect[sel].x0, Item_rect[sel].y0,  Item_rect[sel].x1, Item_rect[sel].y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringAt(Item_str[sel],Item_pos[sel].x, Item_pos[sel].y);
			//未选中项画灰色
			sel = (sel+1)&0x01;
			GUI_SetColor(USER_COLOR7_GRAY);
			GUI_FillRect(Item_rect[sel].x0, Item_rect[sel].y0,  Item_rect[sel].x1, Item_rect[sel].y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			GUI_SetBkColor(USER_COLOR7_GRAY);
			GUI_SetColor(GUI_BLACK);
			GUI_DispStringAt(Item_str[sel],Item_pos[sel].x, Item_pos[sel].y);
			break;
		default:
			break;
	}
}
/*
DrawConfirmCancelUI:画左侧底部有confirm cancel两个选项的左侧界面
int sel:选择项0~1
int temp:temp值
int smoke：smoke值
int update更新项选择0：画界面，默认选择 1：只画更新选择sel
int ui_sel：界面文字选择，可以为一下宏：
#define UI_CONFITRM_CANCEL_STARTUP       		0
#define UI_CONFITRM_CANCEL_APLPLY        		1
#define UI_CONFITRM_CANCEL_ONLY_USE_FOR  		2
#define UI_CONFITRM_CANCEL_ONLY_USE_BYPASS	3
#define UI_CONFITRM_CANCEL_SHUTDOWN      		4
#define UI_CONFITRM_CANCEL_RESET         		5
*/
void DrawConfirmCancelUI(int sel, int temp, int smoke, int update, int ui_sel)
{
	char dispStr[64] = "";
	GUI_POINT logo_pos={105,28};
	GUI_RECT logo={10, 10,  243,  208};
	GUI_RECT Item_rect[] = {{10, 210, 243, 258},
									        {10, 260, 243, 308}};
	char *Item_str[]={"CONFIRM","CANCEL"};
	GUI_POINT Item_pos[]={{50, 12+50*4},//comfirm
										    {50, 12+50*5}};//cancel
	switch(update){
		case 0://画左侧界面
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
			GUI_SetColor(USER_COLOR7_GRAY);
			GUI_FillRect(Item_rect[0].x0, Item_rect[0].y0,  Item_rect[0].x1, Item_rect[0].y1);
			GUI_FillRect(Item_rect[1].x0, Item_rect[1].y0,  Item_rect[1].x1, Item_rect[1].y1);
	
			switch(ui_sel)
			{
				case UI_CONFITRM_CANCEL_STARTUP:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_power);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("STARTUP",		    65, 90);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
					sprintf(dispStr,"%d TEMP // %d SMOKE",temp, smoke);
					GUI_DispStringAt(dispStr,		 25, 133);
					break;
				case UI_CONFITRM_CANCEL_APLPLY :
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_power);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn38);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("Apply Changes", 25, 90);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
					sprintf(dispStr,"%d TEMP // %d SMOKE",temp, smoke);
					GUI_DispStringAt(dispStr,		 25, 133);
					break;
				case UI_CONFITRM_CANCEL_ONLY_USE_FOR:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_power);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("ONLY USE FOR INITIAL", 40, 82);
					GUI_DispStringAt("SETUP OR WHEN GRILL HAS", 20, 110);
					GUI_DispStringAt("RUN OUT OF PELLETS", 45, 140);
					break;
				case UI_CONFITRM_CANCEL_ONLY_USE_BYPASS:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_power_H);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("ONLY USE BYPASS WHEN", 35, 85);
					GUI_DispStringAt("GRILL IS STILL HOT.DON'T", 35, 114);
					GUI_DispStringAt("USE IF GRILL HAS BEEN OFF", 18, 139);
					GUI_DispStringAt("FOR MORE THAN 3 MINUTES", 18, 168);
					break;
				case UI_CONFITRM_CANCEL_SHUTDOWN:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_power);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("SHUTDOWN", 50, 89);
					break;
				case UI_CONFITRM_CANCEL_RESET:
					//image_display(logo_pos.x,logo_pos.y,(u8*)gImage_warning);
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt("RESET PELLET GRILL TO", 35, 84);
					GUI_DispStringAt("FACTORY SETTINGS", 50, 112);
					GUI_DispStringAt("(FORGET NETWORK)", 50, 152);
					break;
				default:
					break;
			}
			//GUI_DispStringAt("logo",		 30, 12);//logo 
			break;
		case 1://更新选择项
			//选中项画红色
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(Item_rect[sel].x0, Item_rect[sel].y0,  Item_rect[sel].x1, Item_rect[sel].y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringAt(Item_str[sel],Item_pos[sel].x, Item_pos[sel].y);
			//未选中项画灰色
			sel = (sel+1)&0x01;
			GUI_SetColor(USER_COLOR7_GRAY);
			GUI_FillRect(Item_rect[sel].x0, Item_rect[sel].y0,  Item_rect[sel].x1, Item_rect[sel].y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			GUI_SetBkColor(USER_COLOR7_GRAY);
			GUI_SetColor(GUI_BLACK);
			GUI_DispStringAt(Item_str[sel],Item_pos[sel].x, Item_pos[sel].y);
			break;
		default:
			break;
	}
}
void GUIDEMO_mainMenu(void);
void GUI_ABOUT_UI(void)
{
	int i;
	int menu_index=0;
	int menu_index_last=0;
	int current_temp=183;
	int temp_val[5]={11,12,13,14};
	int big_temp=180;
	char dispStr[64] = "";
	EC11_STA ec_sta = EC11_IDLE;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawLeft6ItemsUpUI(menu_index, menu_index_last, 0, 1);//画左侧6个菜单
	DrawAboutUI("123456","CC-Guest","IOS DEVICE", "CCPG V1.7");
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		menu_index_last = menu_index;
		if (EC11_LEFT == ec_sta) {// up
			if(menu_index>0)
				menu_index--;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(menu_index<5)
				menu_index++;
		}
		if((ec_sta != EC11_IDLE)&&(ec_sta != EC11_BUT))
		{
			// 被选中的项目
			DrawLeft6ItemsUpUI(menu_index, menu_index_last, 1, 1);//更新选择界面
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			switch(menu_index)
			{
				case 0:
					//set temp
					GUIDEMO_mainMenu();
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				default:
					ec_sta = EC11_IDLE;
					break;
			}
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);

}
void GUI_SETUP_INTERFACR(void)
{
	int i;
	int menu_index=0;
	int menu_index_last=0;
	int current_temp=183;
	int temp_val[5]={11,12,13,14};
	int big_temp=180;
	char dispStr[64] = "";
	EC11_STA ec_sta = EC11_IDLE;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawLeft6ItemsUpUI(menu_index, menu_index_last, 0, 1);//画左侧6个菜单
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画4个框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新框内4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//更新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		menu_index_last = menu_index;
		if (EC11_LEFT == ec_sta) {// up
			if(menu_index>0)
				menu_index--;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(menu_index<5)
				menu_index++;
		}
		if((ec_sta != EC11_IDLE)&&(ec_sta != EC11_BUT))
		{
			// 被选中的项目
			DrawLeft6ItemsUpUI(menu_index, menu_index_last, 1, 1);//更新选择界面
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			switch(menu_index)
			{
				case 0:
					//set temp
					GUI_ABOUT_UI();
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				default:
					ec_sta = EC11_IDLE;
					break;
			}
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUI_COMPLETE(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawCompleteUI();//画左侧框
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			//DrawConfirmCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_SETUP_INTERFACR();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUI_exit_FEED_NOT(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawExitUI(12, 0, UI_EXIT_FEED_NOT);//画左侧框
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			//DrawConfirmCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_COMPLETE();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUI_exit_OVER_TEMP(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawExitUI(12, 0, UI_EXIT_OVER_TEMP);//画左侧框
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			//DrawConfirmCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_exit_FEED_NOT();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUI_exit_SENSOR(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawExitUI(12, 0, UI_EXIT_SENSOR);//画左侧框
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			//DrawConfirmCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_exit_OVER_TEMP();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUI_exit_FLAME_ERROR(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawExitUI(12, 0, UI_EXIT_FLAME_ERROR);//画左侧框
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			//DrawConfirmCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_exit_SENSOR();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUI_exit_shutdown(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawExitUI(12, 0, UI_EXIT_SHUTDOWN);//画左侧框
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			//DrawConfirmCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_exit_FLAME_ERROR();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUI_exit_FEED(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawExitUI(12, 0, UI_EXIT_FEED_MODE);//画左侧框
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			//DrawConfirmCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_exit_shutdown();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);	
}
void GUI_cancel_FEED(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawCancelUI( 12, 0);//画左侧框
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			//DrawConfirmCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_exit_FEED();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);	
}

void GUI_bypass_cancel_shutdown(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};

	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawBypassCancelUI(sel, 12, 0, 0, UI_BYPASS_CANCEL_SHUTDOWN);//画左侧框
	DrawBypassCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);//更新选中项
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			DrawConfirmCancelUI(sel, 12, 0, 1, UI_BYPASS_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_cancel_FEED();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);	
}

void GUI_RESET(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};

	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawConfirmCancelUI(sel, 215, 5, 0, UI_CONFITRM_CANCEL_RESET);//画左侧框
	DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_RESET);//更新选中项
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_RESET);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_bypass_cancel_shutdown();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);	
}

void GUI_SHUTDOWN(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};

	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawConfirmCancelUI(sel, 215, 5, 0, UI_CONFITRM_CANCEL_SHUTDOWN);//画左侧框
	DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_SHUTDOWN);//更新选中项
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_SHUTDOWN);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_RESET();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);	
}
void GUI_ONLY_USE_BYPASS(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};

	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawConfirmCancelUI(sel, 215, 5, 0, UI_CONFITRM_CANCEL_ONLY_USE_BYPASS);//画左侧框
	DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_ONLY_USE_BYPASS);//更新选中项
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_ONLY_USE_BYPASS);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_SHUTDOWN();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);	
}
void GUI_ONLY_USE_FOR_INITIAL(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp[4]={11,22,33,44};

	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawConfirmCancelUI(sel, 215, 5, 0, UI_CONFITRM_CANCEL_ONLY_USE_FOR);//画左侧框
	DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_ONLY_USE_FOR);//更新选中项
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//跟新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_ONLY_USE_FOR);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_ONLY_USE_BYPASS();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);	
}
void GUI_ApplyChange(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;
	int temp_val[5]={31,32,33,34};
	int big_temp=182;

	clear_screen(0,0,480,320);//清除屏幕
	DrawConfirmCancelUI(sel, 215, 5, 0, UI_CONFITRM_CANCEL_APLPLY);//画左侧框
	DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_APLPLY);//画左侧选中项
  
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个温度框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//更新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 更新被选中的项目
			DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_APLPLY);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_ONLY_USE_FOR_INITIAL();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			//GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);	
}
void GUI_startMode(void)
{
	int i;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	char dispStr[64] = "";
	int menu_index=0;

	int temp_val[5]={31,32,33,34};
	int big_temp=182;

	clear_screen(0,0,480,320);//清除屏幕
	DrawConfirmCancelUI(sel, 215, 5, 0, UI_CONFITRM_CANCEL_STARTUP);
	DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_STARTUP);
  
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画右侧4个框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新4温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//更新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(sel>0)
				sel --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(sel<1)
				sel ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			DrawConfirmCancelUI(sel, 215, 5, 1, UI_CONFITRM_CANCEL_STARTUP);// 被选中的项目
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_ApplyChange();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUI_setSmoke(void)
{
	EC11_STA ec_sta = EC11_IDLE;
	int degree_step =6;
	int smoke_level=0;

	int temp_val[5]={31,32,33,34};////右侧显示的4个温度值
	int big_temp=182;
	int step=0;	

	clear_screen(0,0,480,320);//清除屏幕
	Draw4Temp_Big(temp_val, 0, big_temp, 0);//画4个框
	Draw4Temp_Big(temp_val, 0, big_temp, 1);//更新4个温度
	DrawTimer(0,0,0);//底部灰色条
	DrawArcScale(step*36, 0);//左侧圆
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(step>0)
				step --;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(step<10)
				step ++;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			// 被选中的项目
			DrawArcScale(step*36, 0);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_startMode();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUI_setTemp(void)
{
	EC11_STA ec_sta = EC11_IDLE;
	int degree_step =6;
	int smoke_level=0;
	GUI_POINT arc_pos={130,133};//圆心点
	int arc_r=118;//圆半径
	int step=6;
	int temp_val[5]={21,22,23,24};//右侧显示的4个温度默认值
	int big_temp=181;

	clear_screen(0,0,480,320);//清除屏幕
	Draw4Temp_Big(temp_val, 0, big_temp, 0);//画4个框
	Draw4Temp_Big(temp_val, 0, big_temp, 1);//更新4个温度
	DrawTimer(0,0,0);//底部灰色条
	DrawArcScale(step, 1);//画圆
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {// up
			if(step>0)
				step-=6;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(step<360)
				step+=6;
		}
		if((ec_sta != EC11_IDLE)&&(EC11_BUT != ec_sta))
		{
			//更新圆
			DrawArcScale(step, 1);
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			GUI_setSmoke();
			ec_sta = EC11_IDLE;
		}else if(EC11_BUT_LONG == ec_sta)
		{
			GUI_startMode();
			ec_sta = EC11_IDLE;
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}
void GUIDEMO_mainMenu(void)
{
	int i;
	int menu_index=0;
	int menu_index_last=0;
	int current_temp=183;
	int temp_val[5]={11,12,13,14};
	int big_temp=180;
	char dispStr[64] = "";
	EC11_STA ec_sta = EC11_IDLE;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawLeft6ItemsUpUI(menu_index, menu_index_last, 0, 0);//画左侧6个菜单
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画4个框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新框内4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//更新big温度
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		menu_index_last = menu_index;
		if (EC11_LEFT == ec_sta) {// up
			if(menu_index>0)
				menu_index--;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(menu_index<5)
				menu_index++;
		}
		if((ec_sta != EC11_IDLE)&&(ec_sta != EC11_BUT))
		{
			// 被选中的项目
			DrawLeft6ItemsUpUI(menu_index, menu_index_last, 1, 0);//更新选择界面
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			switch(menu_index)
			{
				case 0:
					//set temp
					GUI_setTemp();
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				default:
					ec_sta = EC11_IDLE;
					break;
			}
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}

void GUIDEMO_mainMenuX(void)
{
	int i;
	int menu_index=0;
	int menu_index_last=0;
	int current_temp=183;
	int temp_val[5]={11,12,13,14};
	int big_temp=180;
	char dispStr[64] = "";
	EC11_STA ec_sta = EC11_IDLE;
	
	clear_screen(0,0,480,320);//清除屏幕
	DrawLeft6ItemsUpUI(menu_index, menu_index_last, 0, 0);//画左侧6个菜单
	Draw4Temp_Big(temp_val, 1, big_temp, 0);//画4个框
	Draw4Temp_Big(temp_val, 1, big_temp, 1);//更新框内4个温度
	Draw4Temp_Big(temp_val, 1, big_temp, 2);//更新big温度
	
	g_temp_update_enable = 1;
	
	do{
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		menu_index_last = menu_index;
		if (EC11_LEFT == ec_sta) {// up
			if(menu_index>0)
				menu_index--;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(menu_index<5)
				menu_index++;
		}
		if((ec_sta != EC11_IDLE)&&(ec_sta != EC11_BUT))
		{
			// 被选中的项目
			DrawLeft6ItemsUpUI(menu_index, menu_index_last, 1, 0);//更新选择界面
			ec_sta = EC11_IDLE;
		}
		if (EC11_BUT == ec_sta) 
	  {
			switch(menu_index)
			{
				case 0:
					//set temp
					GUI_setTemp();
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				default:
					ec_sta = EC11_IDLE;
					break;
			}
		}
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
}

void GUIDEMO_UpdateTemp(void)
{
	int temp_val[5] = {0};
	
	if (g_temp_update_enable) {
		temp_val[0] = g_temp_val_new.temp1;
		temp_val[1] = g_temp_val_new.temp2;
		temp_val[2] = g_temp_val_new.temp3;
		temp_val[3] = g_temp_val_new.temp4;
		temp_val[4] = g_temp_val_new.temp5;
		
		Draw4Temp_Big(temp_val, 1, 0, 0);//画4个框
		Draw4Temp_Big(temp_val, 1, 0, 1);//更新框内4个温度
		Draw4Temp_Big(temp_val, 1, 0, 2);//更新big温度
	}
}
void GUIDEMO_AfterLogo(void)
{
	// 默认子项目ID，对应”---“项
	g_event_val_new.menu_index = 11;
	g_temp_val_new.temp_unit = 0;// 华氏度
	
	g_temp_val_last = g_temp_val_new;
	
	//GUIDEMO_SubMenu();
	GUIDEMO_mainMenuX();
}

// RUN界面
void GUIDEMO_MainUI(void)
{
	int pen_size = 0;
	char dispStr[64] = "";
	int progress_offset = 0;
	int temp1_error_last = 0xFF;
	int temp2_error_last = 0xFF;
	int temp3_error_last = 0xFF;
	int temp4_error_last = 0xFF;
	int temp5_error_last = 0xFF;
	int time_remain = 10;
	int disp_switch = 0;
	
	EC11_STA ec_sta = EC11_IDLE;
	TEMP_VAL temp_val = g_temp_val_new;
	
	GUI_SetColor(bk_color);
	GUI_FillRect(0,0,480,320);
	
	GUI_SetColor(font_color);
	GUI_SetBkColor(bk_color);
	GUI_SetFont(&GUI_FontTradeGothicCn48);
	if (0 == g_temp_val_new.temp_unit) {
		sprintf(dispStr, "Target %.3d°F", temp_val.target_val);
	} else {
		sprintf(dispStr, "Target %.3d°C", TEMP_F2C(temp_val.target_val));
	}
	GUI_DispStringAt(dispStr, 15, 135);
	
	sprintf(dispStr, "Time %.2d:%.2d", (g_run_mode_counter_mins/60)%24, g_run_mode_counter_mins%60);
	GUI_DispStringAt(dispStr, 15, 185);
	
	GUI_SetColor(USER_COLOR5_GRAY);
	GUI_SetFont(&GUI_FontTradeGothicCn19);
	GUI_DispStringAt("ONE", 305+10+3, 175-92);
	GUI_DispStringAt("TWO", 305+90+10+3, 175-92);
	GUI_DispStringAt("THREE", 302-10+12+5, 175+92-92+10);
	GUI_DispStringAt("FOUR", 302+90+10+3, 175+92-92+10);
	
	pen_size = GUI_GetPenSize();
	GUI_SetPenSize(2);
	
	GUI_DrawLine(280, 20, 465, 20);
	GUI_DrawLine(280, 225,465, 225);
	GUI_DrawLine(280, 20, 280, 225);
	GUI_DrawLine(465, 20, 465, 225);
	
	GUI_DrawLine(373, 20, 373, 225);
	GUI_DrawLine(280, 123, 465, 123);
	GUI_SetPenSize(pen_size);
			
	GUI_SetColor(USER_COLOR4_GRAY);
	GUI_FillRect(15, 270, 58, 312);
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRect(422, 270, 465, 312);
	image_display(25, 275, (u8*)gImage_p1_leftdown);
	image_display(435, 275, (u8*)gImage_p1_rightdown);
	progress_offset = 364 - (364 * g_smoke_val_percent) / 100;
	
	if (progress_offset <= 20) {
		progress_offset = 20;
	}
	if (progress_offset >= 344) {
		progress_offset = 344;
	}
	
	GUI_SetColor(USER_COLOR4_GRAY);
	GUI_FillRect(58, 270, 58+progress_offset, 312);
	GUI_SetColor(USER_COLOR1_RED);
	GUI_FillRect(58+progress_offset, 270, 422, 312);
	
	GUI_SetColor(GUI_WHITE);
	GUI_DrawLine(58+progress_offset, 275, 58+progress_offset, 307);
	GUI_FillPolygon (&_aPointArrowProgressLeft[0], 3, 58+progress_offset-20, 291);
	GUI_FillPolygon (&_aPointArrowProgressRight[0], 3, 58+progress_offset+20, 291);
	
	do 
	{
		ec_sta = EC11_IDLE;
		temp_val = g_temp_val_new;
				
		if ((temp_val.temp5 < 0) || (temp_val.temp5 > 500)) {
			temp_val.temp5 = 0;
		}
		
		temp_val.temp5 = temp_val.temp5 % (temp_val.target_val+1);
		
		g_temp5_error = 0;
	//	temp_val.temp5 = 200;
		 
		if (0 == time_remain%10) {
			if (1 == g_run_mode) {
				GUI_SetColor(font_color);
				GUI_SetFont(&GUI_FontTradeGothicCn48);
				if (0 == disp_switch) {
					disp_switch = 1;
					sprintf(dispStr, "Time %.2d:%.2d", (g_run_mode_counter_mins/60)%24, g_run_mode_counter_mins%60);
				} else {
					disp_switch = 0;
					sprintf(dispStr, "Time %.2d %.2d", (g_run_mode_counter_mins/60)%24, g_run_mode_counter_mins%60);
				}
				
				GUI_DispStringAt(dispStr, 15, 185);
			}
		
			GUI_SetColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontTradeGothicCn144);
					
			if (0 == g_temp5_error) {
				g_event_val_new.force_flush = 0;
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d", temp_val.temp5);
				} else {
					sprintf(dispStr, "%.3d", TEMP_F2C(temp_val.temp5));
				}
				GUI_DispStringAt(dispStr, 15, 0);
				GUI_SetFont(&GUI_FontTradeGothicCn58);
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "°F");
				} else {
					sprintf(dispStr, "°C");
				}
				GUI_DispStringAt(dispStr, 175, 20);
				g_temp_val_last.temp5 = temp_val.temp5;
			} else {
				if (temp5_error_last != g_temp5_error) {
					GUI_SetColor(bk_color);
					GUI_FillRect(0,0,270,130);
					
					GUI_SetColor(USER_COLOR1_RED);
			
					pen_size = GUI_GetPenSize();
					GUI_SetPenSize(3);
					GUI_DrawLine(70, 70, 90, 70);
					GUI_DrawLine(100, 70, 120, 70);
					GUI_DrawLine(130, 70, 150, 70);
					GUI_SetPenSize(pen_size);
				}
			}
			temp5_error_last = g_temp5_error;
			GUI_SetBkColor(bk_color);		
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn38);
			if (0 == g_temp1_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d°F", temp_val.temp1);
				} else {
					sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val.temp1));
				}
				GUI_DispStringAt(dispStr, 290+5, 125-92);
			} else {// 如果温度棒故障，则显示”-“
				if (temp1_error_last != g_temp1_error) {
					GUI_DispStringAt("          ", 290+5, 125-92-14+10);
					GUI_DispStringAt("_ _ _", 290+10, 125-92-14+10);
				}
			}
			temp1_error_last = g_temp1_error;
			if (0 == g_temp2_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d°F", temp_val.temp2);
				} else {
					sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val.temp2));
				}
				GUI_DispStringAt(dispStr, 290+92+5, 125-92);
			} else {
				if (temp2_error_last != g_temp2_error) {
					GUI_DispStringAt("          ", 290+92+5, 125-92-14+10);
					GUI_DispStringAt("_ _ _", 290+92+10, 125-92-14+10);
				}
			}
			temp2_error_last = g_temp2_error;
			if (0 == g_temp3_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d°F", temp_val.temp3);
				} else {
					sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val.temp3));
				}
				GUI_DispStringAt(dispStr, 290+5, 125+92-92+10);
			} else {
				if (temp3_error_last != g_temp3_error) {
					GUI_DispStringAt("          ", 290+5, 125+92-92-14+10+10);
					GUI_DispStringAt("_ _ _", 290+10, 125+92-92-14+10+10);
				}
			}
			temp3_error_last = g_temp3_error;
			if (0 == g_temp4_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d°F", temp_val.temp4);
				} else {
					sprintf(dispStr, "%.3d°C", TEMP_F2C(temp_val.temp4));
				}
				GUI_DispStringAt(dispStr, 290+92+5, 125+92-92+10);
			} else {
				if (temp4_error_last != g_temp4_error) {
					GUI_DispStringAt("          ", 290+92+5, 125+92-92-14+10+10);
					GUI_DispStringAt("_ _ _", 290+92+10, 125+92-92-14+10+10);
				}
			}
			temp4_error_last = g_temp4_error;
		}

		// 最好该函数内部能识别出是快转还是慢转
		ec_sta = EC11_KEY_Scan(0);
		
		// no support in this ui
		if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		
		#if FOR_DEBUG_USE
		if (g_fatal_error != 0) {
		#else
		if ((g_fatal_error != 0) || (g_temp5_error != 0)) {
		#endif
			GUI_SetBkColor(bk_color);
			GUI_SetColor(bk_color);
			GUI_FillRect(15, 270, 465, 312);
			delay_ms(2000);
			GUI_SetColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			while(1) {
				if (1 == g_fatal_error) {
					GUI_DispStringAt("FLAME ERROR", 50, 275);
				} else {
					GUI_DispStringAt("SENSOR ERROR", 50, 275);
				}
				
				delay_ms(1000);
				
				GUI_DispStringAt("                              ", 50, 275);
				
				delay_ms(1000);
			}
		}
		
		delay_ms(80);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		g_event_val_new.force_flush = 1;
		g_event_val_new.menu_index = 11;
		
		GUIDEMO_SubMenu();
	} else {
		if (180 == temp_val.target_val) {
			g_event_val_new.menu_index = 5;
		} else if (175 == temp_val.target_val) {
			g_event_val_new.menu_index = 6;
		} else if (170 == temp_val.target_val) {
			g_event_val_new.menu_index = 7;
		} else if (165 == temp_val.target_val) {
			g_event_val_new.menu_index = 8;
		} else if (490 == temp_val.target_val) {
			g_event_val_new.menu_index = 3;
		} else if (495 == temp_val.target_val) {
			g_event_val_new.menu_index = 2;
		} else if (500 == temp_val.target_val) {
			g_event_val_new.menu_index = 1;
		} else {
			g_event_val_new.menu_index = 4;
		}
		
		g_event_val_new.force_flush = 1;
		GUIDEMO_SubMenu();
	}
}
/*************************** End of file ****************************/
