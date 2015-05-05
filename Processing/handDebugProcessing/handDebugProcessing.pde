// This program is for debugging and testing the prosthetic hand...
// Written by Xan Lorimer (xanlorimer@gmail.com), using graphing code by Tom Igoe

// Features:
// Read & Graph all devices separately

// Planned: 
// Manually input motor values and activate methods
// Manually reset certain variables (maybe)

// To do:
// Implement serial comms TO the arduino, so that we can manually set angles, reset vars, and generally just debug stuff

// Expects a serial input FROM the Arduino with "A" or "B" as the sensor ID prepended to each sensor output value.
// Example output from Arduino:
/*
A120
B0
C4
A45
B2
C21
A10
B3
C128
*/

import processing.serial.*; 
import java.util.Scanner; // So that we can name the file with the date
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.regex.*;

// TEST VARIABLES
String servoVarSplit = "INIT";
String fingerCmdVarSplit = "INIT";
int servoVar;
int fingerCmdVar;
int angleVar = 1337;

String test = "INIT"; // TEST STRING, REMOVE LATER.
int iterator = 0;
String cmdOut = "INIT";
// END TEST VARIABLES

  String fileName = new SimpleDateFormat("yyyy-MM-dd-hh-mm'.txt'").format(new Date());
  PFont font;


  Serial usbPort; // Initialize the serial port
  PrintWriter fileWrite;

  int intensity = 1; // Amplitude modifier of the graph. Must be scaled *with* thresholds!
  int totalIterations = 0; //How many times has the graph gone through
  int xPos = 1; // Horizontal position for the graphs
  float aMax,bMax,cMax,dMax = 0; // Init maximum values for display purposes
  float aValIn,bValIn,cValIn,dValIn = 0; // Init sensor input values
  float aValInPrevious,bValInPrevious,cValInPrevious,dValInPrevious = 0;
  
  float thresholdA = 350; // Initialize the thresholds
  float thresholdB = 300;
  float thresholdC = 0;
  
  float heightModA,heightModB,heightModC;
  
  char aValGreater = 'F'; // Is the value GREATER than the threshold or no?
  char bValGreater = 'F'; 
 
  String cmdString = "";
  
// Setup method  
void setup() 
{
  size(1280,600);
  //println(Serial.list());
  usbPort = new Serial(this, Serial.list()[3], 9600); // New serial port on port 4 @9600bd. Autodetect USB/Ask?
  usbPort.bufferUntil('\n'); // Only buffer until we get a newline character
  
  fileWrite = createWriter("emg_data_log_" + fileName);
  font = createFont("Arial",16,true); // Arial, 16 point, anti-aliasing on
  textFont(font,14);
  fill(255);  
  
  background(0); // Black background
}

// Draw method
void draw() 
{
  // Loop
  clearText();
  drawText(iterator);
  iterator++;

  line(0, 402, 1280, 402);
  stroke(0);

}

// serialEvent all of the important stuff happens in here.
void serialEvent(Serial usbPort) 
{
  String rawStringIn = usbPort.readStringUntil('\n'); // Read serial until newline
  
  fileWrite.println(rawStringIn); // Attempt to write the data to the log file.
  
  if(rawStringIn != null) // If the raw input is not null,
  { 
    rawStringIn = trim(rawStringIn); // Trim off the newline character
    
    // This switch determines what we do with the data that we receive
    switch(rawStringIn.charAt(0)) // Get the determinant character (index: 0)
    { 
      case 'A': // If the ID is a (sensor A)
        aValIn = float(removeCharAt(rawStringIn,0));
        if (aValIn > thresholdA)
        {
          aValGreater = 'T';
        }
        else
        {
          aValGreater = 'F';
        }
        break;
      case 'B': // If the ID is 2 (sensor B)
        bValIn = float(removeCharAt(rawStringIn,0));
        if (bValIn > thresholdB)
        {
          bValGreater = 'T';
        }
        else
        {
          bValGreater = 'F';
        }
        break;
        
      /*  
      // A block for testing whether threshold averaging and the like actually works.  
      case 'w': // Threshold triggered. EXPERIMENTAL
        aValIn = float(removeCharAt(rawStringIn,0)); 
        System.out.println("\n\n\n\n\n\n\n\n\n\nTHRESHOLD 'W' TRIGGERED!");          
        break;
      case 'x': // Threshold triggered? EXPERIMENTAL
        aValIn = float(removeCharAt(rawStringIn,0));
        System.out.println("\n\n\n\n\n\n\n\n\n\nTHRESHOLD 'X' TRIGGERED!");
        break;
      case 'y': // Threshold triggered. EXPERIMENTAL
        bValIn = float(removeCharAt(rawStringIn,0));
        System.out.println("\n\n\n\n\n\n\n\n\n\nTHRESHOLD 'Y' TRIGGERED!");
        break;
      case 'z': // Threshold triggered. EXPERIMENTAL
        bValIn = float(removeCharAt(rawStringIn,0));
        System.out.println("\n\n\n\n\n\n\n\n\n\nTHRESHOLD 'Z' TRIGGERED!");
        break;
      default:
        System.out.println("ERROR: Unknown sensor ID!!! Check the code!");
        break; // Something else? Error info? Blink LED?
    } // End switch */
    
    }
    
    // This block of code could probably be made more efficient... To do, I guess.
    // This sets the max value that we've seen.
    if(aValIn >= aMax) 
    {
      aMax = aValIn;
    }
      System.out.println("CURR 'A' VALUE:\t" +aValIn);
      System.out.println("MAX VALUE:\t" +aMax);
    
    if(bValIn >= bMax) 
    {
      bMax = bValIn;
    }
    
    // Print some debug info TODO: Make this displayed on the white part of the window.
      System.out.println("CURR 'B' VALUE:\t" +bValIn); 
      System.out.println("MAX VALUE:\t" +bMax);
    
      System.out.println("\nIterations:\t" +totalIterations);
      System.out.println("A ACTIVE?: " +aValGreater +"\t" + "B ACTIVE?: " +bValGreater);

    // Connect the dots...
    if (aValInPrevious > aValIn)
      heightModA = (aValInPrevious - aValIn);
    else if (aValInPrevious < aValIn)
      heightModA = (aValIn - aValInPrevious); 
    else if (aValInPrevious == aValIn)
      heightModA = 0;
      
    if (bValInPrevious > bValIn)
      heightModB = (bValInPrevious - bValIn);
    else if (bValInPrevious < bValIn)
      heightModB = (bValIn - bValInPrevious); 
    else if (bValInPrevious == bValIn)
      heightModB = 0;


    // Let's draw the lines:
    stroke(255,0,0);
    line(xPos, ((height - 200) - (intensity * aValIn + heightModA)), xPos, ((height - 200) - (intensity * aValIn)));
    stroke(0,255,0);
    line(xPos, ((height - 200) - (intensity * bValIn + heightModB)), xPos, ((height - 200) - (intensity * bValIn)));
    
    stroke(0,0,255); // For a third EMG
    line(xPos, ((height - 200) - (intensity * cValIn + heightModC)), xPos, ((height - 200) - (intensity * cValIn)));
    
    // And then let's draw the thresholds:
    stroke(255,0,255);
    line(0, thresholdA, 1280, thresholdA);
    stroke(255,0,255);
    line(0, thresholdB, 1280, thresholdB);
    stroke(255,255,0);
    line(0, thresholdC, 1280, thresholdC);
    
    // If the graph goes over the edge, we'll erase the graph portion of everything and start over.  
    if(xPos >= width) 
    {
      xPos = 0;
      fill(0);
      rect(0, 0, 1280, 400);
      totalIterations++;
    }
    else 
    {
      xPos++; // Increment x position
    }
    
  } // End if
} // End serialEvent
    
    
// To remove a character from a string
public static String removeCharAt(String s, int pos) 
{
   return s.substring(0,pos)+s.substring(pos+1);
} 

// KeyPressTest
void keyPressed() 
{
  //println("pressed " + char(key) + " " + keyCode);
  if(keyCode == 10 || keyCode == 15) // First check if the key was a newline
  {
    System.out.println("\n");
    cmdOut = cmdString; // We do this before so that we don't have to worry about the newline character.
    commandHandler(cmdOut);
    cmdOut = "INIT";
    cmdString = "";
  }
  
  if(!(keyCode == 10) && !(keyCode > 255) && !(keyCode < 0)) // Make sure it's in the ASCII charset and not a newline.
  {
    cmdString += key; // If it passes the test, add the key to the cmdString.
  }
  System.out.print(key);
}


// TEST METHOD
void drawText(int iterator)
{
    fill(0);
    text(("Number of Loops: " + iterator),10,420);
    //delay(1000);
    text(("2x the number of loops: " + (iterator*2)),10,440);
    text("Current Status: Disconnected",10,30);
}
// END TEST METHOD

void clearText()
{
  fill(255);
  rect(0, 399, 1280, 600);
}

void commandHandler(String cmdIn)
{
  System.out.println("Command received: " + cmdIn);
  
  if(cmdIn.equalsIgnoreCase("init"))
  {
    System.out.println("Error! commandHandler was called with the init value!");
  }
  else if(cmdIn.equalsIgnoreCase("help"))
  {
    System.out.println("Available commands: HELP, OPEN, SHUT, RSTL, TWAV, PWAV, PNCH, FING, PEAC, ROCK, EXIT."); 
  }
  else if(cmdIn.equalsIgnoreCase("open"))
  {
    System.out.println("Sending OPEN (open hand) instruction...");
    usbPort.write("xOPEN\n"); 
  }
  else if(cmdIn.equalsIgnoreCase("shut"))
  {
    System.out.println("Sending SHUT (shut hand) instruction..."); 
    usbPort.write("xSHUT\n");
  }
  else if(cmdIn.equalsIgnoreCase("rstl"))
  {
    System.out.println("Sending RSTL (reset loop count) instruction...");
    usbPort.write("xRSTL\n");
  }
  else if(cmdIn.equalsIgnoreCase("twav"))
  {
    System.out.println("Sending TWAV (thumb wave) instruction...");
    usbPort.write("xTWAV\n");
  }
  else if(cmdIn.equalsIgnoreCase("pwav"))
  {
    System.out.println("Sending PWAV (pinkie wave) instruction...");
    usbPort.write("xPWAV\n");
  }
  else if(cmdIn.equalsIgnoreCase("pnch"))
  {
    System.out.println("Sending PNCH (pinch) instruction...");
    usbPort.write("xPNCH\n");
  }
  else if(cmdIn.equalsIgnoreCase("rock"))
  {
    System.out.println("Sending ROCK (rock symbol) instruction..."); 
    usbPort.write("xROCK\n");
  }
  else if(cmdIn.equalsIgnoreCase("peac"))
  {
    System.out.println("Sending PEAC (peace symbol) instruction...");
    usbPort.write("xPEAC\n");
  }
  else if(cmdIn.equalsIgnoreCase("exit"))
  {
    System.out.println("Exiting...");
    System.exit(0);
  }
  else if(cmdIn.equalsIgnoreCase("fing") || cmdIn.equalsIgnoreCase("fing [0-9]++"))
  {
    System.out.println("Enter 'fing' follwed by the determinant servo value (0-4) (pinkie-thumb), followed by the angle to be written (0-180). e.g. fing 3 132 (132° to servo 3)");
  }
  else if(cmdIn.matches("fing [0-9]++ [0-9]++"))
  {
    System.out.println(cmdIn);
    //fingerCmdVar = "";
    String[] fingerCmdVar = cmdIn.split(" "); //Split cmdIn at the spaces. We don't care about index 0. index 1 is the servo value, while index 2 is the angle value.
    
    int servoVar = Integer.parseInt(fingerCmdVar[1]); // Convert servo id to int
    int angleVar = Integer.parseInt(fingerCmdVar[2]); // Convert angle to int

    if(servoVar > 4 || servoVar < 0 || angleVar > 180 || angleVar < 0)
    {
      System.out.println("Invalid angle or servo ID. Syntax is: fing servoID angle");  
      draw();
    }

    System.out.println("Writing angle " +angleVar +" to servo " +servoVar +".");
    usbPort.write("FING"); // Enter FING method
    delay(10); // Wait a little bit
    usbPort.write(servoVar +angleVar); // Write servo and angle
    usbPort.write(9999); // Exit to main loop
  }
  else
  {
    System.out.println("Unknown command! Type 'help' for a command reference.");  
  }
}
