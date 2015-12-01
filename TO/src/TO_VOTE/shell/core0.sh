#!/bin/sh 


(
    sleep 3;
    echo "root";
    sleep 1;
    echo
    sleep 2;
    echo "./cont1_messenger";
    sleep 1000;
) | telnet 192.168.1.31
