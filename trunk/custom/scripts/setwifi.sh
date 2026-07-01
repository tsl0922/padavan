#!/bin/sh
wifi_2g_ssid=$1
wifi_2g_pwd=$2
wifi_5g_ssid=$3
wifi_5g_pwd=$4
defaultfile=$5

if [ -n "$wifi_2g_ssid" ]; then
	sed -i "40s/BOARD_PID \"_%s\"/\"$wifi_2g_ssid\"/g" $defaultfile
fi
if [ -n "$wifi_2g_pwd" ]; then
	sed -i "44s/\"1234567890\"/\"$wifi_2g_pwd\"/g" $defaultfile
fi
if [ -n "$wifi_5g_ssid" ]; then
	sed -i "41s/BOARD_PID \"_5G_%s\"/\"$wifi_5g_ssid\"/g" $defaultfile
fi
if [ -n "$wifi_5g_pwd" ]; then
        sed -i "45s/\"1234567890\"/\"$wifi_5g_pwd\"/g" $defaultfile
fi
