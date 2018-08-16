#include <stdlib.h>
#include <stdio.h>

#define FOR_DEBUG_USE 1

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

typedef struct {
	int menu_index;
} EVENT_VAL;

typedef enum {
	EC11_IDLE = 0,
	EC11_LEFT,
	EC11_RIGHT,
	EC11_LEFT_FAST,
	EC11_RIGHT_FAST,
	EC11_BUT,
	EC11_BUT_LONG,
	EC11_BUT_NO_FREE
} EC11_STA;

EC11_STA GetEC11Sta(int choise)
{
	if (1 == choise) {// up
		return EC11_LEFT;
	} else if (2 == choise) {// dn
		return EC11_RIGHT;
	} else if (3 == choise) {// UP
		return EC11_LEFT_FAST;
	} else if (4 == choise) {// DN
		return EC11_RIGHT_FAST;
	} else {
		return EC11_IDLE;
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

int main(int argc, char* argv[])
{
	int i = 0;
	int temp_val_array[5] = {0};
	int tmep_val_gap = 5;
	int temp_val_center = 0;
	int choise_val = 0;
	EC11_STA ec_sta = EC11_IDLE;
	EVENT_VAL evt_val;
	
	int menu_index_pos1 = 0;
	int menu_index_pos2 = 0;
	int menu_index_pos3 = 0;
	int menu_index_pos4 = 0;
	int menu_index_pos5 = 0;

	evt_val.menu_index = 11;

	center_initial(&temp_val_center, evt_val.menu_index);

	while(1) {
		for (i=0; i<5; i++) {
			temp_val_array[i] = 0;
		}

		menu_index_pos1 = 0;
		menu_index_pos2 = 0;
		menu_index_pos3 = 0;
		menu_index_pos4 = 0;
		menu_index_pos5 = 0;

		if (temp_val_center > 500) {
			temp_val_center = 500;
		}
		if (temp_val_center < 140) {
			temp_val_center = 140;
		}

		CalcTempVal(temp_val_center, temp_val_array, tmep_val_gap);
/*
		for (int i=0; i<5; i++) {
			if (temp_val_array[i] != 0) {
				printf("temp_val_array[%d] = %d\n", i, temp_val_array[i]);
			} else {
				printf("temp_val_array[%d] = XXX\n", i);
			}
		}
*/
		menu_index_initial(&evt_val.menu_index, temp_val_center, temp_val_array);

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

		printf("menu_index = %d\n", evt_val.menu_index);

		if (menu_index_pos1 > 0) {
			if ((evt_val.menu_index > 3) && (evt_val.menu_index < 9)) {
				printf("%d°C\n", temp_val_array[0]);
			} else {
				printf("%s\n", SubMenuTextAllC[menu_index_pos1-1]);
			}
		} else {
			printf("XXX\n");
		}

		if (menu_index_pos2 > 0) {
			if ((evt_val.menu_index > 2) && (evt_val.menu_index < 8)) {
				printf("%d°C\n", temp_val_array[1]);
			} else {
				printf("%s\n", SubMenuTextAllC[menu_index_pos2-1]);
			}
		} else {
			printf("XXX\n");
		}

		if (menu_index_pos3 > 0) {
			if ((evt_val.menu_index > 1) && (evt_val.menu_index < 7)) {
				printf("%d°C\n", temp_val_array[2]);
			} else {
				printf("%s\n", SubMenuTextAllC[menu_index_pos3-1]);
			}
		} else {
			printf("XXX\n");
		}

		if (menu_index_pos4 > 0) {
			if ((evt_val.menu_index > 0) && (evt_val.menu_index < 6)) {
				printf("%d°C\n", temp_val_array[3]);
			} else {
				printf("%s\n", SubMenuTextAllC[menu_index_pos4-1]);
			}
		} else {
			printf("XXX\n");
		}

		if (menu_index_pos5 > 0) {
			if ((evt_val.menu_index > 0) && (evt_val.menu_index < 5)) {
				printf("%d°C\n", temp_val_array[4]);
			} else {
				printf("%s\n", SubMenuTextAllC[menu_index_pos5-1]);
			}
		} else {
			printf("XXX\n");
		}

		printf("Please input your choise(1:up,2:dn,3:UP,4:DN,OT:Exit):");
		scanf("%d", &choise_val);

		ec_sta = GetEC11Sta(choise_val);

		if (EC11_LEFT == ec_sta) {// up
			if (temp_val_center < 500) {// menu_index = 1
				tmep_val_gap = 5;
				temp_val_center += 5;
				#if !FOR_DEBUG_USE
				g_event_val_new.force_flush = 1;
				#endif
			}
			ec_sta = EC11_IDLE;
		} else if (EC11_RIGHT == ec_sta) {// dn
			if (temp_val_center > 140) {// menu_index = 13
				tmep_val_gap = 5;
				temp_val_center -= 5;
				#if !FOR_DEBUG_USE
				g_event_val_new.force_flush = 1;
				#endif
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
				#if !FOR_DEBUG_USE
				g_event_val_new.force_flush = 1;
				#endif
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
				#if !FOR_DEBUG_USE
				g_event_val_new.force_flush = 1;
				#endif
			}
			ec_sta = EC11_IDLE;
		}
	}

	return 0;
}
