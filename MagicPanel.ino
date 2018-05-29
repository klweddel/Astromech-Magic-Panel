/******************************************************************************

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

*****************************************************************************/

/******************************************************************************
Library Includes
******************************************************************************/
#include <Wire.h>                 // Arduino I2C Library                - https://www.arduino.cc/en/Reference/Wire
#include <Adafruit_GFX.h>         // Adafruit LED Control Library       - https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_LEDBackpack.h> // Adafruit LED backpack(HT16K33) IDE - https://github.com/adafruit/Adafruit_LED_Backpack

/******************************************************************************
Configuration Parameters
******************************************************************************/
uint8_t slaveI2CAddress = 0x20;
uint8_t matrixI2CAddress = 0x70;  // 0x70, 0x71, 0x72 or 0x73 - https://learn.adafruit.com/adafruit-led-backpack/changing-i2c-address
uint8_t brightness = 0x15;        // 0x01-0x15

/******************************************************************************
Global Declarations
******************************************************************************/
Adafruit_8x16matrix matrix = Adafruit_8x16matrix();
int currentEventNumber = 0;


/******************************************************************************
Arduino Core Methods
******************************************************************************/
void setup() {
  String startupText = "RockHopper v1.0";

  // Open serial port & set data rate
  Serial.begin(9600);
  Serial.println(startupText);

  // Start I2C Bus as a slave
  Wire.begin(slaveI2CAddress);

  // Register event to listen to I2C commands
  Wire.onReceive(ReceiveEvent);

  // Initialize matrix
  matrix.begin(matrixI2CAddress);
  matrix.setBrightness(brightness);

  // Initialize random generator
  randomSeed(analogRead(A3));

  // Jumper Pins
  pinMode(7, INPUT_PULLUP);
  pinMode(8, OUTPUT);      // - used to allow a jumper from pin 7 or 9
  pinMode(9, INPUT_PULLUP);

  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);

  Event_HorizontalText(startupText, 100, 1);
}

void loop() {
  // Check pin jumpers
  if (digitalRead(7) == LOW) {
    if (currentEventNumber != 1) {
      // Turn-On all LEDs
      currentEventNumber = 1;
      Event_AllOn();
    }
  }
  else if (digitalRead(9) == LOW) {
    // Random event
    DisplayRandomEvent();
  }
  else if (currentEventNumber != 0) {
    // Clear previous event
    currentEventNumber = 0;
    Event_AllOff();
  }
}


/******************************************************************************
Event Selector Methods
******************************************************************************/
// I2C event handler
void ReceiveEvent(int numBytesRead) {
  int eventNumber = Wire.read();
  //sei(); // Interrupt any current event

  switch (eventNumber) {
  case 0x00:
    Event_AllOff();
    break;
  case 0x01:
    Event_AllOn();
    break;
  case 0x02:
    Event_Test(1, 150, 1);
    break;
  case 0x03:
    Event_Test(2, 150, 1);
    break;
  case 0x04:
    Event_TraceUp(1, 200, 5);
    break;
  case 0x05:
    Event_TraceUp(2, 200, 5);
    break;
  case 0x06:
    Event_TraceDown(1, 200, 5);
    break;
  case 0x07:
    Event_TraceDown(2, 200, 5);
    break;
  case 0x08:
    Event_TraceRight(1, 200, 5);
    break;
  case 0x09:
    Event_TraceRight(2, 200, 5);
    break;
  case 0x10:
    Event_TraceLeft(1, 200, 5);
    break;
  case 0x11:
    Event_TraceLeft(2, 200, 5);
    break;
  case 0x12:
    Event_Expand(1, 300, 5);
    break;
  case 0x13:
    Event_Expand(2, 300, 5);
    break;
  case 0x14:
    Event_Compress(1, 300, 5);
    break;
  case 0x15:
    Event_Compress(2, 300, 5);
    break;
  case 0x16:
    Event_Cross(5000);
    break;
  case 0x17:
    Event_CylonHorizontal(140, 5);
    break;
  case 0x18:
    Event_CylonVertical(140, 5);
    break;
  case 0x19:
    Event_EyeScan(100, 5);
    break;
  case 0x20:
    Event_Fade(200, 5);
    break;
  case 0x21:
    Event_Fade(100, 5);
    break;
  case 0x22:
    Event_FlashAll(500, 5);
    break;
  case 0x23:
    Event_FlashVertical(500, 5);
    break;
  case 0x24:
    Event_FlashQuadrant(200, 5);
    break;
  case 0x25:
    Event_TwoLoop(150, 5);
    break;
  case 0x26:
    Event_OneLoop(150, 5);
    break;
  case 0x27:
    Event_RandomPixel(150, 20);
    break;
  }
}

// Displays a random event for a random time then waits a random delay before continuing
void DisplayRandomEvent() {
  // Determine some random values
  int randomEventNumber = random(0, 24);
  int randomPostDelay = random(8000, 14000);

  Serial.print("Random Event - ");
  Serial.print(randomEventNumber);
  Serial.print(" - ");
  Serial.println(randomPostDelay);

  switch (randomEventNumber) {
  case 0:
    break;
  case 1:
    Event_TraceUp(1, 200, 5);
    break;
  case 2:
    Event_TraceUp(2, 200, 5);
    break;
  case 3:
    Event_TraceDown(1, 200, 5);
    break;
  case 4:
    Event_TraceDown(2, 200, 5);
    break;
  case 5:
    Event_TraceRight(1, 200, 5);
    break;
  case 6:
    Event_TraceRight(2, 200, 5);
    break;
  case 7:
    Event_TraceLeft(1, 200, 5);
    break;
  case 8:
    Event_TraceLeft(2, 200, 5);
    break;
  case 9:
    Event_Expand(1, 300, 5);
    break;
  case 10:
    Event_Expand(2, 300, 5);
    break;
  case 11:
    Event_Compress(1, 300, 5);
    break;
  case 12:
    Event_Compress(2, 300, 5);
    break;
  case 13:
    Event_Cross(5000);
    break;
  case 14:
    Event_CylonHorizontal(140, 5);
    break;
  case 15:
    Event_CylonVertical(140, 5);
    break;
  case 16:
    Event_EyeScan(100, 5);
    break;
  case 17:
    Event_Fade(100, 5);
    break;
  case 18:
    Event_Fade(200, 5);
    break;
  case 19:
    Event_FlashAll(8, 5);
    break;
  case 20:
    Event_FlashVertical(500, 5);
    break;
  case 21:
    Event_FlashQuadrant(500, 5);
    break;
  case 22:
    Event_TwoLoop(150, 5);
    break;
  case 23:
    Event_OneLoop(150, 5);
    break;
  case 24:
    Event_RandomPixel(150, 20);
    break;
  }

  // Delay between random events
  delay(randomPostDelay);
}


/******************************************************************************
Event Display Methods
******************************************************************************/
// Turns all LEDs off
void Event_AllOff() {
  Serial.println("Event - All Off");

  matrix.clear();
  matrix.writeDisplay();
}

// Turns all LEDs on
void Event_AllOn() {
  Serial.println("Event - All On");

  matrix.clear();
  matrix.fillRect(0, 2, 8, 14, LED_ON);
  matrix.writeDisplay();
}

// Each pixel is lit sequentially and then unlit based on pattern flag
void Event_Test(int patternFlag, int speedDelay, int iterations) {
  Serial.println("Event - Test");

  matrix.clear();
  for (int8_t i = 1; i <= iterations; i++) {
    for (int8_t y = 0; y <= 15; y++) {
      for (int8_t x = 0; x <= 7; x++) {
        if (patternFlag == 1) {
          matrix.clear();
        }
        matrix.drawPixel(x, y, LED_ON);
        matrix.writeDisplay();
        delay(speedDelay);
      }
    }
    for (int8_t y = 15; y >= 0; y--) {
      for (int8_t x = 7; x >= 0; x--) {
        if (patternFlag == 1) {
          matrix.clear();
          matrix.drawPixel(x, y, LED_ON);
        }
        else {
          matrix.drawPixel(x, y, LED_OFF);
        }
        matrix.writeDisplay();
        delay(speedDelay);
      }
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Scrolls text along long side
void Event_HorizontalText(String textToScroll, int speedDelay, int iterations) {
  Serial.println("Event - Horizontal Text");

  matrix.clear();
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);

  for (int8_t i = 1; i <= iterations; i++) {
    for (int8_t x = 0; x >= -100; x--) {
      matrix.clear();
      matrix.setCursor(x, 6);
      matrix.print(textToScroll);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Scrolls text along short side
void Event_VerticalText(String textToScroll, int speedDelay, int iterations) {
  Serial.println("Event - Vertical Text");

  matrix.clear();
  matrix.setRotation(1); // Rotate text
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);

  for (int8_t i = 1; i <= iterations; i++) {
    for (int8_t x = 7; x >= -36; x--) {
      matrix.clear();
      matrix.setCursor(x, 0);
      matrix.print(textToScroll);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.setRotation(0); // Reset text rotation
  matrix.clear();
  matrix.writeDisplay();
}

// Each row is illuminated from top to bottom followed by each column left to right
void Event_EyeScan(int speedDelay, int iterations) {
  Serial.println("Event - Eye Scan");

  for (int8_t i = 1; i <= iterations; i++) {
    for (int8_t y = 0; y <= 15; y++) {
      matrix.clear();
      matrix.drawLine(0, y, 7, y, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }

    for (int8_t x = 0; x <= 7; x++) {
      matrix.clear();
      matrix.drawLine(x, 0, x, 15, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Each column illuminated one at a time from left to right back to left
void Event_CylonHorizontal(int speedDelay, int iterations) {
  Serial.println("Event - Cylon Horizontal");

  for (int8_t i = 1; i <= iterations; i++) {
    for (int8_t x = 0; x <= 7; x++) {
      matrix.clear();
      matrix.drawLine(x, 2, x, 15, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
    for (int8_t x = 7; x >= 0; x--) {
      matrix.clear();
      matrix.drawLine(x, 2, x, 15, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Each row illuminated one at a time from top to bottom back to top
void Event_CylonVertical(int speedDelay, int iterations) {
  Serial.println("Event - Cylon Vertical");

  for (int8_t i = 1; i <= iterations; i++) {
    for (int8_t y = 2; y <= 15; y++) {
      matrix.clear();
      matrix.drawLine(0, y, 7, y, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
    for (int8_t y = 15; y >= 2; y--) {
      matrix.clear();
      matrix.drawLine(0, y, 7, y, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// All LEDs flash
void Event_FlashAll(int speedDelay, int iterations) {
  Serial.println("Event - Flash All");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.fillRect(0, 2, 8, 14, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    matrix.clear();
    matrix.writeDisplay();
    delay(speedDelay);
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Top and bottom half alternate
void Event_FlashHorizontal(int speedDelay, int iterations) {
  Serial.println("Event - Flash Horizontal");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    matrix.fillRect(0, 2, 8, 7, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    matrix.clear();
    matrix.fillRect(0, 9, 8, 7, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Left and right half alternate
void Event_FlashVertical(int speedDelay, int iterations) {
  Serial.println("Event - Flash Vertical");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    matrix.fillRect(0, 2, 4, 16, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    matrix.clear();
    matrix.fillRect(4, 2, 4, 16, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Alternating quadrants flash
void Event_FlashQuadrant(int speedDelay, int iterations) {
  Serial.println("Event - Flash Quadrant");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    matrix.fillRect(0, 2, 4, 7, LED_ON);
    matrix.fillRect(4, 9, 4, 7, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    matrix.clear();
    matrix.fillRect(4, 2, 4, 7, LED_ON);
    matrix.fillRect(0, 9, 4, 7, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
  }

  matrix.clear();
  matrix.writeDisplay();
}

// A single pixel is lit individually completing a loop around the 2nd ring from panel edge
void Event_OneLoop(int speedDelay, int iterations) {
  Serial.println("Event - One Loop");

  for (int8_t i = 1; i <= iterations; i++) {
    int8_t x = 1;
    int8_t y = 3;
    for (x = 1; x <= 6; x++) {
      matrix.clear();
      matrix.drawPixel(x, y, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
    x = 6;
    for (y = 4; y <= 14; y++) {
      matrix.clear();
      matrix.drawPixel(x, y, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
    y = 14;
    for (x = 5; x >= 1; x--) {
      matrix.clear();
      matrix.drawPixel(x, y, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
    x = 1;
    for (y = 13; y >= 4; y--) {
      matrix.clear();
      matrix.drawPixel(x, y, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Dual pixels are lit opposite each other completing a loop around the 2nd ring from panel edge
void Event_TwoLoop(int speedDelay, int iterations) {
  Serial.println("Event - Two Loop");

  for (int8_t i = 1; i <= iterations; i++) {
    int8_t x1 = 1;
    int8_t y1 = 3;
    int8_t x2 = 6;
    int8_t y2 = 14;
    for (x1 = 1; x1 <= 6; x1++) {
      Serial.print("(");
      Serial.print(x2);
      Serial.print(",");
      Serial.print(y2);
      Serial.println(")");

      matrix.clear();
      matrix.drawPixel(x1, y1, LED_ON);
      matrix.drawPixel(x2, y2, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
      x2--;
    }
    x1 = 6;
    x2 = 1;
    y2 = 13;
    for (y1 = 4; y1 <= 14; y1++) {
      Serial.print("(");
      Serial.print(x2);
      Serial.print(",");
      Serial.print(y2);
      Serial.println(")");

      matrix.clear();
      matrix.drawPixel(x1, y1, LED_ON);
      matrix.drawPixel(x2, y2, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
      y2--;
    }
    y1 = 14;
    x2 = 2;
    y2 = 3;
    for (x1 = 5; x1 >= 1; x1--) {
      Serial.print("(");
      Serial.print(x2);
      Serial.print(",");
      Serial.print(y2);
      Serial.println(")");

      matrix.clear();
      matrix.drawPixel(x1, y1, LED_ON);
      matrix.drawPixel(x2, y2, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
      x2++;
    }
    x1 = 1;
    x2 = 6;
    y2 = 4;
    for (y1 = 13; y1 >= 4; y1--) {
      Serial.print("(");
      Serial.print(x2);
      Serial.print(",");
      Serial.print(y2);
      Serial.println(")");

      matrix.clear();
      matrix.drawPixel(x1, y1, LED_ON);
      matrix.drawPixel(x2, y2, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
      y2++;
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Gradually and randomly fades out and then fades back in 
void Event_Fade(int speedDelay, int iterations) {
  Serial.println("Event - Fade");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    matrix.fillRect(0, 2, 8, 14, LED_ON);
    matrix.writeDisplay();

    for (int8_t b = 0; b <= 15; b++) {
      matrix.setBrightness(b);
      delay(speedDelay);
    }
    for (int8_t b = 15; b >= 0; b--) {
      matrix.setBrightness(b);
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
  matrix.setBrightness(brightness);
}

// Displays an X
void Event_Cross(int speedDelay) {
  Serial.println("Event - Cross");

  matrix.clear();
  matrix.drawLine(0, 2, 7, 15, LED_ON);
  matrix.drawLine(0, 15, 7, 2, LED_ON);
  matrix.writeDisplay();
  delay(speedDelay);

  matrix.clear();
  matrix.writeDisplay();
}

// Rows light up from top to bottom
void Event_TraceDown(int patternFlag, int speedDelay, int iterations) {
  Serial.println("Event - Trace Down");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    for (int8_t y = 2; y <= 15; y++) {
      if (patternFlag == 1) {
        matrix.clear();
      }
      matrix.drawLine(0, y, 7, y, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Rows light up from bottom to top
void Event_TraceUp(int patternFlag, int speedDelay, int iterations) {
  Serial.println("Event - Trace Up");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    for (int8_t y = 15; y >= 2; y--) {
      if (patternFlag == 1) {
        matrix.clear();
      }
      matrix.drawLine(0, y, 7, y, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Rows light up from right to left
void Event_TraceLeft(int patternFlag, int speedDelay, int iterations) {
  Serial.println("Event - Trace Left");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    for (int8_t x = 0; x <= 7; x++) {
      if (patternFlag == 1) {
        matrix.clear();
      }
      matrix.drawLine(x, 2, x, 15, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Rows light up from left to right
void Event_TraceRight(int patternFlag, int speedDelay, int iterations) {
  Serial.println("Event - Trace-Right");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    for (int8_t x = 7; x >= 0; x--) {
      if (patternFlag == 1) {
        matrix.clear();
      }
      matrix.drawLine(x, 2, x, 15, LED_ON);
      matrix.writeDisplay();
      delay(speedDelay);
    }
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Random pixels flash individually
void Event_RandomPixel(int speedDelay, int iterations) {
  Serial.println("Event - Random Pixel");

  int randomPixelTime = 0;

  int x;
  int y;
  while (randomPixelTime < iterations) {
    x = random(0, 8);
    y = random(2, 16);

    matrix.clear();
    matrix.drawPixel(x, y, LED_ON);
    matrix.writeDisplay();

    randomPixelTime++;
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Expands from center filling entire panel 
void Event_Expand(int patternFlag, int speedDelay, int iterations) {
  Serial.println("Event - Expand");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    matrix.drawRect(3, 7, 2, 4, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    if (patternFlag == 1) {
      matrix.clear();
    }
    matrix.drawRect(2, 6, 4, 6, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    if (patternFlag == 1) {
      matrix.clear();
    }
    matrix.drawRect(1, 5, 6, 8, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    if (patternFlag == 1) {
      matrix.clear();
    }
    matrix.drawRect(0, 4, 8, 10, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);

    matrix.clear();
    matrix.writeDisplay();
    delay(speedDelay);
  }

  matrix.clear();
  matrix.writeDisplay();
}

// Ring of pixels compresses from outer edge of panel
void Event_Compress(int patternFlag, int speedDelay, int iterations) {
  Serial.println("Event - Compress");

  for (int8_t i = 1; i <= iterations; i++) {
    matrix.clear();
    matrix.drawRect(0, 4, 8, 10, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    if (patternFlag == 1) {
      matrix.clear();
    }
    matrix.drawRect(1, 5, 6, 8, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    if (patternFlag == 1) {
      matrix.clear();
    }
    matrix.drawRect(2, 6, 4, 6, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);
    if (patternFlag == 1) {
      matrix.clear();
    }
    matrix.drawRect(3, 7, 2, 4, LED_ON);
    matrix.writeDisplay();
    delay(speedDelay);

    matrix.clear();
    matrix.writeDisplay();
    delay(speedDelay);
  }

  matrix.clear();
  matrix.writeDisplay();
}
