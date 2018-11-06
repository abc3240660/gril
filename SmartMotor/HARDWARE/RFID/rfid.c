#include "cpr74.h"
#include "usart5.h" 

u8 cpr74_recv_size = 0;
u8 cpr74_send_buf[MAX_BUF_SIZE_RFID] = "";
u8 calypso_serial_num[SERIAL_NUM_SIZE] = "";
u8 calypso_card_id[CARD_ID_SIZE+1] = "";
static u8 pGotVal = NULL;

static void cpr74_parse_ack(u8 bit_pos, u8 bit_len)
{
	u8 i = 0;
	u8 j = 0;
	u8 offset = 0;
	u8 bit_cnt = 0;
	u8 tmp_val = 0;

	for (i=0; i<recv_size; i++) {
		for (j=0; j<8; j++) {
			if (bit_pos <= (i*8 + j + 1)) {
				offset = 7 - ((bit_pos+bit_cnt-1)%8);
				tmp_val += ((recv_buf[i]>>offset)&0x01) << (3-bit_cnt%4);
				bit_cnt++;
				if (0 == (bit_cnt%4)) {
					if (pGotVal != NULL) {
						if (tmp_val <= 9) {
							pGotVal[bit_cnt/4-1] = (tmp_val - 0x00) + '0';
						} else {
							pGotVal[bit_cnt/4-1] = (tmp_val - 0x0a) + 'A';
						}
					}
					tmp_val = 0;
				}
				if (bit_len == bit_cnt) {
					break;
				}
			}
		}
		if (bit_len == bit_cnt) {
			break;
		}
	}
}

static RET_RFID cpr74_check_ack(void)
{
	u8 data_len = 0;
	RET_RFID ret = RET_RFID_OK;

	if (USART5_RX_STA&0X8000) {
		if (USART5_RX_BUF[0] != 0x02) {// Header Check
			ret = RET_RFID_RECV_DAT_VALID;
		} else {
			data_len = (USART5_RX_BUF[1] << 8) + USART5_RX_BUF[2];

			if (data_len != (USART5_RX_STA&0X7FFF)) {// Length Check
				ret = RET_RFID_RECV_LEN_VALID;
			}
		}
	} 

	return ret;
}

static RET_RFID cpr74_send_cmd(u8 *cmd, u8 len, u16 waittime)
{
	RET_RFID ret = RET_RFID_OK;

	USART5_RX_STA = 0;

	for (i=0; i<len; i++) {
		while((USART5->SR&0X40) == 0);
		USART5->DR = cmd[i];
	}

	if (waittime) {
		while (--waittime) {
			delay_ms(10);
			if (USART5_RX_STA&0X8000) {// ACK RECVED
				ret = cpr74_check_ack(ack);
				break;
			} 
		}

		if (waittime == 0) {
			ret = RET_RFID_RECV_TIM_OUT;// NO ACK RECVED
		}
	}

	return ret;
}

// SEND:020009FFB001001843
// RECV:02001300B0000105710000000053511071D7F0
static RET_RFID cpr74_inventory()
{
	RET_RFID ret = RET_RFID_OK;

	cpr74_send_buf[0] = 0x02;
	cpr74_send_buf[1] = 0x00;
	cpr74_send_buf[2] = 0x09;
	cpr74_send_buf[3] = 0xFF;
	cpr74_send_buf[4] = 0xB0;
	cpr74_send_buf[5] = 0x01;
	cpr74_send_buf[6] = 0x00;
	cpr74_send_buf[7] = 0x18;
	cpr74_send_buf[8] = 0x43;

	ret = cpr74_send_cmd(cpr74_send_buf, 0x09, 1000);
	if (ret != RET_RFID_OK) {
		return ret;
	}

	// Get Serial Number
	pGotVal = calypso_serial_num;

	cpr74_parse_ack(18*4+1, 64);

	return ret;
}

// SEND:02001100B0252100000000535110713E59
// RECV:02000D00B00002B3717000957E
static RET_RFID cpr74_select_stage1()
{
	RET_RFID ret = RET_RFID_OK;

	cpr74_send_buf[0] = 0x02;
	cpr74_send_buf[1] = 0x00;
	cpr74_send_buf[2] = 0x11;
	cpr74_send_buf[3] = 0x00;
	cpr74_send_buf[4] = 0xB0;
	cpr74_send_buf[5] = 0x25;
	cpr74_send_buf[6] = 0x21;

	// serial number
	cpr74_send_buf[7]  = 0x00;
	cpr74_send_buf[8]  = 0x00;
	cpr74_send_buf[9]  = 0x00;
	cpr74_send_buf[10] = 0x00;
	cpr74_send_buf[11] = 0x53;
	cpr74_send_buf[12] = 0x51;
	cpr74_send_buf[13] = 0x10;
	cpr74_send_buf[14] = 0x71;

	cpr74_send_buf[15] = 0x3E;
	cpr74_send_buf[16] = 0x59;

	ret = cpr74_send_cmd(cpr74_send_buf, 0x11, 1000);
	if (ret != RET_RFID_OK) {
		return ret;
	}

	return ret;
}

// SEND:0200080066807350
// RECV: 02001800660017E33DD00000000000002D38000000D7F7
static RET_RFID cpr74_select_stage2()
{
	RET_RFID ret = RET_RFID_OK;

	cpr74_send_buf[0] = 0x02;
	cpr74_send_buf[1] = 0x00;
	cpr74_send_buf[2] = 0x08;
	cpr74_send_buf[3] = 0x00;
	cpr74_send_buf[4] = 0x66;
	cpr74_send_buf[5] = 0x80;
	cpr74_send_buf[6] = 0x73;
	cpr74_send_buf[7] = 0x50;

	ret = cpr74_send_cmd(cpr74_send_buf, 0x08, 1000);
	if (ret != RET_RFID_OK) {
		return ret;
	}

	return ret;
}

// SEND:02000EFFB2BE8100B201E41D011D
// RECV:02002A00B20002000004004C21808400000095584E3F90000805C0C8400019500101492492FC90001E6E
static RET_RFID cpr74_apdu_stage1()
{
	RET_RFID ret = RET_RFID_OK;

	cpr74_send_buf[0] = 0x02;
	cpr74_send_buf[1] = 0x00;
	cpr74_send_buf[2] = 0x0E;
	cpr74_send_buf[3] = 0xFF;
	cpr74_send_buf[4] = 0xB2;
	cpr74_send_buf[5] = 0xBE;
	cpr74_send_buf[6] = 0x81;
	cpr74_send_buf[7] = 0x00;
	cpr74_send_buf[8] = 0xB2;
	cpr74_send_buf[9] = 0x01;
	cpr74_send_buf[10] = 0xE4;
	cpr74_send_buf[11] = 0x1D;
	cpr74_send_buf[12] = 0x01;
	cpr74_send_buf[13] = 0x1D;

	ret = cpr74_send_cmd(cpr74_send_buf, 0x0E, 1000);
	if (ret != RET_RFID_OK) {
		return ret;
	}

	// Get Card ID
	pGotVal = calypso_card_id;

	cpr74_parse_ack(22*4+3, 76);

	return ret;
}

RET_RFID cpr74_read_calypso()
{
	RET_RFID ret = RET_RFID_OK;

	ret = cpr74_inventory();
	if (ret != RET_RFID_OK) {
		return ret;
	}

	ret = cpr74_select_stage1();
	if (ret != RET_RFID_OK) {
		return ret;
	}

	ret = cpr74_select_stage2();
	if (ret != RET_RFID_OK) {
		return ret;
	}

	ret = cpr74_apdu_stage1();

	return ret;
}
