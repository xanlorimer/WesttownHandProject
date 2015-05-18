// This program is for debugging and testing the prosthetic hand...
// Written by Xan Lorimer (xanlorimer@gmail.com), using graphing code by Tom Igoe

// Features:
// Read & Graph all devices separately

// Planned: 
// Manually input motor values and activate methods
// Manually reset certain variables (maybe).
//   Like thresholds.

// To do:
// Make it so that we write to the same file based on the date. Right now it overwrites/screws up the old file, so I've increased how accurate the filename is in regards to the date.
// (Higher likelihood that it will create a new file rather than overwriting data)

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
int R = 255; // Change cleartext method to just use white... or maybe change blue to 255?
int G = 0;
int B = 255;

int servoVar;
int fingerCmdVar;
int angleVar = 1337;

int iterator = 0;
String cmdOut = "INIT";
// END TEST VARIABLES

  String fileName = new SimpleDateFormat("yyyy-MM-dd-hh-mm'.txt'").format(new Date()); // For now...
  PFont font;

  Serial usbPort; // Initialize the serial port
  PrintWriter fileWrite;

  int intensity = 1; // Amplitude modifier of the graph. Scaled with thresholds! (Test feature)
  int totalIterations = 0; //How many times has the graph gone through
  int xPos = 1; // Horizontal position for the graphs
  float aMax,bMax,cMax = 0; // Init maximum values for display purposes
  float aValIn,bValIn,cValIn = 0; // Init sensor input values
  float aValInPrevious,bValInPrevious,cValInPrevious = 0;
  
  float thresholdA = (350 * intensity); // Initialize the thresholds
  float thresholdB = (300 * intensity);
  float thresholdC = (250 * intensity);
  
  float heightModA,heightModB,heightModC; 
 
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

  background(0); // Black background
  
  fill(255);  
  rect(0, 399, 1280, 600); // Draw info area
  
  drawThresholds();
  drawText(3, totalIterations);
}

// Draw method (loop)
void draw() 
{
  
}

// serialEvent - all of the important stuff happens in here. (Doing everything in serialEvent is probably not the best practice, but whatever. Todo, eventually.)
void serialEvent(Serial usbPort) 
{
  String rawStringIn = usbPort.readStringUntil('\n'); // Read serial until newline
  
  fileWrite.print(rawStringIn); // Attempt to write the data to the log file.
  
  if(rawStringIn != null) // If the raw input is not null,
  { 
    rawStringIn = trim(rawStringIn); // Trim off the newline character
    
    // This switch determines what we do with the data that we receive
    switch(rawStringIn.charAt(0)) // Get the determinant character (index: 0)
    { 
      case 'A': // If the ID is A (sensor A)
        aValIn = float(removeCharAt(rawStringIn,0)); // Cast aValIn as a float
        break;
      case 'B': // If the ID is B (sensor B)
        bValIn = float(removeCharAt(rawStringIn,0));
        break;
      case 'C': // If the ID is C (sensor C)
        cValIn = float(removeCharAt(rawStringIn,0));
        break;
      default:
        System.out.println("ERROR: Unknown sensor ID! Check the code!");
        break; // Something else? Error info? Blink LED?
    }   
        
    // This sets the max value that we've seen:
    if(aValIn >= aMax) 
    {
      aMax = aValIn;
      clearText(0);
      drawText(0, aMax);
    }
    
    if(bValIn >= bMax) 
    {
      bMax = bValIn;
      clearText(1);
      drawText(1, bMax);
    }
    
    if(cValIn >= cMax)
    {
      cMax = cValIn;
      clearText(2);
      drawText(2, cMax);
    }
    
    clearText(4);
    drawText(4, aValIn);
    
    clearText(5);
    drawText(5, bValIn);
    
    clearText(6);
    drawText(6, cValIn);

    // Connect the dots: (Comment out this code to see what I mean.)
    if (aValInPrevious > aValIn) 
      heightModA = (aValInPrevious - aValIn);
    else if (aValInPrevious < aValIn)
      heightModA = -(aValIn - aValInPrevious); 
    else if (aValInPrevious == aValIn)
      heightModA = 0;
      
    if (bValInPrevious > bValIn)
      heightModB = (bValInPrevious - bValIn);
    else if (bValInPrevious < bValIn)
      heightModB = -(bValIn - bValInPrevious); 
    else if (bValInPrevious == bValIn)
      heightModB = 0;
      
    if (cValInPrevious > cValIn)
      heightModC = (cValInPrevious - cValIn);
    else if (cValInPrevious < cValIn)
      heightModC = -(cValIn - cValInPrevious); 
    else if (cValInPrevious == cValIn)
      heightModC = 0;


    // Let's draw the lines:
    stroke(255,0,0);
    line(xPos, ((height - 200) - (intensity * aValIn + heightModA)), xPos, ((height - 200) - (intensity * aValIn)));
    
    stroke(255,255,0);
    line(xPos, ((height - 200) - (intensity * bValIn + heightModB)), xPos, ((height - 200) - (intensity * bValIn)));
    
    stroke(0,255,0); // For a third EMG
    line(xPos, ((height - 200) - (intensity * cValIn + heightModC)), xPos, ((height - 200) - (intensity * cValIn)));
    
    aValInPrevious = aValIn;
    bValInPrevious = bValIn;
    cValInPrevious = cValIn;
    
    // If the graph goes over the edge, we'll erase the graph portion of everything and start over.  
    if(xPos >= width) 
    {
      xPos = 0;
      fill(0);
      stroke(0);
      rect(0, 0, 1280, 400);
      drawThresholds(); // Redraw thresholds
      totalIterations++;
      clearText(3);
      drawText(3, totalIterations);
    }
    else 
    {
      xPos++; // Increment x position
    }    
  }
  // Ignore input if null.
}
    
    
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
void drawText(int instruction, float input)
{
    fill(0); // Black text
        
    switch(instruction)
    {
      case 0: // Draw max a value
        text(("Max A Value: " +input), 10, 420);
      break;
      case 1: // Draw max b value
        text(("Max B Value: " +input), 10, 435);
      break;
      case 2: // Draw max c value
        text(("Max C Value: " +input), 10, 450);
      break;
      
      case 3: // Draw number of iterations
        text(("Graph Iterations: " +input), 10, 480);
        break;
        
      case 4:
        text(("A Value: " +input), 10, 510);
        break;
      case 5:
        text(("B Value: " +input), 10, 525);
        break;
      case 6:
        text(("C Value: " +input), 10, 540);
        break;
        
      default:
      text(("Unknown Instruction."),10,495);
    }
}

void clearText(int instruction)
{
  //fill(0);
  stroke(255);
  switch(instruction)
  {
    case 0:
      fill(R,G,B);
      rect(5, 405, 170, 15); // ((xpos1, ypos1),(xposrelative, yposrelative)
      break;
    case 1:
      fill(R,G,B);
      rect(5, 420, 170, 15); 
      break;
    case 2:
      fill(R,G,B);
      rect(5, 435, 170, 15);
      break;
    case 3:
      fill(R,G,B);
      rect(5, 465, 170, 15);
      break;
    
    case 4:
      fill(R,G,B);
      rect(5, 495, 170, 15);
      break;
    case 5:
      fill(R,G,B);
      rect(5, 510, 170, 15);
      break;
    case 6:
      fill(R,G,B);
      rect(5, 525, 170, 15);
      break;
    
    default:
      fill(R,G,B);
      rect(0, 399, 1280, 600);
      break;
  }
  //rect(0, 399, 1280, 600);
}

void drawThresholds() // This method is called both at the beginning and whenever the graph reaches the edge.
{
    stroke(127,0,0); // Shade of red for A thresh
    line(0, thresholdA, 1280, thresholdA);
    stroke(0,127,0); // Shade of green for B thresh
    line(0, thresholdB, 1280, thresholdB);
    stroke(0,0,127); // Shade of blue for C thresh
    line(0, thresholdC, 1280, thresholdC);
    stroke(0);
    line(0, 402, 1280, 402); // Draw separator line
}

// To handle debug commands to the hand
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
  else if(cmdIn.equalsIgnoreCase("mark"))
  {
    stroke(255,0,255);
    line(xPos, 400, xPos, 0);
    fileWrite.println("#");
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
