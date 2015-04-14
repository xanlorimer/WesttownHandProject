// Westtown School hand test code v1.0
// Programmed by Xan Lorimer (xanlorimer@gmail.com)
// Code for basic testing of multiple EMGs.

// This code is published under the GNU GPL v2 license.

/*
The purpose of this code will be to test multiple EMG sensors, and
output something useful as the result.

The program should implement averaging before activation, and multiple
thresholds. Different things will happen based on the
threshold reached and the sensor that was activated.
*/

////////////////////////////////////////////////////////////////////////////////////////
// THIS CODE ISN'T COMPLETELY TESTED AS OF 3/30/15. TESTED TO COMPILATION BUT NO MORE //
////////////////////////////////////////////////////////////////////////////////////////

// Declare variables
int averageTotal = 10; // Number of samples to take for the average

int loopDelay = 50; // Determines how often (in ms) we send data over comms
int readDelay = 5; 

// Activation thresholds
int aThreshold = 50;
int bThreshold = 100;
int cThreshold = 50;
int dThreshold = 100;

// Pre thresholds
int aPreThreshold = 45;
int bPreThreshold = 95;
int cPreThreshold = 45;
int dPreThreshold = 95;

// Initialize a few things for averaging
int sensorOutA;
int sensorOutB;
int average;

boolean debugMode = true;

//int [a-z] // Reserved for iteration in for loops.

void setup()
{
  pinMode(0, INPUT); // EMG sensors (two for now)
  pinMode(1, INPUT); 
  
  Serial.begin(9600); // So that we can get a result
}

void loop()
{
  // Be sure to allow for processing to hook in and graph each EMG sensor separately.
  delay(loopDelay); // Wait
  
  sensorOutA = analogRead(0); // Read sensor A
  sensorOutB = analogRead(1); // Read sensor B
  
  if (debugMode == true) // Debug block. Prints raw serial values to be graphed in Processing.
  {
    Serial.print('A');
    Serial.println(sensorOutA);
    
    Serial.print('B');
    Serial.println(sensorOutB);
  }
  
  if(sensorOutA >= aPreThreshold) 
  {
    if(sensorOutA >= bPreThreshold)
    {
      actionB();
    }
    else
    {
      actionA();
    }
  }
  
  if(sensorOutB >= cPreThreshold)
  {
    if(sensorOutB >= dPreThreshold)
    {
      actionD();
    }
    else
    {
      actionC();
    }
  }
}

// Threshold activation code:
void actionA()
{
  average = 0;
  for(int a=1; a < averageTotal; a++)
  {
    average = ((average + analogRead(0)) / a); // Sum of the analog reads over averageTotal iterations, divided by the number of iterations.
    if(average < (0.5 * aThreshold)) // If the average drops below half of aThreshold, cancel and loop.
    {
      loop();
    }
    delay(readDelay); // Delay for a short amount of time.
  }
  if(average > aThreshold)
  {
    // handActionA(); // Non-test code would be similar to this.
    Serial.print('w'); // Identifier so that we can graph this in Processing
    Serial.println(average); 
  }
  loop(); // This line may be unnecessary
}

void actionB()
{
  average = 0;
  for(int b=1; b < averageTotal; b++)
  {
    average = ((average + analogRead(0)) / b); // Sum of the analog reads over averageTotal iterations, divided by the number of iterations.
    if(average < (0.5 * bThreshold)) // If the average drops below half of the threshold, cancel and loop.
    {
      loop();
    }
    delay(readDelay); // Delay for a short amount of time.
  }
  if(average > bThreshold)
  {
    // handActionB(); // Non-test code would be similar to this.
    Serial.print('x'); // Identifier so that we can graph this in Processing
    Serial.println(average); 
  }
  loop(); // This line may be unnecessary
}

void actionC()
{
  average = 0;
  for(int c=1; c < averageTotal; c++) // Insert witty comment about incrementing c
  {
    average = ((average + analogRead(0)) / c); // Sum of the analog reads over averageTotal iterations, divided by the number of iterations.
    if(average < (0.5 * cThreshold)) // If the average drops below half of the threshold, cancel and loop.
    {
      loop();
    }
    delay(readDelay); // Delay for a short amount of time.
  }
  if(average > cThreshold)
  {
    // handActionC(); // Non-test code would be similar to this.
    Serial.print('y'); // Identifier so that we can graph this in Processing
    Serial.println(average); 
  }
  loop(); // This line may be unnecessary
}

void actionD()
{
  average = 0;
  for(int d=1; d < averageTotal; d++)
  {
    average = ((average + analogRead(0)) / d); // Sum of the analog reads over averageTotal iterations, divided by the number of iterations.
    if(average < (0.5 * dThreshold)) // If the average drops below half of the threshold, cancel and loop.
    {
      loop();
    }
    delay(readDelay); // Delay for a short amount of time.
  }
  if(average > dThreshold)
  {
    // handActionD(); // Non-test code would be similar to this.
    Serial.print('z'); // Identifier so that we can graph this in Processing
    Serial.println(average); 
  }
  loop(); // This line may be unnecessary
}

// Hand actions would go here

/*
Now that the code is written, we should figure out how to make it better! Just like writing an essay.

...but a fun essay. :)


Can we combine all of the sensing code into one method?
  - What if we passed the threshold ID to the method as a,b,c, or d? Then we could average it quite
  easily.
  - A problem arises: What if we have different thresholds? 
  - Can we vary the threshold that is used, based on the value triggered?
     - Variable variables... Unsure of how to implement this right now... Probably can't do this in C
     - Can we listen to multiple activation events at the same time? Is the delay too small for us to worry about this?
*/
