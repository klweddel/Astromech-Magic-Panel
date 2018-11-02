// Magic Panel FX by IA-PARTS.com 
//
//// Release History
// v010.5 - Re-added Working I2C and additional display sequences  (FlthyMcNsty 05-21-2014)
// v010 - Remove I2C code and clean up
// v009 - Combine Big Happy Dude functions to v008 + allow for 3 pin binary input
// v008 - Final Code Release May 24 2013 - D Dobyns
// v007 - New Functions for Program Pin Control - D Dobyns
// v006 - Jumper Pin Program - D Dobyns
// v005 - Cleaned code - Production
// v004 - Added JEDI support for JEDI Serial Address 10 decimal 
// v003 - Decode events... next add selectable jumper support.
// v002 - Added default operation & I2C support
// v001 - Initial Demo Sketch
//
//We always have to include the library
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
   #include "WProgram.h"
#endif
#include "LedControl.h"
#include "Wire.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////  Assign IC2 Address Below   //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   byte I2CAdress = 20;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned long time       	= 0;
unsigned long last_time  	= 0;
byte Speed 			= 1;

byte first_time       = 1;	// used for 4-bit (0 2 3 5) input reading - reset all variables
  			        // when 4-bit address value changes (except the first time on power up)
byte DigInState       = 0;
byte lastDigInState;  //= 0;
  
// State Variables

int AlertTime       = 0;
int TraceUpTime     = 0;
int TraceDownTime   = 0;
int TraceRightTime  = 0;
int TraceLeftTime   = 0;
int QuadrantTime    = 0;
int CompressTime    = 0;
int ExpandTime      = 0;
int ToggleTime      = 0;
int RandomPixelTime = 0;

byte TopRow = 0;
byte BotRow = 0;

byte RandomState = 0;
byte RandomMode;     // selected mode

unsigned long RandomTime = 0;
             
//unsigned long RandomInterval = 0;     // time between function calls
unsigned long RandomOnTime = 0;

/*
To load a sketch onto Magic Panel as Arduino Duemilanove w/ ATmega328

 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****

7221
Pin 1 - Data IN
Pin 12 - Load
Pin 13 - CLK
Pin 24 - Data Out

Top 7221 = 0
Bottom 7221 =1

Assign the pins from the 328p to LedControl
 
 pin D8 is connected to the DataIn 
 pin D7 is connected to the CLK 
 pin D6 is connected to LOAD 
 We have two MAX7221 on the Magic Panel, so we use 2.
 */
LedControl lc=LedControl(8,7,6,2);

unsigned long delaytime=30;
unsigned long scrolldelaytime=50;

boolean VMagicPanel[8][8];  // [Row][Col]
unsigned char MagicPanel[16];
int NumLoops=2;

void setup() 
{ 
  Wire.begin(I2CAdress);                   // Start I2C Bus as Master I2C Address
  Wire.onReceive(receiveEvent);            // register event so when we receive something we jump to receiveEvent();
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  lc.shutdown(1,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,15);
  lc.setIntensity(1,15);
  
  /* and clear the display */
  lc.clearDisplay(0);
  lc.clearDisplay(1);

  randomSeed(analogRead(A3));           // Randomizer
  
  // SETUP 6 DIGITAL PINS FOR MANUAL CONTROL
  
  // Jumpe Pins
  pinMode(11, INPUT);             // set pin 11 to input - input 3  
  pinMode(12, OUTPUT);            // set pin PB4 to output - pin 4 - used to allow a jumper from pin 4 to adjacent pin to pull down the adjacent pin
  pinMode(13, INPUT);             // set pin 13 to input - input 5


  digitalWrite(A0, HIGH);         // turn on pullup resistors
  digitalWrite(A1, HIGH);         // turn on pullup resistors
  digitalWrite(A2, HIGH);         // turn on pullup resistors
  
  digitalWrite(11, HIGH);         // turn on pullup resistors
  digitalWrite(13, HIGH);         // turn on pullup resistors

  digitalWrite(12, LOW);         // set pin PC1 to output - pin 1
  ////digitalWrite(A1, LOW);         // set pin PB4 to output - pin 4
}

void loop() { 

  time = millis();		        // get an updated time stamp
  if (time - last_time > Speed)  	// check if time has passed to change states - used to slow down the main loop
  {
    last_time = time;			// reset the timer
    
   //***********************************************
   // These are the Binary Coded Input Pins
   //   
    DigInState = 0;			// read in the 4-bit address line
    if (digitalRead(A0) == LOW)        	// 0
    { DigInState = DigInState + 4; }
    if (digitalRead(A1) == LOW)        	// 1
    { DigInState = DigInState + 2; }
    if (digitalRead(A2) == LOW)       	// 2
    { DigInState = DigInState + 1; }
   
   
   //***********************************************
   // These are the Jumper Pins
   //
    if (digitalRead(11) == LOW)        	// 3
    { DigInState = 8;    }              // if Jumper-1 is Placed - Don't look at Rotary Sw Values
    if (digitalRead(13) == LOW)        	// 5
    { DigInState = 9;    }              // if Jumper-2 is Placed - Don't look at any other Input Values
      
    if(first_time)
    {
      lastDigInState = DigInState;	// dont allow a change in DigInState values if this is the first time through
      first_time = 0;
    }

    if (DigInState != lastDigInState)  	//if DigInState has changed...
    {
       blankPANEL();                    // Clear LED's
    }

    lastDigInState = DigInState;	// Store the current Input Status  
      
    switch (DigInState)			// Call the appropriate code routine - based on the input address values of the control lines A B C
    {
       case 0: 			        // DO NOTHING - ALLOW I2C FUNCTIONS TO OVER-RIDE 6-PIN INPUT
       {
        
        ///////////////////////////////////
        //  Toggle(100);
        //  Alert(100);
        //  TraceUp(1);
        //  TraceDown(1);
        //  Compress(5, 100);
        //  Expand (10, 50);
        //  Random();       
        
         //////////////////////////////////
         
          break;
       }
       case 1: 			        // FADE IN AND OUT: 
       {  allOFF();
          FadeOutIn(1);
          allOFF();
          break;
       }
       case 2: 			        // RANDOM FAST: 
       {  
          allOFF();
          FlashAll(8, 200);
          allOFF();
          break;
       }
       case 3: 			        // 2 LOOP: 
       {  
          allOFF();
          TwoLoop(2);
          allOFF();
          break;
       }
       case 4: 			        // TRACE DOWN: 
       {  
          allOFF();
          TraceDown(5,1);
          allOFF();
          break;
       }
       case 5: 			        // ONE TEST: 
       {  
          allOFF();
          OneTest(30);
          allOFF();
          break;
       }   
       case 6: 			        // Random Fast: 
       {  
          Random(random(8000,14000));
          break;
       }
       case 7: 			        // RANDOM SLOW: 
       {
          Random(random(40000,60000));    // 5 to 8 minute interval between light patterns
          break;
       }
        case 8: 			// Jumper 1  : 
       {  
          allONTimed(0);   
          break;
       }
       case 9: 			        // Jumper 2  : 
       {  
          Random(random(8000,14000));  // 1 to 2 min interval between light patterns
          break;
       }

       default:
       {                                // OFF: 
          allOFF();                     // Clear LED's
          break;
       }
    }//end switch,\

/*6
 loopCount++; 
   if (t > twitchMPtime)
    {
     randomDisplay();                           // call playRandom routine
     loopCount = 0;                           // reset loopCount
     twitchMPtime = random (20,90) * 500;   // set the next twitchMPtime
//     delay (twitchMPtime);
     
  }
 blankPANEL(); 
*/ 
  }// end of if(time)
}//end of loop


void Random(int RandomInterval)
{
 switch (RandomState) 
  {
    case 0:
    {
      RandomMode = random(0,35); // randomly select DisplayMode 0 to 25
      //RandomMode = 25;
        //  0 = Toggle(100);
        //  1 = Alert(100);
        //  2 = TraceUp(1);
        //  3 = TraceDown(1);
        //  4 = Compress(5, 100);
        //  5 = Expand (10, 50);
        //  6 = AllON();
        //  7 = AllOFF();
        //...
        
        
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
 //     RandomInterval = 1500;// 2500;//random(50000, 100000);  // randomly select the interval between function calls - Panel is OFF
//      RandomInterval = random(500, 1000);  // randomly select the interval between function calls - Panel is OFF
      ///
      ///  5000 = apprx 40 sec
      ///  50000 = 400 secs or 6.5 mins
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      RandomOnTime   = random(1000, 1500);// 8 to 12 seconds // ON Time - THIS TIME WILL CHANGE BASED ON THE SEQUENCE CALLED (For State Machine Based Functions)
      RandomTime = 0;     
      RandomState++;         // proceed to next state
      break;
    }
    case 1:
    {
      switch(RandomMode)
      {
        case 0:              //  0 = Turns Panel Off
        {    
          allOFF();
          break;
        }
        case 2:              //  2 = Turns Panel on for 2s
        {
          allOFF();
          allONTimed(2000);
          RandomTime = RandomOnTime + 1;
          allOFF();
          break;  
        }
        case 3:              //  3 = Begins Toggle Sequence: Top and Bottom Half of Panel Alternate
        {
          allOFF();
          Toggle(10);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 4:              // 4 = Begins Alert Sequence (4s):  Panel Rapidly Flashes On & Off
        {
          allOFF();
          Alert(8);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }       
        case 5:              //  5 = Begins Trace Up Sequence (Type 1):  Each row of the MP lights up from bottom to top filling entire panel
        {
          allOFF();
          TraceUp(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 6:              //  6 = Begins Trace Up Sequence (Type 2):  Each row of the MP lights up from bottom to top individually
        {
          allOFF();
          TraceUp(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }        
        case 7:             //  7 = Begins Trace Down Sequence (Type 1):  Each row of the MP lights up from top to bottom filling entire panel
        {
          allOFF();
          TraceDown(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 8:              //  8 = Begins Trace Down Sequence (Type 2):  Each row of the MP lights up from top to bottom individually
        {
          allOFF();
          TraceDown(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 9:              //  9 = Begins Expand Sequence (Type 1): Panel expands from center filling entire panel 
        {
          allOFF();
          Expand(5,1);
          RandomTime = RandomOnTime + 1;  
          allOFF();  
          break;
        }
        case 10:               //  10 = Begins Expand Sequence (Type 2): Ring of pixels expands from center of panel
        {
          allOFF();
          Expand(5,2); 
          RandomTime = RandomOnTime + 1; 
          allOFF();  
          break;
        }
        case 11:              //  11 = Begins Compress Sequence (Type 1): Panel compresses from outer edge filling entire panel 
        {
          allOFF();
          Compress(5,1);
          RandomTime = RandomOnTime + 1;  
          allOFF();  
          break;
        }
        case 12:               //  12 = Begins Compress Sequence (Type 2): Ring of pixels compresses from outer edge of panel
        {
          allOFF();
          Compress(5,2);
          RandomTime = RandomOnTime + 1;  
          allOFF();  
          break;
        }
        case 13:              //  13 = Begins Cross Sequence: Panel is lit to display an X for 3s
        {
          allOFF();
          Cross();
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 14:              //  14 = Begins Cyclon Column Sequence:
        {
          allOFF();
          CylonCol(2, 140);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 15:              //  15 = Begins Cyclon Row Sequence:
        {
          allOFF();
          CylonRow(2, 140);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 16:              // 16 = Begins Eye Scan Sequence:
        {
          EyeScan(2, 100);
          RandomTime = RandomOnTime + 1;     
          allOFF(); 
          break;
        }        
        case 17:             //  17 = Begins Fade Out/In Sequence:
        {
          FadeOutIn(1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 18:             //  18 = Begins Fade Out Sequence:
        {
          FadeOutIn(2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }          
        case 19:              //  19 = Begins Flash Sequence:
        {
          FlashAll(8, 200);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 20:              //  20 = Begins Flash V Sequence:
        {
          FlashV(8, 200);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 21:              //  21 = Begins Flash Q Sequence:  Alternating quadrants of MP flash rapidly
        {
          FlashQ(8, 200);
          RandomTime = RandomOnTime + 1;  
          allOFF();  
          break;
        }
        case 22:              //  22 = Begins Two Loop Sequence:
        {
          allOFF();
          TwoLoop(2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 23:              //  23 = Begins One Loop Sequence:
        {
          allOFF();
          OneLoop(2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 24:              //  24 = Begins Test Sequence (Type 1):
        {
          allOFF();
          TheTest(30);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 25:              //  25 = Begins Test Sequence (Type 2):
        {
          allOFF();
          OneTest(30);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 26:              //  26 = Begins AI Logo Sequence:  Displays the AI Aurebesh characters for 3s that we see all over our awesome packages from Rotopod and McWhlr 
        {
          allOFF();
          Symbol();
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 27:              //  27 = Begins Quadrant Sequence (Type 1):  Each Panel Quadrant lights up individually (TL, TR, BR, BL) 
        {
          allOFF();
          Quadrant(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 28:              //  28 = Begins Quadrant Sequence (Type 2):  Each Panel Quadrant lights up individually (TR, TL, BL, BR) 
        {
          allOFF();
          Quadrant(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 29:              //  29 = Begins Quadrant Sequence (Type 3):  Each Panel Quadrant lights up individually (TR, BR, BL, TL) 
        {
          allOFF();
          Quadrant(5, 3);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 30:              //  30 = Begins Quadrant Sequence (Type 4):  Each Panel Quadrant Rights up individually (TL, BL, BR, TR) 
        {
          allOFF();
          Quadrant(5, 4);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }  
        case 31:              //  31 = Begins Random Pixel Sequence:  Random pixels flashe individually for 6s  
        {
          allOFF();
          RandomPixel(40);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 32:             //  32 = Begins Trace Right Sequence (Type 1):  Each column of the MP lights up from left to right filling entire panel
        {
          allOFF();
          TraceRight(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 33:              //  33 = Begins Trace Right Sequence (Type 2):  Each column of the MP lights up from left to right individually
        {
          allOFF();
          TraceRight(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 34:             //  34 = Begins Trace Left Sequence (Type 1):  Each column of the MP lights up from right to left filling entire panel
        {
          allOFF();
          TraceLeft(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 35:              //  35 = Begins Trace Left Sequence (Type 2):  Each column of the MP lights up from right to left individually
        {
          allOFF();
          TraceLeft(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
               
      }// end of nested switch(RandomMode)  
       if(RandomTime++ > RandomOnTime)
      {
        RandomTime = 0;    // reset timer
        //allOFF();         // turn off All Leds     
        RandomState++;     // proceed to next state
      }        
      break;
    }
    
    //
    case 2:
    {
       allOFF();         // turn off All Leds    
       
       if(RandomTime++ > RandomInterval)        // OFF until timer expires
       {
        RandomTime  = 0;    // reset timer
        RandomState = 0;     // proceed to next state
       }
       break;     
    }     
  }// end switch
}//end Random



////////////////////////////////////////////////////////////////////////////////////////
// BHD Functions
////////////////////////////////////////////////////////////////////////////////////////

void EyeScan(int Repeats, int FlashDelay){
  for(int i=0; i<Repeats; i++)
  {
    for(int j=0; j<8; j++){
      SetRow(j, B11111111);
      MapBoolGrid();
      PrintGrid();
      delay(FlashDelay);
      SetRow(j, B00000000);
    }
    allOFF();
    delay(FlashDelay);
    for(int j=0; j<8; j++){
      SetCol(7-j, B11111111);
      MapBoolGrid();
      PrintGrid();
      delay(FlashDelay);
      SetCol(7-j, B00000000);
    }
    allOFF();
    delay(FlashDelay);
  }
}


void CylonCol(int Repeats, int FlashDelay){
  for(int i=0; i<Repeats; i++){
    for(int j=0; j<8; j++){
      SetCol(j, B11111111);
      MapBoolGrid();
      PrintGrid();
      delay(FlashDelay);
      SetCol(j, B00000000);
    }
    for(int j=0; j<6; j++){
      SetCol(6-j, B11111111);
      MapBoolGrid();
      PrintGrid();
      delay(FlashDelay);
      SetCol(6-j, B00000000);
    }
  }
}

void CylonRow(int Repeats, int FlashDelay){
  for(int i=0; i<Repeats; i++){
    for(int j=0; j<8; j++){
      SetRow(j, B11111111);
      MapBoolGrid();
      PrintGrid();
      delay(FlashDelay);
      SetRow(j, B00000000);
    }
    for(int j=0; j<6; j++){
      SetRow(6-j, B11111111);
      MapBoolGrid();
      PrintGrid();
      delay(FlashDelay);
      SetRow(6-j, B00000000);
    }
  }
}

void FlashH(int Repeats, int FlashDelay){
  for(int i=0; i<Repeats; i++){
    for(int j=0; j<4; j++){
      SetRow(j, B11111111);
      SetRow(j+4, B00000000);
    }
    MapBoolGrid();
    PrintGrid();
    delay(FlashDelay);
    for(int j=0; j<4; j++){
      SetRow(j, B00000000);
      SetRow(j+4, B11111111);
    }
    MapBoolGrid();
    PrintGrid();
    delay(FlashDelay);
  }
}

void FlashV(int Repeats, int FlashDelay){
  for(int i=0; i<Repeats; i++){
    for(int j=0; j<4; j++){
      SetCol(j, B11111111);
      SetCol(j+4, B00000000);
    }
    MapBoolGrid();
    PrintGrid();
    delay(FlashDelay);
    for(int j=0; j<4; j++){
      SetCol(j, B00000000);
      SetCol(j+4, B11111111);
    }
    MapBoolGrid();
    PrintGrid();
    delay(FlashDelay);
  }
}

void FlashQ(int Repeats, int FlashDelay){
  for(int i=0; i<Repeats; i++){
    SetRow(0, B00001111);
    SetRow(1, B00001111);
    SetRow(2, B00001111);
    SetRow(3, B00001111);
    SetRow(4, B11110000);
    SetRow(5, B11110000);
    SetRow(6, B11110000);
    SetRow(7, B11110000);
    MapBoolGrid();
    PrintGrid();
    delay(FlashDelay);
    SetRow(0, B11110000);
    SetRow(1, B11110000);
    SetRow(2, B11110000);
    SetRow(3, B11110000);
    SetRow(4, B00001111);
    SetRow(5, B00001111);
    SetRow(6, B00001111);
    SetRow(7, B00001111);
    MapBoolGrid();
    PrintGrid();
    delay(FlashDelay);
  }
}

void FlashAll(int Repeats, int FlashDelay){
  for(int i=0; i<Repeats; i++){
    allON();
    delay(FlashDelay);
    allOFF();
    delay(FlashDelay);
  }
}

void OneLoop(int Repeats){
  for(int j=0; j<Repeats; j++){
    for(int i=0; i<6; i++){
      VMagicPanel[1][7-(1+i)]=true;
      MapBoolGrid();
      PrintGrid();
      delay(100);
      VMagicPanel[1][7-(1+i)]=false;
    }
    for(int i=0; i<4; i++){
      VMagicPanel[2+i][1]=true;
      MapBoolGrid();
      PrintGrid();
      delay(150);
      VMagicPanel[2+i][1]=false;
    }
    for(int i=0; i<6; i++){
      VMagicPanel[6][(1+i)]=true;
      MapBoolGrid();
      PrintGrid();
      delay(100);
      VMagicPanel[6][(1+i)]=false;
    }
    for(int i=0; i<4; i++){
      VMagicPanel[7-(2+i)][6]=true;
      MapBoolGrid();
      PrintGrid();
      delay(150);
      VMagicPanel[7-(2+i)][6]=false;
    }
  }
}

void TwoLoop(int Repeats){
  for(int j=0; j<Repeats; j++){
    for(int i=0; i<6; i++){
      VMagicPanel[1][7-(1+i)]=true;
      VMagicPanel[6][(1+i)]=true;
      MapBoolGrid();
      PrintGrid();
      delay(100);
      VMagicPanel[1][7-(1+i)]=false;
      VMagicPanel[6][(1+i)]=false;
    }
    for(int i=0; i<4; i++){
      VMagicPanel[2+i][1]=true;
      VMagicPanel[7-(2+i)][6]=true;
      MapBoolGrid();
      PrintGrid();
      delay(150);
      VMagicPanel[2+i][1]=false;
      VMagicPanel[7-(2+i)][6]=false;
    }
    for(int i=0; i<6; i++){
      VMagicPanel[6][(1+i)]=true;
      VMagicPanel[1][7-(1+i)]=true;
      MapBoolGrid();
      PrintGrid();
      delay(100);
      VMagicPanel[6][(1+i)]=false;
      VMagicPanel[1][7-(1+i)]=false;
    }
    for(int i=0; i<4; i++){
      VMagicPanel[7-(2+i)][6]=true;
      VMagicPanel[2+i][1]=true;
      MapBoolGrid();
      PrintGrid();
      delay(150);
      VMagicPanel[7-(2+i)][6]=false;
      VMagicPanel[2+i][1]=false;
    }
  }
}

void FadeOutIn(byte type) {                                // FlthyMcNsty added a variable to this function pass a type value to allow for just a fade out sequence as well
  for(int i=0; i<2*NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)|random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, random(256));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)&random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)&random(256)&random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)&random(256)&random(256)&random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)&random(256)&random(256)&random(256)&random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
 ///// allOFF();
 ///// delay(1000);
 if (type == 1) {
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)&random(256)&random(256)&random(256)&random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)&random(256)&random(256)&random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)&random(256)&random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)&random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, random(256));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
  for(int i=0; i<2*NumLoops; i++){
    for(int i=0; i<16; i++){
      SetRow(i, (random(256)|random(256)));
    }
    MapBoolGrid();
    PrintGrid();
    delay(150);
  }
 }
 ///// allON();
/////  delay(2000);
}

void TheTest(int FlashDelay){
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      VMagicPanel[row][col]=true;
      MapBoolGrid();
      PrintGrid();
      delay(FlashDelay);
    }
  }
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      VMagicPanel[row][col]=false;
      MapBoolGrid();
      PrintGrid();
      delay(delaytime);
    }
  }
}

void OneTest(int FlashDelay){
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      VMagicPanel[row][col]=true;
      MapBoolGrid();
      PrintGrid();
      delay(FlashDelay);
      VMagicPanel[row][col]=false;
    }
  }
}

void Symbol(){
  SetRow(0, B00000100);
  SetRow(1, B00000010);
  SetRow(2, B11111111);
  SetRow(3, B00000000);
  SetRow(4, B11100111);
  SetRow(5, B00100100);
  SetRow(6, B00100100);
  SetRow(7, B01000010);
  MapBoolGrid();
  PrintGrid();
  delay(3000);
}

void Cross(){
  SetRow(0, B00000000);
  SetRow(1, B01000010);
  SetRow(2, B00100100);
  SetRow(3, B00011000);
  SetRow(4, B00011000);
  SetRow(5, B00100100);
  SetRow(6, B01000010);
  SetRow(7, B00000000);
  MapBoolGrid();
  PrintGrid();
  delay(3000);
}

void MapBoolGrid(){
  for(int Row=0; Row<8; Row++){
    MagicPanel[2*Row]=128*VMagicPanel[Row][7]+64*VMagicPanel[Row][6]+32*VMagicPanel[Row][5]+16*VMagicPanel[Row][4];       // 0, 2, 4, 6, 8, 10, 12, 14
    MagicPanel[2*Row+1]=8*VMagicPanel[Row][3]+4*VMagicPanel[Row][2]+2*VMagicPanel[Row][1]+VMagicPanel[Row][0];            // 1, 3, 5, 7, 9, 11, 13, 15
  }
}

void PrintGrid(){
  for(int i=0; i<16; i++){
    if(i<8){
      lc.setRow(0, i, MagicPanel[i]);
    }else{
      lc.setRow(1, i-8, MagicPanel[i]);
    }
  }
}

void SetRow(int LEDRow, unsigned char RowState){
  for(int Col=0; Col<8; Col++){
    VMagicPanel[LEDRow][Col]=((RowState >> Col) & 1);
  }
}

void SetCol(int LEDCol, unsigned char ColState){
  for(int Row=0; Row<8; Row++){
    VMagicPanel[Row][LEDCol]=((ColState >> Row) & 1);
  }
}

void allON() {  //all LEDs ON simple style - Huh how does this work?
  for(int ic=0;ic<1;ic++) {
    for(int row=0;row<8;row++) {
      for(int col=0;col<8;col++) {
        VMagicPanel[row][col]=true;
      }
    }
  }
  MapBoolGrid();
  PrintGrid();
}

void allOFF() {
  for(int ic=0;ic<1;ic++) {
    for(int row=0;row<8;row++) {
      for(int col=0;col<8;col++) {
        VMagicPanel[row][col]=false;
      }
    }
  }
  MapBoolGrid();
  PrintGrid();
}


////////////////////////////////////// end BHD //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
// FlthyMcNsty Functions
////////////////////////////////////////////////////////////////////////////////////////
void allONTimed(int timer) 
{ 
    allOFF();
    for(int row=0;row<8;row++)      
    {
      SetRow(row, B11111111);
    }
    MapBoolGrid();
    PrintGrid();
    if (timer < 1)            // Passing a value of 0 or below turns panel on indefinately (well for 1000s anyway)
    {
      delay(1000000);   
    }
    else {
      delay(timer);          // Otherwise it stays on for the number of ms passed.
      allOFF;
    }     
}

void TraceDown(int timer, byte type)
{
  while (TraceDownTime < timer) {
        for(int row=0;row<8;row++) 
        {
          SetRow(row, B11111111);
          MapBoolGrid();
          PrintGrid();
          delay(200);
          if (type == 2) {
            SetRow(row, B00000000);
            MapBoolGrid();
            PrintGrid();
          }
        } 
        allOFF();
        TraceDownTime++;
  }  
  TraceDownTime=0;  
}

void TraceUp(int timer, byte type)
{
  while (TraceUpTime < timer) {
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B11111111);
          MapBoolGrid();
          PrintGrid();
          delay(200);
          if (type == 2) {
            SetRow(row, B00000000);
            MapBoolGrid();
            PrintGrid();
          }
        }
        allOFF(); 
        TraceUpTime++;
  }  
  TraceUpTime=0;  
}

void TraceLeft(int timer, byte type)
{
  while (TraceLeftTime < timer) {
       if( type== 1 ) {
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00000001);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00000011);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00000111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00001111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00011111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00111111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B01111111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B11111111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
       }  
       else if (type == 2) {
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00000001);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00000010);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00000100);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00001000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00010000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00100000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B01000000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B10000000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
       }
       allOFF(); 
       TraceLeftTime++;
  }  
  TraceLeftTime=0;  
}

void TraceRight(int timer, byte type)
{
  while (TraceRightTime < timer) {
       if( type== 1 ) {
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B10000000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B11000000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B11100000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B11110000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B11111000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B11111100);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B11111110);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B11111111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
       }  
       else if (type == 2) {
                for(int row=7;row>=0;--row) 
        {
          SetRow(row, B10000000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B01000000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00100000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00010000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00001000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00000100);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00000010);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        for(int row=7;row>=0;--row) 
        {
          SetRow(row, B00000001);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
       }
       allOFF(); 
       TraceRightTime++;
  }  
  TraceRightTime=0;  
}

void RandomPixel(int timer) {
  int randRow;
  int randCol;
  while (RandomPixelTime < timer) {
    randRow = random(0,7);
    randCol = random(0,7);
    switch(randCol)
      {
        case 0:              
        {    
          SetRow(randRow, B10000000);
          break;
        }
        case 1:              
        {    
          SetRow(randRow, B01000000);
          break;
        }
        case 2:              
        {    
          SetRow(randRow, B00100000);
          break;
        }
        case 3:              
        {    
          SetRow(randRow, B00010000);
          break;
        }
        case 4:              
        {    
          SetRow(randRow, B00001000);
          break;
        }
        case 5:              
        {    
          SetRow(randRow, B00000100);
          break;
        }
        case 6:              
        {    
          SetRow(randRow, B00000010);
          break;
        }
        case 7:              
        {    
          SetRow(randRow, B00000001);
          break;
        }
      }
    MapBoolGrid();
    PrintGrid();
    delay(150); 
    allOFF();
    RandomPixelTime++;
  }  
  RandomPixelTime=0;  
}
void Quadrant(int timer, byte type)
{
  while (QuadrantTime < timer) {
    if ( type == 1 ) {
        for(int row=0;row<4;row++) {
          SetRow(row, B11110000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200); 
        for(int row=0;row<4;row++) {
          SetRow(row, B00001111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        allOFF();
        for(int row=4;row<8;row++) {
          SetRow(row, B11110000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200); 
        for(int row=4;row<8;row++) {
          SetRow(row, B00001111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);    
    }
    else if ( type == 2 ) {
        for(int row=0;row<4;row++) {
          SetRow(row, B00001111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200); 
        for(int row=0;row<4;row++) {
          SetRow(row, B11110000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        allOFF();
        for(int row=4;row<8;row++) {
          SetRow(row, B00001111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200); 
        for(int row=4;row<8;row++) {
          SetRow(row, B11110000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200); 
    }
    else if ( type == 3 ) {
        for(int row=0;row<4;row++) {
          SetRow(row, B00001111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        allOFF();
        for(int row=4;row<8;row++) {
          SetRow(row, B00001111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        allOFF();
        for(int row=4;row<8;row++) {
          SetRow(row, B11110000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        allOFF();  
        for(int row=0;row<4;row++) {
          SetRow(row, B11110000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
    }
    else if ( type == 4 ) {
        for(int row=0;row<4;row++) {
          SetRow(row, B11110000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        allOFF();
        for(int row=4;row<8;row++) {
          SetRow(row, B11110000);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200); 
        allOFF();
        for(int row=4;row<8;row++) {
          SetRow(row, B00001111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
        allOFF(); 
        for(int row=0;row<4;row++) {
          SetRow(row, B00001111);
        }
        MapBoolGrid();
        PrintGrid();
        delay(200);
    }   
    allOFF();
    QuadrantTime++;
  }  
  QuadrantTime=0;  
}

void Toggle(int timer)
{
  while (ToggleTime < timer) {
        for(int row=0;row<4;row++) 
        {
          SetRow(row, B11111111);
        }
        for(int row=4;row<8;row++) 
        {
          SetRow(row, B00000000);
        } 
        MapBoolGrid();
        PrintGrid();
        delay(500);
        for(int row=0;row<4;row++) 
        {
          SetRow(row, B00000000);
        }
        for(int row=4;row<8;row++) 
        {
          SetRow(row, B11111111);
        } 
        MapBoolGrid();
        PrintGrid();
        delay(500); 
        ToggleTime++;
  }  
  ToggleTime=0;  
}

void Alert(int timer)
{
    while (AlertTime < timer) {
        for(int row=0;row<8;row++) 
        allON();
        delay(250);
        allOFF();
        delay(250); 
        AlertTime++;
  }  
  AlertTime=0;  
}

void Expand(int timer, byte type)
{
    while (ExpandTime < timer) { 
      if(type == 1) {   
        SetRow(0, B00000000);
        SetRow(1, B00000000);
        SetRow(2, B00000000);
        SetRow(3, B00011000);
        SetRow(4, B00011000);
        SetRow(5, B00000000);
        SetRow(6, B00000000);
        SetRow(7, B00000000);  
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B00000000);
        SetRow(1, B00000000);
        SetRow(2, B00111100);
        SetRow(3, B00111100);
        SetRow(4, B00111100);
        SetRow(5, B00111100);
        SetRow(6, B00000000);
        SetRow(7, B00000000);   
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B00000000);
        SetRow(1, B01111110);
        SetRow(2, B01111110);
        SetRow(3, B01111110);
        SetRow(4, B01111110);
        SetRow(5, B01111110);
        SetRow(6, B01111110);
        SetRow(7, B00000000);      
        MapBoolGrid();
        PrintGrid();
        delay(200);
        allON();
        delay(200);
      }
      else if (type == 2) {
        SetRow(0, B00000000);
        SetRow(1, B00000000);
        SetRow(2, B00000000);
        SetRow(3, B00011000);
        SetRow(4, B00011000);
        SetRow(5, B00000000);
        SetRow(6, B00000000);
        SetRow(7, B00000000);      
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B00000000);
        SetRow(1, B00000000);
        SetRow(2, B00111100);
        SetRow(3, B00100100);
        SetRow(4, B00100100);
        SetRow(5, B00111100);
        SetRow(6, B00000000);
        SetRow(7, B00000000);      
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B00000000);
        SetRow(1, B01111110);
        SetRow(2, B01000010);
        SetRow(3, B01000010);
        SetRow(4, B01000010);
        SetRow(5, B01000010);
        SetRow(6, B01111110);
        SetRow(7, B00000000);      
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B11111111);
        SetRow(1, B10000001);
        SetRow(2, B10000001);
        SetRow(3, B10000001);
        SetRow(4, B10000001);
        SetRow(5, B10000001);
        SetRow(6, B10000001);
        SetRow(7, B11111111);      
        MapBoolGrid();
        PrintGrid();
        delay(200);
      }
        allOFF();
        delay(200); 
        ExpandTime++;
  }  
  ExpandTime=0;  
}

void Compress(int timer, byte type)
{
    while (CompressTime < timer) {
      if(type == 1) { 
        allON();
        delay(200);  
        SetRow(0, B00000000);
        SetRow(1, B01111110);
        SetRow(2, B01111110);
        SetRow(3, B01111110);
        SetRow(4, B01111110);
        SetRow(5, B01111110);
        SetRow(6, B01111110);
        SetRow(7, B00000000);      
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B00000000);
        SetRow(1, B00000000);
        SetRow(2, B00111100);
        SetRow(3, B00111100);
        SetRow(4, B00111100);
        SetRow(5, B00111100);
        SetRow(6, B00000000);
        SetRow(7, B00000000);      
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B00000000);
        SetRow(1, B00000000);
        SetRow(2, B00000000);
        SetRow(3, B00011000);
        SetRow(4, B00011000);
        SetRow(5, B00000000);
        SetRow(6, B00000000);
        SetRow(7, B00000000);       
        MapBoolGrid();
        PrintGrid();
        delay(200);
      }
      else if (type == 2) {
        SetRow(0, B11111111);
        SetRow(1, B10000001);
        SetRow(2, B10000001);
        SetRow(3, B10000001);
        SetRow(4, B10000001);
        SetRow(5, B10000001);
        SetRow(6, B10000001);
        SetRow(7, B11111111);         
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B00000000);
        SetRow(1, B01111110);
        SetRow(2, B01000010);
        SetRow(3, B01000010);
        SetRow(4, B01000010);
        SetRow(5, B01000010);
        SetRow(6, B01111110);
        SetRow(7, B00000000);     
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B00000000);
        SetRow(1, B00000000);
        SetRow(2, B00111100);
        SetRow(3, B00100100);
        SetRow(4, B00100100);
        SetRow(5, B00111100);
        SetRow(6, B00000000);
        SetRow(7, B00000000);     
        MapBoolGrid();
        PrintGrid();
        delay(200);
        SetRow(0, B00000000);
        SetRow(1, B00000000);
        SetRow(2, B00000000);
        SetRow(3, B00011000);
        SetRow(4, B00011000);
        SetRow(5, B00000000);
        SetRow(6, B00000000);
        SetRow(7, B00000000);      
        MapBoolGrid();
        PrintGrid();
        delay(200);        
      }
        allOFF();
        delay(200); 
        CompressTime++;
  }  
  CompressTime=0;  
}


void MySymbol(){
  SetRow(0, B00000000);
  SetRow(1, B01111110);
  SetRow(2, B00000010);
  SetRow(3, B01111110);
  SetRow(4, B01000000);
  SetRow(5, B01000000);
  SetRow(6, B01111110);
  SetRow(7, B00000000);
  MapBoolGrid();
  PrintGrid();
  delay(1000);
  SetRow(0, B00000000);
  SetRow(1, B01111110);
  SetRow(2, B01000010);
  SetRow(3, B01000000);
  SetRow(4, B01001110);
  SetRow(5, B01000010);
  SetRow(6, B01111110);
  SetRow(7, B00000000);
  MapBoolGrid();
  PrintGrid();
  delay(1000);
  SetRow(0, B00000000);
  SetRow(1, B01000010);
  SetRow(2, B01000010);
  SetRow(3, B01000010);
  SetRow(4, B01011010);
  SetRow(5, B01100110);
  SetRow(6, B01000010);
  SetRow(7, B00000000);
  MapBoolGrid();
  PrintGrid();
  delay(1000);
  SetRow(0, B00000000);
  SetRow(1, B01111100);
  SetRow(2, B01000010);
  SetRow(3, B01000010);
  SetRow(4, B01000010);
  SetRow(5, B01000010);
  SetRow(6, B01111100);
  SetRow(7, B00000000);
  MapBoolGrid();
  PrintGrid();
  delay(1000);
}

//////
//////////////////////////////// end FlthyMcNsty //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void blankPANEL() {
  lc.clearDisplay(0);
  lc.clearDisplay(1);
}

// function that executes whenever data is received from an I2C master
// this function is registered as an event, see setup()
void receiveEvent(int eventCode) {
  RandomOnTime   = random(1000, 1500);// 8 to 12 seconds // ON Time - THIS TIME WILL CHANGE BASED ON THE SEQUENCE CALLED (For State Machine Based Functions)
  RandomTime = 0;
  int i2cEvent = Wire.read();
  int event = floor(i2cEvent/1);
  sei();
  switch (event) {
        case 0:              //  0 = Turns Panel Off
        {    
          allOFF();
          break;
        }
        case 1:              //  1 = Turns Panel On Indefinately
        {
          allONTimed(0);
          RandomTime = RandomOnTime + 1;  // force time to expire.
          break;
        }
        case 2:              //  2 = Turns Panel on for 2s
        {
          allONTimed(2000);
          RandomTime = RandomOnTime + 1;  
        }
        case 3:              //  3 = Turns Panel on for 5s
        {
          allONTimed(5000);
          RandomTime = RandomOnTime + 1; 
          break;
        }
        case 4:              //  4 = Turns Panel on for 10s
        {
          allONTimed(10000);
          RandomTime = RandomOnTime + 1; 
          break;
        }
        case 5:              //  5 = Begins Toggle Sequence: Top and Bottom Half of Panel Alternate
        {
          allOFF();
          Toggle(10);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 6:              // 6 = Begins Alert Sequence (4s):  Panel Rapidly Flashes On & Off
        {
          allOFF();
          Alert(8);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 7:              // 7 = Begins Alert Sequence (10s):  Panel Rapidly Flashes On & Off
        {
          allOFF();
          Alert(20);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }         
        case 8:              //  8 = Begins Trace Up Sequence (Type 1):  Each row of the MP lights up from bottom to top filling entire panel
        {
          allOFF();
          TraceUp(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 9:              //  9 = Begins Trace Up Sequence (Type 2):  Each row of the MP lights up from bottom to top individually
        {
          allOFF();
          TraceUp(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }        
        case 10:             //  10 = Begins Trace Down Sequence (Type 1):  Each row of the MP lights up from top to bottom filling entire panel
        {
          allOFF();
          TraceDown(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 11:              //  11 = Begins Trace Down Sequence (Type 2):  Each row of the MP lights up from top to bottom individually
        {
          allOFF();
          TraceDown(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 12:             //  12 = Begins Trace Right Sequence (Type 1):  Each column of the MP lights up from left to right filling entire panel
        {
          allOFF();
          TraceRight(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 13:              //  13 = Begins Trace Right Sequence (Type 2):  Each column of the MP lights up from left to right individually
        {
          allOFF();
          TraceRight(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 14:             //  14 = Begins Trace Left Sequence (Type 1):  Each column of the MP lights up from right to left filling entire panel
        {
          allOFF();
          TraceLeft(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 15:              //  15 = Begins Trace Left Sequence (Type 2):  Each column of the MP lights up from right to left individually
        {
          allOFF();
          TraceLeft(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 16:              //  16 = Begins Expand Sequence (Type 1): Panel expands from center filling entire panel 
        {
          allOFF();
          Expand(5,1);
          RandomTime = RandomOnTime + 1;  
          allOFF();  
          break;
        }
        case 17:               //  17 = Begins Expand Sequence (Type 2): Ring of pixels expands from center of panel
        {
          allOFF();
          Expand(5,2);
          RandomTime = RandomOnTime + 1;  
          allOFF();  
          break;
        }
        case 18:              //  18 = Begins Compress Sequence (Type 1): Panel compresses from outer edge filling entire panel 
        {
          allOFF();
          Compress(5,1); 
          RandomTime = RandomOnTime + 1; 
          allOFF();  
          break;
        }
        case 19:               //  19 = Begins Compress Sequence (Type 2): Ring of pixels compresses from outer edge of panel
        {
          allOFF();
          Compress(5,2);
          RandomTime = RandomOnTime + 1;  
          allOFF();  
          break;
        }


        case 20:              //  20 = Begins Cross Sequence: Panel is lit to display an X for 3s
        {
          allOFF();
          Cross();
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 21:              //  21 = Begins Cyclon Column Sequence: Each column illuminated one at a time from left to right back to left. (like the Cylons from Battlestar Galactica)
        {
          allOFF();
          CylonCol(2, 140);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 22:              //  22 = Begins Cyclon Row Sequence: Each row illuminated one at a time from top to bottom back to top. (like the Cylons from Battlestar Galactica)
        {
          allOFF();
          CylonRow(2, 140);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 23:              // 23 = Begins Eye Scan Sequence:  Each row is illuminated from top to bottom followed by each column left to right. (like the eye ball scanners in the Mission Impossible movies)
        {
          EyeScan(2, 100);
          RandomTime = RandomOnTime + 1;     
          allOFF(); 
          break;
        }        
        case 24:             //  24 = Begins Fade Out/In Sequence:  MP gradually and randomly fades out and then fades back in the same manner. 
        {
          FadeOutIn(1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 25:             //  25 = Begins Fade Out Sequence:  MP gradually and randomly fades out (Similar to the Short Circuit Sequence on Teeces). 
        {
          FadeOutIn(2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }           
        case 26:              //  26 = Begins Flash Sequence:  MP flashes rapidly for 5 seconds (Similar to Alarm Sequence)
        {
          FlashAll(8, 200);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 27:              //  27 = Begins Flash V Sequence: Left and Right Half of Panel Alternate
        {
          FlashV(8, 200);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 28:              //  28 = Begins Flash Q Sequence:  Alternating quadrants of MP flash rapidly
        {
          FlashQ(8, 200);
          RandomTime = RandomOnTime + 1;  
          allOFF();  
          break;
        }
        case 29:              //  29 = Begins Two Loop Sequence: Dual pixels are lit opposite each other completeting a loop around the 2nd ring from panel edge.
        {
          allOFF();
          TwoLoop(2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 30:              //  30 = Begins One Loop Sequence: A single pixel is lit individually completeting a loop around the 2nd ring from panel edge.
        {
          allOFF();
          OneLoop(2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 31:              //  31 = Begins Test Sequence (Type 1):  Each pixel of the MP is lit sequentially from row 0, column 7 to row 7, column 0 until panel is filled, then unlit in the same order.
        {
          allOFF();
          TheTest(30);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 32:              //  32 = Begins Test Sequence (Type 2):  Each pixel of the MP is lit indivually from row 0, column 7 to row 7, column 0. 
        {
          allOFF();
          OneTest(30);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 33:              //  33 = Begins AI Logo Sequence:  Displays the AI Aurebesh characters for 3s (...that we see all over our awesome packages from Rotopod and McWhlr) 
        {
          allOFF();
          Symbol();
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 34:              //  34 = Begins 2GWD Logo Sequence: Displays the Characters 2-G-W-D sequentially every 1s (...shameless, I know.) 
        {
          allOFF();
          MySymbol();
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 35:              //  35 = Begins Quadrant Sequence (Type 1):  Each Panel Quadrant lights up individually (TL, TR, BR, BL) 
        {
          allOFF();
          Quadrant(5, 1);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }
        case 36:              //  36 = Begins Quadrant Sequence (Type 2):  Each Panel Quadrant lights up individually (TR, TL, BL, BR) 
        {
          allOFF();
          Quadrant(5, 2);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 37:              //  37 = Begins Quadrant Sequence (Type 3):  Each Panel Quadrant lights up individually (TR, BR, BL, TL) 
        {
          allOFF();
          Quadrant(5, 3);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        } 
        case 38:              //  38 = Begins Quadrant Sequence (Type 4):  Each Panel Quadrant lights up individually (TL, BL, BR, TR) 
        {
          allOFF();
          Quadrant(5, 4);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }  
        case 39:              //  39 = Begins Random Pixel Sequence:  Random pixels flashe individually for 6s  
        {
          allOFF();
          RandomPixel(40);
          RandomTime = RandomOnTime + 1;  
          allOFF();
          break;
        }    
     }    
}
