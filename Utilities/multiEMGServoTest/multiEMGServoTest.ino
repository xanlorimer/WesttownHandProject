// Code for the upcoming hand test with multiple EMGs and connected servos
// Written by Xan Lorimer (xanlorimer@gmail.com)

// This code is published under the GNU GPL v2 license.

////////////////////////////////////////////////////////////////////////////////////////
// THIS CODE ISN'T COMPLETELY TESTED AS OF 4/22/15. TESTED TO COMPILATION BUT NO MORE //
////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>

// As usual, we'll begin by declaring variables
int loopHaltAmount = 100000; // Number of loops before we stop the test. How many data points do we want to gather?
int numberOfLoops = 0; // Initialize the number of loops so that we can check for and use a stopping point.
int loopDelay = 50; // Amount of time (in ms) for the main loop to wait.
int waveDelay = 100; // Amount of time (in ms) between each finger moving in any wave.

int shutAngle[] = {90,90,90,90,90}; // Angle when "shut" - From pinkie to thumb.
int openAngle[] = {90,90,90,90,90}; // Angle when "open" - From pinkie to thumb.

char inChar;
String inputString;

// Declare all of the servos
Servo servo0; // Pinkie
Servo servo1;
Servo servo2;
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
  
}
// Action methods:
// Based on value of EMG (x), do (y)

// Control methods:
// Raw servo control methods

// Finger "wave" starting with the thumb (Servo 5)
void thumbWave()
{
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

void fingerControl()
{
  
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
    else 
    {
      Serial.println("Unknown Command. Available: DONE, OPEN, SHUT, RSTL, TWAV, PWAV, FING"); //Handle unknown commands
    }
  inputString = ""; // Then we clear the string
}



/*
Notes:

In the electronics portion of this, perhaps we should have a transistor allowing power to pass to the servos. 
If something bad happens, the servos can all go limp as the transistor is turned off.
We could implement this by adding an emergency stop button.

But yeah, the servos should have a separate power supply.

At this point, why not just design a simple power circuit? We'll need a high-availability 5V rail for the Arduino, as well as a rail for the servos.
A third, stable 9v rail will be needed for the EMGs. Ask Larry about this.

Oh, we also want to log all of this information to a file, probably. Maybe we should take a video as well, to associate activations with movements.

What should our actions be? I guess hand open, hand close. Maybe something to control groups of fingers? pinch?
Some inputtable states like "Wave" would be cool. Stuff to idle with. *WAVE IMPLEMENTED

Major TODO:
  Make SerialEvent return a String so that we can have more versatility when calling it. As it is now, this is really inefficient :/ -- DONE
  
*/ 
