
IMX287_CROSS_COMPILE := arm-imx28-linux-
TARGETDIR_vote := ~/rootfs/rootfs_imx287/root/


IMX287_CC :=$(IMX287_CROSS_COMPILE)gcc
.PHONY:all clean upload

INC := -I.
INC += -Ithreadpool
INC += -Ivote_event_handle
INC += -Iapp_socket
INC += -Imessage_queue
INC += -Icomm_check
INC += -Ilog
INC += -Ilog/zlog

all:$(wildcard *.c) $(wildcard threadpool/*.c) $(wildcard app_socket/*.c) $(wildcard message_queue/*.c) $(wildcard comm_check/*.c) $(wildcard vote_event_handle/*.c) $(wildcard log/*.c) $(wildcard log/zlog/*.c)
	$(IMX287_CC) $^ $(INC) -lpthread -o TO_VOTE -DBOARDID=3 -DLOG_DEBUG=1

clean:
	@rm TO_VOTE -rf

upload:
	sudo cp TO_VOTE $(TARGETDIR_vote)

