// This code will control a servo, taking input from a Muscle Sensor v3 EMG.
// It uses the input to open or close a prosthetic hand.
// Servos will not be used in the final version...
// This code is for testing purposes only!

// Based on a threshold, a servo will be activated.
// Programmed by Xan Lorimer (xanlorimer@gmail.com)
// This code is published under the GNU GPL v2 license.

////////////////////////////////////////////////////////////////////////////////////////
// THIS CODE ISN'T COMPLETELY TESTED AS OF 4/22/15. TESTED TO COMPILATION BUT NO MORE //
////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>

// Let's declare our variables below, and initialize a few things:
String inputString = ""; // A string to hold incoming data
char inChar; // Input character for commands.

int inputInt; // Initialize...
int analogPort = 0; //Which analog port is the sensor on?
int loopDelay = 50; // How long to wait during the main loop.
int readDelay = 500; // How long to wait after servo activation/threshold trigger.
int openHandAngle = 180; // Default angle value (deg) for the "open" state of the servo.
int shutHandAngle = 15; // Default angle value (deg) for the "shut" state of the servo.
int openHandGradualAngle = 5; // Number of degrees we should turn per loop when gradually opening.
int shutHandGradualAngle = 5; // Number of degrees we should turn per loop when gradually shutting.
int threshold = 100; // Intensity before we trigger the servo (threshold)
//int angleBuff[2]; // Initialize the angle buffer for manual sets.
int angleFull = 90; // "Full" angle for manual servo input.
int defaultAngle = 90; // Set to this when we initialize.
int sensorOut; // Initialize the sensor output

boolean serialDisplay = true; // Do we want to display raw serial data in console?
boolean handState = true; // What state should the hand start in? State flag.

Servo handServo; // Single hand servo

// Setup method
void setup() 
{
  handServo.attach(6); // Attach the servo to serial PWM 9
  handServo.write(defaultAngle); // Set starting angle
  
  Serial.begin(9600); // Initialize a serial connection at 9600bd
  inputString.reserve(200); // Reserve 200 bytes for the inputString 
}

// Main loop
void loop() 
{
  
  sensorOut = analogRead(analogPort) * 2; // Get the sensor value, multiply by two.
  
  if(sensorOut >= threshold) // If the sensor value is triggering
  {
    handState = !(handState);
    Serial.println(handState);
    
    if(handState) 
      Serial.println("Opening!");
      //openHandInstant();
    if(!handState)
      Serial.println("Closing!");
      //shutHandInstant();
  }
  
  if(serialDisplay == true) // If we're printing serial data
    Serial.println("A"); // From sensor 1. A is the identifier.
    Serial.print(sensorOut);
    //Serial.println("B"); // From sensor 2 (NOT IMPLEMENTED IN THIS CODE)
    //Serial.print(sensor2Out);
  
  delay(loopDelay); // Wait a bit so as not to overload the port
    serialEvent();  
}

// This method accepts serial data.
void serialEvent() // Whenever new data comes in on the Arduino
{
  if (Serial.available()) // If there's a character in the buffer 
  { 

    if(inChar != ' ') // Get the new char, as long as it's not a space
    { 
      inChar = (char)Serial.read(); // Read the character
      inputString += inChar; // Add the character to the string
    }
    
    if (inChar == '\n') // If newline, call cmdHandler 
    { 
      Serial.println(inputString);
      cmdHandler(inputString);
      inputString = "";
    } 
  }
}

// This method handles commands
void cmdHandler(String inputString) 
{ 
  Serial.println("Command recieved: " +inputString); 
    if(inputString.equalsIgnoreCase("ASET")) 
    {
      angleSet();
    }
    if(inputString.equalsIgnoreCase("OPEN")) 
    {
      openHandInstant();
    }
    if(inputString.equalsIgnoreCase("SHUT")) 
    {
      shutHandInstant();
    }
    else 
    {
      Serial.println("Unknown Command. Options: ASET, OPEN, SHUT."); //Handle unknown commands
    }
    
  inputString = ""; // Then we clear the string
}


// This method manually sets the servo angle
void angleSet() 
  {
  Serial.println("Enter an angle between 0 and 180: \n");
    if (Serial.available()) 
    {
       
      if(inChar != ' ') // If the input char is not a space
        { 
        inChar = (char)Serial.read(); // Read serial
        inputString += inChar; // Add character to string
        }
    
      if (inChar == '\n') // When it's a newline 
      { 
        Serial.println("Final string: "); 
        Serial.print(inputString);
      
        inputInt = inputString.toInt();
      
        if(inputInt > 180 || inputInt < 0)
        {
          inputInt = defaultAngle;
          Serial.println("Value too big. Set to default.");
        }
        handServo.write(inputInt);
        inputString = "";
        loop(); // Return to main loop
      }
      angleSet(); // Recurse.
    }
}


 
// Hand Control Methods:

// This method instantly opens the hand
void openHandInstant() 
{
  handServo.write(openHandAngle);
  delay(readDelay);
  Serial.println("Open"); // Debug
}

// This method instantly shuts the hand
void shutHandInstant() 
{
  handServo.write(shutHandAngle);
  delay(readDelay);
  Serial.println("Shut"); // Debug
}

/*
// This method gradually opens the hand
void openHandGradual() 
{
  handServo.write(handServo.read() + openHandGradualAngle);
  Serial.println("Gradual Open"); // Debug 
}

// This method gradually shuts the hand
void shutHandGradual() 
{
  handServo.write(handServo.read() - shutHandGradualAngle);
  Serial.println("Gradual Shut"); // Debug
}
*/
