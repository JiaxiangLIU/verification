#!/bin/sh 


(
    sleep 3;
    echo "root";
    sleep 1;
    echo
    sleep 8;
    echo "./TO_COMM_0";
    sleep 1000;
) | telnet 192.168.1.35
