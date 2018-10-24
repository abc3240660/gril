#include	"extern.h"

// duty ratio=X%, duty_ratio=X
static BYTE duty_ratio;
static WORD duty_reg;

static void pwm_setup(void);
static void pwm_change(void);

BIT		p_InA	:	PA.0;
BIT		p_InB	:	PA.4;

BIT		p_OutC	:	PA.3;


void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/2		//	SYSCLK=IHRC/2

    $ p_OutC		    Out, Low;// off

	$ p_InA		    In;
	$ p_InB		    In;

	$ T16M		IHRC, /1, BIT10;			//	16MHz / 1 = 16MHz : the time base of T16.
	$ TM2C		IHRC, Disable, Period, Inverse;

	//	Insert Initial Code
	duty_ratio = 0;

	BYTE	Key_Flag;
	Key_Flag = _FIELD(p_InA, p_InB);

	BYTE	Sys_Flag = 0;
	BIT		t16_10ms			:	Sys_Flag.0;
	BIT		f_KeyA_Short_Trig	:	Sys_Flag.1;
	BIT		f_KeyA_Long_Trig	:	Sys_Flag.2;
	BIT		f_KeyB_Short_Trig	:	Sys_Flag.3;
	BIT		f_KeyB_Long_Trig	:	Sys_Flag.4;
	BIT		f_KeyA_Switch_ON	:	Sys_Flag.5;// short press
	BIT		f_KeyA_Changing_END	:	Sys_Flag.6;
	BIT		f_KeyA_TUNE_ADD		:	Sys_Flag.7;// duty_ratio = duty_ratio + 1;

	BYTE	Sys_FlagX = 0;
	BIT		f_KeyB_TIME_OUT		:	Sys_FlagX.0;

	BYTE	mode_KeyB_short = 0;// default off mode

	BYTE	t16_flag;
	BYTE	count1 = 1;
	BYTE	count_l = 0;
	BYTE	count_h = 0;
	BYTE	count_one_sec = 0;

	// debounce_tim = N * 10ms
	BYTE	debounce_time_KeyA_short	 =	55;// Key debounce time = 550 mS
	BYTE	debounce_time_KeyA_long		 =	4; // Key debounce time = 40 mS
	BYTE	debounce_time_KeyB_short	 =	55;// Key debounce time = 550 mS
	BYTE	debounce_time_KeyB_long		 =	4; // Key debounce time = 40 mS

	BYTE	period_time_dutyratio_update =	50; // 50 mS

	while (1)
	{
		if  (INTRQ.T16)
		{
			INTRQ.T16		=	0;
			If (--count1 == 0)					//	DZSN  count
			{
				count1		=	78;				//	128uS * 78 = 10 mS 
				t16_10ms	=	1;
			}
		}

		while (t16_10ms)
		{
			t16_10ms	=	0;

			// 0: off
			// 1: 30 minutes = 30 * 60 = 1800s
			// 2: 60 minutes = 60 * 60 = 3600s
			if (mode_KeyB_short != 0) {
				if (!f_KeyB_TIME_OUT) {
					count_one_sec++;

					if (100 == count_one_sec) {// one second
						count_one_sec = 0;

						// N second
			            count_l++;
			            
			            if (100 == count_l) {
			                count_l = 0;
			                count_h++;
			                if (100 == count_h)
			                    count_h = 0;
						}
					}

					if (2 == mode_KeyB_short) {
						if ((0 == count_l) && (18 == count_h)) {
							f_KeyB_TIME_OUT = 1;
						}
					} else if (3 == mode_KeyB_short) {
						if ((0 == count_l) && (36 == count_h)) {
							f_KeyB_TIME_OUT = 1;
						}
					}
				}
			}

			// step1: last InA = 1(normal volt level)
			// step5: last InA = 0
			// step7: last InA = 0

			// step5~: last InA = 1
			A	=	(PA ^ Key_Flag) & _FIELD(p_InA);
			// step2: Start Push Button -> !ZF Active
			// step6 / step4~: Button Non-Releasing -> !ZF Non-Active
			// step8 / step6~: Start Release Button -> !ZF Active
			if (! ZF)
			{
				// Button Down
				if (!p_InA) {// step3: InA = 0 for 550ms or step3~: InA = 0 for 5ms(short press case)
					if (--debounce_time_KeyA_long == 0)
					{
						Key_flag	^=	_FIELD(p_InA);// step4: update InA = 0
						f_KeyA_Long_Trig	=	1;

						if (f_KeyA_TUNE_ADD) {
							f_KeyA_TUNE_ADD = 0;// 2nd long press: duty_ratio = duty_ratio - 1
						} else {
							f_KeyA_TUNE_ADD = 1;// 1st long press: duty_ratio = duty_ratio + 1
						}

						debounce_time_KeyA_long	=	55;
					}
				} else {// Button Up
					f_KeyA_Long_Trig = 0;
					Key_flag	^=	_FIELD(p_InA);// step9: update InA = 1
				}
			} else {
				if (debounce_time_KeyA_long < 50) {
					f_KeyA_Short_Trig = 1;// step7~: no need to update InA
				}
				debounce_time_KeyA_long	=	55;
			}

			A	=	(PA ^ Key_Flag) & _FIELD(p_InB);
			if (! ZF)
			{
				// Button Down
				if (!p_InB) {
					if (--debounce_time_KeyB_long == 0)
					{
						Key_flag	^=	_FIELD(p_InB);
						f_KeyB_Long_Trig	=	1;
						debounce_time_KeyB_long	=	55;
					}
				} else {// Button Up
					Key_flag	^=	_FIELD(p_InB);
				}
			} else {
				if (debounce_time_KeyB_long < 50) {
					f_KeyB_Short_Trig = 1;
				}
				debounce_time_KeyB_long	=	55;

			}

			
			if (f_KeyA_Switch_ON) {
				if (!f_KeyA_Changing_END) {
					if (50 == period_time_dutyratio_update) {
						if (duty_ratio < 50) {
							duty_ratio = duty_ratio + 1;
							pwm_change();
						} else {
							f_KeyA_Changing_END = 1;
						}
					}

					period_time_dutyratio_update--;

					if (0 == period_time_dutyratio_update) {
						period_time_dutyratio_update = 50;
					}
				}
			} else {
				if (!f_KeyA_Changing_END) {
					if (50 == period_time_dutyratio_update) {
						if (duty_ratio > 0) {
							duty_ratio = duty_ratio - 1;
							pwm_change();
						} else {
							f_KeyA_Changing_END = 1;
						}
					}

					period_time_dutyratio_update--;

					if (0 == period_time_dutyratio_update) {
						period_time_dutyratio_update = 50;
					}
				}
			}

			if (f_KeyA_Short_Trig) {
				f_KeyA_Short_Trig = 0;
				period_time_dutyratio_update = 50;

				if (f_KeyA_Switch_ON) {// ON->OFF
					f_KeyA_Switch_ON = 0;
					f_KeyA_Changing_END = 0;
				} else {// OFF->ON
					f_KeyA_Switch_ON = 1;
					f_KeyA_Changing_END = 0;

					pwm_setup();// Open PWM
				}
			}

			if (f_KeyA_Long_Trig) {
				if (f_KeyA_Switch_ON) {
					if (50 == period_time_dutyratio_update) {
						if (f_KeyA_TUNE_ADD) {
							if (duty_ratio < 100) {
								duty_ratio = duty_ratio + 1;
								pwm_change();
							}
						} else {
							if (duty_ratio > 5) {
								duty_ratio = duty_ratio - 1;
								pwm_change();
							}
						}
					}

					period_time_dutyratio_update--;

					if (0 == period_time_dutyratio_update) {
						period_time_dutyratio_update = 50;
					}
				}
			}

			if (f_KeyB_Short_Trig) {
				f_KeyB_Short_Trig = 0;
				mode_KeyB_short = mode_KeyB_short + 1;

				if (3 == mode_KeyB_short) {
					mode_KeyB_short = 0;
				}

			}

			if (!f_KeyB_TIME_OUT) {
				if (50 == period_time_dutyratio_update) {
					if (duty_ratio > 0) {
						duty_ratio = duty_ratio - 1;
						pwm_change();
					}
				}

				period_time_dutyratio_update--;

				if (0 == period_time_dutyratio_update) {
					period_time_dutyratio_update = 50;
				}
			}
		}
	}
}

// Enable Timer2 PWM to 500Hz(2ms)
void pwm_setup(void)
{
	tm2ct = 0x0;
	// Duty Ratio = (K+1) / 256, K = tm2b
	// Duty Ratio = (0+1) / 256 = 1 / 256
	duty_reg = 0;
	tm2b = duty_reg;// 0

	// PWM_HZ = SYSCLK / (256 * S1 * (S2+1))
	// PWM_HZ = 8M / 256 (4 * (14+1)) = 488Hz
	// PWM_HZ = 8M / 256 (4 * (15+1)) = 520Hz
	tm2s = 0b001_01110;// SYSCLK=IHRC/2=8MHz, S1=4(tm2s[6:5]=1), S2=14(tm2s[4:0])

	tm2c = 0b0001_1010;// CLK(=IHRC/2) | PA3 | PWM | Disable Inverse
}

// Duty Ratio = (K+1) / 256, K = tm2b
void pwm_change(void)
{
	BYTE odd_flag = 0;
	BYTE i = duty_ratio;// duty_ratio range: 0 ~ 100

	//tm2ct = 0x0;

	if (0 == duty_ratio) {
		duty_reg = 0;
		tm2b = duty_reg;
		tm2c = 0b0000_0000;// Disable Timer2 PWM
	} else if (100 == duty_ratio) {
		duty_reg = 255;
		tm2b = duty_reg;
	} else {
		while(i--) {
			if (odd_flag) {
				odd_flag = 0;
			} else {
				odd_flag = 1;
			}

			if (odd_flag) {
				if (duty_reg >= 254) {
					duty_reg = 255;
				} else {
					duty_reg = duty_reg + 2;
				}
			} else {
				if (duty_reg >= 253) {
					duty_reg = 255;
				} else {
					duty_reg = duty_reg + 3;
				}
			}
		}
		tm2b = duty_reg;
	}

	//tm2s = 0b000_01111;// 15
	//tm2s = 0b000_00111;// 7
	//tm2c = 0b0001_1000;// CLK(=IHRC/2) | PA3 | Period | Disable Inverse
}

#if 0
void	Interrupt (void)
{
	pushaf;

	if (Intrq.T16)
	{	//	T16 Trig
		//	User can add code
		Intrq.T16	=	0;
		//...
	}

	popaf;
}
#endif