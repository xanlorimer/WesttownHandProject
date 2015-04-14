// Westtown School 3D printed hand code v3.0.0
// Programmed by Xan Lorimer (xanlorimer@gmail.com)
// This is the main control code for the hand.
// All debug functions can be commented out/disabled.

// Attached to Arduino (eventually): 3 EMGs(in), 5 POTs(in), 5 Motors(out)

// This code is published under the GNU GPL v2 license.

////////////////////////////////////////////////////////////////////////////////////////
// THIS CODE ISN'T COMPLETELY TESTED AS OF 4/14/15. TESTED TO COMPILATION BUT NO MORE //
////////////////////////////////////////////////////////////////////////////////////////

// Let's begin, shall we?

// First, we'll declare some variables:
// Some frequently changed variables:
int loopDelay = 10; // Amount in ms for the main loop delay
int readDelay = 50; // How long to wait after a threshold trigger
int aThreshold = 50; // EMG intensity before first trigger action (instant)
int bThreshold = 70; // EMG intensity before second trigger action (gradual)
int fingerDeathHigh = 511; // Upper pot value limit (out of 1023) UNTESTED
int fingerDeathLow = 127; // Upper pot value limit (out of 1023) UNTESTED

// Initialize Arduino PWM output pins. One for each motor controller.
int oPin0 = 3; // MTR1
int oPin1 = 5; // MTR2
int oPin2 = 6; // MTR3
int oPin3 = 9; // MTR4
int oPin4 = 10; // MTR5

// Initialize local output values for data processing
int potIn; // Potentiometer output
static int emgIn[3]; // EMG output array
int index; // Used for motor activation

// Debug initializations
char serialChar; 
String serialString; // The string we get from the serial connection

// Setup
void setup() 
{
  // Declare the inputs
  for(int i=0; i<8; i++)
  {
    pinMode(i, INPUT); // Set all analog pins as input
  }
  
  // Declare the outputs (motors).
  pinMode(oPin0, OUTPUT); // Pinky
  pinMode(oPin1, OUTPUT);
  pinMode(oPin2, OUTPUT); // ...
  pinMode(oPin3, OUTPUT);
  pinMode(oPin4, OUTPUT); // Thumb

  // Begin the serial connection for debug
  Serial.begin(9600); 
}


// Main loop
void loop()
{
  /* 
  This method needs a lot of work.
  We have to read each emg first. If the value of any given EMG is greater than the primary and/or
  secondary activation thresholds, we'll activate the motorCtrl class with a specific angle to write.
  */
  // Read EMGs
  // If any EMG value is > threshold, set index = iterator and call the motorset method.
  for(int a=0; a<3; a++)
  {
    emgIn[a] = emgReader(a); // We get the EMG value of the EMG on pin (a)
    Serial.println(emgIn[a]);
    
    if(emgIn[a] >= aThreshold) 
    {
      emgReader(a);
      
      /*
      Averaging...
      // Let's do the following:
      for (int c = 0; c < averageTotal; c++) // [Insert unfunny joke about C++]
      {
        average = ((average + analogRead(index)) / c); // Average = sum of reads divided by number of reads
        delay(averageDelay)
      }
      
      if(average > bThreshold)
        //action1();
      if(average > aThreshold)
        //action2();
      
      */
    }
  }
  delay(loopDelay);
  if(Serial.available()) // If there's a char in the buffer
    debugCmd(); // Run debug command method
}


// Control & Read methods
int potReader(int index) // Reads the potentiometer
{
  potIn = analogRead(index+3); // Store each pot value in the array. j+3 because potpins start at 3.
  return potIn;
}

int emgReader(int index) // Reads the EMG
{
  int emgRaw = analogRead(index); // We read each EMGs value and store it in an array.
  return emgRaw;
}

void motorCtrl(int potIn, int index) // Activates the motor
{
  // I don't know what motor controllers I'm going to use, so this section will remain blank for now.
  // In any case, we can implement anti-motor-eating code here:
  if((potReader(index) > fingerDeathHigh) || (potReader(index) < fingerDeathLow))
  {
    loop(); // Just return to the loop. 
    // Need to add something to tell the motor to compensate so that we don't loop endlessly.
  }
  if((potReader(index) < fingerDeathHigh) || (potReader(index) > fingerDeathLow))
  {
    //openHandGradual(); //...
  }
}


// Hand "write" methods:
void openHandGradual(int *potIn, int index)
{
  
}

void shutHandGradual(int *potIn, int index)
{
  
}

void openHandInstant(int *potIn, int index)
{
  
}

void shutHandInstant(int *potIn, int index)
{
  
}


// Debug methods:
// Remove or comment out the code below in the final version of the hand. Or don't.

String serialIn() // Method to get manually entered serial data.
{          
    if (Serial.available()) // If there's a character in the serial buffer
    {
      if(serialChar != (' ' || '\t')) // Remove whitespace
      { 
        serialChar = (char)Serial.read(); // Read the character
        serialString += serialChar; // Add the character to the string
      }
    
    if (serialChar == '\n') // Return value on newline
    { 
      return serialString; // Return whatever the full input string was.
    } 
  }
  else
  {
    serialIn(); // Recurse
  }
}

void debugCmd()
{
  String commandString = serialIn();
    if(commandString.equalsIgnoreCase("command1"))
    {
      // Command action
    }
    if(commandString.equalsIgnoreCase("command2"))
    {
      // Command action
    }
    else
    {
      Serial.println("Unknown instruction. Check the source code or documentation.");
    }
    commandString = "";
}


/*
Notes:

Before more code can be written, we should figure out the precise hand functions, as well as whether
or not we can read and use multiple EMG signals. 

Open hand/shut hand gradually or instantly using one sensor.
Operate individual fingers or have pre-defined poses?
  *Maybe a "locking" pot for finger select?
  *Hand functions are a huge thing to go over. For now, we should just have two functions. 
     *Also we should do tests with multiple EMGs. Should set up a meeting with hand recipient soon.
Let's implement "motorCtrl" as more of a checker, rather than a writer. Maybe.
Or we could have it increment stuff? I don't know, it just seems like a lot of work.

In any case, we really need to actually get the motors and pots and stuff, so this code will remain
unwritten and untested until actual testing can be done.

*/
