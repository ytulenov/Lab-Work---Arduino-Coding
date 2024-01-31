
//Global Variable Definitions
const int Pin_RedLED = 30;	//Constant setting the red LED pin as 30
unsigned long nextMillis;	//Variable for holding the time for the next second tick
unsigned long currentMillis;	//Variable for holding the current time

void setup() {			//Setup function runs once when the board starts
    pinMode(Pin_RedLED,OUTPUT);	//Set the red LED pin as an output pin
    nextMillis=millis()+1000;	//Initialize nextMillis so it will tick for the first time in a second
}

void loop() {				//Loop function runs repeatedly while the board is active
  //blink(Pin_RedLED);			//Call the blink() function to blink the red LED
  //delay(1000);			//Wait for 1000 ms
  currentMillis = millis();		//Update the currentMillis value
  if (currentMillis >= nextMillis) {	//Check if it's time for the clock to tick
      nextMillis += 1000;		//Add 1000 to nextMillis to set the next time to tick
      blink(Pin_RedLED);		//Call the blink() function to blink the red LED
  }
}

void blink(int pin) {			//Defines a function called "blink" with no outputs and an integer input called "pin"
    int ledState = digitalRead(pin);	//Read the current value of "pin" and store it in an integer called "ledState"
    digitalWrite(pin,!ledState);	//Write the opposite of "ledState" to "pin"
}