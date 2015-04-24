# Westtown Hand Project

#Information:
A repository of code for an Arduino-based, EMG controlled prosthetic hand.

Most of the code is untested as of right now. That doesn’t mean it won’t work, though!



#Status:
mainHandCode.ino is untested and not working.

handCodeServo.ino is untested but probably working. It will be removed in the future.

multiEMGServoTest.ino is being actively developed. Untested.

multiEMGTest.ino will probably be removed in the future. Untested.

servoWriterTest.ino will be removed in the future. It should be working.


handDebugProcessing.pde is working, but only tested with certain utilities. (multiEMGServoTest and multiEMGTest)



#Purpose:
mainHandCode:

  The main hand code. This is what will be loaded on the final version of the hand. Currently just a framework.


handCodeServo:

  The old hand code. This program will no longer be used. (In favor of multiEMGServoTest.)


handDebugProcessing:

  The processing code. Draws pretty graphs. Will have commands and data saving in the future.
  
  
multiEMGServoTest:

  A program to test multiple EMGs at the same time to control five servos. Will be used quite a bit in testing.
  
  
multiEMGTest:
  
  A program to get value averaging working correctly.
  
  
servoWriterTest:

  - A program to manipulate servos. Abandoned in favor of multiEMGServoTest.



#Commands:
  multiEMGServoTest:
  
    - OPEN: Opens the hand. Sets all servos to their open values.
    
    - SHUT: Shuts the hand. Sets all servos to their shut values.
    
    - RSTL: Resets the loop counter for data collection.
    
    - TWAV: Thumb wave. Finger wave starting with the thumb.
    
    - PWAV: Pinkie wave. Finger wave starting with the pinkie.
    
    - FING: Finger control. Allows individual fingers to be controlled.
    
    - PNCH: "Pinch" state. Closes thumb and index finger.
    
    - HELP: Displays commands available, without descriptions.


  handCodeServo:
    
    - ASET: Angle set. Allows main servo angle to be set.
    
    - OPEN: Opens the hand.
    
    - SHUT: Shuts the hand.
    
    
  servoWriterTest:
  
    - T: Toggles the servo state.
    
    - S: Sets the servo state. Usage: Sa, where a is an angle between 0 and 180.
    
    - R: Read. Reads the current angle.
    
    
  mainHandCode:
    
    - command1: Placeholder command. Currently does nothing.
    
    - command2: Placeholder command. Currently does nothing.
