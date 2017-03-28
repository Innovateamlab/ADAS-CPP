#! /bin/sh

echo shell begin
sleep 10
echo config GPIO pins
sudo /usr/local/bin/gpio export 2 out
sudo /usr/local/bin/gpio export 3 out
sudo /usr/local/bin/gpio export 4 out

echo launch program...
cd /home/pi/Documents/ADAS-CPP

sudo ./ShapeColorDetector.a `sudo ls ImagesSave/Red/ | grep ".jpg" | wc -l` `sudo ls ImagesSave/Blue/ | grep ".jpg" | wc -l` `sudo ls ImagesSave/Global/ | grep ".jpg" | wc -l` &

sleep 1

sudo ./FlagInterface.a


