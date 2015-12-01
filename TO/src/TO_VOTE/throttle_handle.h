#ifndef __THROTTLE_HANDLE_H__
#define __THROTTLE_HANDLE_H__


#define AUTO_MANUAL_INDEX 8
#define AUTO_MANUAL_BIT 0
#define AUTO_DRIVE 1
#define MANUAL_DRIVE 0



#define THROTTLE_DATA_LEN 9
#define THROTTLE_RECV_HEAD 0xC3
#define THROTTLE_SEND_HEAD 0xfe

#define MIN_THROTTLE_NUM (-8)
#define MAX_THROTTLE_NUM 8


#define FRONT_STATE 0x08
#define STOP_STATE 0x02
#define BACK_STATE 0x03
#define BACK_PRE_SET 0x02

#define THROTTLE_TYPE_INDEX 6
#define THROTTLE_PRESET_INDEX 7
#define THROTTLE_TYPE_FRONT 8
#define THROTTLE_TYPE_STOP  4
#define THROTTLE_TYPE_BACK  5

#define THROTTLE_NUM_INDEX 5
#define THROTTLE_NUM_TOTAL 9


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


	void *receive_throttle(void *arg);
	void *sent_throttle(void *arg);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
