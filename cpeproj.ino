#include "dht.h"
#include <LiquidCrystal.h>
#include <Time.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Wire.h>

/* 
 1) Monitor the water levels in a reservoir and print an alert when the level is too low
    - use water level detection sensor module
 2) Monitor and display the current air temp and humidity on an LCD screen
    - use DHT11 Temperature and Humidity Module
 3) Start and stop a fan motor as needed when the temperature falls out of a specified range(high or low)
    - use Fan blade and 3-6V Motor
 4) Allow a user to use a control to adjust the angle of an output vent from the system
    - manual adjust (no code implementation)
 5) Allow as user to enable or disable the system using an on/off button
    - use push button 
    - Yellow/Disabled , Green/Idle , Red/Error, Blue/Running
 6) Record the time and date every time the motor is turned on or off. This information should 
     be transmitted to a host computer (over USB)
     - user timer exmples in avr-source code??

 extra:
    - https://github.com/klancaster-unr/avr-source
    - at some point an analog signal needs to be converted to digital (MAYBE)
    -https://www.youtube.com/watch?v=n7WRi5U5lQk&t=101s SETTING UP ARDUINO WATER SENSOR
    - https://www.circuitbasics.com/how-to-set-up-the-dht11-humidity-sensor-on-an-arduino/ HUMIDITY SENSOR SETUP
    -https://www.youtube.com/watch?v=G2WJvblxAGQ DC MOTOR SETUP

  File was created by Jennings Topps version: 1.0 (12/6/2020)
  File was modified by Miguel Michell version: 1.1 (12/9/2020)

*/

int waterLevel = 0;                                                 //water level initialization
int Spin = A0;                                                      //water level sensor pin

//LiquidCrystal lcd(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);                            // setting up LCD display                                                            //setting up the LCD
#define dht_analogPin A1                                            // directing the humidity/temperature sensor to
dht DHT;                                                            // analog pin A1

int motor = 4;                                                      // enable motor at pwm pin 1
int motorIN_1 = 3;                                                  // setup direction M1 direction 0/1 of motor at pwm pin 3
int motorIN_2 = 2;                                                  // setup direction M1 direction 1/0 of motor at pwm pin 2

const int buttonPin = 50;                                            //push button pin to be read from digital I/O pin 9
int buttonState = 0;

                                                                    // setting up LED pins through 74HC595 IC
int latchPin = 6;                                                   // Latch pin (ST_CP) defined at digital I/O pin 6 on Arduino
int clockPin = 7;                                                   // clock pin (SH_CP) defined at digital I/O pin 7 on Arduino
int dataPin = 5;                                                    // data pin (DS) defined at digital I/O pin 5 on Arduino


int ledPinYellow;                                         //LED Disabled
int ledPinGreen;                                          //LED IDLE
int ledPinRed;                                            //LED Error
int ledPinBlue;                                           //LED Running

const char *monthName[12] = {                                       // establish time elements for RTC module
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;                                                    // variable used for time

bool getDate(const char *str)                                       // function utilized from DS1307RTC/examples/SetTime/SetTime.ino
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

 bool getTime(const char *str)                                // function utilized from DS1307RTC/examples/SetTime/SetTime.ino
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

boolean shiftRegister[8];
void writeReg()
{
  digitalWrite(clockPin, LOW);
  
  for (int i = 0; i<4; i++)
  {
    digitalWrite(latchPin, LOW);
    digitalWrite(dataPin, shiftRegister[i]);
    digitalWrite(latchPin, HIGH);
  }
  digitalWrite(clockPin, HIGH);
}

void setup() {
  // begin initial setup
 
  lcd.begin(16, 2);                                                 //setup the LCD display
  
  pinMode(motor, OUTPUT);                                           //setup motor
  pinMode(motorIN_1, OUTPUT);                                       // initialize direction of motor for IN_1, and IN_2
  pinMode(motorIN_2, OUTPUT);
  digitalWrite(motorIN_1, LOW);                                     // motor should be off initially
  digitalWrite(motorIN_2, LOW);

  pinMode(latchPin,  OUTPUT);                                       // initializing latch pin on 74HC595 for LED setup
  pinMode(dataPin,  OUTPUT);                                        // initializing data pin on 74HC595  for LED setup
  pinMode(clockPin, OUTPUT);                                        // initializing clock pin on 74HC595 for LED setup
  pinMode(buttonPin, INPUT);                                        // push button setup

  bool parse=false;                                                 // boolean variables used for time functions
  bool config=false;

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {                     // record the date and time
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
     }
    }

    Serial.begin(9600);         //setup normal delay
    while (!Serial) ;
    // print the date and time to the LCD display
    lcd.setCursor(0,0);
    lcd.print("Time is: ");
    lcd.setCursor(0,1);
    lcd.print(__TIME__);
    delay(2000);
    lcd.setCursor(0,0);
    lcd.print("Today's date:");
    lcd.setCursor(0,1);
    lcd.print(__DATE__);
    delay(2000);
    lcd.clear();
  
} // end of initial setup

void loop() {
  // begin program
  
    digitalWrite(latchPin, LOW);
    digitalWrite(dataPin, shiftRegister[2]);
    digitalWrite(latchPin, HIGH);
  //digitalWrite(dataPin, HIGH);
  //writeReg();
 
  buttonState = digitalRead(buttonPin);                             //read push button for on and off
  buttonState = HIGH;
 
  waterLevel = analogRead(Spin);                                    //reading the water sensor analog pin                                     


  //Setup for the LCD display and Humidity/Temperature
  DHT.read11(A2);
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);                   //character for degrees
  lcd.print("C");
  lcd.setCursor(0,1); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature*9/5 + 32 );    // conversion from Celsius to Farenheit
  lcd.print((char)223);                   
  lcd.print("F");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.setCursor(0,1);
  lcd.print(DHT.humidity);
  lcd.print("%");
  delay(3000);
  lcd.clear();
  
  
  // Motor threshold test code mock up
  if(DHT.temperature >= 20)                                         // when the temperature is above 20 degress Celsius,
  {                                                                // motor will pick up speed
    digitalWrite(motor, 200);
    digitalWrite(motorIN_1, HIGH);
    digitalWrite(motorIN_2, LOW);
  }
  else                                                             // else, motor will still run around 100 when idle
  {
    digitalWrite(motor, 0);
    digitalWrite(motorIN_1, HIGH);
    digitalWrite(motorIN_2, LOW);
  }

  if(buttonState == LOW)                      //DISABLED
  {
    lcd.setCursor(0,0);                                               // display system state indicator on LCD
    lcd.print("SWAMP COOLER");
    lcd.setCursor(0,1);
    lcd.print("MODE: DISABLED");
    delay(3000);
    lcd.clear();
    //bitSet(leds, ledPinYellow);                                       // Turn on Yellow LED for disabled
    //updateShiftRegister();
    //bitClear(leds, 0);                                              // Turn off other LEDs
    //updateShiftRegister();                                          // reflect the state of the LEDs
    digitalWrite(motor, 0);                                           // turn off motor
    digitalWrite(motorIN_1, LOW);                                     
    digitalWrite(motorIN_2, LOW);
  }
  //else                                        //IDLE
  //{
    //bitSet(leds, ledPinGreen);                                        // Turn on Green LED for Idle until cooler starts running
    //bitClear(leds, 0);                                                // Turn off other LEDs
    //updateShiftRegister();                                            // reflect the state of the LEDs
  //}

  
  if(waterLevel <= 100)                       //ERROR              
  {
         //bitSet(leds, ledPinRed);                                     // Turn on Red LED for error
         //bitClear(leds, 0);                                           // Turn off other LEDs
         //updateShiftRegister();                                       // reflect the state of the LEDs

         digitalWrite(motor, 0);                                      // 0 is the motor not moving, change up to 255 to switch
         digitalWrite(motorIN_1, LOW);                                // to switch the speed
         digitalWrite(motorIN_2, LOW);
         
         lcd.setCursor(0,0);
         lcd.print("ERROR: WATER");                             // indicate water level on LCD display
         lcd.setCursor(0,1);
         lcd.print("LEVEL IS LOW");
         delay(3000);
         lcd.clear();
  }
  
  else if (waterLevel > 100 && waterLevel <= 300){
         lcd.print("WATER: LOW");                             // indicate water level on LCD display
         delay(3000);
         lcd.clear();                                // indicate other water level ranges on display
  }
  else if (waterLevel > 300 && waterLevel <= 330){
         lcd.print("WATER: MEDIUM");
         delay(3000);
         lcd.clear();
  }
  else if (waterLevel > 330){
         lcd.print("WATER: HIGH");
         delay(3000);
         lcd.clear();
  }
  //delay(3000);
  

  if(DHT.temperature > 30)                     //RUNNING
  {
     lcd.setCursor(0,0);                                             // display system indicator on LCD
     lcd.print("SWAMP COOLER");
     lcd.setCursor(0,1);
     lcd.print("MODE: RUNNING");
     delay(3000);
     lcd.clear();
     //bitSet(leds, ledPinBlue);                                       // Turn on Blue LED for running
     //bitClear(leds, 0);                                              // Turn off other LEDs
     //updateShiftRegister();                                          // reflect the state of the LEDs
  }

  
  //delay(3000);                                                      //setup normal delay
} // end of loop
