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

void GUIDEMO_SubMenu(void)
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
		
		if (g_fatal_error != 0) {
			GUI_SetBkColor(bk_color);
			GUI_SetColor(bk_color);
			GUI_FillRect(15, 270, 465, 312);
			delay_ms(1000);

			GUI_SetColor(USER_COLOR1_RED);
			GUI_SetFont(&GUI_FontTradeGothicCn42);
			
			if (1 == g_fatal_error) {
				GUI_DispStringAt("FLAME ERROR", 50, 275);
			} else {
				GUI_DispStringAt("SENSOR ERROR", 50, 275);
			}
			
			delay_ms(1000);
			continue;
		}
		
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
						sprintf(dispStr, "%.3d°C", temp_val_array[0]);
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
						sprintf(dispStr, "%.3d°C", temp_val_array[1]);
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
						sprintf(dispStr, "%.3d°C", temp_val_center);
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
						sprintf(dispStr, "%.3d°C", temp_val_array[3]);
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
						sprintf(dispStr, "%.3d°C", temp_val_array[4]);
					}
				} else {
					sprintf(dispStr, "%s", pSubMenuTextAll[menu_index_pos5-1]);
				}
				GUI_DispStringAt(dispStr, SubMenuPosArray[evt_val.menu_index-1][4]-40, 210);
			}
			
			g_event_val_last.menu_index = evt_val.menu_index;
		}
		
		temp_val_center_last = temp_val_center;
		g_temp_center = temp_val_center;
		
		if (0 == time_remain%10) {
			GUI_SetBkColor(bk_color);		
			GUI_SetColor(font_color);
			GUI_SetFont(&GUI_FontTradeGothicCn38);
			if (0 == g_temp1_error) {
				if (0 == g_temp_val_new.temp_unit) {
					sprintf(dispStr, "%.3d°F", temp_val.temp1);
				} else {
					sprintf(dispStr, "%.3d°C", temp_val.temp1);
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
					sprintf(dispStr, "%.3d°C", temp_val.temp2);
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
					sprintf(dispStr, "%.3d°C", temp_val.temp3);
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
					sprintf(dispStr, "%.3d°C", temp_val.temp4);
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
					tmep_val_gap = 25;
					temp_val_center += 25;
				}
				g_event_val_new.force_flush = 1;
			}
		} else if (EC11_RIGHT_FAST == ec_sta) {// DN
			if (temp_val_center > 140) {
				if (temp_val_center < 175) {// HS LS FEED SHUTD --- SETUP EXIT
					tmep_val_gap = 5;
					temp_val_center -= 5;
				} else {
					tmep_val_gap = 25;
					temp_val_center -= 25;
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
