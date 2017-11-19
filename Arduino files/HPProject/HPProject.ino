/** Sketch for the central Simblee device                       **/
/** This central device listens to commands on the Serial lines **/
/** and broadcasts the command on SimbleeCOM                    **/

#include "SimbleeCOM.h"

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  SimbleeCOM.begin();
}

void loop() 
{
  //Continuously listen over Serial
  if(Serial.available())
  {
    String input = Serial.readStringUntil('\n');
    Serial.println(input);
    if(input == "FLIP_BLINDS" 
         || input == "LOCOMOTOR" 
         || input == "UpdateBoard" 
         || input == "1"
         || input == "2"
         || input == "3"
         || input == "!"
         || input == "@"
         || input == "#")
    {
      Serial.println("Sending spell");
      char inputCharArray[input.length()+1];
      input.toCharArray(inputCharArray, input.length()+1);  //SimbleeCOM.send() takes a char array as payload argument
      SimbleeCOM.send(inputCharArray, sizeof(inputCharArray)); 
    } 
  }
}

