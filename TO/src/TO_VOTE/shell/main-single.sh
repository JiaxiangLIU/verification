#!/bin/bash
#--geometry=80x25+10+10 
gnome-terminal --geometry=80x24+600+10 --window --title="com0_can2net" -e ./com0_can2net.sh --tab --title="com0_messenger" -e ./com0.sh  --tab --title="com0_app" -e ./appcom0.sh
gnome-terminal --geometry=80x24+10+10 --window --title="core0_messenger" -e ./core0.sh --tab --title="core0_app" -e ./appcore0.sh
gnome-terminal --geometry=80x24+600+130 --window --title="vote_can2net" -e ./vote_can2net.sh --tab --title="vote_messenger" -e ./vote.sh --tab --title="vote_app" -e ./appvote.sh

