#include	"extern.h"

static BYTE duty_ratio_l;
static BYTE duty_ratio_h;

static WORD duty_reg;

static void pwmgg0_enable(void);
static void pwmgg0_disable(void);
static void pwmgg1_enable(void);
static void pwmgg1_disable(void);
static void pwmgg2_enable(void);
static void pwmgg2_disable(void);

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
	.ADJUST_IC	SYSCLK = IHRC/2// SYSCLK = IHRC/2 = 8MHz

	$ p_Out3	Out, Low;// off
	$ p_Out5	Out, Low;// off
	$ p_Out7	Out, Low;// off

	$ p_In2		In;
	$ p_In6		In;

	$ T16M		IHRC, /1, BIT10;// 16MHz/1 = 16MHz:the time base of T16.
	$ TM2C		IHRC, Disable, Period, Inverse;

	gpcs   = 0b1111_1101;// Vinternal R = Vdd*14/32
	gpcc   = 0b1001_0111;// +:PA4, -:Vinternal R
	padier = 0b111_0_1111;// disable digital input for PA4

	// Insert Initial Code
	duty_ratio_l = 0;

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

		if (f_5ms_Trig) {// every 5ms
			f_5ms_Trig = 0;

			if (3 == mode_In6) {
				if (f_In7_value) {
					p_Out7 = 0;
					f_In7_value = 0;
				} else {
					p_Out7 = 1;
					f_In7_value = 1;
				}
			} else if (5 == mode_In6){
				if (f_In3_value) {
					p_Out3 = 0;
					f_In3_value = 0;
				} else {
					p_Out3 = 1;
					f_In3_value = 1;
				}
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

			// 30 mins time out, NEXT to close all output
			if ((0 == count_l) && (18 == count_h)) {
				pwmg1_disable();

				p_Out3 = 0;
				p_Out5 = 0;
				p_Out7 = 0;

				f_In3_value = 0;
				f_In7_value = 0;
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
					if (--debounce_time_In6 == 0) {
						f_cmptor_valid = 1;
						f_In6_Trig = 1;
						debounce_time_In6 = 4;
					}
				} else {//ButtonUp
					f_In6_Trig = 0;
					f_cmptor_valid = 0;
				}
			} else {
				debounce_time_In6 = 4;
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

					f_In6_Trig = 0;
					mode_In6++;
					if (7 == mode_In6) {
						mode_In6 = 0;
					}
				}
				if (mode_In6 != mode_In6_last) {
					if (0 == mode_In6) {
						p_Out3 = 0;
						p_Out7 = 0;

						f_In3_value = 0;
						f_In7_value = 0;

						//duty_ratio = 30;
						pwmg1_duty_set();
						pwmg1_enable();	
					} else if (1 == mode_In6) {
						p_Out3 = 0;
						p_Out7 = 0;

						f_In3_value = 0;
						f_In7_value = 0;

						pwmg1_disable();
						//duty_ratio = 60;
						pwmg1_duty_set();
						pwmg1_enable();	
					} else if (2 == mode_In6) {
						pwmg1_disable();

						p_Out3 = 0;
						p_Out5 = 1;
						p_Out7 = 0;

						f_In3_value = 0;
						f_In7_value = 0;
					} else if (3 == mode_In6) {
						p_Out3 = 0;
						p_Out5 = 0;
						p_Out7 = 0;// later pwm

						f_In3_value = 0;
						f_In7_value = 0;
					} else if (4 == mode_In6) {
						p_Out3 = 0;
						p_Out5 = 0;
						p_Out7 = 1;

						f_In3_value = 0;
						f_In7_value = 1;
					} else if (5 == mode_In6) {
						p_Out3 = 0;// later pwm
						p_Out5 = 0;
						p_Out7 = 0;

						f_In3_value = 0;
						f_In7_value = 0;
					} else if (6 == mode_In6) {
						p_Out3 = 1;
						p_Out5 = 0;
						p_Out7 = 0;

						f_In3_value = 1;
						f_In7_value = 0;
					}

					mode_In6_last = mode_In6;
				}

			}
		}
	}
}
// PWMG0/1/2 Share the same Freq but different duty ratio
// Setting PWM's Freq to 100Hz
// Fpwm_freq = Fpwm_clk / (CB + 1)
void pwm_freq_set()
{
	pwmgcubl = 0b0000_0000;
	pwmgcubh = 0b1001_1100;// CB = {pwmgcubh[7:0], pwmgcubl[7:6]} = 624
	
	//pwmgclk = 0b1111_0000;// Fpwm_clk = = SYSCLK / 128 -> Fpwm_freq = 100HZ
    pwmgclk = 0b1110_0000;// Fpwm_clk = = SYSCLK / 64 -> Fpwm_freq = 200HZ
}

void duty_ratio_changing(void)
{
	duty_ratio_l++;
	if (4 == duty_ratio_l) {
		duty_ratio_l = 0;
		duty_ratio_h++;
		
		if (256 == duty_ratio_h) {
			duty_ratio_h = 0;
		}
	}	
}

// Enable PWMG0 Output with 50% duty ratio
void pwmg0_enable(void)
{
#if 0
	pwmg0dtl = 0b0000_0000;// DB0 = pwmg0dtl[5] = 0
	pwmg0dth = 0b0100_1110;// DB10_1 = {pwmg0dth[7:0], pwmg0dtl[7:6]} = 312
#else
	pwmg0dtl = duty_ratio_l;
	pwmg0dth = duty_ratio_h;
#endif

	// Fpwm_duty = [DB10_1 + DB0*5 + 0.5] / (CB + 1) = 312.5 / 625 = 50%
	pwmg0c = 0b0000_0110;// PA0 PWM
}

// Disable PWMG0
void pwmg0_disable(void)
{
	pwmg0c = 0b0000_0000;// do not output PWM
}

// Enable PWMG1 Output with 50% duty ratio
void pwmg1_enable(void)
{
	pwmg1dtl = 0b0000_0000;// DB0 = pwmg1dtl[5] = 0
	pwmg1dth = 0b0100_1110;// DB10_1 = {pwmg1dth[7:0], pwmg1dtl[7:6]} = 312

	// Fpwm_duty = [DB10_1 + DB0*5 + 0.5] / (CB + 1) = 312.5 / 625 = 50%
	pwmg1c = 0b0000_0110;// PA4 PWM
}

// Disable PWMG1
void pwmg1_disable(void)
{
	pwmg1c = 0b0000_0000;// do not output PWM
}

// Enable PWMG1 Output with 50% duty ratio
void pwmg2_enable(void)
{
	pwmg2dtl = 0b0000_0000;// DB0 = pwmg2dtl[5] = 0
	pwmg2dth = 0b0100_1110;// DB10_1 = {pwmg2dth[7:0], pwmg2dtl[7:6]} = 312

	// Fpwm_duty = [DB10_1 + DB0*5 + 0.5] / (CB + 1) = 312.5 / 625 = 50%
	pwmg2c = 0b0000_0110;// PA3 PWM
}

// Disable PWMG2
void pwmg2_disable(void)
{
	pwmg2c = 0b0000_0000;// do not output PWM
}

#if 0
void	Interrupt(void)
{
	pushaf;

	if(Intrq.T16)
	{	//	T16Trig
		//	Usercanaddcode
		Intrq.T16	=	0;
		//...
	}

	popaf;
}
#endif
