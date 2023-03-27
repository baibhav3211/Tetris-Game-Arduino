#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void setup()
{
if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.display();
    display.clearDisplay();
    Serial.begin(9600);

}
String temp = "";
void loop()
{
  if(Serial.available()){
    char c = Serial.read();
    if(c == '\n'){

      display.clearDisplay();
      display.setCursor(0, 10);
      display.println(temp);
      display.display();
      temp = "";
    }else{
      temp+=c;
    }
  }
  
}