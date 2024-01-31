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
unsigned long nextMillis;       //Variable for holding the time for the next second tick
unsigned long currentMillis;    //Variable for holding the current time
int hour = 0;                   //Variable for holding the hour, initialized to 1
int minute = 0;                 //Variable for holding the minute, initialized to 0
int second = 0;                 //Variable for holding the second, initialized to 0       
String timeString;              //Variable for holding the time as a string for the display

void setup() {                      //Setup function runs once when the board starts
    Serial.begin(9600);             //Initializes the serial connection to the computer at 9600 baud
    myScreen.begin();               //Initializes the LCD screen
    lightSensor.begin();            //Initializes the light sensor
    pinMode(Pin_RedLED,OUTPUT);     //Set the red LED pin as an output pin
    pinMode(Pin_Backlight,OUTPUT);  //Set the backlight pin as an input pin
    nextMillis=millis()+1000;       //Initialize nextMillis so it will tick for the first time in a second
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
        Serial.println(i32toa(readings));                   //Print readings value to the console
        int backlight = map(readings, 0, 500, 50, 255);     //Map the readings value to an 8-bit output for analogWrite()
        backlight = constrain(backlight, 50, 255);          //Constrain the output in case it goes out of range
        analogWrite(Pin_Backlight, backlight);              //Write the output to the backlight pin
    }
    timeString=i32toa(hour,1,0,2)+":"+i32toa(minute,1,0,2)+":"+i32toa(second,1,0,2);
                                //Combine the time numbers into a string separated by colons
                                //Each number with a multiplication factor of 1, no decimal places, two characters
    timeString.replace(": ",":0");  //Time formatting: replaces spaces after colons with zeroes. " 1: 2:19" -> " 1:02:19"
    myScreen.gText(0,0,timeString,whiteColour,blackColour,2,2);
                                //Write string to LCD screen
                                //Coordinates 0,0 (upper left), white text on black background, scaled by 2 in both directions
}

void blink(int pin) {                   //Defines a function called "blink" with no outputs and an integer input called "pin"
    int ledState = digitalRead(pin);    //Read the current value of "pin" and store it in an integer called "ledState"
    digitalWrite(pin,!ledState);        //Write the opposite of "ledState" to "pin"
}
