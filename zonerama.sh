#!/bin/bash

# this is a simple start script that runs zonerama in a endless loop

cd /root/zonerama
for (( ; ; ))
do
 	time ./Zonerama
	sleep 10 
done

