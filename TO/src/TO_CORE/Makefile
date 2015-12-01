
AM3517_CROSS_COMPILE := arm-am3517-linux-gnueabi-
TARGETDIR_cont0 := /home/ryan/rootfs/rootfs_am3517_cont00/root/
TARGETDIR_cont1 := /home/ryan/rootfs/rootfs_am3517_cont01/root/
TARGETDIR_cont2 := /home/ryan/rootfs/rootfs_am3517_cont10/root/
TARGETDIR_cont3 := /home/ryan/rootfs/rootfs_am3517_cont11/root/

IMX287_CC :=$(AM3517_CROSS_COMPILE)gcc
.PHONY:all clean upload

INC := -I.
INC += -Ithreadpool
INC += -Iapp_socket
INC += -Imessage_queue
INC += -Ilog
INC += -Ilog/zlog
INC += -IOPTIMIZE
INC += -IOPTIMIZE/optimize
INC += -IOPTIMIZE/optimize/common
INC += -IOPTIMIZE/optimize/preprocess
INC += -IOPTIMIZE/optimize/rawoptimize
INC += -IOPTIMIZE/optimize/rawoptimize/base
INC += -IOPTIMIZE/optimize/limitadjust
INC += -IOPTIMIZE/optimize/timeadjust
INC += -IOPTIMIZE/datacontrol
INC += -IOPTIMIZE/drivecontrol
INC += -IOPTIMIZE/exception
INC += -IOPTIMIZE/tools
INC += -IOPTIMIZE/optimize/afterprocess
INC += -Iconf_manage/xmlopt
INC += -Iconf_manage 
INC += -IOPTIMIZE/drivecontrol/rt_process_n_controller
INC += -IOPTIMIZE/drivecontrol/linked_list_operations
INC += -IOPTIMIZE/drivecontrol/train_model_4_testing

OBJ := TO_CORE_0 TO_CORE_1 TO_CORE_2 TO_CORE_3

SRC := $(wildcard *.c) $(wildcard threadpool/*.c) $(wildcard app_socket/*.c) $(wildcard ./message_queue/*.c) $(wildcard ./log/*.c) $(wildcard ./log/zlog/*.c)

OPT_SRC := $(wildcard ./OPTIMIZE/*.c) $(wildcard ./OPTIMIZE/datacontrol/*.c) $(wildcard ./OPTIMIZE/optimize/*.c) $(wildcard ./OPTIMIZE/optimize/common/*.c) $(wildcard ./OPTIMIZE/optimize/limitadjust/*.c) $(wildcard ./OPTIMIZE/optimize/preprocess/*.c) $(wildcard ./OPTIMIZE/optimize/rawoptimize/*.c) $(wildcard ./OPTIMIZE/optimize/rawoptimize/base/*.c) $(wildcard ./OPTIMIZE/optimize/timeadjust/*.c) $(wildcard ./OPTIMIZE/tools/*.c) $(wildcard ./OPTIMIZE/xmlopt/*.c) $(wildcard ./OPTIMIZE/drivecontrol/*.c) $(wildcard ./OPTIMIZE/exception/*.c) $(wildcard ./OPTIMIZE/optimize/afterprocess/*.c) $(wildcard ./conf_manage/*.c) $(wildcard ./conf_manage/xmlopt/*.c) $(wildcard ./OPTIMIZE/drivecontrol/*.c) $(wildcard ./OPTIMIZE/drivecontrol/rt_process_n_controller/*.c) $(wildcard ./OPTIMIZE/drivecontrol/linked_list_operations/*.c) $(wildcard ./OPTIMIZE/drivecontrol/train_model_4_testing/*.c)		

all: $(OBJ)
TO_CORE_0: $(SRC) $(OPT_SRC)	
	$(IMX287_CC) $^ $(INC) -lpthread -lm -o $@ -DBOARDID=7 -DOPT_DEBUG
TO_CORE_1: $(SRC) $(OPT_SRC)	
	$(IMX287_CC) $^ $(INC) -lpthread -lm -o $@ -DBOARDID=6 -DOPT_DEBUG
#TO_CORE_2: $(SRC) $(OPT_SRC)	
#	$(IMX287_CC) $^ $(INC) -lpthread -lm -o $@ -DBOARDID=5 -DOPT_DEBUG
#TO_CORE_3: $(SRC) $(OPT_SRC)	
#	$(IMX287_CC) $^ $(INC) -lpthread -lm -o $@ -DBOARDID=4 -DOPT_DEBUG

clean:
	@rm TO_CORE_0 TO_CORE_1 TO_CORE_2 TO_CORE_3 -rf

upload:
	sudo cp TO_CORE_0 $(TARGETDIR_cont0)
	sudo cp TO_CORE_0 $(TARGETDIR_cont1)
#	sudo cp TO_CORE_2 $(TARGETDIR_cont2)
#	sudo cp TO_CORE_3 $(TARGETDIR_cont3)
	
