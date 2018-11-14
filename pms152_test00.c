#include	"extern.h"

//#define FOR_DEBUG_USE1 1
//#define FOR_DEBUG_USE2 1

static BYTE duty_ratio_l;
static BYTE duty_ratio_h;

static void pwm_freq_set(void);
static void duty_ratio_adding(void);
static void duty_ratio_deling(void);
static void pwmg0_enable(void);
static void pwmg0_disable(void);
static void pwmg1_enable(void);
static void pwmg1_disable(void);
static void pwmg2_enable(void);
static void pwmg2_disable(void);

BIT p_In2	:	PB.6;// Power Charge Input(0-charging, 1-non-charging)
BIT p_In3	:	PB.7;// Cmptor Input(0-Valid, 1-Invalid)
BIT p_In6	:	PA.6;// Whole Switch Input

BIT p_Out5	:	PA.7;// White LED
BIT p_Out8	:	PA.3;// PG2PWM RGB LED
BIT p_Out9	:	PA.4;// PG1PWM RGB LED
BIT p_Out10	:	PA.0;// PG0PWM RGB LED
BIT p_Out12	:	PB.0;// LED

void FPPA0(void)
{
	.ADJUST_IC	SYSCLK = IHRC/4// SYSCLK = IHRC/4 = 4MHz

	$ p_Out5	Out, Low;// off
	$ p_Out8	Out, Low;// off
	$ p_Out9	Out, Low;// off
	$ p_Out10	Out, Low;// off
	$ p_Out12	Out, Low;// off

	$ p_In2		In;
	$ p_In3		In;
	$ p_In6		In;

	$ T16M		IHRC, /1, BIT10;// 16MHz/1 = 16MHz:the time base of T16.
	$ TM2C		IHRC, Disable, Period, Inverse;

	gpcs   = 0b1111_1101;// Vinternal R = Vdd*14/32
	gpcc   = 0b1000_1010;// -:PB7, +:Vinternal R
	pbdier = 0b0111_1111;// disable digital input for PB7

	// Insert Initial Code
	duty_ratio_l = 0;
	duty_ratio_h = 0;

	BYTE	Key_Flag;
	Key_Flag = _FIELD(p_In2, p_In6);

	BYTE	Sys_Flag = 0;
	BIT	f_10ms_Trig	:	Sys_Flag.0;
	BIT	f_16ms_Trig	:	Sys_Flag.1;
	BIT	f_In2_Trig	:	Sys_Flag.2;
	BIT	f_In3_Trig	:	Sys_Flag.3;
	BIT	f_In6_Trig	:	Sys_Flag.4;
	BIT	f_In6_lock	:	Sys_Flag.5;
	BIT	f_cmptor_valid	:	Sys_Flag.6;
	BIT	f_Out12_value	:	Sys_Flag.7; 

	// 0~7 : A~H
	BYTE	mode_In3 	= 0;
	BYTE	mode_In3_last 	= 8;
	BYTE	count_10ms	= 1;
	BYTE	count_16ms	= 1;
	BYTE	count_one_sec 	= 0;
	BYTE	count_4s 	= 0;
	BYTE	sub_mode_In3 	= 0;
	BYTE	sub_mode_In3_last 	= 8;

	// debounce_time = N*10ms
	BYTE	debounce_time_In2	=	4;// Key debounce time = 40ms
	BYTE	debounce_time_In3	=	2;// Key debounce time = 20ms
	BYTE	debounce_time_In6	=	4;// Key debounce time = 40ms

	while (1) {
		if (INTRQ.T16) {
			INTRQ.T16 = 0;

			if (--count_10ms == 0) {
				count_10ms = 78;// 128uS*78=10ms
				f_10ms_Trig = 1;
			}

			if (--count_16ms == 0) {
				count_16ms = 125;// 128uS*125=16ms
				f_16ms_Trig = 1;
			}
		}

		if (f_10ms_Trig) {// every 10ms
			f_10ms_Trig = 0;

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
				if (!GPCC.6) {// PB7 > Vinternal R ---> GPCC.6=0
					if (--debounce_time_In3 == 0) {
						f_cmptor_valid = 1;
						f_In3_Trig = 1;
						debounce_time_In3 = 2;
					}
				} else {//ButtonUp
					f_In3_Trig = 0;
					f_cmptor_valid = 0;
				}
			} else {
				debounce_time_In3 = 2;
			}

			A = (PA ^ Key_Flag) & _FIELD(p_In6);
			if (!ZF) {
				//ButtonDown
				if (!p_In6) {
					if (--debounce_time_In6 == 0) {
						Key_Flag ^= _FIELD(p_In6);
						f_In6_Trig = 1;
						debounce_time_In6 = 4;
					}
				} else {//ButtonUp
					f_In6_Trig = 0;
					Key_Flag ^= _FIELD(p_In6);
				}
			} else {
				debounce_time_In6 = 4;
			}

			if (f_In2_Trig) {// switch every 0.5s
				if (50 == count_one_sec) {// 50*10ms = 500ms = 0.5s
					count_one_sec = 0;
				}

				if (0 == count_one_sec) {
					if (f_Out12_value) {
						p_Out12 = 0;
						f_Out12_value = 0;
					} else {
						p_Out12 = 1;
						f_Out12_value = 1;
					}
				}

				count_one_sec++;
			} else {
				p_Out12 = 0;
				f_Out12_value = 0;
				count_one_sec = 0;
			}

			if (f_In6_Trig) {
				f_In6_Trig = 0;

				if (f_In6_lock) {
					pwmg0_disable();
					pwmg1_disable();
					pwmg2_disable();

					p_Out5  = 0;
					p_Out8  = 0;
					p_Out9  = 0;
					p_Out10 = 0;

					f_In6_lock = 0;
				} else {
					p_Out5 = 1;
					f_In6_lock = 1;
				}
			}

			if (!f_In6_lock) {
				if (f_In3_Trig) {
					f_In3_Trig = 0;
					mode_In3++;
					if (4 == mode_In3) {
						mode_In3 = 0;
					}
				}

				if (mode_In3 != mode_In3_last) {
					mode_In3_last = mode_In3;
#ifdef FOR_DEBUG_USE1
					if (0 == mode_In3) {
						p_Out5 = 1;
						p_Out8 = 1;
					} else if (1 == mode_In3) {
						p_Out9 = 1;
					} else if (2 == mode_In3) {
						p_Out10 = 1;
					} else if (3 == mode_In3) {
						p_Out5  = 0;
						p_Out8  = 0;
						p_Out9  = 0;
						p_Out10 = 0;
					}
#else
					if (2 == mode_In3) {
						p_Out5  = 0;
						p_Out8  = 0;
						p_Out9  = 0;
						p_Out10 = 0;
					} else if (3 == mode_In3) {
						p_Out5 = 1;
					}
#endif
				}
			}
		}
#ifdef FOR_DEBUG_USE1
#else
		if (f_16ms_Trig) {// every 16ms
			f_16ms_Trig = 0;

			if (!f_In6_lock) {
				continue;
			}

			if (0 == mode_In3) {
				if (0 == count_4s) {// every 4s
					if (sub_mode_In3 != sub_mode_In3_last) {
						sub_mode_In3_last = sub_mode_In3;
#ifdef FOR_DEBUG_USE2
					if (0 == sub_mode_In3) {
						p_Out5  = 1;
						p_Out8  = 0;
						p_Out9  = 0;
						p_Out10 = 1;
					} else if (1 == sub_mode_In3) {
						p_Out5  = 1;
						p_Out8  = 1;
						p_Out9  = 0;
						p_Out10 = 0;
					} else if (2 == sub_mode_In3) {
						p_Out5  = 0;
						p_Out8  = 1;
						p_Out9  = 1;
						p_Out10 = 0;
					} else if (3 == sub_mode_In3) {
						p_Out5  = 0;
						p_Out8  = 0;
						p_Out9  = 1;
						p_Out10 = 1;
					} else if (4 == sub_mode_In3) {
						p_Out5  = 1;
						p_Out8  = 1;
						p_Out9  = 1;
						p_Out10 = 1;
					} else if (5 == sub_mode_In3) {
						p_Out5  = 0;
						p_Out8  = 0;
						p_Out9  = 0;
						p_Out10 = 0;
					}
#else
						if (0 == sub_mode_In3) {
							pwmg0_disable();
							pwmg1_disable();
							pwmg2_disable();

							p_Out9 = 0;
							p_Out10 = 0;

							duty_ratio_l = 0;
							duty_ratio_h = 0;

							pwmg0_enable();	
						} else if (1 == sub_mode_In3) {
							pwmg0_disable();
							pwmg1_disable();
							pwmg2_disable();

							p_Out10 = 0;

							duty_ratio_l = 1;
							duty_ratio_h = 62;

							pwmg0_enable();	
							pwmg1_enable();	
						} else if (2 == sub_mode_In3) {
							pwmg0_disable();
							pwmg1_disable();
							pwmg2_disable();

							p_Out8 = 0;
							p_Out10 = 0;

							duty_ratio_l = 0;
							duty_ratio_h = 0;

							pwmg1_enable();	
						} else if (3 == sub_mode_In3) {
							pwmg0_disable();
							pwmg1_disable();
							pwmg2_disable();

							p_Out8 = 0;

							duty_ratio_l = 1;
							duty_ratio_h = 62;

							pwmg1_enable();	
							pwmg2_enable();	
						} else if (4 == sub_mode_In3) {
							pwmg0_disable();
							pwmg1_disable();
							pwmg2_disable();

							p_Out8 = 0;
							p_Out9 = 0;

							duty_ratio_l = 0;
							duty_ratio_h = 0;

							pwmg2_enable();	
						} else if (5 == sub_mode_In3) {
							pwmg0_disable();
							pwmg1_disable();
							pwmg2_disable();

							p_Out9 = 0;

							duty_ratio_l = 1;
							duty_ratio_h = 62;

							pwmg0_enable();	
							pwmg2_enable();	
						}
#endif
					}
				} else {
#ifdef FOR_DEBUG_USE2
#else
					if (0 == sub_mode_In3) {
						pwmg0_disable();
						duty_ratio_adding();
						pwmg0_enable();	
					} else if (1 == sub_mode_In3) {
						pwmg0_disable();
						pwmg1_disable();
						duty_ratio_deling();
						pwmg0_enable();	
						pwmg1_enable();	
					} else if (2 == sub_mode_In3) {
						pwmg1_disable();
						duty_ratio_adding();
						pwmg1_enable();	
					} else if (3 == sub_mode_In3) {
						pwmg1_disable();
						pwmg2_disable();
						duty_ratio_deling();
						pwmg1_enable();	
						pwmg2_enable();	
					} else if (4 == sub_mode_In3) {
						pwmg2_disable();
						duty_ratio_adding();
						pwmg2_enable();	
					} else if (5 == sub_mode_In3) {
						pwmg0_disable();
						pwmg2_disable();
						duty_ratio_deling();
						pwmg0_enable();	
						pwmg2_enable();	
					}
#endif
				}

				count_4s++;
				if (250 == count_4s) {// 16*256 = 4000ms = 4s
					count_4s = 0;
					sub_mode_In3++;

					if (6 == sub_mode_In3) {
						sub_mode_In3 = 0;
					}
				}
			} else {
				count_4s = 0;
				sub_mode_In3 = 0;
				sub_mode_In3_last = 8;
			}
		}
#endif
	}
}
// PWMG0/1/2 Share the same Freq but different duty ratio
// Setting PWM's Freq to 500Hz
// Fpwm_freq = Fpwm_clk / (CB + 1) = 4M/32/250 = 500Hz
void pwm_freq_set(void)
{
	pwmgcubl = 0b0100_0000;
	pwmgcubh = 0b0011_1110;// CB = {pwmgcubh[7:0], pwmgcubl[7:6]} = 249
	
	pwmgclk = 0b1101_0000;// Fpwm_clk = SYSCLK / 32
}

// 249 ~ 0 -> duty = (250 ~ 1) / 250
void duty_ratio_deling(void)
{
	BYTE duty_ratio = 0;

	if ((0 == duty_ratio_l) && (0 == duty_ratio_h)) {
		return;
	}

	if (duty_ratio_l > 0) {
		duty_ratio_l--;
	} else {
		duty_ratio_l = 3;
		if (duty_ratio_h > 0) {
			duty_ratio_h--;
		}
	}
}

// 0 ~ 249 -> duty = (1 ~ 250) / 250
void duty_ratio_adding(void)
{
	BYTE duty_ratio = 0;

	duty_ratio = duty_ratio_h + duty_ratio_h + duty_ratio_h + duty_ratio_h + duty_ratio_l;

	if (249 == duty_ratio) {
		return;
	}

	if (duty_ratio_l < 3) {
		duty_ratio_l++;
	} else {
		duty_ratio_l = 0;
		duty_ratio_h++;
	}
}

// Enable PWMG0 Output with X% duty ratio
void pwmg0_enable(void)
{
	if (0 == duty_ratio_l) {
		pwmg0dtl = 0b10_0000;// DB0 = pwmg0dtl[5] = 1
		pwmg0dth = duty_ratio_h;// DB10_1 = {pwmg0dth[7:0], pwmg0dtl[7:6]}
	} else if (1 == duty_ratio_l) {
		pwmg0dtl = 64 + 0b10_0000;// DB0 = pwmg0dtl[5] = 1
		pwmg0dth = duty_ratio_h;// DB10_1 = {pwmg0dth[7:0], pwmg0dtl[7:6]}
	} else if (2 == duty_ratio_l) {
		pwmg0dtl = 128 + 0b10_0000;// DB0 = pwmg0dtl[5] = 1
		pwmg0dth = duty_ratio_h;// DB10_1 = {pwmg0dth[7:0], pwmg0dtl[7:6]}
	} else if (3 == duty_ratio_l) {
		pwmg0dtl = 192 + 0b10_0000;// DB0 = pwmg0dtl[5] = 1
		pwmg0dth = duty_ratio_h;// DB10_1 = {pwmg0dth[7:0], pwmg0dtl[7:6]}
	}

	// Fpwm_duty = [DB10_1 + DB0*0.5 + 0.5] / (CB + 1) = (DB10_1 + 1) / 250
	pwmg0c = 0b0000_0110;// PA0 PWM
}

// Disable PWMG0
void pwmg0_disable(void)
{
	pwmg0c = 0b0000_0000;// do not output PWM
}

// Enable PWMG1 Output with X% duty ratio
void pwmg1_enable(void)
{
	if (0 == duty_ratio_l) {
		pwmg1dtl = 0b10_0000;// DB0 = pwmg1dtl[5] = 1
		pwmg1dth = duty_ratio_h;// DB10_1 = {pwmg1dth[7:0], pwmg1dtl[7:6]}
	} else if (1 == duty_ratio_l) {
		pwmg1dtl = 64 + 0b10_0000;// DB0 = pwmg1dtl[5] = 1
		pwmg1dth = duty_ratio_h;// DB10_1 = {pwmg1dth[7:0], pwmg1dtl[7:6]}
	} else if (2 == duty_ratio_l) {
		pwmg1dtl = 128 + 0b10_0000;// DB0 = pwmg1dtl[5] = 1
		pwmg1dth = duty_ratio_h;// DB10_1 = {pwmg1dth[7:0], pwmg1dtl[7:6]}
	} else if (3 == duty_ratio_l) {
		pwmg1dtl = 192 + 0b10_0000;// DB0 = pwmg1dtl[5] = 1
		pwmg1dth = duty_ratio_h;// DB10_1 = {pwmg1dth[7:0], pwmg1dtl[7:6]}
	}

	// Fpwm_duty = [DB10_1 + DB0*0.5 + 0.5] / (CB + 1) = (DB10_1 + 1) / 250
	pwmg1c = 0b0000_0110;// PA4 PWM
}

// Disable PWMG1
void pwmg1_disable(void)
{
	pwmg1c = 0b0000_0000;// do not output PWM
}

// Enable PWMG1 Output with X% duty ratio
void pwmg2_enable(void)
{
	if (0 == duty_ratio_l) {
		pwmg2dtl = 0b10_0000;// DB0 = pwmg2dtl[5] = 1
		pwmg2dth = duty_ratio_h;// DB10_1 = {pwmg2dth[7:0], pwmg2dtl[7:6]}
	} else if (1 == duty_ratio_l) {
		pwmg2dtl = 64 + 0b10_0000;// DB0 = pwmg2dtl[5] = 1
		pwmg2dth = duty_ratio_h;// DB10_1 = {pwmg2dth[7:0], pwmg2dtl[7:6]}
	} else if (2 == duty_ratio_l) {
		pwmg2dtl = 128 + 0b10_0000;// DB0 = pwmg2dtl[5] = 1
		pwmg2dth = duty_ratio_h;// DB10_1 = {pwmg2dth[7:0], pwmg2dtl[7:6]}
	} else if (3 == duty_ratio_l) {
		pwmg2dtl = 192 + 0b10_0000;// DB0 = pwmg2dtl[5] = 1
		pwmg2dth = duty_ratio_h;// DB10_1 = {pwmg2dth[7:0], pwmg2dtl[7:6]}
	}

	// Fpwm_duty = [DB10_1 + DB0*0.5 + 0.5] / (CB + 1) = (DB10_1 + 1) / 250
	pwmg2c = 0b0000_0110;// PA3 PWM
}

// Disable PWMG2
void pwmg2_disable(void)
{
	pwmg2c = 0b0000_0000;// do not output PWM
}