// Code for the upcoming hand test with multiple EMGs and connected servos
// Written by Xan Lorimer (xanlorimer@gmail.com)

// This code is published under the GNU GPL v2 license.

//////////////////////////////////////////////
// THIS CODE IS FULLY TESTED AS OF 5/12/15. //
//////////////////////////////////////////////

#include <Servo.h>
#include "Timer.h"

boolean flag = true;
boolean threshFlag = true;
boolean thresholdSwapFlag = true; // This variable MUST be initialized as TRUE in order for the thresholds to be set during setup(). Used to swap threshold flags.
int capA = 512; // "Out of bounds" value for MSK A. 
int capB = 512; // Ditto, B
int capC = 512; // Ditto, C

// As usual, we'll begin by declaring variables
int loopHaltAmount = 100000; // Number of loops before we stop the test. How many data points do we want to gather?
int numberOfLoops = 0; // Initialize the number of loops so that we can check for and use a stopping point.
int loopDelay = 50; // Amount of time (in ms) for the main loop to wait.
int waveDelay = 1000; // Amount of time (in ms) between each finger moving in any wave.
int fingerOpenDelay = 500; // Amount of time (in ms) that it takes for the slowest finger to open.
int fingerShutDelay = 500; // Amount of time (in ms) that it takes for the slowest finger to shut.

// Stuff for averaging:
int emgAvgRead; // Just initializing the variable where the average is stored
int thresholdA,thresholdB,thresholdC;
int thresholdAStore = 50; // Threshold for EMG A
int thresholdBStore = 50; // Threshold for EMG B
int thresholdCStore = 100; // Stored threshold for EMG C
int averagerDelay = 20; // How long we wait before taking each average point
int emgAvgReadCount = 5; // Number of points we collect to take the average
int thresholdModifier = 5; // Modifier for preliminary threshold activation

int fingerControlVal; // Initialze the angle that we'll use for individual finger controls.

int emgA,emgB,emgC; // Where EMG data will be stored
int shutAngle[] = {35,15,15,40,5}; //{5,35,15,15,30}; // Angle when "shut" - From pinkie to thumb.
int openAngle[] = {135,130,150,135,160}; //{160,140,180,180,140}; // Angle when "open" - From pinkie to thumb.

char inChar;
String inputString;

// Declare all of the servos
Servo servo0; // Pinkie
Servo servo1;
Servo servo2; // ...
Servo servo3;
Servo servo4; // Thumb

Timer t;

// Setup method
void setup()
{   
  thresholdSwap(thresholdSwapFlag);
  
  // Declare the EMG inputs
  pinMode(0, INPUT); // Input on analog pin 0
  pinMode(1, INPUT); // Input on analog pin 1
  pinMode(2, INPUT); // Input on analog pin 2

  // Declare the servo outputs
  servo0.attach(10); // Pinkie <3>
  servo1.attach(9); // <5>
  servo2.attach(6); // ...
  servo3.attach(5); // <9>
  servo4.attach(3); // Thumb <10>

  // Begin the serial connection for debug
  Serial.begin(9600); 
  openHandInstant();
}

// Main loop
void loop()
{
    t.update();
    
    // Each time we loop, we'll check the EMG values and evaluate them. Based on those values, we can execute an action of some sort.
    emgCheck();
    
    if(Serial.read() == 'x') // We know it's a command if an x precedes it
    {
      Serial.println("Received command character.");
      //Serial.flush();
      commandHandler(getSerial()); // Call the command method with whatever's in the serial buffer.
    }
    
    delay(loopDelay); // And let's have a slight delay so as not to overload the Arduino
}

// This method accepts serial data.
String getSerial() // Get new serial data if there is any
{
  if (Serial.available()) // If there's a character in the buffer 
  {
    inChar = (char)Serial.read(); // Read the character

    if(inChar != ' ' && inChar != '\n') // Get the new char, as long as it's not a space or newline.
    { 
      inputString += inChar; // Add the character to the string
    }
    
    if(inChar == '\n') // If the character IS a newline
    {
      return inputString; // Return the string (because the newline signifies that it's complete)
      inputString = ""; // And reset this string
    }
    delay(100); // Slight delay.
    getSerial();
  }
  return inputString; // Return otherwise. EVALUATE (may not be necessary)
  delay(100); // Slight delay
}

// Sensor methods:
// Get EMG sensor values for each EMG
void emgCheck()
{
// Read EMGs 1 and 2 to determine what to do next.
      
  emgA = analogRead(0);
  Serial.print('A');
  Serial.println(emgA);
    if(emgA > (thresholdA - thresholdModifier) && emgA < capA) // If emgA is bigger than the threshold but smaller than the cap
    {
      for(int i=1; i < emgAvgReadCount; i++)
      {
        delay(averagerDelay);
        emgAvgRead = ((emgAvgRead + analogRead(0)) / i);
      }
      
      if(emgAvgRead > thresholdA)
      {
        // What do we do?
        flag = !flag; // Invert the flag
        
        if(flag) // If flag is true
        {
          openHandInstant(); // Action
        } 
        else // Otherwise, if flag is not true (false)
        {
          shutHandInstant(); // Action
        }
        emgAvgRead = 0; // Reset average value
      }
    }

  emgB = analogRead(1);
  Serial.print('B');
  Serial.println(emgB);
    if(emgB > (thresholdB - thresholdModifier) && emgB < capB)
    {
      for(int i=1; i < emgAvgReadCount; i++)
      {
        delay(averagerDelay);
        emgAvgRead = ((emgAvgRead + analogRead(1)) / i);
      }
      
      if(emgAvgRead > thresholdB)
      {
        // What do we do?
        flag = !flag; // Invert the flag
        
        if(flag) // If flag is true
        {
          openHandInstant(); // Action
        } 
        else // Otherwise, if flag is not true (false)
        {
          shutHandInstant(); // Action
        }
        emgAvgRead = 0; // Reset average value
      }
    }
  
  emgC = analogRead(2); // Read emgC (on analog port 2)
  Serial.print('C'); // Print "C" and the emg value for graphing.
  Serial.println(emgC);
    if(emgC > (thresholdC - thresholdModifier) && emgC < capC) // If emgC is greater than the threshold but less than the cap,
    {
      for(int i=1; i < emgAvgReadCount; i++) // Averager counter
      {
        delay(averagerDelay);
        emgAvgRead = ((emgAvgRead + analogRead(2)) / i);
      }
      
      if(emgAvgRead > thresholdC) // If greater than the threshold
      {
        // What do we do?
        flag = !flag; // Invert the flag
        
        if(flag) // If flag is true
        {        
          servo4.write(openAngle[4]);
        } 
        else // Otherwise, if flag is not true (false)
        {
          delay(500);        
          servo4.write(shutAngle[4]);  
        }
       
        emgAvgRead = 0; // Reset the average read.
      }
    }    
}

// Pinch method
void indexPinch()
{
  openHandInstant();
  servo4.write(shutAngle[4]); 
  servo3.write(shutAngle[3]);
}

void peaceSign()
{
  openHandInstant(); // Open the hand
  servo0.write(shutAngle[0]); // Close the pinkie, ring, and thumb
  servo1.write(shutAngle[1]);
  servo4.write(shutAngle[4]);  
}

void rockSign()
{
  openHandInstant(); // Open the hand
  servo4.write(shutAngle[4]); // Shut the thumb
  delay(100);
  servo1.write(shutAngle[1]); // Shut the ring and middle fingers
  servo2.write(shutAngle[2]);
}

// This lets us open the hand easily. Sets all fingers to their respective open angles.
void openHandInstant() 
{
  servo0.write(openAngle[0]); // Pinkie
  servo1.write(openAngle[1]);
  servo2.write(openAngle[2]); // ...
  servo3.write(openAngle[3]);
  servo4.write(openAngle[4]); // Thumb
  delay(fingerOpenDelay); 
}

// Shuts the hand.
void shutHandInstant()
{
  servo0.write(shutAngle[0]); // Pinkie
  servo1.write(shutAngle[1]);
  servo2.write(shutAngle[2]); // ...
  servo3.write(shutAngle[3]);
  delay(500); // Might remove this later.
  servo4.write(shutAngle[4]); // Thumb
  delay(fingerShutDelay);
}

// Finger "wave" starting with the thumb (Servo 5)
void thumbWave()
{
  openHandInstant();
  servo4.write(shutAngle[4]); // Close first
  delay(waveDelay); // Delay
  servo3.write(shutAngle[3]); // Close next
  servo4.write(openAngle[4]); // Open first
  delay(waveDelay);
  servo2.write(shutAngle[2]);
  servo3.write(openAngle[3]);
  delay(waveDelay);
  servo1.write(shutAngle[1]);
  servo2.write(openAngle[2]);
  delay(waveDelay);
  servo0.write(shutAngle[0]);
  servo1.write(openAngle[1]);
  delay(waveDelay);
  servo0.write(openAngle[0]);   
}

// Finger "wave" starting with the pinkie (Servo 0)
void pinkieWave()
{
  openHandInstant();
  servo0.write(shutAngle[0]); // Close first
  delay(waveDelay); // Delay
  servo1.write(shutAngle[1]); // Close next
  servo0.write(openAngle[0]); // Open first
  delay(waveDelay);
  servo2.write(shutAngle[2]);
  servo1.write(openAngle[1]);
  delay(waveDelay);
  servo3.write(shutAngle[3]);
  servo2.write(openAngle[2]);
  delay(waveDelay);
  servo4.write(shutAngle[4]);
  servo3.write(openAngle[3]);
  delay(waveDelay);
  servo4.write(openAngle[4]);  
}

// This method is definitely not the best. Doesn't matter too much though, since this is just for testing. BROKEN AS OF CMD CHARACTER ADDITION. TODO: Rewrite fingerControl method.
void fingerControl()
{
  /*
  //Serial.println("Enter the determinant servo value (0-4) and the angle to be written (0-180). Enter 9999 to exit to main loop.");
  fingerControlVal = getSerial().toInt(); // Set value to an int
  
  // For each thousand, a different servo is selected. We use the leading digit to determine which servo we're controlling, and the remaining three digits
  // to determine what angle we'll write.
  if(fingerControlVal <= 180 && fingerControlVal >= 000)  
  {
    fingerControlVal = fingerControlVal - 0;
    servo0.write(fingerControlVal);
    Serial.println("Triggered 0");
  }
  else if(fingerControlVal <= 1180 && fingerControlVal >= 1000)  
  {
    fingerControlVal = fingerControlVal - 1000;
    servo1.write(fingerControlVal);
    Serial.println("Triggered 1");
  }
  else if(fingerControlVal <= 2180 && fingerControlVal >= 2000) 
  {
    fingerControlVal = fingerControlVal - 2000;
    servo2.write(fingerControlVal);
    Serial.println("Triggered 2");
  }
  else if(fingerControlVal <= 3180 && fingerControlVal >= 3000)  
  {
    fingerControlVal = fingerControlVal - 3000;
    servo3.write(fingerControlVal);
    Serial.println("Triggered 3");
  }
  else if(fingerControlVal <= 4180 && fingerControlVal >= 4000)  
  {
    fingerControlVal = fingerControlVal - 4000;
    servo4.write(fingerControlVal);
    Serial.println("Triggered 4");
  }
  else
  {
    loop();
  }
  */
}

// Command Handler
void commandHandler(String command)
{
  Serial.println("Command recieved: " +command); 
    
    if(command.equalsIgnoreCase("DONE")) 
    {
      Serial.println("Line limit reached. Enter text to continue."); // Unknown if this block of code works properly.
      if(Serial.available())
      {
        commandHandler(getSerial());
      }
      delay(30000); // 30 second delay
      commandHandler("RSTL");
    }
    else if(command.equalsIgnoreCase("OPEN")) 
    {
      openHandInstant();
    }
    else if(command.equalsIgnoreCase("SHUT")) 
    {
      shutHandInstant();
    }
    else if(command.equalsIgnoreCase("RSTL")) 
    {
      numberOfLoops = 0; // Reset loops variable
    }
    else if(command.equalsIgnoreCase("TWAV")) 
    {
      thumbWave();
    }
    else if(command.equalsIgnoreCase("PWAV")) 
    {
      pinkieWave(); // Okie dokie, loki!
    }
    else if(command.equalsIgnoreCase("FING")) 
    {
      fingerControl();
    }
    else if(command.equalsIgnoreCase("PNCH")) 
    {
      indexPinch();
    }
    else if(command.equalsIgnoreCase("PEAC"))
    {
      peaceSign();  
    }
    else if(command.equalsIgnoreCase("ROCK"))
    {
      rockSign();  
    }
    else if(command.equalsIgnoreCase("HELP")) 
    {
      Serial.println("Available: OPEN, SHUT, RSTL, TWAV, PWAV, FING, PNCH, IDLE, TRSH, HELP.");
      delay(3000);
    }
    else if(command.equalsIgnoreCase("IDLE"))
    {
      idleFunction();  
    }
    else if(command.equalsIgnoreCase("TRSH"))
    {
      if(threshFlag)
      {
        threshFlag = !threshFlag;
        thresholdA = 100;  
      }
      else if(!threshFlag)
      {
        threshFlag = !threshFlag;
        thresholdA = 50;  
      }
    }
    else 
    {
      Serial.println("Unknown Command. Type 'HELP' for more info."); //Handle unknown commands
    }
  command,inputString = ""; // Then we clear the string
}

void idleFunction()
{
  shutHandInstant();
  t.after(1500,openHandInstant());
  t.after(1500,indexPinch());
  t.after(1500,peaceSign());
  t.after(1500,rockSign());
  t.after(1500,pinkieWave());
  t.after(1500,thumbWave());
}

void simpleEMGReadA()
{
  Serial.print("A");
  Serial.println(analogRead(0));
}

void simpleEMGReadB()
{
  Serial.print("B");
  Serial.println(analogRead(1));
}

void simpleEMGReadC()
{
  Serial.print("C");
  Serial.println(analogRead(2));  
}

void thresholdSwap(boolean thresholdSwapFlag) // To reset the thresholds so that we can have a timed event instead of a delay.
{
  if(thresholdSwapFlag) // If the flag is true
  {
    thresholdA = thresholdAStore; // Set the three thresholds to their init values
    thresholdB = thresholdBStore;
    thresholdC = thresholdCStore;
  }
  else // If the flag is false
  { 
    thresholdA = 1024; // Set the thresholds to an unreachable value. Sensor range: [0,1023] < 1024
    thresholdB = 1024;
    thresholdC = 1024;
  }    
}

void timerSwapWait(int waitDelay)
{
  thresholdSwap(false); // First we swap the thresholds to their higher values (out of range)
  //t.after(waitDelay,thresholdSwap(true)); // Then after 1000ms (one second) we set them back.
}

/*
Notes:

TODO: Add a kill switch or button. (electronics)
TODO: Set up two 9v voltage regulators and two 5v regulators running off of one supply. (electronics) 
TODO: Remove all calls to delay() in favour of a timer. (timer.h?)


Method list:

setup - basic setup method
loop - main loop
getSerial - gets input from the serial connection
emgCheck - checks the EMG for threshold triggering
indexPinch - pinches using index and thumb
openHand - opens all fingers to their "open" values
shutHand - shuts all fingers to their "shut" values
thumbWave - finger wave starting with the thumb
pinkieWave - finger wave starting with the pinkie
fingerControl - individualized finger control method
commandHandler - handles all debug commands
idleFunction - cycles through hand states
simpleEMGReadA - Prints emg A data
simpleEMGReadB - Prints emg B data
simpleEMGReadC - Prints emg C data
thresholdSwap - Swaps the thresholds HIGH so that nothing activates (As an alternative to calling delay())

*/ 
