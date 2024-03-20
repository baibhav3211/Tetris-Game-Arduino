// GitHub: https://github.com/baibhav3211/Tetris-Game-Arduino
// Code for the game Tetris on an Arduino with a 109 LED matrix
// The game is controlled by 4 buttons, 2 for movement, 1 for rotation and 1 for speeding up the piece
// The game is played on a 10x14 grid, with the top 4 rows hidden from the player
// Coded by: Robotics Society NIT Hamirpur

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