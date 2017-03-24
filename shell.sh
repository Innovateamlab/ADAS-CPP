#! /bin/sh

echo shell begin
sleep 60
echo config GPIO pins
sudo /usr/local/bin/gpio export 17 out
sudo /usr/local/bin/gpio export 18 out
echo launch program...
cd /home/pi/ShapeColorDetect_v3_3
./ShapeColorDetector.a


