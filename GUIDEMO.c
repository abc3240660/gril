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
#include "PID.h"

#if 0
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn19;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn38;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn42;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn48;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn58;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn77;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTradeGothicCn144;
#endif
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn15;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn20;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLTStdCn23;
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

extern const unsigned char gImage_logo_day[11092];
extern const unsigned char gImage_logo_night[14264];


void GUIDEMO_FeedShutdownTmerUI(int mode);
void GUIDEMO_ClearScreen(int x0, int y0,int x1,int y1);
void GUIDEMO_StartupTimerLeftUI(int sel, int time1, int time2, int update, int ui_sel);
void GUIDEMO_StartupTimerUI(void);
void GUIDEMO_StartupInitialUI(int mode);
void GUIDEMO_FeedShutdownTmerUI(int mode);
/*******************************************************************
*
*       Static variables
*
********************************************************************
*/

static char *MainMenuTextAll[][2]=
{
	"SET TEMP",   "NIGHT MODE",
	"FEED",		  "UINTS // °F",
	"SHUTDOWN",   "RESET",
	"SETUP",      "ABOUT",
	"HELP",       "EXIT",
	"EXIT",       ""
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
#define USER_COLOR8_GRAY   	0x009DA0A5

EVENT_VAL g_event_val_new;
EVENT_VAL g_event_val_last;

TEMP_VAL g_temp_val_new;
TEMP_VAL g_temp_val_last;
int g_direct_shutwown=0;
int g_direct_startup=0;
int g_direct_feed=0;

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
int g_target_temp_val   = 50;
int g_set_temp   = 50;//用于控制计算的设定温度


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

//未选中的菜单背景或右侧4个温度的背景
GUI_COLOR g_unselected_menu_color   = USER_COLOR7_GRAY;// or GUI_WHITE

//最大温度及其附属说明文字的颜色
GUI_COLOR g_big_temp_color = USER_COLOR1_RED;// or GUI_WHITE

//smoke百分比及其附属说明文字的颜色
GUI_COLOR g_big_smoke_color = GUI_BLACK;// or GUI_WHITE

//大圆圈背景颜色
GUI_COLOR g_circle_extenal_color = USER_COLOR2_RED;// or USER_COLOR7_GRAY or GUI_WHITE

//大圆圈实体颜色
GUI_COLOR g_circle_internal_color = USER_COLOR1_RED;// or GUI_BLACK

//主菜单UI中，WIFI标志的颜色
GUI_COLOR g_wifi_color = GUI_BLACK;// or GUI_WHITE

//Timer进度条颜色
GUI_COLOR g_progress_color = USER_COLOR3_GRAY;// or GUI_WHITE

//Timer进度条上字体颜色
GUI_COLOR g_progress_text_color = GUI_WHITE;// or GUI_BLACK

int g_temp_center = 0;

extern EC11_STA ec11_int_event;
extern u16 step_left;
extern u16 step_right;

extern u16 g_startup_mode_counter;
extern u16 g_run_mode_counter_mins;// minutes
extern u16 g_run_mode_counter_sec;// seconds
extern u16 g_run_mode_counter_hour;// seconds
extern u16 g_feed_mode_counter;
extern u16 g_shutdown_mode_counter;

extern void image_display(u16 x,u16 y,u8 * imgx);

void GUIDEMO_StartupTimerUI(void);
void GUIDEMO_MainMenu(int sel);
void GUIDEMO_DayNightSwitch(void);
void GUIDEMO_DayModeSet(void);
void GUIDEMO_ResetInitialUI(void);
void GUIDEMO_MainMenu(int sel);
void GUIDEMO_UpdateTemp(int *temp_val);
void GUIDEMO_StartupInitialUI(int mode);
void GUIDEMO_BigCircleUI(int is_smoke_ui);
void GUIDEMO_StartupTimerBypassedUI(void);
void GUIDEMO_LeftOneExitUI(int time1, int time2,int ui_sel);

int g_temp_hasbig = 1;
int g_temp_update_enable = 0;
int g_is_night_mode=0;

int g_time_remain = 0;
int g_is_lastui_bypassdetail = 0;
void direct_switch_check(void)
{
		if(g_direct_shutwown==1)
		{
			g_direct_shutwown=0;
			GUIDEMO_ClearScreen(0,0,480,320);//清除屏幕
			GUIDEMO_FeedShutdownTmerUI(1);
		}
		if(g_direct_startup==1)
		{
			g_direct_startup=0;
			GUIDEMO_ClearScreen(0,0,480,320);//清除屏幕
			GUIDEMO_StartupInitialUI(0);
		}
		if(g_direct_feed==1)
		{
			g_direct_feed=0;
			GUIDEMO_ClearScreen(0,0,480,320);//清除屏幕
			GUIDEMO_FeedShutdownTmerUI(0);
		}
		
}
/*
清除屏幕某一区域
*/
void GUIDEMO_ClearScreen(int x0, int y0,int x1,int y1)
{
	GUI_SetColor(bk_color);
	GUI_FillRect(x0,y0,x1,y1);
}

void GUIDEMO_ClearScreen_memdev(x0,y0,x1,y1)
{
#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;

	hMem = GUI_MEMDEV_Create(x0, y0, x1-x0, y1-y0);
	GUI_MEMDEV_Select(hMem);
#endif
	GUI_SetColor(bk_color);
	GUI_FillRect(x0,y0,x1,y1);

#ifdef SRAM_MEMDEV
	GUI_MEMDEV_CopyToLCDAt(hMem, x0, y0);
	GUI_MEMDEV_Delete(hMem);
	GUI_MEMDEV_Select(0);
#endif
}

/*
GUIDEMO_ProgressTimeBar 函数花底部的timer灰色条
int time1:小时
int time2:分钟
int just_update_time:是否只更新时间，0：整个条都重新画，1：只更新时间
*/
void GUIDEMO_ProgressTimeBar(int time1,int time2, int just_update_time)
{
	char dispStr[64] = "";
	GUI_RECT  bottom={10, 268, 470, 308};//底部深灰色框
	GUI_POINT bottom_text_pos={175, 270};//底部timer文字位置

#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;
#endif

//	GUI_SetColor(GUI_WHITE);
//	GUI_FillRect(bottom.x0,bottom.y0,bottom.x1,bottom.y1);
	if(!just_update_time)//只更新时间
	{
		//底部灰色条
		GUI_SetColor(g_progress_color);
		GUI_FillRect(bottom.x0, bottom.y0, bottom.x1, bottom.y1);
	}
	//更新时间
#ifdef SRAM_MEMDEV
	hMem = GUI_MEMDEV_Create(175, 270, 180, 45);
	GUI_MEMDEV_Select(hMem);
#endif

	GUI_SetBkColor(g_progress_color);
	GUI_SetColor(g_progress_text_color);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
	sprintf(dispStr,"TIMER // %2d:%.2d",time1, time2);
	GUI_DispStringAt(dispStr, bottom_text_pos.x, bottom_text_pos.y);
	
#ifdef SRAM_MEMDEV
	GUI_MEMDEV_CopyToLCDAt(hMem, 175, 270);
	GUI_MEMDEV_Delete(hMem);
	GUI_MEMDEV_Select(0);
#endif
}

/*
GUIDEMO_RightTempUI:画右侧界面，带4个温度框
int *temp：4个温度值
int has_big；是否有上面的那个big温度。1；有big温度 0：没有big温度。
int big；如果has_big=1，这个是big温度值
int update：更新的内容 0：只画4个温度框 1：画框里面的温度值 2：画上面的big温度值
*/
/*1:all draw 0:draw temp only*/
void GUIDEMO_RightTempUI(int *temp, int has_big, int big, int update)
{
	int i;
	char dispStr[64] = "";
	EC11_STA ec_sta = EC11_IDLE;	
	int time_remain = 10;
	int temp_val1[5]={11,12,13,14};
	GUI_RECT  rect4temp_no_big[4]={{260, 10,  360, 130},
								   {362, 10,  462, 130},
								   {260, 132, 360, 252},
								   {362, 132, 462, 252}};
	
	GUI_RECT *pRect;
	TEMP_VAL temp_val = g_temp_val_new;
	
	int temp1_error_last = 0xFF;
	int temp2_error_last = 0xFF;
	int temp3_error_last = 0xFF;
	int temp4_error_last = 0xFF;
	int temp5_error_last = 0xFF;
	
#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;
#endif

//	if(g_fatal_error!=0)
//	{
//		if(g_fatal_error==1)//flame
//		{
//			GUIDEMO_LeftOneExitUI(12, 0, UI_EXIT_FLAME_ERROR);
//		}else if(g_fatal_error==2)//sensor
//		{
//			GUIDEMO_LeftOneExitUI(12, 0, UI_EXIT_SENSOR);
//		}else if(g_fatal_error==3)//overtemp	
//		{
//			GUIDEMO_LeftOneExitUI(12, 0, UI_EXIT_OVER_TEMP);
//		}
//		
//		//GUIDEMO_RightTempUI(temp_val1, 1, big, 0);//画4个框
//		
//		g_fatal_error=0;
//		EC11_Clear();
//		
//		do{
//			// 扫面EC11编码器有无动作
//			ec_sta = EC11_KEY_Scan(0);

//			if ((0 == time_remain%10)) {
//				GUIDEMO_UpdateTemp(temp_val1);
//				GUIDEMO_RightTempUI(temp_val1, 1, big, 1);//更新框内4个温度
//				GUIDEMO_RightTempUI(temp_val1, 1, big, 2);//更新big温度
//			}
//			time_remain--;
//			
//			if (0 == time_remain) {
//				time_remain = 10;
//			}
//		
//			if (EC11_BUT == ec_sta)
//			{
//				ec_sta = EC11_IDLE;
//				GUIDEMO_BigCircleUI(2);
//				
//			}
//			delay_ms(100);
//		} while(EC11_BUT != ec_sta);
//	}
	g_temp_hasbig = has_big;
	
	pRect = rect4temp_no_big;
	
	if(has_big)//根据是否有big温度选择坐标
	{
		pRect[0].y0 += 80;
		pRect[0].y1 += 68;
		
		pRect[1].y0 += 80;
		pRect[1].y1 += 68;
		
		pRect[2].y0 += 68;
		pRect[2].y1 += 56;
		
		pRect[3].y0 += 68;
		pRect[3].y1 += 56;
	}
	
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

			if (has_big) {
				GUI_DispStringAt("ONE",      295, 165-80+80);
				GUI_DispStringAt("TWO",      295+102-5, 165-80+80);
				GUI_DispStringAt("THREE",    285, 165+110-60+65);
				GUI_DispStringAt("FOUR",     290+102, 165+110-60+65);
			} else {
				GUI_DispStringAt("ONE",      295, 165-80);
				GUI_DispStringAt("TWO",      295+102-5, 165-80);
				GUI_DispStringAt("THREE",    285, 165+110-60);
				GUI_DispStringAt("FOUR",     290+102, 165+110-60);
			}
	
			break;
		case 1://更新4温度
#ifdef SRAM_MEMDEV
			hMem = GUI_MEMDEV_Create(pRect[0].x0+13, pRect[0].y0+10, 80, 60);
			GUI_MEMDEV_Select(hMem);
#endif
			GUI_SetBkColor(USER_COLOR7_GRAY);
			GUI_SetColor(GUI_BLACK);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn60);
			if (0 == g_temp1_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.d", temp_val.temp1);
				} else {
					sprintf(dispStr, "%.d", TEMP_F2C(temp_val.temp1));
				}
				GUI_DispStringHCenterAt("   ", (pRect[0].x1-pRect[0].x0)/2+pRect[0].x0, pRect[0].y0+10);
				GUI_DispStringHCenterAt(dispStr, (pRect[0].x1-pRect[0].x0)/2+pRect[0].x0, pRect[0].y0+10);
			} else {// 如果温度棒故障，则显示”-“
				if (temp1_error_last != g_temp1_error) {
					GUI_DispStringHCenterAt("   ", (pRect[0].x1-pRect[0].x0)/2+pRect[0].x0, pRect[0].y0+10);
					GUI_DispStringHCenterAt("---", (pRect[0].x1-pRect[0].x0)/2+pRect[0].x0, pRect[0].y0+10);
				}
			}
#ifdef SRAM_MEMDEV
			GUI_MEMDEV_CopyToLCDAt(hMem, pRect[0].x0+13, pRect[0].y0+10);
			GUI_MEMDEV_Delete(hMem);
			
			hMem = GUI_MEMDEV_Create(pRect[1].x0+13, pRect[1].y0+10, 80, 60);
			GUI_MEMDEV_Select(hMem);
#endif
			temp1_error_last = g_temp1_error;
			if (0 == g_temp2_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.d", temp_val.temp2);
				} else {
					sprintf(dispStr, "%.d", TEMP_F2C(temp_val.temp2));
				}
				GUI_DispStringHCenterAt("   ", (pRect[1].x1-pRect[1].x0)/2+pRect[1].x0, pRect[1].y0+10);
				GUI_DispStringHCenterAt(dispStr, (pRect[1].x1-pRect[1].x0)/2+pRect[1].x0, pRect[1].y0+10);
			} else {
				if (temp2_error_last != g_temp2_error) {
					GUI_DispStringHCenterAt("   ", (pRect[1].x1-pRect[1].x0)/2+pRect[1].x0, pRect[1].y0+10);
					GUI_DispStringHCenterAt("---", (pRect[1].x1-pRect[1].x0)/2+pRect[1].x0, pRect[1].y0+10);
				}
			}
#ifdef SRAM_MEMDEV
			GUI_MEMDEV_CopyToLCDAt(hMem, pRect[1].x0+13, pRect[1].y0+10);
			GUI_MEMDEV_Delete(hMem);
			
			hMem = GUI_MEMDEV_Create(pRect[2].x0+13, pRect[2].y0+10, 80, 60);
			GUI_MEMDEV_Select(hMem);
#endif
			temp2_error_last = g_temp2_error;
			if (0 == g_temp3_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.d", temp_val.temp3);
				} else {
					sprintf(dispStr, "%.d", TEMP_F2C(temp_val.temp3));
				}
				GUI_DispStringHCenterAt("   ", (pRect[2].x1-pRect[2].x0)/2+pRect[2].x0, pRect[2].y0+18);
				GUI_DispStringHCenterAt(dispStr, (pRect[2].x1-pRect[2].x0)/2+pRect[2].x0, pRect[2].y0+18);
			} else {
				if (temp3_error_last != g_temp3_error) {
					GUI_DispStringHCenterAt("   ", (pRect[2].x1-pRect[2].x0)/2+pRect[2].x0, pRect[2].y0+18);
					GUI_DispStringHCenterAt("---", (pRect[2].x1-pRect[2].x0)/2+pRect[2].x0, pRect[2].y0+18);
				}
			}
#ifdef SRAM_MEMDEV
			GUI_MEMDEV_CopyToLCDAt(hMem, pRect[2].x0+13, pRect[2].y0+10);
			GUI_MEMDEV_Delete(hMem);
			
			hMem = GUI_MEMDEV_Create(pRect[3].x0+13, pRect[3].y0+10, 80, 60);
			GUI_MEMDEV_Select(hMem);
#endif
			temp3_error_last = g_temp3_error;
			if (0 == g_temp4_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.d", temp_val.temp4);
				} else {
					sprintf(dispStr, "%.d", TEMP_F2C(temp_val.temp4));
				}
				GUI_DispStringHCenterAt("   ", (pRect[3].x1-pRect[3].x0)/2+pRect[3].x0, pRect[3].y0+18);
				GUI_DispStringHCenterAt(dispStr, (pRect[3].x1-pRect[3].x0)/2+pRect[3].x0, pRect[3].y0+18);
			} else {
				if (temp4_error_last != g_temp4_error) {
					GUI_DispStringHCenterAt("   ", (pRect[3].x1-pRect[3].x0)/2+pRect[3].x0, pRect[3].y0+18);
					GUI_DispStringHCenterAt("---", (pRect[3].x1-pRect[3].x0)/2+pRect[3].x0, pRect[3].y0+18);
				}
			}
#ifdef SRAM_MEMDEV
			GUI_MEMDEV_CopyToLCDAt(hMem, pRect[3].x0+13, pRect[3].y0+10);
			GUI_MEMDEV_Delete(hMem);
#endif		
			temp4_error_last = g_temp4_error;
			if(has_big)//根据是否有big温度选择坐标
			{
#ifdef SRAM_MEMDEV
				hMem = GUI_MEMDEV_Create(  pRect[0].x0, pRect[0].y0-90, 250, 90);
				GUI_MEMDEV_Select(hMem);
#endif
				if(g_is_night_mode==0)
				{
					GUI_SetBkColor(bk_color);
					GUI_SetColor(GUI_BLACK);
				}else{
					GUI_SetBkColor(bk_color);
					GUI_SetColor(GUI_WHITE);
				}
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
				if(g_temp_val_new.temp_unit==0)
					GUI_DispStringAt("°F ",445, 10);
				else
					GUI_DispStringAt("°C",445, 10);

				GUI_SetBkColor(bk_color);
				GUI_SetColor(g_big_temp_color);				
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn89);
				//temp5 flame
				if (0 == g_fatal_error) {
					if (0 == g_temp_val_new.temp_unit) {
						sprintf(dispStr, "%.d", temp_val.temp5);
					} else {
						sprintf(dispStr, "%.d", TEMP_F2C(temp_val.temp5));
					}
					GUI_DispStringHCenterAt("   ",  pRect[0].x1, pRect[0].y0-90);
					if (0 == g_temp5_error) {
						GUI_DispStringHCenterAt(dispStr,pRect[1].x0, pRect[0].y0-90);
					} else {
						GUI_DispStringHCenterAt("---",  pRect[1].x0, pRect[0].y0-90);
					}
				} else if(1 == g_fatal_error){//flame error
					//if (temp5_error_last != g_temp5_error) 
					{
						GUI_DispStringHCenterAt("      ",  pRect[1].x0, pRect[0].y0-90);
						GUI_DispStringHCenterAt("ERROR",  pRect[1].x0, pRect[0].y0-90);
					}
				}else if(2 == g_fatal_error){//sensor error
					{
						GUI_DispStringHCenterAt("      ",  pRect[1].x0, pRect[0].y0-90);
						GUI_DispStringHCenterAt("ERROR",  pRect[1].x0, pRect[0].y0-90);
					}
				}else if(3 == g_fatal_error){//over temp
					{
						GUI_DispStringHCenterAt("      ",  pRect[1].x0, pRect[0].y0-90);
						GUI_DispStringHCenterAt("ERROR",  pRect[1].x0, pRect[0].y0-90);
					}
				}

#ifdef SRAM_MEMDEV
				GUI_MEMDEV_CopyToLCDAt(hMem, pRect[0].x0, pRect[0].y0-90);
				
				GUI_MEMDEV_Delete(hMem);
				
				GUI_MEMDEV_Select(0);
#endif
				temp5_error_last = g_temp5_error;
			}
			
			break;
		default:
			break;
	}
}
int temp_temp=0;
int temp_smoke=0;
// temperature setting ui / smoke setting ui
void GUIDEMO_BigCircleUI(int is_smoke_ui)
{
	int first_enter_flag = 1;
	int pen_size = 0;
	int time_remain = 100;
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
	
	int g_target_temp_val_last = g_target_temp_val;
	
#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;
#endif

	GUI_SetBkColor(bk_color);
	GUIDEMO_ClearScreen(0,0,480,320);//清除屏幕
#if 0
	pen_size = GUI_GetPenSize();
	GUI_SetPenSize(10);
	GUI_SetColor(g_circle_extenal_color);
	GUI_DrawEllipse(130,133,118,118);
	GUI_SetColor(g_circle_internal_color);
	GUI_DrawArc(130,133,118,118,-30,30);
	GUI_SetPenSize(pen_size);
#endif
	GUIDEMO_RightTempUI(NULL, 0, 0, 0);//画4个框
	
	GUIDEMO_ProgressTimeBar(0, 0, 0);//底部灰色条
	
	EC11_Clear();

	//if (g_temp_center != 0) {
	//	temp_val_center = g_temp_center;
	//}

	if (1 == is_smoke_ui) {
		temp_val_center = g_temp_val_new.target_smoke;
	}else{
		temp_val_center = g_temp_val_new.target_val;
	}
	
	do
	{
		ec_sta = EC11_IDLE;
		evt_val = g_event_val_new;
		temp_val = g_temp_val_new;
		
		if (0 == time_remain%10) {
			GUIDEMO_RightTempUI(NULL, 0, 0, 1);//更新框内4个温度
			
			if (2 == is_smoke_ui) {
#ifdef SRAM_MEMDEV
				//GUIDEMO_ClearScreen(55, 60, 210, 160);
				if (0 == first_enter_flag) {
					hMem = GUI_MEMDEV_Create(55, 60, 155, 100);
					GUI_MEMDEV_Select(hMem);
				}
#endif
				GUI_SetBkColor(bk_color);
				GUI_SetColor(g_big_temp_color);
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn120);
				if (0 == g_temp5_error) {
					if (0 == g_temp_val_new.temp_unit) {
						sprintf(dispStr, "%.d", temp_val.temp5);
					} else {
						sprintf(dispStr, "%.d", TEMP_F2C(temp_val.temp5));
					}
					GUI_DispStringHCenterAt("   ",      130, 50);
					GUI_DispStringHCenterAt(dispStr,      130, 50);
				} else {
					if (temp5_error_last != g_temp5_error) {
						GUI_DispStringHCenterAt("   ",      130, 50);
						GUI_DispStringHCenterAt("---",      130, 50);
					}
				}
				
				temp5_error_last = g_temp5_error;
				
#ifdef SRAM_MEMDEV
				if (0 == first_enter_flag) {
					GUI_MEMDEV_CopyToLCDAt(hMem, 55, 60);
					GUI_MEMDEV_Delete(hMem);
					GUI_MEMDEV_Select(0);
				}
#endif
			}
			
			if (1 == g_run_mode) {
				#if FOR_DEBUG_USE
					GUIDEMO_ProgressTimeBar(g_run_mode_counter_mins, g_run_mode_counter_sec, 1);//底部灰色条
				#else
					GUIDEMO_ProgressTimeBar(g_run_mode_counter_hour, g_run_mode_counter_mins, 1);//底部灰色条
				#endif
			}
		}
		
		if (temp_val_center_last != temp_val_center) {
#ifdef SRAM_MEMDEV
			if (0 == first_enter_flag) {
				hMem = GUI_MEMDEV_Create(0, 0, 255, 260);
				GUI_MEMDEV_Select(hMem);
			}
#endif

			g_event_val_new.force_flush = 0;
		
			if (is_smoke_ui != 2) {
				GUI_SetBkColor(bk_color);
				GUIDEMO_ClearScreen(0, 0, 255, 255);
			}
			
			if (1 == is_smoke_ui) {// set smoke
				GUI_SetColor(g_big_smoke_color);
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn134);
				sprintf(dispStr, "%d", temp_val_center);
				GUI_DispStringHCenterAt("   ",      130, 50);
				GUI_DispStringHCenterAt(dispStr,      130, 55);

				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
				GUI_DispStringHCenterAt("SET SMOKE",  130, 180);	
			} else if (0 == is_smoke_ui) {// set temperature
				
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);

				GUI_SetColor(g_big_temp_color);
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.d", temp_val_center);
				} else {
					sprintf(dispStr, "%.d", TEMP_F2C(temp_val_center));
				}
				if (150 == temp_val_center) {
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn79);
					GUI_DispStringHCenterAt("LOW",      130, 45);
					GUI_DispStringHCenterAt("SMOKE",    130, 110);
					
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
					GUI_DispStringHCenterAt("160 SET TEMP",  130, 185);
					
					g_smoke_val_percent = 10;
				} else if (155 == temp_val_center) {
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn79);
					GUI_DispStringHCenterAt("HIGH",     130, 45);
					GUI_DispStringHCenterAt("SMOKE",    130, 110);

					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
					GUI_DispStringHCenterAt("220 SET TEMP",  130, 185);
					
					g_smoke_val_percent = 10;
				} else if (500 == temp_val_center) {
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn114);
					GUI_DispStringHCenterAt("HIGH",     130, 65);

					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
					GUI_DispStringHCenterAt("SET TEMP",  130, 180);
				} else {
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn134);
					GUI_DispStringHCenterAt("   ",      130, 50);
					GUI_DispStringHCenterAt(dispStr,      130, 55);

					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
					GUI_DispStringHCenterAt("SET TEMP",  130, 180);				
				}
				//单位 F/C
				if (g_is_night_mode)// night
				{
					GUI_SetBkColor(GUI_BLACK);
					GUI_SetColor(GUI_WHITE);
					if(g_temp_val_new.temp_unit==0)
						GUI_DispStringAt("°F ",10, 10);
					else
						GUI_DispStringAt("°C",10, 10);
				}else{//day
					GUI_SetBkColor(GUI_WHITE);
					GUI_SetColor(GUI_BLACK);
					if(g_temp_val_new.temp_unit==0)
						GUI_DispStringAt("°F ",10, 10);
					else
						GUI_DispStringAt("°C",10, 10);
				}
			} else if (2 == is_smoke_ui) {// main screen
				GUI_SetColor(g_big_temp_color);
				pen_size = GUI_GetPenSize();
				GUI_SetPenSize(3);
				GUI_DrawLine(55, 190+5, 210, 190+5);
				GUI_SetPenSize(pen_size);
				GUI_SetBkColor(bk_color);
				if (g_is_night_mode) {// night
					GUI_SetColor(GUI_WHITE);
				} else {// day
					GUI_SetColor(GUI_BLACK);
				}
				GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);

				g_smoke_val_percent = g_temp_val_new.target_smoke;

				g_target_temp_val = g_temp_val_new.target_val;
	
				if (150 == g_target_temp_val) {
					GUI_DispStringHCenterAt("160 SET TEMP",  130, 160+5);
					GUI_DispStringHCenterAt("LOW SMOKE",  130, 195+5);

				} else if (155 == g_target_temp_val) {
					GUI_DispStringHCenterAt("220 SET TEMP",  130, 160+5);
					GUI_DispStringHCenterAt("HIGH SMOKE",  130, 195+5);
				} else if (500 == g_target_temp_val) {
					GUI_DispStringHCenterAt("HIGH SET TEMP",  130, 160+5);
					sprintf(dispStr, "%d SMOKE", g_smoke_val_percent);
					GUI_DispStringHCenterAt(dispStr,  130, 195+5);
				} else {
					sprintf(dispStr, "%.d SET TEMP", g_target_temp_val);
					GUI_DispStringHCenterAt(dispStr,  130, 160+5);
					sprintf(dispStr, "%d SMOKE", g_smoke_val_percent);
					GUI_DispStringHCenterAt(dispStr,  130, 195+5);
					
				}
				//单位 F/C
				if (g_is_night_mode)// night
				{
					GUI_SetBkColor(GUI_BLACK);
					GUI_SetColor(GUI_WHITE);
					if(g_temp_val_new.temp_unit==0)
						GUI_DispStringAt("°F ",10, 10);
					else
						GUI_DispStringAt("°C",10, 10);
				}else{//day
					GUI_SetBkColor(GUI_WHITE);
					GUI_SetColor(GUI_BLACK);
					if(g_temp_val_new.temp_unit==0)
						GUI_DispStringAt("°F ",10, 10);
					else
						GUI_DispStringAt("°C",10, 10);
				}
			}
			
			pen_size = GUI_GetPenSize();
			GUI_SetPenSize(10);
			if (g_is_night_mode) {// night
				GUI_SetColor(GUI_WHITE);
			} else {// day
				if (1 == is_smoke_ui) {
					GUI_SetColor(USER_COLOR8_GRAY);
				} else {
					GUI_SetColor(USER_COLOR2_RED);
				}
			}
			GUI_DrawEllipse(130,133,118,118);
			if (g_is_night_mode) {// night
				if (1 == is_smoke_ui) {
					GUI_SetColor(USER_COLOR4_GRAY);
				} else {
					GUI_SetColor(USER_COLOR1_RED);
				}
			} else {// day
				if (1 == is_smoke_ui) {
					GUI_SetColor(GUI_BLACK);
				} else {
					GUI_SetColor(USER_COLOR1_RED);
				}
			}
			GUI_SetPenSize(12);
			if (1 == is_smoke_ui) {// set smoke
				GUI_DrawArc(130,133,118,118,90-temp_val_center*36,90);
			} else {
				if (2 == is_smoke_ui) {
					temp_val_center = g_target_temp_val;
				}
				
				if (495 == temp_val_center) {
					GUI_DrawArc(130,133,118,118,(90-2-(temp_val_center-150)*360/350),90);
				} else {
					GUI_DrawArc(130,133,118,118,(90-5-(temp_val_center-150)*360/350),90);
				}
			}
			GUI_SetPenSize(pen_size);
			
			g_event_val_last.menu_index = evt_val.menu_index;
			
#ifdef SRAM_MEMDEV
			if (0 == first_enter_flag) {
				GUI_MEMDEV_CopyToLCDAt(hMem, 0, 0);
				
				GUI_MEMDEV_Delete(hMem);
				
				GUI_MEMDEV_Select(0);
			}
#endif
			if (1 == first_enter_flag) {
				first_enter_flag = 0;
			}
		}
		
		temp_val_center_last = temp_val_center;

			if(g_temp_val_new.target_update == 1)
			{
				if(is_smoke_ui == 1)
					temp_val_center = g_temp_val_new.target_smoke;
				else
					temp_val_center = g_temp_val_new.target_val;
				
				g_temp_val_new.target_update =0;
			}
		// 扫面EC11编码器有无动作
		if (0 == is_smoke_ui) {// set temperature
			ec_sta = EC11_KEY_Scan(1);
		} else {// set smoke & main screen
			ec_sta = EC11_KEY_Scan(0);
		}

		if (is_smoke_ui != 2) {
			if (EC11_LEFT == ec_sta) {// up
				if (is_smoke_ui) {
					if (temp_val_center < 10) {
						temp_val_center += 1;
						g_event_val_new.force_flush = 1;
						temp_val.target_smoke = temp_val_center;
						
					}
				} else {
					if (temp_val_center < 500) {// menu_index = 1
						temp_val_center += 5;
						g_event_val_new.force_flush = 1;
						temp_val.target_val = temp_val_center;
						
					}
				}
				ec_sta = EC11_IDLE;
			} else if (EC11_RIGHT == ec_sta) {// dn
				if (is_smoke_ui) {
					if (temp_val_center > 1) {
						temp_val_center -= 1;
						g_event_val_new.force_flush = 1;
						temp_val.target_smoke = temp_val_center;
						
					}
				} else {
					if (temp_val_center > 150) {// menu_index = 13
						temp_val_center -= 5;
						g_event_val_new.force_flush = 1;
						temp_val.target_val = temp_val_center;
						
					}
				}
				ec_sta = EC11_IDLE;
			} else if (EC11_LEFT_FAST == ec_sta) {// UP
				if (is_smoke_ui) {
					if (temp_val_center < 10) {
						temp_val_center += 1;
						g_event_val_new.force_flush = 1;
						temp_val.target_smoke = temp_val_center;
						
					}
				} else {
					if (temp_val_center < 500) {
						temp_val_center += BIG_GAP;
						temp_val.target_val = temp_val_center;
						
						g_event_val_new.force_flush = 1;
					}
				}
				ec_sta = EC11_IDLE;
			} else if (EC11_RIGHT_FAST == ec_sta) {// DN
				if (is_smoke_ui) {
					if (temp_val_center > 1) {
						temp_val_center -= 1;
						g_event_val_new.force_flush = 1;
						temp_val.target_smoke = temp_val_center;
						
					}
				} else {
					if (temp_val_center > 150) {
						temp_val_center -= BIG_GAP;
						temp_val.target_val = temp_val_center;
						
						g_event_val_new.force_flush = 1;
					}
				}
				ec_sta = EC11_IDLE;
			} else if (EC11_BUT == ec_sta) {
				if (1 == is_smoke_ui) {//smoke mode 短按进入startup
					g_smoke_val_percent = temp_val_center;
					//g_temp_val_new.target_smoke = temp_val_center;
					temp_smoke = temp_val_center;
					if (0 == g_run_mode) {
						GUIDEMO_StartupInitialUI(0);
					} else {
						GUIDEMO_StartupInitialUI(1);
					}
					ec_sta = EC11_IDLE;// 忽略该消息
				}
			} else if (EC11_BUT_LONG == ec_sta) {
				if (is_smoke_ui != 1) {
					ec_sta = EC11_IDLE;// 忽略该消息
				}
			} else if (EC11_BUT_NO_FREE <= ec_sta) {
				ec_sta = EC11_IDLE;// 忽略该消息
			}
		}
		if (1 == is_smoke_ui) {
			if (temp_val_center > 10) {
				temp_val_center = 10;
			}
			if (temp_val_center < 1) {
				temp_val_center = 1;
			}
		} else if (0 == is_smoke_ui) {
			if (temp_val_center > 500) {
				temp_val_center = 500;
			}
			if (temp_val_center < 150) {
				temp_val_center = 150;
			}
		}
				
		time_remain--;
		if (0 == time_remain) {
			time_remain = 100;
		}
		
//		#if FOR_DEBUG_USE
//		if (g_fatal_error != 0) {
//		#else
//		if ((g_fatal_error != 0) || (g_temp5_error != 0)) {
//		#endif
//			GUI_SetBkColor(bk_color);
//			GUI_SetColor(bk_color);
//			GUI_FillRect(15, 270, 465, 312);
//			delay_ms(1000);

//			GUI_SetColor(USER_COLOR1_RED);
//			//GUI_SetFont(&GUI_FontTradeGothicCn42);
//			
//			while(1) {
//				if (1 == g_fatal_error) {
//					GUI_DispStringAt("FLAME ERROR", 50, 275);
//				} else {
//					GUI_DispStringAt("SENSOR ERROR", 50, 275);
//				}
//				
//				delay_ms(1000);
//				
//				GUI_DispStringAt("                              ", 50, 275);
//				
//				delay_ms(1000);
//			}
//		}
		if(g_fatal_error!=0)
			break;
		direct_switch_check();

		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if ((is_smoke_ui != 2)&&(g_fatal_error==0)) {
		// 退出该界面，跳转到下一个界面
		if (EC11_BUT == ec_sta) {
			g_event_val_new.force_flush = 1;
			if(is_smoke_ui==1)
				g_temp_val_new.target_smoke = temp_val_center;
			else
				//g_temp_val_new.target_val = temp_val_center;
				temp_temp = temp_val_center;
				g_target_temp_val = temp_val_center;
			
				if(g_target_temp_val == 150) g_set_temp=160; //用于控制计算的设定温度
				else if(g_target_temp_val == 155) g_set_temp=220;
				else g_set_temp = g_target_temp_val;
			
			if (g_target_temp_val_last != g_target_temp_val) {
				I_Err_Sum = 0;
				P_out = 0;
				I_out = 0;
				D_out = 0;
				
				g_flame_update_status=1;
			}
			if (temp_val_center >= 160) {
				if (GUI_BLACK == bk_color) {
					g_circle_extenal_color = GUI_WHITE;
					g_circle_internal_color = USER_COLOR4_GRAY;
				} else {
					g_circle_extenal_color = USER_COLOR8_GRAY;
					g_circle_internal_color = GUI_BLACK;
				}
				GUIDEMO_BigCircleUI(1);
			}else{
				if (0 == g_run_mode) {
					GUIDEMO_StartupInitialUI(0);
				} else {
					GUIDEMO_StartupInitialUI(1);
				}
			}
		} else if (EC11_BUT_LONG == ec_sta) {
			if (1 == is_smoke_ui) {
				g_circle_extenal_color = USER_COLOR2_RED;
				g_circle_internal_color = USER_COLOR1_RED;
				if (0 == g_run_mode) {
					GUIDEMO_StartupInitialUI(0);
				} else {
					GUIDEMO_StartupInitialUI(1);
				}
			}
		}
	} else if(g_fatal_error==0){
		GUIDEMO_MainMenu(0);
	}
}

void GUIDEMO_PowerIcon(void)
{
	GUI_POINT pos={128, 50};
	int r=16;
	GUI_SetPenSize(3);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetColor(GUI_WHITE);
		GUI_DrawArc( pos.x,pos.y, r, r, -240, 60 );
		GUI_DrawLine(pos.x,pos.y+3,pos.x,pos.y-r-5);
	}else{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(USER_COLOR1_RED);
		GUI_DrawArc( pos.x,pos.y, r, r, -240, 60 );
		GUI_DrawLine(pos.x,pos.y+3,pos.x,pos.y-r-5);
	}
}
void GUIDEMO_PowerBypassIcon(void)
{
	GUI_POINT pos={128, 50};
	int r=16;
	GUI_POINT aPointArrowUp[] = {
	  {  0,  2 },
	  { 35, 2 },
	  { 35, 0 },
	  { 42, 3 },
	  {35, 6 },
	  {35, 4 },
	  {0, 4 },};
	
	GUI_SetPenSize(3);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetColor(GUI_WHITE);
		GUI_DrawArc( pos.x,pos.y, r, r, -240, -200 );
		GUI_DrawArc( pos.x,pos.y, r, r, -150, -30 );
		GUI_DrawArc( pos.x,pos.y, r, r, 20, 60 );
		GUI_DrawLine(pos.x,pos.y-5,pos.x,pos.y-r-5);
		GUI_FillPolygon (&aPointArrowUp[0], 7, 185-40-30-5, 48);
		
	}else{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(USER_COLOR1_RED);
		GUI_DrawArc( pos.x,pos.y, r, r, -240, -200 );
		GUI_DrawArc( pos.x,pos.y, r, r, -150, -30 );
		GUI_DrawArc( pos.x,pos.y, r, r, 20, 60 );
		GUI_DrawLine(pos.x,pos.y-5,pos.x,pos.y-r-5);
		GUI_FillPolygon (&aPointArrowUp[0], 7, 185-40-30-5, 48);
	}
}

void GUIDEMO_WarnIcon(void)
{
	
	GUI_POINT aPoint[3]={25,6,
										3, 42,
										47,42};
	GUI_SetPenSize(3);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetColor(GUI_WHITE);
		GUI_DrawPolygon(aPoint, 3, 128-22,50-25);
		GUI_DrawLine(131,43,131,55);
		GUI_DrawPoint(131,60);
		
	}else{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(USER_COLOR1_RED);
		GUI_DrawPolygon(aPoint, 3, 128-22,50-25);
		GUI_DrawLine(131,43,131,55);
		GUI_DrawPoint(131,60);
	}

}

void GUIDEMO_WifiIcon(int single)
{
	GUI_POINT pos_lt={443,36};
	int r=5;
	//
	GUI_SetPenSize(5);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
	}else{
		GUI_SetBkColor(USER_COLOR3_GRAY);
		GUI_SetColor(GUI_WHITE);
	}
	GUI_DrawPoint( pos_lt.x+15, pos_lt.y+21);
	GUI_SetPenSize(2);
	GUI_DrawArc( pos_lt.x+15, pos_lt.y+20, r, r, 		 60, 120);
	GUI_DrawArc( pos_lt.x+15, pos_lt.y+20, 2*r, 2*r, 50, 130);
	GUI_DrawArc( pos_lt.x+15, pos_lt.y+20, 3*r, 3*r, 45, 135);
}

void GUIDEMO_BluetoothIcon(void)
{
	GUI_POINT pos_lt={443,60};
	
	GUI_POINT pos_1={8,  7};
	GUI_POINT pos_2={21, 20};
	GUI_POINT pos_3={15, 26};
	GUI_POINT pos_4={15, 4};
	GUI_POINT pos_5={21, 10};
	GUI_POINT pos_6={8,  23};
	
	pos_1.x +=pos_lt.x;
	pos_1.y +=pos_lt.y;
	pos_2.x +=pos_lt.x;
	pos_2.y +=pos_lt.y;
	pos_3.x +=pos_lt.x;
	pos_3.y +=pos_lt.y;
	pos_4.x +=pos_lt.x;
	pos_4.y +=pos_lt.y;
	pos_5.x +=pos_lt.x;
	pos_5.y +=pos_lt.y;
	pos_6.x +=pos_lt.x;
	pos_6.y +=pos_lt.y;
	
	GUI_SetPenSize(2);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
	}else{
		GUI_SetBkColor(USER_COLOR3_GRAY);
		GUI_SetColor(GUI_WHITE);
	}
	GUI_DrawLine(pos_1.x,pos_1.y,pos_2.x,pos_2.y);
	GUI_DrawLine(pos_2.x,pos_2.y,pos_3.x,pos_3.y);
	GUI_DrawLine(pos_3.x,pos_3.y,pos_4.x,pos_4.y);
	GUI_DrawLine(pos_4.x,pos_4.y,pos_5.x,pos_5.y);
	GUI_DrawLine(pos_5.x,pos_5.y,pos_6.x,pos_6.y);
}

void GUIDEMO_BigWifiIcon(int singnal)
{
	GUI_POINT pos_lt={295,224};
	int r=10;
	//
	GUI_SetPenSize(8);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
	}else{
		GUI_SetBkColor(USER_COLOR3_GRAY);
		GUI_SetColor(GUI_WHITE);
	}
	GUI_DrawPoint( pos_lt.x+25, pos_lt.y+32);
	GUI_SetPenSize(3);
	GUI_DrawArc( pos_lt.x+25, pos_lt.y+32, r, r, 		 60, 120);
	GUI_DrawArc( pos_lt.x+25, pos_lt.y+32, 2*r, 2*r, 50, 130);
	GUI_DrawArc( pos_lt.x+25, pos_lt.y+32, 3*r, 3*r, 45, 135);
}

void GUIDEMO_BigBluetoothIcon(void)
{
	GUI_POINT pos_lt={392,220};
	
	GUI_POINT pos_1={13,  14};
	GUI_POINT pos_2={35, 	35};
	GUI_POINT pos_3={24,  45};
	GUI_POINT pos_4={24,  5};
	GUI_POINT pos_5={35,  15};
	GUI_POINT pos_6={13,  36};
	
	pos_1.x +=pos_lt.x;
	pos_1.y +=pos_lt.y;
	pos_2.x +=pos_lt.x;
	pos_2.y +=pos_lt.y;
	pos_3.x +=pos_lt.x;
	pos_3.y +=pos_lt.y;
	pos_4.x +=pos_lt.x;
	pos_4.y +=pos_lt.y;
	pos_5.x +=pos_lt.x;
	pos_5.y +=pos_lt.y;
	pos_6.x +=pos_lt.x;
	pos_6.y +=pos_lt.y;
	
	GUI_SetPenSize(2);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
	}else{
		GUI_SetBkColor(USER_COLOR3_GRAY);
		GUI_SetColor(GUI_WHITE);
	}	
	GUI_DrawLine(pos_1.x,pos_1.y,pos_2.x,pos_2.y);
	GUI_DrawLine(pos_2.x,pos_2.y,pos_3.x,pos_3.y);
	GUI_DrawLine(pos_3.x,pos_3.y,pos_4.x,pos_4.y);
	GUI_DrawLine(pos_4.x,pos_4.y,pos_5.x,pos_5.y);
	GUI_DrawLine(pos_5.x,pos_5.y,pos_6.x,pos_6.y);	
}

void GUIDEMO_AboutRightUI(char *strGrill_pin, char *strNet, char *strBlueTooth, char *strFW)
{
	char dispStr[64] = "";

	GUI_RECT  MiddleBlank_RECT={270,175,463,200};
	
	//添加上部文字
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(bk_color);
		GUI_SetColor(GUI_BLACK);
	}else{
		GUI_SetBkColor(bk_color);
		GUI_SetColor(GUI_WHITE);
	}
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
	GUI_DispStringAt("ABOUT", 270, 20);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
	sprintf(dispStr, "GRILL PIN // %s", strGrill_pin);
	GUI_DispStringAt(dispStr, 270, 20+28);
	sprintf(dispStr, "NETWORK // %s", strNet);
	GUI_DispStringAt(dispStr, 270, 20+28*2);
	sprintf(dispStr, "BLUETOOTH // %s", strBlueTooth);
	GUI_DispStringAt(dispStr, 270, 20+28*3);
	sprintf(dispStr, "FIRMWARE // %s", strFW);
	GUI_DispStringAt(dispStr, 270, 20+28*4);
	//中部灰色条
	if(g_is_night_mode==0)
		GUI_SetColor(USER_COLOR3_GRAY);	
	else
		GUI_SetColor(GUI_WHITE);	
	GUI_FillRect(MiddleBlank_RECT.x0, MiddleBlank_RECT.y0, MiddleBlank_RECT.x1, MiddleBlank_RECT.y1);	
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(USER_COLOR3_GRAY);
		GUI_SetColor(GUI_WHITE);
	}else{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(USER_COLOR3_GRAY);
	}
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
	GUI_DispStringAt("SIGNAL STRENGTH", 302, 178);
	

	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
	}else{
		GUI_SetBkColor(bk_color);
		GUI_SetColor(GUI_WHITE);
	}
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn15);
	GUI_DispStringAt("CONNECTED", 290, 280);
	GUI_DispStringAt("CONNECTED", 390, 280);
	GUIDEMO_BigWifiIcon(4);
	GUIDEMO_BigBluetoothIcon();
	
	
	if(g_is_night_mode==0)
		GUI_SetColor(USER_COLOR1_RED);
	else
		GUI_SetColor(GUI_WHITE);
	GUI_DrawLine(372, 217, 372, 295);
}

/*
GUIDEMO_LeftMenuItems:画左侧有6个选项的菜单项
int sel_new:选择的ITEM(0~5)
int sel_last:旧的选择的ITEM(0~5)
int update：更新内容选择0：画整个6菜单项，并选中在sel_new上 1：只更新选择项，即sel_new画红色，sel_last画灰色，这个用于旋转按钮时菜单的切换
int ui：界面文字选择共有两种(0~1)  0:主界面的6个  1：setup的5个+最后一个灰色，为1时选择sel_new sel_last的范围是0~4
*/
void GUIDEMO_LeftMenuItems(int sel_new,int sel_last,int update, int ui)
{
	int i;
#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;
#endif
	
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
	
	//GUIDEMO_ClearScreen(0, 0, 245, 320);
	
	if(g_temp_val_new.temp_unit==0)
		MainMenuTextAll[1][1]="UINTS // °F";
	else
		MainMenuTextAll[1][1]="UINTS // °C";
	if (!update)
	{
		//画6个条
		GUI_SetColor(g_unselected_menu_color);
		for(i=0;i<6;i++)
			GUI_FillRect(left_menu[i].x0, left_menu[i].y0,  left_menu[i].x1, left_menu[i].y1);
		//添加文字
		GUI_SetBkColor(g_unselected_menu_color);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
		GUI_SetColor(GUI_BLACK);
		if(g_is_night_mode==1)
			MainMenuTextAll[0][1]="DAY MODE";
		else
			MainMenuTextAll[0][1]="NIGHT MODE";
			
		for(i=0;i<6;i++)
		{
				GUI_DispStringAt(MainMenuTextAll[i][ui],Text[i].x, Text[i].y);
		}
		//默认选择sel_new
		GUI_SetColor(USER_COLOR1_RED);
		GUI_FillRect(left_menu[sel_new].x0, left_menu[sel_new].y0,  left_menu[sel_new].x1, left_menu[sel_new].y1);	
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt(MainMenuTextAll[sel_new][ui],  Text[sel_new].x, Text[sel_new].y);	
	} else {
#ifdef SRAM_MEMDEV
		hMem = GUI_MEMDEV_Create(left_menu[sel_last].x0, left_menu[sel_last].y0, left_menu[sel_last].x1-left_menu[sel_last].x0+1, left_menu[sel_last].y1-left_menu[sel_last].y0+1);
		GUI_MEMDEV_Select(hMem);
#endif
		//还原sel_last选择项为灰色
		GUI_SetColor(g_unselected_menu_color);
		GUI_FillRect(left_menu[sel_last].x0, left_menu[sel_last].y0,  left_menu[sel_last].x1, left_menu[sel_last].y1);	
		GUI_SetBkColor(g_unselected_menu_color);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt(MainMenuTextAll[sel_last][ui],  Text[sel_last].x, Text[sel_last].y);
#ifdef SRAM_MEMDEV
		GUI_MEMDEV_CopyToLCDAt(hMem, left_menu[sel_last].x0, left_menu[sel_last].y0);
		GUI_MEMDEV_Delete(hMem);
		
		hMem = GUI_MEMDEV_Create(left_menu[sel_new].x0, left_menu[sel_new].y0, left_menu[sel_new].x1-left_menu[sel_new].x0+1, left_menu[sel_new].y1-left_menu[sel_new].y0+1);
		GUI_MEMDEV_Select(hMem);
#endif
		//更新sel_new选择项为红色
		GUI_SetColor(USER_COLOR1_RED);
		GUI_FillRect(left_menu[sel_new].x0, left_menu[sel_new].y0,  left_menu[sel_new].x1, left_menu[sel_new].y1);	
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt(MainMenuTextAll[sel_new][ui],  Text[sel_new].x, Text[sel_new].y);
#ifdef SRAM_MEMDEV
		GUI_MEMDEV_CopyToLCDAt(hMem, left_menu[sel_new].x0, left_menu[sel_new].y0);
		
		GUI_MEMDEV_Delete(hMem);
		
		GUI_MEMDEV_Select(0);
#endif
	}
}

/*
画shutdown complete界面
*/
void GUIDEMO_ShutdownCompleteUI(void)
{
	GUI_RECT logo={10, 10,  243,  308};
	int temp_val[5]={11,12,13,14};
	int big_temp=0;
	int time_remain=10;
	//画左侧界面
	if(g_is_night_mode==0)
		GUI_SetColor(USER_COLOR1_RED);
	else
		GUI_SetColor(GUI_WHITE);
	GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetColor(GUI_WHITE);
	}else{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
	}
	//logo
//	image_display(logo_pos.x,logo_pos.y,(u8*)gImage_power);
	
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
	
	GUI_DispStringAt("SHUTDOWN",		    63, 93);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn38);
	GUI_DispStringAt("COMPLETE",		    62, 147);
	GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
	GUI_DispStringAt("CYCLE POWER",		    82, 190);
	
	while(1){
		if ((0 == time_remain%10)) {
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		delay_ms(100);
	}//更新温度
}

/*
	GUIDEMO_LeftOneExitUI:画左侧底部只有一个exit选项的左侧界面
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
void GUIDEMO_LeftOneExitUI(int time1, int time2,int ui_sel)
{	
	char dispStr[64] = "";
	GUI_RECT logo={10, 10,  243,  258};
	GUI_RECT Item_rect = {10, 260, 243, 308};
	char *Item_str={"EXIT"};
	GUI_POINT Item_pos={95, 12+50*5};//exit

#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;
	hMem = GUI_MEMDEV_Create(0, 0, 245, 320);
	GUI_MEMDEV_Select(hMem);
#endif

	//画左侧界面
	if(g_is_night_mode==0)
		GUI_SetColor(USER_COLOR1_RED);
	else
		GUI_SetColor(GUI_WHITE);
	GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
	if(g_is_night_mode==0)
	{
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetColor(GUI_WHITE);
	}else{
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
	}
	switch(ui_sel)
	{
		case UI_EXIT_FEED_MODE:
			GUIDEMO_WarnIcon();
			if(g_is_night_mode==0)
			{
				GUI_SetBkColor(USER_COLOR1_RED);
				GUI_SetColor(GUI_WHITE);
			}else{
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
			GUI_DispStringAt("FEED MODE",		    65, 93);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn38);
			GUI_DispStringAt("COMPLETE",		    63, 155);
			break;
		case UI_EXIT_SHUTDOWN :
			GUIDEMO_PowerIcon();
			if(g_is_night_mode==0)
			{
				GUI_SetBkColor(USER_COLOR1_RED);
				GUI_SetColor(GUI_WHITE);
			}else{
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
			GUI_DispStringAt("SHUTDOWN", 65, 93);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn38);
			if(g_is_night_mode!=0)
				GUI_SetColor(USER_COLOR3_GRAY);
			sprintf(dispStr,"%.2d:%.2d",time1, time2);
			GUI_DispStringAt(dispStr,		 95, 153);
			break;
		case UI_EXIT_FLAME_ERROR:
			GUIDEMO_WarnIcon();
			if(g_is_night_mode==0)
			{
				GUI_SetBkColor(USER_COLOR1_RED);
				GUI_SetColor(GUI_WHITE);
			}else{
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
			GUI_DispStringAt("FLAME ERROR", 45, 93);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn23);
			if(g_is_night_mode!=0)
				GUI_SetColor(USER_COLOR3_GRAY);
			GUI_DispStringAt("FLAME HAS EXTINGUISHED.", 21, 135);
			GUI_DispStringAt("CLEAN GRILL BEFORE USE.", 22, 164);
			break;
		case UI_EXIT_SENSOR:
			GUIDEMO_WarnIcon();
			if(g_is_night_mode==0)
			{
				GUI_SetBkColor(USER_COLOR1_RED);
				GUI_SetColor(GUI_WHITE);
			}else{
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
			GUI_DispStringAt("SENSOR", 84, 88);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn23);
			if(g_is_night_mode!=0)
				GUI_SetColor(USER_COLOR3_GRAY);
			GUI_DispStringAt("INTERNAL THERMOMETER", 27, 129);
			GUI_DispStringAt("HAS FAILED.CLEAN OR", 37, 158);
			GUI_DispStringAt("REPLACE BEFORE USE.", 37, 188);
			break;
		case UI_EXIT_OVER_TEMP:
			GUIDEMO_WarnIcon();
			if(g_is_night_mode==0)
			{
				GUI_SetBkColor(USER_COLOR1_RED);
				GUI_SetColor(GUI_WHITE);
			}else{
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
			GUI_DispStringAt("OVER TEMP", 62, 93);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn23);
			if(g_is_night_mode!=0)
				GUI_SetColor(USER_COLOR3_GRAY);
			GUI_DispStringAt("NOEMAL OPERATING", 50, 135);
			GUI_DispStringAt("TEMPERATURE HAS BEEN", 31, 162);
			GUI_DispStringAt("EXCEEDED.CLEAN GRILL.", 35, 190);
			GUI_DispStringAt("BEFORE USE.", 82, 217);
			break;
		case UI_EXIT_FEED_NOT:
			GUIDEMO_WarnIcon();
			if(g_is_night_mode==0)
			{
				GUI_SetBkColor(USER_COLOR1_RED);
				GUI_SetColor(GUI_WHITE);
			}else{
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn23);
			GUI_DispStringAt("FEED NOT AVAILABLE WHEN", 20, 97);
			GUI_DispStringAt("GRILL IS OPERATION.", 45, 130);
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
	
#ifdef SRAM_MEMDEV
		GUI_MEMDEV_CopyToLCDAt(hMem, 0, 0);
		GUI_MEMDEV_Delete(hMem);
		GUI_MEMDEV_Select(0);
#endif
}

/*
GUIDEMO_LeftOneCancelUI:画左侧底部只有一个cancel选项的左侧界面
int time1：小时
int time2：分钟
*/
void GUIDEMO_LeftOneCancelUI(int time1, int time2, int mode)
{
	char dispStr[64] = "";
	GUI_RECT logo={10, 10,  243,  258};
	GUI_RECT Item_rect={10, 260, 243, 308};
	char *Item_str="CANCEL";
	GUI_POINT Item_pos={65, 12+50*5};//cancel

#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;
	hMem = GUI_MEMDEV_Create(0, 0, 245, 320);
	GUI_MEMDEV_Select(hMem);
#endif

	if (mode != 2) {
		//画左侧界面
		if(g_is_night_mode==0)
			GUI_SetColor(USER_COLOR1_RED);
		else
			GUI_SetColor(GUI_WHITE);
		GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
		if(g_is_night_mode==0)
			GUI_SetColor(USER_COLOR7_GRAY);
		else
			GUI_SetColor(GUI_WHITE);
		GUI_FillRect(Item_rect.x0, Item_rect.y0,  Item_rect.x1, Item_rect.y1);

		GUIDEMO_WarnIcon();
		if(g_is_night_mode==0)
		{
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(GUI_WHITE);
		}else{
			GUI_SetBkColor(GUI_WHITE);
			GUI_SetColor(GUI_BLACK);
		}
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
		if (0 == mode) {
			GUI_DispStringHCenterAt("FEED MODE",		    126, 90);
		} else {
			GUI_DispStringHCenterAt("SHUTDOWN", 126, 90);
		}
	} else {// update timer only
		if (0 == g_is_night_mode) {
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(GUI_WHITE);
		} else {
			GUI_SetBkColor(GUI_WHITE);
			GUI_SetColor(GUI_BLACK);
		}
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn38);
		sprintf(dispStr,"%.2d:%.2d",time1, time2);
		GUI_DispStringHCenterAt(dispStr,		 126, 151);
	}
	
	if (mode != 2) {
		//选中项画红色
		GUI_SetColor(USER_COLOR1_RED);
		GUI_FillRect(Item_rect.x0, Item_rect.y0,  Item_rect.x1, Item_rect.y1);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
		GUI_SetBkColor(USER_COLOR1_RED);
		GUI_SetColor(GUI_WHITE);
		if (0 == mode) {
			GUI_DispStringAt(Item_str,Item_pos.x, Item_pos.y);
		} else {
			GUI_DispStringAt("EXIT",Item_pos.x + 25, Item_pos.y);
		}
	}
	
#ifdef SRAM_MEMDEV
		GUI_MEMDEV_CopyToLCDAt(hMem, 0, 0);
		GUI_MEMDEV_Delete(hMem);
		GUI_MEMDEV_Select(0);
#endif
}

/*
GUIDEMO_StartupTimerLeftUI:画左侧底部有bypass cancel选项的左侧界面
int sel：当前选择项0~1
int time1：小时 
int time2：分钟 
int update：更新选择 0：画整个界面 1：只更新选择项(旋转更新界面可以选为1) 2: 只更新Timer
int ui_sel：界面文字选择，可选择为一下宏：
#define UI_BYPASS_CANCEL_SHUTDOWN        		6
*/
void GUIDEMO_StartupTimerLeftUI(int sel, int time1, int time2, int update, int ui_sel)
{
	char dispStr[64] = "";
	GUI_RECT logo={10, 10,  243,  208};
	GUI_RECT Item_rect[] = {{10, 210, 243, 258},
									        {10, 260, 243, 308}};
	char *Item_str[]={"BYPASS","CANCEL"};
	GUI_POINT Item_pos[]={{65, 12+50*4},//bypass
										    {65, 12+50*5}};//cancel

#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;
#endif

	switch(update){
		case 0://画左侧界面
			if(g_is_night_mode==0)
				GUI_SetColor(USER_COLOR1_RED);
			else
				GUI_SetColor(GUI_WHITE);
			GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
			if(g_is_night_mode==0)
				GUI_SetColor(USER_COLOR7_GRAY);
			else
				GUI_SetColor(GUI_WHITE);
			
			GUI_FillRect(Item_rect[0].x0, Item_rect[0].y0,  Item_rect[0].x1, Item_rect[0].y1);
			GUI_FillRect(Item_rect[1].x0, Item_rect[1].y0,  Item_rect[1].x1, Item_rect[1].y1);
			if(g_is_night_mode==0)
			{
				GUI_SetBkColor(USER_COLOR1_RED);
				GUI_SetColor(GUI_WHITE);
			}else{
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			switch(ui_sel)
			{
				case UI_BYPASS_CANCEL_SHUTDOWN:
					GUIDEMO_PowerIcon();
					if(g_is_night_mode==0)
					{
						GUI_SetBkColor(USER_COLOR1_RED);
						GUI_SetColor(GUI_WHITE);
					}else{
						GUI_SetBkColor(GUI_WHITE);
						GUI_SetColor(GUI_BLACK);
					}
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
					GUI_DispStringHCenterAt("STARTUP",		    126, 90);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn38);
					if(g_is_night_mode!=0)
						GUI_SetColor(USER_COLOR3_GRAY);
					sprintf(dispStr,"%.2d:%.2d",time1, time2);
					GUI_DispStringHCenterAt(dispStr,		 126, 150);
					break;
				default:
					break;
			}
			break;
		case 1://更新选择项
#ifdef SRAM_MEMDEV
			hMem = GUI_MEMDEV_Create(0, 210, 245, 110);
			GUI_MEMDEV_Select(hMem);
#endif
			//选中项画红色
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(Item_rect[sel].x0, Item_rect[sel].y0,  Item_rect[sel].x1, Item_rect[sel].y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringAt(Item_str[sel],Item_pos[sel].x, Item_pos[sel].y);
			//未选中项画灰色
			sel = (sel+1)&0x01;
			if(g_is_night_mode==0)
				GUI_SetColor(USER_COLOR7_GRAY);
			else
				GUI_SetColor(GUI_WHITE);
			GUI_FillRect(Item_rect[sel].x0, Item_rect[sel].y0,  Item_rect[sel].x1, Item_rect[sel].y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			if(g_is_night_mode==0)
			{
				GUI_SetBkColor(USER_COLOR7_GRAY);
				GUI_SetColor(GUI_BLACK);
			}else{
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			GUI_DispStringAt(Item_str[sel],Item_pos[sel].x, Item_pos[sel].y);
#ifdef SRAM_MEMDEV
		GUI_MEMDEV_CopyToLCDAt(hMem, 0, 210);
		GUI_MEMDEV_Delete(hMem);
		GUI_MEMDEV_Select(0);
#endif
			break;
		case 2:
#ifdef SRAM_MEMDEV
			hMem = GUI_MEMDEV_Create(80, 140, 150, 50);
			GUI_MEMDEV_Select(hMem);
#endif
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn38);
			if (1 == g_is_night_mode) {// day
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(USER_COLOR3_GRAY);
			} else {// night
				GUI_SetBkColor(USER_COLOR1_RED);
				GUI_SetColor(GUI_WHITE);
			}
			sprintf(dispStr,"%.2d:%.2d",time1, time2);
			GUI_DispStringHCenterAt(dispStr,		 126, 150);
#ifdef SRAM_MEMDEV
		GUI_MEMDEV_CopyToLCDAt(hMem, 80, 140);
		GUI_MEMDEV_Delete(hMem);
		GUI_MEMDEV_Select(0);
#endif
			break;
		default:
			break;
	}
}

/*
GUIDEMO_LeftConfirmCancelUI:画左侧底部有confirm cancel两个选项的左侧界面
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
void GUIDEMO_LeftConfirmCancelUI(int sel, int temp, int smoke, int update, int ui_sel)
{
	char dispStr[64] = "";
	GUI_RECT logo={10, 10,  243,  208};
	GUI_RECT Item_rect[] = {{10, 210, 243, 258},
									        {10, 260, 243, 308}};
	char *Item_str[]={"CONFIRM","CANCEL"};
	GUI_POINT Item_pos[]={{50, 12+50*4},//comfirm
										    {60, 12+50*5}};//cancel
#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;
#endif

	switch(update){
		case 0://画左侧界面
			if(g_is_night_mode==0)
				GUI_SetColor(USER_COLOR1_RED);
			else
				GUI_SetColor(GUI_WHITE);
			GUI_FillRect(logo.x0, logo.y0,  logo.x1, logo.y1);
			if(g_is_night_mode==0)
				GUI_SetColor(USER_COLOR7_GRAY);
			else
				GUI_SetColor(GUI_WHITE);
			GUI_FillRect(Item_rect[0].x0, Item_rect[0].y0,  Item_rect[0].x1, Item_rect[0].y1);
			GUI_FillRect(Item_rect[1].x0, Item_rect[1].y0,  Item_rect[1].x1, Item_rect[1].y1);
				if(g_is_night_mode==0)
				{
					GUI_SetBkColor(USER_COLOR1_RED);
					GUI_SetColor(GUI_WHITE);
				}else{
					GUI_SetBkColor(GUI_WHITE);
					GUI_SetColor(GUI_BLACK);
				}	
			switch(ui_sel)
			{
				case UI_CONFITRM_CANCEL_STARTUP:
					GUIDEMO_PowerIcon();
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
					if(g_is_night_mode!=0)
						GUI_SetColor(GUI_BLACK);
					GUI_DispStringHCenterAt("STARTUP",		    126, 90);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
					if(g_is_night_mode!=0)
						GUI_SetColor(USER_COLOR3_GRAY);
					
					if (500 == temp) {
						sprintf(dispStr,"HIGH // %.d SMOKE", smoke);
						GUI_DispStringHCenterAt(dispStr,		 126, 128);
					} else if (150 == temp) {
						sprintf(dispStr,"LOW SMOKE");
						GUI_DispStringHCenterAt(dispStr,		 126, 128);
					} else if (155 == temp) {
						sprintf(dispStr,"HIGH SMOKE");
						GUI_DispStringHCenterAt(dispStr,		 126, 128);
					} else {
						sprintf(dispStr,"%.3d TEMP // %.d SMOKE",temp, smoke);
						GUI_DispStringHCenterAt(dispStr,		 126, 128);
					}
					break;
				case UI_CONFITRM_CANCEL_APLPLY :
					GUIDEMO_PowerIcon();
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
					if(g_is_night_mode!=0)
						GUI_SetColor(GUI_BLACK);
					GUI_DispStringHCenterAt("Apply Changes", 126, 90);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
					if(g_is_night_mode!=0)
						GUI_SetColor(USER_COLOR3_GRAY);
					sprintf(dispStr,"%.3d TEMP // %.2d SMOKE",temp, smoke);
					GUI_DispStringHCenterAt(dispStr,		 126, 128);
					break;
				case UI_CONFITRM_CANCEL_ONLY_USE_FOR:
					GUIDEMO_WarnIcon();
					if(g_is_night_mode!=0)
						GUI_SetColor(USER_COLOR3_GRAY);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn23);
					GUI_DispStringHCenterAt("ONLY USE FOR INITIAL", 126, 90);
					GUI_DispStringHCenterAt("SETUP OR WHEN GRILL HAS", 126, 118);
					GUI_DispStringHCenterAt("RUN OUT OF PELLETS", 126, 146);
					break;
				case UI_CONFITRM_CANCEL_ONLY_USE_BYPASS:
					GUIDEMO_PowerBypassIcon();
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn23);
					if(g_is_night_mode!=0)
						GUI_SetColor(USER_COLOR3_GRAY);
					GUI_DispStringHCenterAt("ONLY USE BYPASS WHEN", 126, 85);
					GUI_DispStringHCenterAt("GRILL IS STILL HOT. DON'T", 126, 112);
					GUI_DispStringHCenterAt("USE IF GRILL HAS BEEN OFF", 126, 139);
					GUI_DispStringHCenterAt("FOR MORE THAN 3 MINUTES", 126, 166);
					break;
				case UI_CONFITRM_CANCEL_SHUTDOWN:
					GUIDEMO_PowerIcon();
					if(g_is_night_mode!=0)
						GUI_SetColor(GUI_BLACK);
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn34);
					GUI_DispStringHCenterAt("SHUTDOWN", 126, 90);
					break;
				case UI_CONFITRM_CANCEL_RESET:
					GUIDEMO_WarnIcon();
					GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn23);
					if(g_is_night_mode!=0)
						GUI_SetColor(USER_COLOR3_GRAY);
					GUI_DispStringHCenterAt("RESET PELLET GRILL TO", 126, 84);
					GUI_DispStringHCenterAt("FACTORY SETTINGS.", 126, 112);
					GUI_DispStringHCenterAt("(FORGET NETWORK)", 126, 152);
					break;
				default:
					break;
			}
			break;
		case 1://更新选择项
#ifdef SRAM_MEMDEV
			hMem = GUI_MEMDEV_Create(Item_rect[sel].x0, Item_rect[sel].y0, Item_rect[sel].x1-Item_rect[sel].x0+1, Item_rect[sel].y1-Item_rect[sel].y0+1);
			GUI_MEMDEV_Select(hMem);
#endif
			//选中项画红色
			GUI_SetColor(USER_COLOR1_RED);
			GUI_FillRect(Item_rect[sel].x0, Item_rect[sel].y0,  Item_rect[sel].x1, Item_rect[sel].y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			GUI_SetBkColor(USER_COLOR1_RED);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringAt(Item_str[sel],Item_pos[sel].x, Item_pos[sel].y);
			
#ifdef SRAM_MEMDEV
			GUI_MEMDEV_CopyToLCDAt(hMem, Item_rect[sel].x0, Item_rect[sel].y0);
			GUI_MEMDEV_Delete(hMem);
#endif

			//未选中项画灰色
			sel = (sel+1)&0x01;
#ifdef SRAM_MEMDEV
			hMem = GUI_MEMDEV_Create(Item_rect[sel].x0, Item_rect[sel].y0, Item_rect[sel].x1-Item_rect[sel].x0+1, Item_rect[sel].y1-Item_rect[sel].y0+1);
			GUI_MEMDEV_Select(hMem);
#endif
			if(g_is_night_mode==0)
				GUI_SetColor(USER_COLOR7_GRAY);
			else
				GUI_SetColor(GUI_WHITE);
			GUI_FillRect(Item_rect[sel].x0, Item_rect[sel].y0,  Item_rect[sel].x1, Item_rect[sel].y1);
			GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn44);
			if(g_is_night_mode==0)
			{
				GUI_SetBkColor(USER_COLOR7_GRAY);
				GUI_SetColor(GUI_BLACK);
			}else{
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			GUI_DispStringAt(Item_str[sel],Item_pos[sel].x, Item_pos[sel].y);
#ifdef SRAM_MEMDEV
			GUI_MEMDEV_CopyToLCDAt(hMem, Item_rect[sel].x0, Item_rect[sel].y0);
			GUI_MEMDEV_Delete(hMem);
			GUI_MEMDEV_Select(0);
#endif
			break;
		default:
			break;
	}
}

void GUIDEMO_SetupSubItemsUI(int index_new, int index_last)
{
	int about_flag = 0;
	int time_remain = 10;
	int menu_index=index_new;
	int menu_index_last=index_last;
	int temp_val[5]={11,12,13,14};
	int big_temp=180;
	EC11_STA ec_sta = EC11_IDLE;
	
	GUIDEMO_ClearScreen(0,0,480,320);//清除屏幕
	GUIDEMO_LeftMenuItems(menu_index, menu_index_last, 0, 1);//画左侧6个菜单
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			if (0 == about_flag) {
				GUIDEMO_UpdateTemp(temp_val);
				GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
				GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
			}
		}
		
		if ((3 == menu_index_last) && (menu_index != 3)) {
			if (1 == about_flag) {
				about_flag = 0;
				GUIDEMO_ClearScreen(260,0,480,320);
				GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
				GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
				GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
			}
		}
			
		menu_index_last = menu_index;
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(0);

		if (EC11_LEFT == ec_sta) {// up
			if(menu_index>0)
				menu_index--;
			else
				menu_index=4;
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(menu_index<4)
				menu_index++;
			else
				menu_index=0;
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		if (EC11_BUT == ec_sta) {
			if (1 == menu_index) {
				if (g_temp_val_new.temp_unit) {
					g_temp_val_new.temp_unit = 0;
				} else {
					g_temp_val_new.temp_unit = 1;
				}
				GUIDEMO_LeftMenuItems(1, 0, 1, 1);//更新选择界面
				ec_sta = EC11_IDLE;
			} else if (3 == menu_index) {
				about_flag = 1;
				GUIDEMO_ClearScreen(260,0,480,320);
				GUIDEMO_AboutRightUI("123456","CC-Guest","IOS DEVICE", "CCPG V1.7");
				ec_sta = EC11_IDLE;
			}
		}

		if (menu_index_last != menu_index)
		{
			// 被选中的项目
			GUIDEMO_LeftMenuItems(menu_index, menu_index_last, 1, 1);//更新选择界面
		}
		
		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		if (0 == menu_index) {
			if (g_is_night_mode) {
				g_is_night_mode = 0;
			} else {
				g_is_night_mode = 1;
			}
			GUIDEMO_DayNightSwitch();
			GUIDEMO_SetupSubItemsUI(0, 0);// update all screen
		} else if (2 == menu_index) {
			GUIDEMO_ResetInitialUI();
		} else if (4 == menu_index) {
			GUIDEMO_MainMenu(3);
		}
	}
}

void GUIDEMO_FeedNotAvaliableUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	GUIDEMO_LeftOneExitUI(0xFF, 0xFF, UI_EXIT_FEED_NOT);//画左侧框
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
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
		if(g_fatal_error!=0)
			break;
		direct_switch_check();		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		GUIDEMO_MainMenu(1);
		g_fatal_error = 0;
	}
}

void GUIDEMO_OverTempErrorUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	GUIDEMO_LeftOneExitUI(0xFF, 0xFF, UI_EXIT_OVER_TEMP);//画左侧框
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
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
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		GUIDEMO_MainMenu(0);
		g_fatal_error = 0;
	}
}

void GUIDEMO_SensorErrorUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	GUIDEMO_LeftOneExitUI(0xFF, 0xFF, UI_EXIT_SENSOR);//画左侧框
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
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
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		GUIDEMO_MainMenu(0);
		g_fatal_error = 0;
	}
}

void GUIDEMO_FlameErrorUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	GUIDEMO_LeftOneExitUI(0xFF, 0xFF, UI_EXIT_FLAME_ERROR);//画左侧框
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
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
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		GUIDEMO_MainMenu(0);
		g_fatal_error = 0;
	}
}

void GUIDEMO_FeedCompleteUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	GUIDEMO_LeftOneExitUI(12, 0, UI_EXIT_FEED_MODE);//画左侧框
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}

		time_remain--;
		if(g_fatal_error!=0)
			break;		
		direct_switch_check();
		if (0 == time_remain) {
			time_remain = 10;
		}
		
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		GUIDEMO_MainMenu(1);// main screen
	}
}

void GUIDEMO_FeedShutdownTmerUI(int mode)
{
	int time_remain = 10;
	int timer_counter = 0;
	EC11_STA ec_sta = EC11_IDLE;
	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	if (0 == mode) {
		g_feed_mode = 1;
		g_shutdown_mode = 0;
		g_feed_mode_counter = 7 * 60;// 7 minutes
#if FOR_DEBUG_USE
		g_feed_mode_counter /= 10;
#endif
		timer_counter = g_feed_mode_counter;
	} else {
		g_feed_mode = 0;
		g_shutdown_mode = 1;
		g_shutdown_mode_counter = 20 * 60;// 10 minutes
#if FOR_DEBUG_USE
		g_shutdown_mode_counter /= 10;
#endif
		timer_counter = g_shutdown_mode_counter;
	}

	g_startup_mode = 0;
	g_run_mode = 0;
	
	EC11_Clear();

	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	GUIDEMO_LeftOneCancelUI(timer_counter/60, timer_counter%60, mode);//画左侧框
	GUIDEMO_LeftOneCancelUI(timer_counter/60, timer_counter%60, 2);//画左侧框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
			
			if (0 == mode) {
				timer_counter = g_feed_mode_counter;
			} else {
				timer_counter = g_shutdown_mode_counter;
			}
			
			GUIDEMO_LeftOneCancelUI(timer_counter/60, timer_counter%60, 2);//画左侧框
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(0);

		if (EC11_LEFT == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}

		if (0 == timer_counter) {
			break;
		}
		
		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		g_feed_mode = 0;
		g_shutdown_mode = 0;
		if (0 == mode)
			GUIDEMO_MainMenu(1);
		else
			GUIDEMO_MainMenu(2);
		

	}
	
	if (0 == timer_counter) {
		if (0 == mode) {
			GUIDEMO_FeedCompleteUI();
		} else {
			GUIDEMO_ShutdownCompleteUI();
		}
	}
}

void GUIDEMO_StartupTimerUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	int sel_last = 0;

	int temp_val[5]={31,32,33,34};
	int big_temp=182;

#ifdef SRAM_MEMDEV
	GUI_MEMDEV_Handle hMem;
#endif

	if (!g_is_lastui_bypassdetail) {
		g_startup_mode = 1;
		g_startup_mode_counter = 6 * 60;// 6 minutes
#if FOR_DEBUG_USE
		g_startup_mode_counter /= 10;
#endif
	}
	g_is_lastui_bypassdetail = 0;
	
#ifdef SRAM_MEMDEV
	hMem = GUI_MEMDEV_Create(0, 0, 245, 320);
	GUI_MEMDEV_Select(hMem);
#endif

	GUIDEMO_ClearScreen(0,0,245,320);//清除屏幕
	GUIDEMO_StartupTimerLeftUI(sel, g_startup_mode_counter/60, g_startup_mode_counter%60, 0, UI_BYPASS_CANCEL_SHUTDOWN);//画左侧框

	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框

#ifdef SRAM_MEMDEV
	GUI_MEMDEV_CopyToLCDAt(hMem, 0, 0);
	GUI_MEMDEV_Delete(hMem);
	GUI_MEMDEV_Select(0);
#endif

	GUIDEMO_StartupTimerLeftUI(sel, g_startup_mode_counter/60, g_startup_mode_counter%60, 1, UI_BYPASS_CANCEL_SHUTDOWN);//更新选中项
	
	do {		
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
			
			GUIDEMO_StartupTimerLeftUI(0xFF, g_startup_mode_counter/60, g_startup_mode_counter%60, 2, 0xFF);//画左侧框
		}
		
		if (0 == g_startup_mode_counter) {
			break;
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(0);
		
		if (EC11_LEFT == ec_sta) {
			if (sel > 0) {
				sel--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if (sel < 1) {
				sel++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		if (sel_last != sel) {
			GUIDEMO_StartupTimerLeftUI(sel, g_startup_mode_counter/60, g_startup_mode_counter%60, 1, UI_BYPASS_CANCEL_SHUTDOWN);//更新选中项
			sel_last = sel;
		}

		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_BUT != ec_sta);
	
	if (EC11_BUT == ec_sta) {
		if (0 == sel) {
			GUIDEMO_StartupTimerBypassedUI();
		} else {
			g_startup_mode=0;//cancel 取消startup mode
			GUIDEMO_MainMenu(0);
		}
	}
	
	if ((0 == g_startup_mode_counter)&&(g_fatal_error==0)) {
		g_run_mode = 1;
		GUIDEMO_BigCircleUI(2);// main screen
	}
}

void GUIDEMO_ResetInitialUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	int sel_last = 0;

	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	//GUIDEMO_ClearScreen(0,0,480,320);//清除屏幕
	GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 0, UI_CONFITRM_CANCEL_RESET);//画左侧框
	GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_RESET);//更新选中项
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {
			if (sel > 0) {
				sel--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if (sel < 1) {
				sel++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		if (sel_last != sel) {
			GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_RESET);// 被选中的项目
			sel_last = sel;
		}

		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		if(g_fatal_error!=0)
			break;	
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		g_feed_mode = 0;
		g_shutdown_mode = 0;
		g_startup_mode = 0;
		g_run_mode = 0;
		
		//g_is_night_mode = 0;
		g_temp_val_new.temp_unit = 0;
		
		//GUIDEMO_DayNightSwitch();
		GUIDEMO_DayModeSet();
		GUIDEMO_SetupSubItemsUI(2, 0);
	}
}

void GUIDEMO_ShutdownInitialUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	int sel_last = 0;

	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 0, UI_CONFITRM_CANCEL_SHUTDOWN);//画左侧框
	GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_SHUTDOWN);//更新选中项
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {
			if (sel > 0) {
				sel--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if (sel < 1) {
				sel++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		if (sel_last != sel) {
			GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_SHUTDOWN);// 被选中的项目
			sel_last = sel;
		}

		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		if (0 == sel) {
			GUIDEMO_FeedShutdownTmerUI(1);
		} else {
			//GUIDEMO_BigCircleUI(2);
			GUIDEMO_MainMenu(2);
		}
	}
}

void GUIDEMO_StartupTimerBypassedUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	int sel_last = 0;

	int temp_val[5]={31,32,33,34};
	int big_temp=182;

	EC11_Clear();
	
	// these funcs have internal MEMDEV
	GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 0, UI_CONFITRM_CANCEL_ONLY_USE_BYPASS);//画左侧框
	GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_ONLY_USE_BYPASS);//更新选中项
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框

	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		
		if ((1 == g_run_mode)&&(0 == g_time_remain)) {
			break;
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(0);

		if (EC11_LEFT == ec_sta) {
			if (sel > 0) {
				sel--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if (sel < 1) {
				sel++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		if (sel_last != sel) {
			GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_ONLY_USE_BYPASS);// 被选中的项目
			sel_last = sel;
		}

		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		if (0 == sel) {
			g_startup_mode = 0;
			g_run_mode = 1;
			GUIDEMO_BigCircleUI(2);
		} else {
			g_is_lastui_bypassdetail = 1;
			GUIDEMO_StartupTimerUI();
		}
	}
	
	if ((1 == g_run_mode)&&(0 == g_time_remain)&&(g_fatal_error==0)) {
		GUIDEMO_BigCircleUI(2);// main screen
	}
}

void GUIDEMO_FeedInitialUI(void)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	int sel_last = 0;

	int temp_val[5]={31,32,33,34};
	int big_temp=182;
	
	GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 0, UI_CONFITRM_CANCEL_ONLY_USE_FOR);//画左侧框
	GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_ONLY_USE_FOR);//更新选中项
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(1);

		if (EC11_LEFT == ec_sta) {
			if (sel > 0) {
				sel--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if (sel < 1) {
				sel++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		if (sel_last != sel) {
			GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_ONLY_USE_FOR);// 被选中的项目
			sel_last = sel;
		}

		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		if (0 == sel) {
			GUIDEMO_FeedShutdownTmerUI(0);
		} else {//选择cancel 进入主界面
			//GUIDEMO_BigCircleUI(2);
			GUIDEMO_MainMenu(1);
		}
	}
}

// mode 0 Before run mode is on
// mode 1 After  run mode is on(Apply Changes)
void GUIDEMO_StartupInitialUI(int mode)
{
	int time_remain = 10;
	EC11_STA ec_sta = EC11_IDLE;
	int sel=0;
	int sel_last=0;

	int temp_val[5]={31,32,33,34};
	int big_temp=182;

	GUIDEMO_ClearScreen(0,0,480,320);//清除屏幕
	if (0 == mode) {
		GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 0, UI_CONFITRM_CANCEL_STARTUP);
		GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_STARTUP);
	} else {
		GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 0, UI_CONFITRM_CANCEL_APLPLY);//画左侧框
		GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_APLPLY);//画左侧选中项
	}
	
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	
	do {
		if (0 == time_remain%10) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(0);
		
		if (EC11_LEFT == ec_sta) {
			if (sel > 0) {
				sel--;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if (sel < 1) {
				sel++;
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}
		
		if (sel_last != sel) {
			if (0 == mode) {
				GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_STARTUP);// 被选中的项目
			} else {
				GUIDEMO_LeftConfirmCancelUI(sel, g_target_temp_val, g_smoke_val_percent, 1, UI_CONFITRM_CANCEL_APLPLY);// 被选中的项目
			}
			sel_last = sel;
		}

		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while (EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		if (0 == sel) {//confirm
			if (0 == mode) {
				g_temp_val_new.target_val = temp_temp;
				g_temp_val_new.target_smoke = temp_smoke;
				GUIDEMO_StartupTimerUI();
			} else {
				GUIDEMO_BigCircleUI(2);
			}
		} else {//cancle
			//temp_temp
			GUIDEMO_MainMenu(0);
			
		}
	}
}

void GUIDEMO_HelpUI(void)
{
	unsigned int color=0x00504E4E;
	if(g_is_night_mode==0)
	{
		GUI_SetColor(color);
		GUI_FillRect(270,13,465,80);
		GUI_FillRect(270,82,465,119);
		
		GUI_SetBkColor(color);
		GUI_SetColor(GUI_WHITE);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
		GUI_DispStringAt("CUSTOMER SERVICE", 285, 90);
		
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
		GUI_DispStringAt("M - F 7AM - 5PM MST", 297, 126);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
		GUI_DispStringAt("800.650.2433", 325, 167);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn23);
		GUI_DispStringAt("SUPPORT@CAMPCHEF.COM", 260, 192);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
		GUI_DispStringAt("CAMPCHEF.COM", 290, 266);
		
		GUI_SetColor(USER_COLOR1_RED);
		GUI_DrawLine(322,247,417,247);
		
		image_display(287, 20, (u8*)gImage_logo_day);
		
	}else{
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(270,13,465,80);
		GUI_FillRect(270,82,465,119);
		
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(color);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn25);
		GUI_DispStringAt("CUSTOMER SEVICE", 285, 90);
		
		GUI_SetBkColor(bk_color);
		GUI_SetColor(GUI_WHITE);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
		GUI_DispStringAt("M - F 7AM - 5PM MST", 297, 126);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn20);
		GUI_DispStringAt("800.650.2433", 325, 167);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn23);
		GUI_DispStringAt("SUPPORT@CAMPCHEF.COM", 260, 192);
		GUI_SetFont(&GUI_FontHelveticaNeueLTStdCn28);
		GUI_DispStringAt("CAMPCHEF.COM", 290, 266);
		
		GUI_SetColor(GUI_WHITE);
		GUI_DrawLine(322,247,417,247);
		
		image_display(287, 25, (u8*)gImage_logo_night);
	}
	
}

void GUIDEMO_UpdateTemp(int *temp_val)
{
	temp_val[0] = g_temp_val_new.temp1;
	temp_val[1] = g_temp_val_new.temp2;
	temp_val[2] = g_temp_val_new.temp3;
	temp_val[3] = g_temp_val_new.temp4;
	temp_val[4] = g_temp_val_new.temp5;
}
void GUIDEMO_DayModeSet(void)
{
		g_is_night_mode = 0;
		bk_color = GUI_WHITE;
		g_unselected_menu_color = USER_COLOR7_GRAY;
		g_big_temp_color = USER_COLOR1_RED;
		g_big_smoke_color = GUI_BLACK;
		g_circle_extenal_color = USER_COLOR2_RED;
		g_circle_internal_color = USER_COLOR1_RED;
		g_wifi_color = GUI_BLACK;
		g_progress_color = GUI_BLACK;
		g_progress_text_color = GUI_WHITE;
	
}
void GUIDEMO_DayNightSwitch(void)
{
	if (GUI_BLACK == bk_color) {
		g_is_night_mode = 0;
		bk_color = GUI_WHITE;
		g_unselected_menu_color = USER_COLOR7_GRAY;
		g_big_temp_color = USER_COLOR1_RED;
		g_big_smoke_color = GUI_BLACK;
		g_circle_extenal_color = USER_COLOR2_RED;
		g_circle_internal_color = USER_COLOR1_RED;
		g_wifi_color = GUI_BLACK;
		g_progress_color = GUI_BLACK;
		g_progress_text_color = GUI_WHITE;
	} else {
		g_is_night_mode = 1;
		bk_color = GUI_BLACK;
		g_unselected_menu_color = GUI_WHITE;
		g_big_temp_color = GUI_WHITE;
		g_big_smoke_color = GUI_WHITE;
		g_circle_extenal_color = GUI_WHITE;
		g_circle_internal_color = USER_COLOR1_RED;
		g_wifi_color = GUI_WHITE;
		g_progress_color = USER_COLOR7_GRAY;
		g_progress_text_color = GUI_BLACK;
	}
}

void GUIDEMO_MainMenu(int sel)
{
	int help_flag = 0;
	int time_remain = 10;
	int menu_index=sel;
	int menu_index_last=sel;
	int temp_val[5]={11,12,13,14};
	int big_temp=180;
	EC11_STA ec_sta = EC11_IDLE;
	
	EC11_Clear();
	
	GUIDEMO_ClearScreen(0,0,480,320);//清除屏幕
	GUIDEMO_LeftMenuItems(menu_index, menu_index_last, 0, 0);//画左侧6个菜单
	GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
	//GUIDEMO_WifiIcon(0);
	//GUIDEMO_BluetoothIcon();
	
	//g_temp_update_enable = 1;
	
	do{
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		// 扫面EC11编码器有无动作
		ec_sta = EC11_KEY_Scan(0);

		if ((0 == time_remain%10)&&(help_flag!=1)) {
			GUIDEMO_UpdateTemp(temp_val);
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
			GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
		}
		
		if ((4 == menu_index_last) && (menu_index != 4)) {
			if (1 == help_flag) {
				help_flag = 0;
				GUIDEMO_ClearScreen(260,0,480,320);
				GUIDEMO_RightTempUI(temp_val, 1, big_temp, 0);//画4个框
				GUIDEMO_RightTempUI(temp_val, 1, big_temp, 1);//更新框内4个温度
				GUIDEMO_RightTempUI(temp_val, 1, big_temp, 2);//更新big温度
				GUIDEMO_WifiIcon(0);
				GUIDEMO_BluetoothIcon();
			}
		}
			
		menu_index_last = menu_index;
		if (EC11_LEFT == ec_sta) {// up
			if(menu_index>0)
				menu_index--;
			else
				menu_index=5;
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if(menu_index<5)
				menu_index++;
			else
				menu_index=0;
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_LONG == ec_sta) {
			ec_sta = EC11_IDLE;
		} else if (EC11_BUT_NO_FREE <= ec_sta) {
			ec_sta = EC11_IDLE;
		}

		if(menu_index_last != menu_index)
		{
			// 被选中的项目
			GUIDEMO_LeftMenuItems(menu_index, menu_index_last, 1, 0);//更新选择界面
		}
		
		time_remain--;
		
		if (0 == time_remain) {
			time_remain = 10;
		}
	
		if ((EC11_BUT == ec_sta)&&(menu_index==4))
		{
			help_flag = 1;
			GUIDEMO_ClearScreen(260,0,480,320);
			GUIDEMO_HelpUI();
			ec_sta = EC11_IDLE;
		}
		if(g_fatal_error!=0)
			break;
		direct_switch_check();
		delay_ms(100);
	} while(EC11_IDLE == ec_sta);
	
	if (EC11_BUT == ec_sta) {
		if (0 == menu_index) {
			GUIDEMO_BigCircleUI(0);
		} else if (1 == menu_index) {
			GUIDEMO_FeedInitialUI();
		} else if (2 == menu_index) {
			GUIDEMO_ShutdownInitialUI();
		} else if (3 == menu_index) {
			GUIDEMO_SetupSubItemsUI(0, 0);
		} else if (4 == menu_index) {
			GUIDEMO_ClearScreen(260,0,480,320);
			GUIDEMO_HelpUI();			
		} else if (5 == menu_index) {
			GUIDEMO_BigCircleUI(2);
		}
	}
}

void GUIDEMO_AfterLogo(void)
{
	// 默认子项目ID，对应”---“项
	g_event_val_new.menu_index = 11;
	g_temp_val_new.temp_unit = 0;// 华氏度
	
	g_temp_val_last = g_temp_val_new;
	
	//GUIDEMO_BigCircleUI(0);
	GUIDEMO_MainMenu(0);
}
/*************************** End of file ****************************/
