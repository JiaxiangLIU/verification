#!/bin/bash

gnome-terminal -x ./com0.sh
gnome-terminal -x ./com1.sh
gnome-terminal -x ./core0.sh
gnome-terminal -x ./core1.sh
gnome-terminal -x ./core2.sh
gnome-terminal -x ./core3.sh 
gnome-terminal -x ./vote.sh 

sleep 3

gnome-terminal -x ./appcom0.sh
gnome-terminal -x ./appcom1.sh
gnome-terminal -x ./appcore0.sh
gnome-terminal -x ./appcore1.sh
gnome-terminal -x ./appcore2.sh
gnome-terminal -x ./appcore3.sh 


sleep 5
gnome-terminal -x ./appvote.sh 
