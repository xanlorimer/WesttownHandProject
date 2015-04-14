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
A45
B2
A10
B3
A1
*/

import processing.serial.*; 
import java.util.Scanner; //Import Scanner

  Serial usbPort; // Initialize the serial port

  int intensity = 1; // Amplitude modifier of the graph. Must be scaled *with* thresholds!
  int totalIterations = 0; //How many times has the graph gone through
  int xPos = 1; // Horizontal position for the graphs
  float aMax,bMax,cMax,dMax = 0; // Init maximum values for display purposes
  float aValIn,bValIn,cValIn,dValIn = 0; // Init sensor input values
  float aValInPrevious,bValInPrevious,cValInPrevious,dValInPrevious = 0;
  
  float thresholdA = 50; // Initialize the thresholds
  float thresholdB = 100;
  
  float heightModA,heightModB;
  
  char aValGreater = 'F'; // Is the value GREATER than the threshold or no?
  char bValGreater = 'F'; 
 
// Setup method  
void setup() 
{
  size(1280,400);
    
  usbPort = new Serial(this, Serial.list()[4], 9600); // New serial port on port 4 @9600bd. Autodetect USB/Ask?
  usbPort.bufferUntil('\n'); // Only buffer until we get a newline character
  
  background(0); // Black background
}

// Draw method
void draw() 
{
  // Unused.
}

// serialEvent all of the important stuff happens in here.
void serialEvent(Serial usbPort) 
{
  String rawStringIn = usbPort.readStringUntil('\n'); // Read serial until newline
  
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
    } // End switch
    
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
    
    // Print some debug info
      System.out.println("CURR 'B' VALUE:\t" +bValIn); 
      System.out.println("MAX VALUE:\t" +bMax);
    
      System.out.println("\nIterations:\t" +totalIterations);
      System.out.println("A ACTIVE?: " +aValGreater +"\t" + "B ACTIVE?: " +bValGreater);

    // Connect the dots...
    if (aValInPrevious > aValIn)
      heightModA = (aValInPrevious - aValIn);
    if (aValInPrevious < aValIn)
      heightModA = (aValIn - aValInPrevious); 
    if (aValInPrevious == aValIn)
      heightModA = 0;
      
    if (bValInPrevious > bValIn)
      heightModB = (bValInPrevious - bValIn);
    if (bValInPrevious < bValIn)
      heightModB = (bValIn - bValInPrevious); 
    if (bValInPrevious == bValIn)
      heightModB = 0;


    // Let's draw the lines:
    stroke(255,0,0);
    line(xPos, height - (intensity * aValIn + heightModA), xPos, height - (intensity * aValIn));
    stroke(0,255,0);
    line(xPos, height - (intensity * bValIn + heightModB), xPos, height - (intensity * bValIn));
    
    //stroke(0,0,255); // For a third EMG
    //line(xPos, height - (intensity * cValIn + heightModC), xPos, height - (intensity * cValIn));
    
    // And then let's draw the thresholds:
    stroke(255,0,255);
    line(xPos, height - (thresholdA - 1), xPos, height - (thresholdA));
    stroke(255,0,255);
    line(xPos, height - (thresholdB - 1), xPos, height - (thresholdB));
    
    // If the graph goes over the edge...  
    if(xPos >= width) 
    {
      xPos = 0;
      background(0);
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
