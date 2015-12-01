#ifndef _COMM_CHECK_H_
#define _COMM_CHECK_H_

int comm_register(int boardid);

void heartbeat_check();

int comm_check_feedback(char *data);

#endif
