// Code for the upcoming hand test with multiple EMGs and connected servos
// Written by Xan Lorimer (xanlorimer@gmail.com)

// This code is published under the GNU GPL v2 license.

////////////////////////////////////////////////////////////////////////////////////////
// THIS CODE ISN'T COMPLETELY TESTED AS OF 4/24/15. TESTED TO COMPILATION BUT NO MORE //
////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>

// As usual, we'll begin by declaring variables
int loopHaltAmount = 100000; // Number of loops before we stop the test. How many data points do we want to gather?
int numberOfLoops = 0; // Initialize the number of loops so that we can check for and use a stopping point.
int loopDelay = 50; // Amount of time (in ms) for the main loop to wait.
int waveDelay = 100; // Amount of time (in ms) between each finger moving in any wave.
int fingerOpenDelay = 1000; // Amount of time (in ms) that it takes for the slowest finger to open.
int fingerShutDelay = 1000; // Amount of time (in ms) that it takes for the slowest finger to shut.

// Stuff for averaging:
int emgAvgRead; // Just initializing the variable where the average is stored
int thresholdA = 50; // Threshold for EMG A
int thresholdB = 40; // Threshold for EMG B
int thresholdC = 30; // Threshold for EMG C
int averagerDelay = 5; // How long we wait before taking each average point
int emgAvgReadCount = 5; // Number of points we collect to take the average
int thresholdModifier = 10; // Modifier for preliminary threshold activation

int fingerControlVal; // Initialze the angle that we'll use for individual finger controls.

int emgA,emgB,emgC; // Where EMG data will be stored
int shutAngle[] = {90,90,90,90,90}; // Angle when "shut" - From pinkie to thumb.
int openAngle[] = {90,90,90,90,90}; // Angle when "open" - From pinkie to thumb.

char inChar;
String inputString;

// Declare all of the servos
Servo servo0; // Pinkie
Servo servo1;
Servo servo2; // ...
Servo servo3;
Servo servo4; // Thumb

// Setup method
void setup()
{    
  // Declare the EMG inputs
  pinMode(0, INPUT); // Input on analog pin 0
  pinMode(1, INPUT); // Input on analog pin 1
  pinMode(2, INPUT); // Input on analog pin 2

  // Declare the servo outputs
  servo0.attach(3); // Pinkie
  servo1.attach(5);
  servo2.attach(6); // ...
  servo3.attach(9);
  servo4.attach(10); // Thumb

  // Begin the serial connection for debug
  Serial.begin(9600); 
}

// Main loop
void loop()
{
    // Each time we loop, we'll check the EMG values and evaluate them. Based on those values, we can execute an action of some sort.
    if(numberOfLoops > loopHaltAmount)
      commandHandler("DONE"); // Let the user decide what to do next.
    emgCheck();
    numberOfLoops++;
    
    if(Serial.available()) // Let's not waste loops
      commandHandler(getSerial()); // Call the command method with whatever's in the serial buffer.
    
    delay(loopDelay); // And let's not overload the Arduino
}

// This method accepts serial data.
String getSerial() // Get new serial data if there is any
{
  if (Serial.available()) // If there's a character in the buffer 
  { 

    if(inChar != ' ') // Get the new char, as long as it's not a space
    { 
      inChar = (char)Serial.read(); // Read the character
      inputString += inChar; // Add the character to the string
    }
    
    if (inChar == '\n') // Set the flag if the character is a newline 
    { 
      Serial.println(inputString);
      //commandHandler(inputString);
    } 
  }
  return(inputString);
  inputString = "";
}


// Sensor methods:
// Get EMG sensor values for each EMG
void emgCheck()
{
// Read EMGs 1 and 2 to determine what to do next.
  emgA = analogRead(0);
    if(emgA > (thresholdA - thresholdModifier))
    {
      for(int i=1; i < emgAvgReadCount; i++)
      {
        delay(averagerDelay);
        emgAvgRead = ((emgAvgRead + analogRead(0)) / i);
      }
      if(emgAvgRead > thresholdA)
        //DO ACTION
        emgAvgRead = 0;
    }
  emgB = analogRead(1);
    if(emgB > (thresholdB - thresholdModifier))
    {
      for(int i=1; i < emgAvgReadCount; i++)
      {
        delay(averagerDelay);
        emgAvgRead = ((emgAvgRead + analogRead(0)) / i);
      }
      if(emgAvgRead > thresholdA)
        //DO ACTION
        emgAvgRead = 0;
    }
  emgC = analogRead(2); 
    if(emgC > (thresholdC - thresholdModifier))
    {
      for(int i=1; i < emgAvgReadCount; i++)
      {
        delay(averagerDelay);
        emgAvgRead = ((emgAvgRead + analogRead(0)) / i);
      }
      if(emgAvgRead > thresholdA)
        //DO ACTION
        emgAvgRead = 0;
    }
}

// Pinch method
void indexPinch()
{
  openHand();
  servo4.write(shutAngle[4]); 
  servo3.write(shutAngle[3]);
}

// This lets us open the hand easily. Sets all fingers to their respective open angles.
void openHand() 
{
  servo0.write(openAngle[0]); // Pinkie
  servo1.write(openAngle[1]);
  servo2.write(openAngle[2]); // ...
  servo3.write(openAngle[3]);
  servo4.write(openAngle[4]); // Thumb
  delay(fingerOpenDelay); 
}

// Shuts the hand.
void shutHand()
{
  servo0.write(shutAngle[0]); // Pinkie
  servo1.write(shutAngle[1]);
  servo2.write(shutAngle[2]); // ...
  servo3.write(shutAngle[3]);
  delay(100); // Might remove this later.
  servo4.write(shutAngle[4]); // Thumb
  delay(fingerShutDelay);
}

// Finger "wave" starting with the thumb (Servo 5)
void thumbWave()
{
  openHand();
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
  servo0.write(shutAngle[0]); // Close first
  delay(waveDelay); // Delay
  servo1.write(shutAngle[1]); // Close next
  servo0.write(openAngle[0]); // Open first
  delay(waveDelay);
  servo2.write(shutAngle[2]);
  servo4.write(openAngle[1]);
  delay(waveDelay);
  servo3.write(shutAngle[3]);
  servo4.write(openAngle[2]);
  delay(waveDelay);
  servo3.write(shutAngle[4]);
  servo4.write(openAngle[3]);
  delay(waveDelay);
  servo3.write(openAngle[4]);  
}

// This method is definitely not the best. Doesn't matter too much though, since this is just for testing.
void fingerControl()
{
  Serial.println("Enter the determinant servo value (0-4) and the angle to be written (0-180). Enter 9999 to exit to main loop.");
  fingerControlVal = getSerial().toInt(); // Set value to an int
  
  // For each thousand, a different servo is selected. We use the leading digit to determine which servo we're controlling, and the remaining three digits
  // to determine what angle we'll write.
  if(fingerControlVal <= 180 && fingerControlVal >= 000)  
  {
    fingerControlVal = fingerControlVal - 0;
    servo0.write(fingerControlVal);
  }
  if(fingerControlVal <= 1180 && fingerControlVal >= 1000)  
  {
    fingerControlVal = fingerControlVal - 1000;
    servo1.write(fingerControlVal);
  }
  if(fingerControlVal <= 2180 && fingerControlVal >= 2000) 
  {
    fingerControlVal = fingerControlVal - 2000;
    servo2.write(fingerControlVal);
  }
  if(fingerControlVal <= 3180 && fingerControlVal >= 3000)  
  {
    fingerControlVal = fingerControlVal - 3000;
    servo3.write(fingerControlVal);
  }
  if(fingerControlVal <= 4180 && fingerControlVal >= 4000)  
  {
    fingerControlVal = fingerControlVal - 4000;
    servo4.write(fingerControlVal);
  }
  if(fingerControlVal == 9999)
  {
    loop();
  } 
  else
  {
    Serial.println("Invalid selection.\n\n\n");
    fingerControl();
  }
}

// Command Handler
void commandHandler(String command)
{
  Serial.println("Command recieved: " +inputString); 
    if(inputString.equalsIgnoreCase("DONE")) 
    {
      Serial.println("Line limit reached. Enter text to continue."); // Unknown if this block of code works properly.
      if(Serial.available())
      {
        commandHandler(getSerial());
      }
      delay(30000); // 30 second delay
      commandHandler("RSTL");
    }
    if(inputString.equalsIgnoreCase("OPEN")) 
    {
      //openHandInstant();
    }
    if(inputString.equalsIgnoreCase("SHUT")) 
    {
      //shutHandInstant();
    }
    if(inputString.equalsIgnoreCase("RSTL")) 
    {
      numberOfLoops = 0; // Reset loops variable
      loop();
    }
    if(inputString.equalsIgnoreCase("TWAV")) 
    {
      thumbWave();
    }
    if(inputString.equalsIgnoreCase("PWAV")) 
    {
      pinkieWave(); // Okie dokie, loki!
    }
    if(inputString.equalsIgnoreCase("FING")) 
    {
      fingerControl();
    }
    if(inputString.equalsIgnoreCase("PNCH")) 
    {
      indexPinch();
    }
    if(inputString.equalsIgnoreCase("HELP")) 
    {
      Serial.println("Available: OPEN, SHUT, RSTL, TWAV, PWAV, FING, HELP, PNCH, HELP.");
      delay(3000);
    }
    else 
    {
      Serial.println("Unknown Command. Type 'HELP' for more info."); //Handle unknown commands
    }
  inputString = ""; // Then we clear the string
}


/*
Notes:

TODO: Add a kill switch or button.
TODO: Power servos, arduino, and MSK separately. Two 5V lines (a steady one for the Arduino and one for the servos), and a stead 9V for the MSK.
TODO: Test the indexPinch method - need to know if the fingers close at the same time or if they have some delay.
TODO: Log serial data to a file through processing. 
TODO: Add processing comms code (A(n), B(n), C(n))
TODO: Re-evaluate the necessity of having loop(); in certain places.
TODO: Implement averaging of EMG data
TODO: Basic testing of all methods.
TODO: Create a list of the methods contained herein.

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

*/ 
