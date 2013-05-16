#!/bin/bash

echo "25" > /sys/class/gpio/unexport
echo "25" > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio25/direction
echo "1" > /sys/class/gpio/gpio25/value

sleep 1

echo "in" > /sys/class/gpio/gpio25/direction
while [ true ]
do
	if [ "$(cat /sys/class/gpio/gpio25/value)" == '1' ]; then
		echo "out" > /sys/class/gpio/gpio25/direction
		echo "1" > /sys/class/gpio/gpio25/value
		shutdown -h now
		exit 1
	fi
	sleep 0.5
done
