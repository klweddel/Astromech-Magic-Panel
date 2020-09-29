Astromech---Magic-Panel
-------------------------------------------------------------------------------
**Sketch for running Astromech Magic Panel using Adafruit 16x8 LED Matrix Backpack and a Arduino Pro Mini**

Re-Worked by: RockHopper (Ken Weddel)  
Original Sketch: IA-PARTS.com

Special Thanks:
- RotoPod
- McWhlr  
- BigHappyDude  
- FlthyMcNsty  

-------------------------------------------------------------------------------
Version 1.0 (11/2017)
-------------------------------------------------------------------------------
**Re-worked IAPARTS sketch to work with Adafruit LED matrix vs. McWhlr magic panel** 

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

**FYI**
Due to the off-the-shelf dimensions of the 16x8 matrix and the CSR specification for the Magic Panel opening size(Panel #5), the top two rows of the matrix are not used since they are not viewable through the opening. Hence those two rows are not used in most of this code so don't think your matrix is broke !!!

Arduino
-------------------------------------------------------------------------------
I used an Arduino micro without headers. By getting it without headers you can only solder  on the headers that are needed which I think helps by preventing miswiring. I then just used some 3M foam tape to attach it to the top of the LED matrix. Clones are fine just make sure it is a 5 volt version.

 - https://www.amazon.com/3M-Double-Sided-Foam-4026-1/dp/B01218RSUS

Mount
-------------------------------------------------------------------------------
The STL for the 3D printed mount can be found here:

 - https://www.thingiverse.com/thing:4610284

You will also need some 0.08" clear acrylic sheet which can be purchased online or Lowes/Home Depot. After cutting the piece to shape, you will need to heat the piece in the oven to mimic the curve of the dome.

 - https://www.lowes.com/pd/DURAPLEX-0-08-in-T-x-18-in-W-x-24-in-L-Clear-Acrylic-Sheet/3143519

And the final item is a fake aluminum sticker. I found this on Amazon:

 - https://www.amazon.com/gp/product/B00F1RUCWI

Wiring
-------------------------------------------------------------------------------
Power comes from a 5V source and needs to be connected to both the Arduino and LED matrix. Using a longer (17mm) pin header bent 90 degrees, you can daisy-chain the connection.
 - https://www.amazon.com/Gikfun-2-54mm-Single-Breakaway-Arduino/dp/B00U8OCENY
(see pictures for more details)

MarcDuino Wiring
-------------------------------------------------------------------------------
The I2C header on the MarcDuino cannot supply enough power to run both Arduino Pro Mini and the LED matrix

Solution: 
- Matrix I2C pin SDA to MarcDuino I2C header pin SDA
- Matrix I2C pin SCL to MarcDuino I2C header pin SCL
- Matrix I2C pin GND to MarcDuino Magic Panel header pin "-"
- Matrix I2C pin +5V to MarcDuino Magic Panel header pin "+"

Patterns
-------------------------------------------------------------------------------
The patterns can be triggered in the following ways:
 
A) Jumper(7 & 8) - All LED ON  
B) Jumper(8 & 9) - Random pattern  
C) MarcDuino Controlled (signals LOW to pin #7 w/ delay to cause a flashing pattern)  
D) I2C Control (via "R2 Touch" through Marcduino)  

I2C commands:
-------------------------------------------------------------------------------
0x00 = Event_AllOff();  
0x01 = Event_AllOn();  
0x02 = Event_Test(1, 150, 1);  
0x03 = Event_Test(2, 150, 1);  
0x04 = Event_TraceUp(1, 200, 5);  
0x05 = Event_TraceUp(2, 200, 5);  
0x06 = Event_TraceDown(1, 200, 5);  
0x07 = Event_TraceDown(2, 200, 5);  
0x08 = Event_TraceRight(1, 200, 5);  
0x09 = Event_TraceRight(2, 200, 5);  
0x10 = Event_TraceLeft(1, 200, 5);  
0x11 = Event_TraceLeft(2, 200, 5);  
0x12 = Event_Expand(1, 300, 5);  
0x13 = Event_Expand(2, 300, 5);  
0x14 = Event_Compress(1, 300, 5);  
0x15 = Event_Compress(2, 300, 5);  
0x16 = Event_Cross(5000);  
0x17 = Event_CylonHorizontal(140, 5);  
0x18 = Event_CylonVertical(140, 5);  
0x19 = Event_EyeScan(100, 5);  
0x20 = Event_Fade(200, 5);  
0x21 = Event_Fade(100, 5);  
0x22 = Event_FlashAll(500, 5);  
0x23 = Event_FlashVertical(500, 5);  
0x24 = Event_FlashQuadrant(200, 5);  
0x25 = Event_TwoLoop(150, 5);  
0x26 = Event_OneLoop(150, 5);  
0x27 = Event_RandomPixel(150, 20);
