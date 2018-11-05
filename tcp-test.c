#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct reg_ack {
	char head[16];
	char tag[8];
	char imei[32];
	char ack[8];
	char cmd[8];
	char time[32];
	char heart[8];
} REG_ACK, *PREG_ACK;

#define PROTOCOL_HEAD		"^MOBIT"
#define DEV_TAG			"ECAR"
#define SW_VERSION		"V1.0"
#define HW_VERSION		"V1.0"

#define CMD_DEV_ACK		"Re"// DEV ACK

#define CMD_DEV_REGISTER	"R0"// SVR ACK
#define CMD_HEART_BEAT		"H0"// SVR ACK
#define CMD_INQUIRE_PARAM	"C0"// SVR CMD
#define CMD_RING_ALARM		"R1"// SVR CMD
#define CMD_DOOR_OPEN		"O0"// SVR CMD
#define CMD_DOOR_CLOSE		"C1"// SVR CMD

#define LEN_SYS_TIME	32
#define LEN_IMEI_NO	32
#define LEN_BAT_VOL	32
#define LEN_RSSI_VAL	32
#define LEN_MAX_SEND	32
#define LEN_MAX_RECV	32

#define DEBUG_USE 1

enum CMD_TYPE {
	DEV_REGISTER = 0,
	HEART_BEAT,
	INQUIRE_PARAM,
	RING_ALARM,
	DOOR_OPEN,
	DOOR_CLOSE,
	UNKNOWN_CMD
};

const char* cmd_list[] = {
	CMD_DEV_REGISTER,
	CMD_HEART_BEAT,
	CMD_INQUIRE_PARAM,
	CMD_RING_ALARM,
	CMD_DOOR_OPEN,
	CMD_DOOR_CLOSE,
	NULL
};

char sync_sys_time[LEN_SYS_TIME+1] = "";

int door_state = 0;
int ring_times = 0;
int lock_state = 0;
int hbeat_time = 0;
char bat_vol[LEN_BAT_VOL] = "88";// defaut is fake
char imei[LEN_IMEI_NO] = "88888888";// defaut is fake
char rssi[LEN_RSSI_VAL] = "88";// defaut is fake
char dev_time[LEN_SYS_TIME] = "20181105151955";// defaut is fake

int get_cmd_cnt()
{
	int cnt = 0;
	while(1) {
		if (NULL == cmd_list[cnt]) {
			break;
		}
		cnt++;
	}

	return cnt;
}

int is_supported_cmd(int cnt, char* str)
{
	int i = 0;

	for (i=0; i<cnt; i++) {
		if (0 == strncmp(str, cmd_list[i], strlen(cmd_list[i]))) {
			break;
		}
	}

	if (i != UNKNOWN_CMD) {
		printf("Recved CMD/ACK %s\n", str);
	}

	return i;
}

// DEV ACK
void do_door_open(char* send)
{
	// FuncCall TBD

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%d\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_ACK, CMD_DOOR_OPEN, door_state);

	printf("SEND:%s\n", send);
}

// DEV ACK
void do_door_close(char* send)
{
	// FuncCall TBD

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%d\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_ACK, CMD_DOOR_CLOSE, door_state);

	printf("SEND:%s\n", send);
}

void parse_msg(char* msg, char* send)
{
	int i = 0;
	int index = 0;
	int data_pos = 0;
	char delims[] = ",";
	char* split_str = NULL;

	enum CMD_TYPE cmd_type = UNKNOWN_CMD;

	int cmd_count = get_cmd_cnt();

#ifdef DEBUG_USE
	//printf("Support %d CMDs\n", cmd_count);
#endif

	split_str = strtok(msg, delims);
	while(split_str != NULL) {
#ifdef DEBUG_USE
		//printf("split_str = %s\n", split_str);
#endif
		if ((3 == index) || (4 == index)) {
			if (UNKNOWN_CMD == cmd_type) {
				cmd_type = is_supported_cmd(cmd_count, split_str);

				if (cmd_type != UNKNOWN_CMD) {
					if (0 == data_pos) {
						data_pos = index;
					}
				} else {
					if (DOOR_OPEN == cmd_type) {
						do_door_open(send);
					} else if (DOOR_OPEN == cmd_type) {
						do_door_close(send);
					}
				}
			}
		}

		if (index > data_pos) {
			if (DEV_REGISTER == cmd_type) {
				if (5 == index) {
					strncpy(sync_sys_time, split_str, LEN_SYS_TIME);
					sync_sys_time[LEN_SYS_TIME] = '\0';
					printf("sync_sys_time = %s\n", sync_sys_time);
				} else if (6 == index) {
					hbeat_time = atoi(split_str);
					printf("hbeat_time = %d\n", hbeat_time);
				}
			} else if (HEART_BEAT == cmd_type) {
			} else if (INQUIRE_PARAM == cmd_type) {
			} else if (RING_ALARM == cmd_type) {
				ring_times = atoi(split_str);
				printf("ring_times = %d\n", ring_times);
				// FuncCall TBD
			}
		}
		split_str = strtok(NULL, delims);
		index++;
	};
}

// DEV Auto Send
void do_dev_register(char* send)
{
	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_REGISTER, HW_VERSION, SW_VERSION, bat_vol);

	printf("SEND:%s\n", send);
}

// DEV Auto Send
void do_heart_beat(char* send)
{
	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%d,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_HEART_BEAT, dev_time, lock_state, rssi, bat_vol);

	printf("SEND:%s\n", send);
}

int main()
{	
	char send_buf[LEN_MAX_SEND] = "";
	char recv_buf[LEN_MAX_RECV] = "";

	do_dev_register(send_buf);

	do {
		if (hbeat_time != 0) {
#ifdef DEBUG_USE
			hbeat_time = 0;// 
#endif
			do_heart_beat(send_buf);
		}

		printf("reg_ack:   ^MOBIT,ECAR,88888888,Re,R0,20181105153255,120\n");
		printf("hbeat_ack: ^MOBIT,ECAR,88888888,Re,H0\n");
		printf("ring_cmd:  ^MOBIT,ECAR,88888888,R1,8\n");
		printf("Please Enter RECVED DATA:\n");

		gets(recv_buf);

		//printf("GOT MSG: %s\n", recv_buf);

		parse_msg(recv_buf, send_buf);

		usleep(1000);
	} while(1);
}
