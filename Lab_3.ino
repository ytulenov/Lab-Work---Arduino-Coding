//Header Files
#include <Energia.h>
#include <SPI.h>
#include <LCD_screen.h>
#include <LCD_screen_font.h> 
#include <LCD_utilities.h>
#include <Screen_HX8353E.h>
#include <Terminal12e.h>
#include <Terminal6e.h>
#include <Terminal8e.h>
#include <Wire.h>
#include <OPT3001.h>

//Defining peripheral instances
Screen_HX8353E myScreen; //LCD screen instance called "myScreen"
opt3001 lightSensor; //Light sensor instance called "lightSensor"

//Global Variable Definitions
const int Pin_RedLED = 30;      //Constant setting the red LED pin as 30
const int Pin_Backlight = 39;   //Constant setting the backlight pin as 39
const int Pin_Button1 = 33;     //Constant setting the button 1 pin as 33
const int Pin_JoystickX = 2;    //Constant setting the joystick X pin as 2
unsigned long nextMillis;       //Variable for holding the time for the next second tick
unsigned long currentMillis;    //Variable for holding the current time
int hour = 1;                   //Variable for holding the hour, initialized to 1
int minute = 0;                 //Variable for holding the minute, initialized to 0
int second = 0;                 //Variable for holding the second, initialized to 0       
String timeString;              //Variable for holding the time as a string for the display
int state = 0;                  //Variable for holding the state
char bluetoothIn;               //Variable for holding the character read from Bluetooth
char timeSet;                    //Variable for holding the time set value from Bluetooth
char Buffer;

void setup() {                      //Setup function runs once when the board starts
    Serial.begin(9600);             //Initializes the serial connection to the computer at 9600 baud
    Serial1.begin(9600);            //Initializes the serial connection to the Bluetooth module
    myScreen.begin();               //Initializes the LCD screen
    lightSensor.begin();            //Initializes the light sensor
    pinMode(Pin_RedLED,OUTPUT);     //Set the red LED pin as an output pin
    pinMode(Pin_Backlight,OUTPUT);  //Set the backlight pin as an output pin
    pinMode(Pin_Button1,INPUT);     //Set the button 1 pin as an input pin
    pinMode(Pin_JoystickX,INPUT);   //Set the joystick X pin as an input pin
    nextMillis=millis()+1000;       //Initialize nextMillis so it will tick for the first time in a second
    attachInterrupt(Pin_Button1,functionbutton1,CHANGE);    //Attach an interrupt to run functionbutton1 whenever the value from Button 1 changes
}

void loop() {                           //Loop function runs repeatedly while the board is active
    //blink(Pin_RedLED);                //Call the blink() function to blink the red LED
    //delay(1000);                      //Wait for 1000 ms
    currentMillis = millis();           //Update the currentMillis value
    if (currentMillis >= nextMillis) {  //Check if it's time for the clock to tick
        nextMillis += 1000;             //Add 100 to nextMillis to set the next time to tick
        blink(Pin_RedLED);              //Call the blink() function to blink the red LED
        second++;                       //Increment the second counter
        if (second==60) {               //When you reach 60 seconds
            second=0;                   //Reset the second counter
            minute++;                   //And increment the minute counter
            if (minute==60) {           //When you reach 60 minutes
                minute=0;               //Reset the minute counter
                hour++;                 //And increment the hour counter
                if (hour==13) {         //When you reach 13 hours
                    hour=1;             //Reset the hour counter
                }
            }
        }
        //Serial.println("Hour="+i32toa(hour)+",minute="+i32toa(minute)+",second="+i32toa(second)); //Print clock variables to the console
        unsigned long readings = lightSensor.readResult();  //Read from the light sensor and save the result as readings
        //Serial.println(i32toa(readings));                   //Print readings value to the console
        int backlight = map(readings, 0, 500, 50, 255);     //Map the readings value to an 8-bit output for analogWrite()
        backlight = constrain(backlight, 50, 255);          //Constrain the output in case it goes out of range
        analogWrite(Pin_Backlight, backlight);              //Write the output to the backlight pin
        while (Serial1.available() > 0){                            //Check if there's Bluetooth data available
            bluetoothIn = Serial1.read();                           //Read a character from Bluetooth
            if (bluetoothIn == 'H' || bluetoothIn == 'h'){          //If the character is an H
                state = 1;                                          //Go to hour-setting state
                timeSet=0;                                          //Clear timeSet variable
            }
            else if (bluetoothIn == 'M' || bluetoothIn == 'm'){     //M for minute-setting state
                state = 2;
                timeSet=0;
            }
            else if (bluetoothIn == 'S' || bluetoothIn == 's'){     //S for second-setting state
                state = 3;
                timeSet=0;
            }
            else if (bluetoothIn == 'C' || bluetoothIn == 'c'){     //C for clock state
                state = 0;
                timeSet=0;
            }
            if (bluetoothIn >= '0' && bluetoothIn <= '9'){     //If the character is an integer
                Buffer = atoi(&bluetoothIn);                  // Unit Digit
                delay(500);
                bluetoothIn = Serial1.read();                     // Tens Digit
                timeSet = atoi(&bluetoothIn)+(Buffer*10);         // Tnes*10+Unit
            }
            if (timeSet>0) {                                   //Other character, and a non-zero timeSet
                if (state == 1){                                    //In state 1
                    hour = constrain(timeSet,1,12);                 //Limit the values of timeSet and use it to set the hour
                }
                else if (state == 2){                               //State 2
                    minute = constrain(timeSet,0,59);
                }
                else if (state == 3){                               //State 3
                    second = constrain(timeSet,0,59);
                }
                timeSet=0;                                          //Reset timeSet
            }
            Serial.println(bluetoothIn);                            //Print character to console for debugging
        }
    }
    timeString=i32toa(hour,1,0,2)+":"+i32toa(minute,1,0,2)+":"+i32toa(second,1,0,2);
                                //Combine the time numbers into a string separated by colons
                                //Each number with a multiplication factor of 1, no decimal places, two characters
    timeString.replace(": ",":0");  //Time formatting: replaces spaces after colons with zeroes. " 1: 2:19" -> " 1:02:19"
    myScreen.gText(0,0,timeString,whiteColour,blackColour,2,2);
                                //Write string to LCD screen
                                //Coordinates 0,0 (upper left), white text on black background, scaled by 2 in both directions
    myScreen.dRectangle(0,20,myScreen.screenSizeX(),2,blackColour);
                                //Draw a black rectangle to erase the underlines
                                //Coordinates 0,20, same width as the screen, height 2, black colour
    if (state==1) {                                 //Hour set code
        myScreen.dRectangle(0,20,25,2,whiteColour); //Underline the hour number: Coordinates 0,20; size 25x2, white
        int changeBy = analogRead(Pin_JoystickX);   //Read from the joystick to an integer called changeBy
        changeBy = map(changeBy, 0, 4096, -1, 2);   //Map changeBy from its original range to be between -1 and 1
        hour += changeBy;                           //Add changeBy to the hour
        if (hour > 12) {                            //Rollover the hour
            hour = 1;
        }
        else if (hour < 1) {
            hour = 12;
        }
    }
    else if (state==2) {                            //Minute set code
        myScreen.dRectangle(35,20,25,2,whiteColour); 
        int changeBy = analogRead(Pin_JoystickX);   
        changeBy = map(changeBy, 0, 4096, -1, 2);
        minute += changeBy;
        if (minute > 59) {
            minute = 0;
        }
        else if (minute < 0) {
            minute = 59;
        }
    }
    else if (state==3) {                            //Second set code
        myScreen.dRectangle(70,20,25,2,whiteColour); 
        int changeBy = analogRead(Pin_JoystickX);   
        changeBy = map(changeBy, 0, 4096, -1, 2);
        second += changeBy;
        if (second > 59) {
            second = 0;
        }
        else if (second < 0) {
            second = 59;
        }
    }
}

void blink(int pin) {                   //Defines a function called "blink" with no outputs and an integer input called "pin"
    int ledState = digitalRead(pin);    //Read the current value of "pin" and store it in an integer called "ledState"
    digitalWrite(pin,!ledState);        //Write the opposite of "ledState" to "pin"
}

void functionbutton1() {                                            //Interrupt function for button 1
    unsigned long debounceTime;                                     //Initialize debounceTime variable
    if (millis()-debounceTime>500 && digitalRead(Pin_Button1)==0){  //If the button is pressed and it's been a long time since the last change
        if (state==3) {                                             //If the state is 3
            state=0;                                                //roll it over to 0
        }
        else {
            state+=1;                                               //otherwise increase it by 1
        }
    }
    debounceTime=millis();                                          //Update the last time the button changed
}
