#include <PinChangeInt.h>

#include <Wire.h>
#include <SPI.h>

// Changing Plant setting
int plant = 0;  // {PLANT,WET_PLNAT,DRY_PLANT}
#define PLANT 0
#define WET_PLANT 1
#define DRY_PLANT 2
#define SWITCH 3


// Humidity_sensor
#include "Adafruit_HTU21DF.h"
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
// Humidity_sensor

// Moisture_sensor
const int AirValue[] = {530,530,530};   //you need to replace this value with Value_1
const int WaterValue[] = {260,310,460};  //you need to replace this value with Value_2
int intervals = (AirValue[0] - WaterValue[0])/3;   
int soilMoistureValue = 0;
// Moisture_sensor

// Light_sensor
#include <BH1750FVI.h>
BH1750FVI LightSensor;
// Light_sensor

// Epaper_display
#include <epd2in9.h>
#include <epdpaint.h>
#include "imagedata.h"

// Solenoid Vale
#define VALVE 2

#define COLORED     0
#define UNCOLORED   1

unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8 
Epd epd;
// Epaper_display

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps

  // Switch setting
  Serial.print("Setup - Switch: ");
  pinMode(SWITCH, INPUT_PULLUP);     //set the pin to input
  attachInterrupt(digitalPinToInterrupt(SWITCH), plantChange,RISING);
  Serial.print("OK!\n");
  delay(100);

  // Solenoid valve setting
  pinMode(VALVE, OUTPUT);     //set the pin to onput
  
  // Temperature & Humidity_sensor setup
  Serial.print("Setup - Temperature&Humidity_sensor: ");
  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }
  Serial.print("OK!\n");
  delay(100);
  
  // Light_sensor setup
  Serial.print("Setup - Light_sensor: ");
  LightSensor.begin();
  LightSensor.SetAddress(Device_Address_L);
  LightSensor.SetMode(Continuous_H_resolution_Mode);
  Serial.print("OK!\n");
  delay(100);
  
  // Epaper_display setup
  if (epd.Init(lut_full_update) != 0) {
    Serial.print("e-Paper init failed");
    return;
  }

  // Clearing memory should be done twice
  epd.ClearFrameMemory(0xFF);   // Clear display, bit set = white, bit reset = black
  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  
  if (epd.Init(lut_partial_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
  }
  
  // Displaying frame image
  epd.SetFrameMemory(IMAGE_DATA);
  epd.DisplayFrame();
  epd.SetFrameMemory(IMAGE_DATA);
  epd.DisplayFrame();

  Serial.print("OK!\n");
  Serial.println("Setup complete!");
}

// Global variables
char moisture[] = {'A','A','A','A','A','A','A','A','\0'};
char humidity[] = {'0','0','0','\0'};
char temperature[] = {'0','0','0','\0'};
char intensity[] = {'0','0','0','0','0','\0'};

void loop() {
  // sensor reading
  Moisture_sense();
  Temperature_sense();
  Humidity_sense();
  Light_sense();
  Epaper_display();
  if(soilMoistureValue < AirValue[plant] && soilMoistureValue > (AirValue[plant] - intervals))
  {
    digitalWrite(VALVE, HIGH);
    Serial.println("Valve ON");
    delay(5000);
    digitalWrite(VALVE, LOW);
    Serial.println("Valve OFF");
    htu.begin();  // re-initialize temp&humid sensor
  }
  delay(100);
}

void plantChange()
{
  Serial.println("Switch On");
  plant = (plant+1)%3;
  intervals = (AirValue[plant] - WaterValue[plant])/3;
}

void Moisture_sense()
{
  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
  Serial.print("MoistureValue: ");
  Serial.print(soilMoistureValue);
  if(soilMoistureValue > WaterValue[plant] && soilMoistureValue <= (WaterValue[plant] + intervals))
  {
    Serial.print("\tToo Wet");
    strcpy(moisture," Too Wet");
  }
  else if(soilMoistureValue > (WaterValue[plant] + intervals) && soilMoistureValue <= (AirValue[plant] - intervals))
  {
    Serial.print("\t   Good\t");
    strcpy(moisture,"    Good");
  }
  else if(soilMoistureValue < AirValue[plant] && soilMoistureValue > (AirValue[plant] - intervals))
  {
    Serial.print("\t Too Dry\t");
    strcpy(moisture," Too Dry");
  }
  else
  {
    Serial.print("   Error");
    strcpy(moisture,"   Error");
  }
//  delay(100);
}

void Humidity_sense()
{
  int val = htu.readHumidity();
  Serial.print("\tHum: "); Serial.print(val);
  if(val>=100)
  {
    humidity[0] = val/100 + '0';
    humidity[1] = val/10 %10 + '0';
    humidity[2] = val %10 + '0';
  }
  else if(val>=10)
  {
    humidity[0] = ' ';
    humidity[1] = val/10 %10 + '0';
    humidity[2] = val %10 + '0';
  }
  else if(val>=0)
  {
    humidity[0] = ' ';
    humidity[1] = ' ';
    humidity[2] = val %10 + '0';
  }
//  delay(100);
}

void Temperature_sense()
{
  int val = htu.readTemperature();
  Serial.print("\t\tTemp: "); Serial.print(val);
  if(val>=100)
  {
    temperature[0] = val/100 + '0';
    temperature[1] = val/10 %10 + '0';
    temperature[2] = val %10 + '0';
  }
  else if(val>=10)
  {
    temperature[0] = ' ';
    temperature[1] = val/10 %10 + '0';
    temperature[2] = val %10 + '0';
  }
  else if(val>=0)
  {
    temperature[0] = ' ';
    temperature[1] = ' ';
    temperature[2] = val %10 + '0';
  }
  else if(val<0)
  {
    temperature[0] = '-';
    temperature[1] = (val*(-1))/10 + '0';
    temperature[2] = (val*(-1))%10 + '0';
  }
//  delay(100);
}

void Light_sense()
{
  uint16_t val = LightSensor.GetLightIntensity();// Get Lux value
  Serial.print("\t\tLight: ");  Serial.print(val);  Serial.println(" lux");
  if(val>=10000)
  {
    intensity[0] = val/10000 +'0';    intensity[1] = val/1000 %10 +'0';
    intensity[2] = val/100 %10 +'0';  intensity[3] = val/10 %10 +'0';
    intensity[4] = val % 10 +'0';
  }
  else if(val>=1000)
  {
    intensity[0] = ' ';    intensity[1] = val/1000 %10 +'0';
    intensity[2] = val/100 %10 +'0';    intensity[3] = val/10 %10 +'0';
    intensity[4] = val % 10 +'0';
  }
  else if(val>=100)
  {
    intensity[0] = ' ';    intensity[1] = ' ';    intensity[2] = val/100 %10 +'0';
    intensity[3] = val/10 %10 +'0';               intensity[4] = val % 10 +'0';
  }
  else if(val>=10)
  {
    intensity[0] = ' ';    intensity[1] = ' ';    intensity[2] = ' ';
    intensity[3] = val/10 %10 +'0';    intensity[4] = val % 10 +'0';
  }
  else if(val>=0)
  {
    intensity[0] = ' ';    intensity[1] = ' ';    intensity[2] = ' ';
    intensity[3] = ' ';    intensity[4] = val % 10 +'0';
  }
//  delay(100);
}

void Epaper_display()
{
  // Display Plant
  paint.SetRotate(ROTATE_180);
  paint.SetWidth(100);
  paint.SetHeight(16);
  paint.Clear(UNCOLORED);
  
  if (plant == PLANT)
    paint.DrawStringAt(0,4,"  PLANT  ",&Font16,COLORED);  // Drawing area (column, raw)
  else if (plant == WET_PLANT)
    paint.DrawStringAt(0,4,"WET PLANT",&Font16,COLORED);  // Drawing area (column, raw)
  else if (plant == DRY_PLANT)
    paint.DrawStringAt(0,4,"DRY PLANT",&Font16,COLORED);  // Drawing area (column, raw)
  epd.SetFrameMemory(paint.GetImage(), 15, 220, paint.GetWidth(), paint.GetHeight());  // location (horizontal, vertical)
  
  // Display temperature
  paint.SetRotate(ROTATE_180);
  paint.SetWidth(30);
  paint.SetHeight(16);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0,4,temperature,&Font16,COLORED);  // Drawing area (column, raw)
  epd.SetFrameMemory(paint.GetImage(), 35, 172, paint.GetWidth(), paint.GetHeight());  // location (horizontal, vertical)

  // Display humidity
  paint.SetRotate(ROTATE_180);
  paint.SetWidth(30);
  paint.SetHeight(16);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0,4,humidity,&Font16,COLORED);  // Drawing area (column, raw)
  epd.SetFrameMemory(paint.GetImage(), 35, 127, paint.GetWidth(), paint.GetHeight());  // location (horizontal, vertical)

 // Display water level
  paint.SetWidth(90);
  paint.SetHeight(16);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0,4,moisture,&Font16,COLORED);  // Drawing area (column, raw)
  epd.SetFrameMemory(paint.GetImage(), 5, 90, paint.GetWidth(), paint.GetHeight());  // location (horizontal, vertical)

  // Display light intensity
  paint.SetWidth(50);
  paint.SetHeight(16);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0,4,intensity,&Font16,COLORED);  // Drawing area (column, raw)
  epd.SetFrameMemory(paint.GetImage(), 35, 32, paint.GetWidth(), paint.GetHeight());  // location (horizontal, vertical)

  epd.DisplayFrame();
 // delay(100);
}
