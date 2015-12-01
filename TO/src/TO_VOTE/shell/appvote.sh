#!/bin/sh 


(
    sleep 3;
    echo "root";
    sleep 1;
    echo
    sleep 4;
    echo "./TO_VOTE";
    sleep 1000;
) | telnet 192.168.1.38
