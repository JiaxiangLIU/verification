#!/bin/sh 


(
    sleep 3;
    echo "root";
    sleep 1;
    echo
    sleep 2;
    echo "./can2net";
    sleep 1000;
) | telnet 192.168.1.38