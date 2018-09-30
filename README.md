Class: Mechatronics at Colorado School of Mines<br>
Lecturer: Dr. Andrew Petruska<br>

Student: Jiseong Yoo<br>
Date: May 1, 2018<br>

Description:<br>
 This system monitors environment of a plant pot; Light intensity, Humidity, Temperature, Moisture. Collected data from the sensors is shown on the E-paper display. When soil is too dry (Moisture = dry), the system waters.


Library Setup
1. Copy and paste library folders;
  - 'Adafruit_HTU21DF', 'BH1750FVI', 'epdif', 'epdpaint', 'epd2in9'

E-paper display
1. Model name waveshare 2.9 inch E-paper Display panel Module
2. Library folder 'epdif', 'epdpaint', 'epd2in9'
3. Resolution 296x128 pixels, black & white
4. Display size 66.89mm x 29.05mm
5. SPI interface

// ImageData
1. 'imagedata.c' are hexadecimal data for the image, original image is 'imagedata.bmp'
2. 'imagedata_180.c' are hexadecimal data for the upsidedown image, original image is 'imagedata_180.bmp'
3. Keep the file 'imagedata.h', 'imagedata.cpp' with 'SensorSpike.ino'.

// Changing image
1. Overwrite IMAGE_DATA[] in 'imagedata.cpp'
2. The image should be 128x296 pixels, and IMAGE_DATA[] should have 4736 hexadecimal numbers. (1 number per 8 pixels)


Temperature/Humidity sensor
1. Model name HTU21DF
2. Library folder 'Adafruit_HTU21DF'
3. Temperature detection range: -30 ~ 90C  accuracy 1C
4. Humiditiy detection range: 5 ~ 95% RH   accuracy 2%
5. I2C

Light intensity
1. Model name BH1750
2. Library folder 'BH1750FVI'
3. Light intensity detection range: 0 ~ 65535 lux
4. I2C

Moisture
1. Model name Capacitive Soil Moisture Sensor V1.0
2. PH2.54-3P
