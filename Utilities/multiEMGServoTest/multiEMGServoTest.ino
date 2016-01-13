// Written by Xan Lorimer (xanlorimer@gmail.com) and Tom Barnett (barnett.kthomas@gmail.com)

// This code is published under the GNU GPL v2 license.

//////////////////////////////////////////////////
// THIS CODE IS NOT FULLY TESTED AS OF 5/19/15. //
//////////////////////////////////////////////////

#include <Servo.h>
#include "Timer.h"

boolean flagA = true;
boolean flagB = true;
boolean flagC = true;
boolean threshFlag = true;
boolean thresholdSwapFlag = true; // This variable MUST be initialized as TRUE in order for the thresholds to be set during setup(). Used to swap threshold flags.
boolean overThresh = false; //triggers whenever emg goes over the threshold
int capA = 512; // "Out of bounds" value for MSK A. 
int capB = 512; // Ditto, B
int capC = 512; // Ditto, C

// As usual, we'll begin by declaring variables
int loopHaltAmount = 100000; // Number of loops before we stop the test. How many data points do we want to gather?
int numberOfLoops = 0; // Initialize the number of loops so that we can check for and use a stopping point.
int loopDelay = 50; // Amount of time (in ms) for the main loop to wait.
int waveDelay = 0; // Amount of time (in ms) between each finger moving in any wave.
int fingerOpenDelay = 0; // Amount of time (in ms) that it takes for the slowest finger to open.
int fingerShutDelay = 0; // Amount of time (in ms) that it takes for the slowest finger to shut.

// Stuff for averaging:
int emgAvgReadA,emgAvgReadB,emgAvgReadC; // Just initializing the variable where the average is stored
int thresholdA = 75;
int thresholdB = 75;
int thresholdC = 75;
int thresholdAStore = 75; // Threshold for EMG A
int thresholdBStore = 75; // Threshold for EMG B
int thresholdCStore = 75; // Stored threshold for EMG C
int averagerDelay = 20; // How long we wait before taking each average point
int emgAvgReadCount = 5; // Number of points we collect to take the average
int thresholdModifier = 5; // Modifier for preliminary threshold activation
int threshlowModifier = 35; //Modifier for lower threshold 

int fingerControlVal; // Initialze the angle that we'll use for individual finger controls.

int emgA,emgB,emgC; // Where EMG data will be stored
int shutAngle[] = {35,35,35,35,35}; //{5,35,15,15,30}; // Angle when "shut" - From pinkie to thumb.
int openAngle[] = {170,170,170,170,170}; //{160,140,180,180,140}; // Angle when "open" - From pinkie to thumb.

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
  if(emgA >= thresholdAStore)
  {
    Serial.print('a');
  }
  else if(emgA < thresholdAStore)
  {
    Serial.print('A'); // Print "A" and the emg value for graphing.
  }
  Serial.println(emgA);

    if(emgA > (thresholdA - thresholdModifier) && emgA < capA) // If emgA is bigger than the threshold but smaller than the cap
    {
      for(int i=1; i <= emgAvgReadCount; i++) //starts averaging the values 
      {
        delay(averagerDelay);
        emgAvgReadA = (emgAvgReadA + analogRead(0));
      }
      
      if((emgAvgReadA / emgAvgReadCount) > thresholdA && overThresh == false) //overThresh is set to false at the beginning
      {
        
        flagA = !flagA; // Invert the flag--determines if the hand is opened or closed
        overThresh = true; //tells code that currently over the threshold 
        

        if(flagA) // If flag is true--hand is shut
        {
          openHandInstant(); // Action
          timerSwapWait(1000); // Wait for a second so that we don't have multiple activations.
        } 
        else // Otherwise, if flag is not true (false) and the hand is open
        {
          shutHandInstant(); // Action
          timerSwapWait(1000);
        }
      
      }
      
      emgAvgReadA = 0; // Reset average value

    }

  if(emgA < (thresholdA - threshlowModifier) && overThresh == true && thresholdA == thresholdAStore) //checks to see if it has gone under the threshold agian, and checks to make sure threshold is normal
  { 
        overThresh = false; 
  }

  emgB = analogRead(1);
  if(emgB >= thresholdBStore)
  {
    Serial.print('b');
  }
  else if(emgB < thresholdBStore)
  {
    Serial.print('B'); // Print "B" and the emg value for graphing.
  }
  Serial.println(emgB);
  
    if(emgB > (thresholdB - thresholdModifier) && emgB < capB)
    {
      for(int i=1; i <= emgAvgReadCount; i++)
      {
        delay(averagerDelay);
        emgAvgReadB = (emgAvgReadB + analogRead(1));
      }
      
      if((emgAvgReadB / emgAvgReadCount) > thresholdB)
      {
        // What do we do?
        flagB = !flagB; // Invert the flag
        
        if(flagB) // If flag is true
        {
          servo0.write(openAngle[0]);
          servo1.write(openAngle[1]);
          servo2.write(openAngle[2]);
          timerSwapWait(1000); // Raise the thresholds for a second to prevent additional activations
        } 
        else // Otherwise, if flag is not true (false)
        {
          servo0.write(shutAngle[0]);
          servo1.write(shutAngle[1]);
          servo2.write(shutAngle[2]);
          timerSwapWait(1000); // Raise the thresholds for a second to prevent additional activations
        }
      }
      emgAvgReadB = 0; // Reset average value
    }
  
  emgC = analogRead(2); // Read emgC (on analog port 2)
  if(emgC >= thresholdCStore)
  {
    Serial.print('c');
  }
  else if(emgC < thresholdCStore)
  {
    Serial.print('C'); // Print "C" and the emg value for graphing.
  }
  Serial.println(emgC);
  
    if(emgC > (thresholdC - thresholdModifier) && emgC < capC) // If emgC is greater than the threshold but less than the cap,
    {
      for(int i=1; i <= emgAvgReadCount; i++) // Averager counter
      {
        delay(averagerDelay);
        emgAvgReadC = (emgAvgReadC + analogRead(2));
      }
      
      if((emgAvgReadC / emgAvgReadCount) > thresholdC) // If greater than the threshold
      {
        // What do we do?
        flagC = !flagC; // Invert the flag
        
        if(flagC) // If flag is true
        {        
          servo4.write(openAngle[4]);
          timerSwapWait(1000);
        } 
        else // Otherwise, if flag is not true (false)
        {
          servo4.write(shutAngle[4]);
          timerSwapWait(1000);  
        }
      }
      emgAvgReadC = 0; // Reset the average read.
    }    
}

// Pinch method (pinches thumb and index finger)
void indexPinch()
{
  openHandInstant();
  
  servo4.write(shutAngle[4]); 
  servo3.write(shutAngle[3]);
}

// Peace sign! 
void peaceSign()
{
  openHandInstant(); // Open the hand
  
  servo0.write(shutAngle[0]); // Close the pinkie, ring, and thumb
  servo1.write(shutAngle[1]);
  servo4.write(shutAngle[4]);  
}

// Rock sign! \m/
void rockSign()
{
  openHandInstant(); // Open the hand
  
  servo4.write(shutAngle[4]); // Shut the thumb
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
}

// Shuts the hand.
void shutHandInstant()
{
  servo0.write(shutAngle[0]); // Pinkie
  servo1.write(shutAngle[1]);
  servo2.write(shutAngle[2]); // ...
  servo3.write(shutAngle[3]);
  servo4.write(shutAngle[4]); // Thumb
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
  t.after(1500,openHandInstant);
  t.after(1500,indexPinch);
  t.after(1500,peaceSign);
  t.after(1500,rockSign);
  //t.after(1500,pinkieWave);
  //t.after(1500,thumbWave);
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

void thresholdSwapLow() // To reset the thresholds so that we can have a timed event instead of a delay.
{
    thresholdA = thresholdAStore; // Set the three thresholds to their init values
    thresholdB = thresholdBStore;
    thresholdC = thresholdCStore;

}

void thresholdSwapHigh()
{
    thresholdA = 1024; // Set the thresholds to an unreachable value. Sensor range: [0,1023] < 1024
    thresholdB = 1024;
    thresholdC = 1024;

}    


void timerSwapWait(int waitDelay)
{
  thresholdSwapHigh(); // First we swap the thresholds to their higher values (out of range)
  t.after(waitDelay,thresholdSwapLow); // Then after the wait delay, we set them back.
  // TODO: Use a flag with this instead?
}

/*
Notes:

TODO: Add a kill switch or button. (electronics)
TODO: Set up two 9v voltage regulators and two 5v regulators running off of one supply. (electronics) 
TODO: Remove all calls to delay() in favour of a timer. (timer.h?) 
TODO: (Low priority) remake and test pinkieWave and thumbWave methods with timers.
TODO: Test how well the timer implementation works.
TODO: Test idleFunction();
TODO: Rewrite fingerControl method (broken as of command character addition a few commits back)
TODO: Test indexPinch, peacesign, and rockSign, all of which call openHandInstant before doing their stuff. Removed the delay calls, so need to implement the delays with timers.
TODO: Write openHandGraudal and shutHandGradual methods.
TODO: Implement commands in a serialEvent instead of the kludginess that we have now.
TODO: Are there analog events? We should use one so that sensor data is always being sent without delay.

Method list:

setup - basic setup method
loop - main loop
getSerial - gets input from the serial connection
emgCheck - checks the EMG for threshold triggering
indexPinch - pinches using index and thumb
peaceSign - a peace sign. (Or at least an attempt at one...)
rockSign - a rock sign \m/
openHand - opens all fingers to their "open" values
shutHand - shuts all fingers to their "shut" values
thumbWave - finger wave starting with the thumb
pinkieWave - finger wave starting with the pinkie
fingerControl - individualized finger control method
commandHandler - handles all debug commands
idleFunction - cycles through hand states
simpleEMGReadA - prints emg A data
simpleEMGReadB - prints emg B data
simpleEMGReadC - prints emg C data
thresholdSwap - swaps the thresholds HIGH so that nothing activates (As an alternative to calling delay())
timerSwapWait - waits for int ms.

*/ 
