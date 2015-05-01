// This code will allow the user to set a servo angle w/ serial input.

// How to use this program:
// Connect the servo to pin 9. Upload the code. Open the serial console.
// You can toggle the servo between angleOpen and angleShut by sending "T" (for toggle)
// You can read the servo angle by sending "R" (for read)
// You can set the servo angle by sending "S000" where 000 is a number from 0 to 180.

// Disclaimer: This program does not handle invalid commands, nor is it perfect.

#include <Servo.h>

Servo handServo; // Create servo object

String togCmd = "T"; // "Command" to toggle
String setCmd = "S"; // "Command" to set
String reaCmd = "R"; // "Command" to read
String inputString; // String made up of inChars
char inChar; // Character to add to string
boolean state; // The flag for toggling the servo

int loopDelay = 50; // Loop delay time
int angleOpen = 115; // What angle will we set the servo to when it is open/shut?
int angleShut = 35;
int inputInt = 90; // Angle to set on startup. This is reset later on.

// Setup
void setup() {
  handServo.attach(3);
  handServo.write(inputInt); // Prevents the servo from eating itself
  Serial.begin(9600);
}

// Loop
void loop() {
  if (Serial.available()) {
    if(inChar != ' ') { // If char is not a space,
      inChar = (char)Serial.read(); // Read inChar
      inputString += inChar; // Add inChar to string
    }
    
    if (inChar == '\n') { // On a newline    
      //Serial.println("Input string: ");
      //Serial.print(inputString); // Print the raw string. Debug function.
      
      // Set mode
      if(inputString.startsWith(setCmd)) {
        inputString.remove(0,1); // Remove the command character
        inputInt = inputString.toInt(); // Convert to int
      
        if(inputInt > 180 || inputInt < 0){
          inputString = "";
          Serial.println("Value too big! Ignorning.");
          //loop();
        }
        else {
          Serial.println("Setting angle: ");
          Serial.println(inputInt);
          handServo.write(inputInt); // Write the angle
          inputString = ""; // Reset input string
        }
      }
      
      // Toggle mode
      if(inputString.startsWith(togCmd)) {  
        Serial.println("Toggling angle.");      
        state = !(state);
          if(state) 
            handServo.write(angleOpen);
          if(!state)
            handServo.write(angleShut);
        inputString = "";
      }
      
      // Read mode
      if(inputString.startsWith(reaCmd)) {
        Serial.println("Current angle:");
        Serial.println(handServo.read());
        inputString = "";
      }
      delay(loopDelay); // Delay
     }
  }
}
