# Astromech---Magic-Panel

Sketch for running Astromech Magic Panel using Adafruit 16x8 LED Matrix Backpack

Re-Worked by: RockHopper (Ken Weddel)
Original Sketch: IA-PARTS.com

Special Thanks:
  RotoPod
  McWhlr
  BigHappyDude
  FlthyMcNsty

-------------------------------------------------------------------------------
Version 1.0 (11/2017)
Re-worked IAPARTS sketch to work with Adafruit LED matrix vs. McWhlr magic panel
-------------------------------------------------------------------------------

Designed specifically to work with the following Adafruit 16x8 LED Matrix backpacks
- http://www.adafruit.com/products/2035 - Round  - Green
- http://www.adafruit.com/products/2036 - Round  - YellowGreen
- http://www.adafruit.com/products/2037 - Round  - Red
- http://www.adafruit.com/products/2038 - Round  - White
- http://www.adafruit.com/products/2039 - Round  - Blue
- http://www.adafruit.com/products/2040 - Square - Blue
- http://www.adafruit.com/products/2041 - Square - Amber
- http://www.adafruit.com/products/2042 - Square - Green
- http://www.adafruit.com/products/2043 - Square - Yellow
- http://www.adafruit.com/products/2044 - Square - White
- http://www.adafruit.com/products/2052 - Round  - Orange

FYI
Due to the off-the-shelf dimensions of the 16x8 matrix and the CSR specification for the Magic Panel opening size(Panel #5), the top two rows of the matrix are not used since they are not viewable through the opening.
Hence those two rows are not used in most of this code so don't think your matrix is broke !!!

Wiring
- Power comes from 5V source and needs to be connected to both the Arduino and LED matrix. Using a longer (17mm) pin header bent 90 degrees, you can daisy-chain the connection. (see pictures)
- The patterns can be triggered in the following ways:

A) Jumper(7 & 8) - All LED ON

B) Jumper(8 & 9) - Random pattern

C) Marcduino Controlled (signals LOW to pin #7 w/ delay to cause a flashing pattern)

D) I2C Control (via "R2 Touch" through Marcduino)
