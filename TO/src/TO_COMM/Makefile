
IMX287_CROSS_COMPILE := arm-imx28-linux-
TARGETDIR_com0 := ~/rootfs/rootfs_imx287_com0/root/
TARGETDIR_com1 := ~/rootfs/rootfs_imx287_com1/root/

IMX287_CC :=$(IMX287_CROSS_COMPILE)gcc
.PHONY:all clean upload

INC := -I.
INC += -Ithreadpool
INC += -Iapp_socket
INC += -Imessage_queue
INC += -Iroute_extract
INC += -Iroute_extract/mini_xml
INC += -Iclient
INC += -Ilog
INC += -Ilog/zlog
INC += -Ifun_packaging

OBJ := TO_COMM_0 TO_COMM_1

SRC := $(wildcard *.c) $(wildcard threadpool/*.c) $(wildcard app_socket/*.c) \
	$(wildcard message_queue/*.c) $(wildcard route_extract/*.c) \
	$(wildcard route_extract/mini_xml/*.c) $(wildcard client/*.c) \
	$(wildcard log/*.c) $(wildcard log/zlog/*.c) $(wildcard fun_packaging/*.c)
all: $(OBJ)

TO_COMM_0: $(SRC)	
	$(IMX287_CC) $^ $(INC) -o $@ -lpthread -DBOARDID=9 -DLOG_DEBUG=1

TO_COMM_1: $(SRC)
	$(IMX287_CC) $^ $(INC) -o $@ -lpthread -DBOARDID=8

clean:
	@rm TO_COMM_0 -rf
	@rm TO_COMM_1 -rf

upload:
	sudo cp TO_COMM_0 $(TARGETDIR_com1)
	sudo cp -rf ./route_data $(TARGETDIR_com1)
	sudo cp -rf ./RawData $(TARGETDIR_com1) 
#	sudo cp TO_COMM_1 $(TARGETDIR_com1)
#	sudo cp -rf ./route_data $(TARGETDIR_com1)
#	sudo cp -rf ./RawData $(TARGETDIR_com1) 
