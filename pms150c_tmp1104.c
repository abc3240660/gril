#include	"extern.h"

// duty ratio = X%, duty_ratio = X
static BYTE duty_ratio;
static WORD duty_reg;

static void pwm_enable(void);
static void pwm_disable(void);
static void pwm_duty_set(void);

BIT p_In2	:	PA.7;
BIT p_In6	:	PA.4;
//BIT p_In6X	:	PA.6;// just for update flag

BIT p_Out3	:	PA.6;
BIT p_Out5	:	PA.3;
BIT p_Out7	:	PA.0;

void FPPA0(void)
{
	.ADJUST_IC	SYSCLK = IHRC/2// SYSCLK = IHRC/2

	$ p_Out3	Out, Low;// off
	$ p_Out5	Out, Low;// off
	$ p_Out7	Out, Low;// off

	$ p_In2		In;
	$ p_In6		In;

	$ T16M		IHRC, /1, BIT10;// 16MHz/1 = 16MHz:the time base of T16.
	$ TM2C		IHRC, Disable, Period, Inverse;

	gpcs   = 0b1101_1111;// Vinternal R = Vdd*16/24 = 4.2*16/24 = 2.8v
	gpcc   = 0b1001_0111;// +:PA4, -:Vinternal R
	padier = 0b111_0_1111;// disable digital input for PA4

	// Insert Initial Code
	duty_ratio = 0;

	BYTE	Key_Flag;
	Key_Flag = _FIELD(p_In2/*, p_In6X*/);

	BYTE	Sys_Flag = 0;
	BIT	f_5ms_Trig	:	Sys_Flag.0;
	BIT	f_10ms_Trig	:	Sys_Flag.1;
	BIT	f_In2_Trig	:	Sys_Flag.2;
	BIT	f_In6_Trig	:	Sys_Flag.3;
	BIT	f_In2_lock	:	Sys_Flag.4;
	BIT	f_In3_value	:	Sys_Flag.5;
	BIT	f_In7_value	:	Sys_Flag.6;
	BIT	f_cmptor_valid	:	Sys_Flag.7;


	// 0~7 : A~H
	BYTE	mode_In6 	= 0;
	BYTE	mode_In6_last 	= 8;
	BYTE	count_5ms 	= 1;
	BYTE	count_10ms	= 1;
	BYTE	count_l 	= 0;
	BYTE	count_h 	= 0;
	BYTE	count_one_sec 	= 0;

	//debounce_tim=N*10ms
	BYTE	debounce_time_In2	=	4;//Keydebouncetime=40mS
	BYTE	debounce_time_In6	=	4;//Keydebouncetime=40mS

	while (1) {
		if (INTRQ.T16) {
			INTRQ.T16 = 0;

			if (--count_5ms == 0) {
				count_5ms = 39;// 128uS*39=5mS
				f_5ms_Trig = 1;
			}

			if (--count_10ms == 0) {
				count_10ms = 78;// 128uS*78=10mS
				f_10ms_Trig = 1;
			}
		}

		if (f_10ms_Trig) {// every 10ms
			f_10ms_Trig = 0;

			// 30 mins = 30*60s = 1800s = 1800*100*10ms
			count_one_sec++;

			if (100 == count_one_sec) {// 100*10ms = 1 second
				count_one_sec = 0;

				// L seconds
				count_l++;
			
				if (100 == count_l) {
					count_l = 0;
					// total time = (100*H + L) seconds
					count_h++;

					if (100 == count_h) {
						count_h = 0;
					}
				}
			}

			A = (PA ^ Key_Flag) & _FIELD(p_In2);
			if (!ZF) {
				//ButtonDown
				if (!p_In2) {
					if (--debounce_time_In2 == 0) {
						Key_Flag ^= _FIELD(p_In2);
						f_In2_Trig = 1;
						debounce_time_In2 = 4;
					}
				} else {//ButtonUp
					f_In2_Trig = 0;
					Key_Flag ^= _FIELD(p_In2);
				}
			} else {
				debounce_time_In2 = 4;

			}

			if (((f_cmptor_valid == 0) && (GPCC.6)) || ((f_cmptor_valid == 1) && (!GPCC.6))) {
				//ButtonDown
				if (GPCC.6) {// PA4 > Vinternal R ---> GPCC.6=1
					f_cmptor_valid = 1;
					f_In6_Trig = 1;
				} else {//ButtonUp
					f_In6_Trig = 0;
					f_cmptor_valid = 0;
				}
			}

			if (f_In2_Trig) {
				// to re-initialize idle-shutdown timer(30 mins)
				count_l = 0;
				count_h = 0;
				count_one_sec = 0;

				f_In2_Trig = 0;
				if (f_In2_lock) {
					f_In2_lock = 0;
				} else {
					f_In2_lock = 1;
				}
			}

			if (!f_In2_lock) {
				if (f_In6_Trig) {
					// to re-initialize idle-shutdown timer(30 mins)
					count_l = 0;
					count_h = 0;
					count_one_sec = 0;

					mode_In6++;
					if (3 == mode_In6) {
						mode_In6 = 0;
					}
				}
				if (mode_In6 != mode_In6_last) {
					if (0 == mode_In6) {
						p_Out5 = 1;
						
						p_Out3 = 0;
						p_Out7 = 0;
					} else if (1 == mode_In6) {
						p_Out7 = 1;
						
						p_Out3 = 0;
						p_Out5 = 0;
					} else if (2 == mode_In6) {
						p_Out3 = 1;
						
						p_Out5 = 0;
						p_Out7 = 0;
					}

					mode_In6_last = mode_In6;
				}

			}
		}
	}
}