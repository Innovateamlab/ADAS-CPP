#! /bin/sh

echo shell begin
sleep 60
echo config GPIO pins
sudo /usr/local/bin/gpio export 17 out
sudo /usr/local/bin/gpio export 18 out
echo launch program...
cd /home/pi/Documents/ADAS-CPP/dist
./ShapeColorDetector_v0.1


