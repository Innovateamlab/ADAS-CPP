#! /bin/sh

echo shell begin
sleep 1
echo config GPIO pins
sudo /usr/local/bin/gpio export 15 out
sudo /usr/local/bin/gpio export 16 out
sudo /usr/local/bin/gpio export 17 out
sudo /usr/local/bin/gpio export 18 out
echo launch program...
cd /home/pi/Documents/ADAS-CPP

./ShapeColorDetector.a `sudo ls ImagesSave/Red/ | grep ".jpg" | wc -l` `sudo ls ImagesSave/Blue/ | grep ".jpg" | wc -l` `sudo ls ImagesSave/Global/ | grep ".jpg" | wc -l` &
./FlagInterface.a


