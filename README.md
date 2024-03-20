# Classical Game of Tetris implemented on Arduino UNO and noepixel LED Strip

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Setup](#setup)
- [Code Structure](#code-structure)
- [Authors](#authors)

## Introduction

This project implements the classic game of Tetris using a Neopixel LED strip and an Arduino UNO. The player interacts with the game using five push buttons, controlling the movement and rotation of falling tetrominoes within a 10x10 matrix. The game features random spawning and locations for tetrominoes, with LED colors changing after each move. Additionally, the OLED display provides real-time feedback on the player's score, including the current score and high score. The game also includes a visually appealing LED effect to indicate game over.

<img src=".\Assets\Full_project.jpg" alt="Alt Text" width="700">

## Features

1. **10 X 10 LED Matrix.**
2. **Controls via 5 Push Buttons.**
    1. Left shift
    2. Right Shift
    3. Down
    4. Rotate
    5. Reset
3. **Displaying Score on OLED Display.**
    1. High Score (H)
    2. Current Score (S)
4. **Six Different Shapes**
    1. I-shape
    2. S-shape
    3. Z-shape
    4. L-shape
    5. Square-shape
    6. T-shape
5. **Random spawning of shapes.**
6. **Random location of spawning of shapes.**
7. **Game over LED effect.**
8. **Change in LED colors after each move.**

## Requirements

- **Hardware:** two Arduino UNOs, noepixel LED strip (109 LEDs), OLED Display, 5V supply (eg. SMPS)
- **Software:** Arduino IDE 
- **Additional Requirements:** 3D-printer, five Push tactile buttons, Acrylic fiber sheet, bread board or PCB, Sub-board/Cardboard.

## Setup

1. **Tetris Body (Optional)**
    1. Via using any 3D printing technique print the modles [box1](/Assets/box1.stl) and [box2](/Assets/box2.stl).
    2. The box1 is used to keep the 10 X 10 LED matrix, whereas, box2 is used to keep Arduino UNOs, wires and OLED Display.
    3. If you want you can use Sunboard or Cardboard to make the subsection for the LED Matrix and cover it with an translucent acrylic sheet.
    4. <img src=".\Assets\boxes.jpg" alt="Alt Text" width="700">
2. **10 X 10 LED Matrix**
    1. To make a 10 X 10 noepixel LED Matrix we need 109 LEDs in the Strip.
    2. Paste Strip of 10 LEDs in a row.
    3. To make the next row fold the strip (folding consumes 1 LED) and make the next row as shown in the figure below.
    4. <img src=".\Assets\LED_Matrix.jpg" alt="Alt Text" width="700">
3. **Flashing the Code**
    1. **NOTE:** *While flashing the code it is recommended to remove/un-plug all the wires/connections of devices attached (LED, another Arduino, OLED Display) to prevent any damage.*
    2. Flash the code [Game.ino](/Game.ino) in one Arduino UNO and lets number it as 01. This code controls LED Matrix and buttons.
    3. Flash the code [Display.ino](/Display.ino) in other Arduino UNO and lets number it as 02. This code controls OLED Display.
4. **Wire Connections**
    1. Do the wire connections as shown in the figure below. Left Arduino UNO is 01 and right one is 02.
    2. <img src=".\Assets\Tetris.png" alt="Alt Text" width="700"> 
5. **Power Supply Connection**
    1. **If you have a 5V power supply like SMPS (Recommended):** Connect the live wire to to 5V pin and ground wire to GND pin of any Arduino UNO.
    2. **If you have a power supply 9V:** Use power jack of any one Arduino UNO.

## Code Structure

``` Coming Soon ```

## Authors

1. [JAI SINGH MALHOTRA](https://www.linkedin.com/in/jai-singh-malhotra/)
2. [BAIBHAV](https://www.linkedin.com/in/baibhav-mishra-90211a197/)
3. [UJJWAL CHAUDHARY](https://www.linkedin.com/in/ujjwal-chaudhary-4436701aa/)
