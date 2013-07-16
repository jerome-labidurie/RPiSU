#!/bin/bash

GPIO=25

if [ ! -f "/sys/class/gpio/gpio$GPIO" ]
then
	echo "$GPIO" > /sys/class/gpio/export
fi
echo "out" > "/sys/class/gpio/gpio$GPIO/direction"
echo "1" > "/sys/class/gpio/gpio$GPIO/value"

sleep 1

echo "in" > "/sys/class/gpio/gpio$GPIO/direction"
while [ true ]
do
	if [ "$(cat /sys/class/gpio/gpio$GPIO/value)" == '1' ]; then
		echo "out" > "/sys/class/gpio/gpio$GPIO/direction"
		echo "1" > "/sys/class/gpio/gpio$GPIO/value"
		shutdown -h now
		exit 1
	fi
	sleep 0.5
done
