#!/bin/sh

func_start(){
	vlmcsd -A 30d -R 30d
	logger -st "vlmcsd" "KMS Activation server is running."
}

func_stop(){
	killall -q vlmcsd
	logger -st "vlmcsd" "KMS Activation server is stoping."
}

case "$1" in
start)
	func_start
	;;
stop)
	func_stop
	;;
restart)
	func_stop
	func_start
	;;
*)
	echo "Usage: $0 { start | stop | restart }"
	exit 1
	;;
esac
