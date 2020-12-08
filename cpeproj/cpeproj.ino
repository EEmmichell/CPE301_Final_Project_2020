//#include <avr/io.h>
//#include <Arduino.h>
#include <dht.h>
#include <LiquidCrystal.h>

/* 
 1) Monitor the water levels in a reservoir and print an alert when the level is too low
    - use water level detection sensor module
 2) Monitor and display the current air temp and humidity on an LCD screen
    - use DHT11 Temperature and Humidity Module
 3) Start and stop a fan motor as needed when the temperature falls out of a specified range(high or low)
    - use Fan blade and 3-6V Motor
 4) Allow a user to use a control to adjust the angle of an output vent from the system
    - 
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

  ---------------------TODO LIST--------------------------------------------------------------
  - be able to adjust the output vent
  - make a clock that tracks the time

*/

int waterLevel = 0;                                                 //water level initialization
int Spin = A0;                                                      //water level sensor pin

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);                              // setting up LCD display
dht DHT;                                                            //setting up the LCD
int dht11 = 4;                                                      //dht temperature and humidifier Digital I/O pin 4 

int motor = 9;                                                      // enable motor at pwm pin 9
int motorIN_1 = 3;                                                  // setup direction M1 direction 0/1 of motor at pwm pin 3
int motorIN_2 = 2;                                                  // setup direction M1 direction 1/0 of motor at pwm pin 2

const int buttonPin = 2;                                            //push button pin
int buttonState = 0;

                                                                    // setting up LED pins through 74HC595 IC
int latchPin = 6;                                                   // Latch pin defined at digital I/O pin 6 on Arduino
int clockPin = 7;                                                   // clock pin defined at digital I/O pin 7 on Arduino
int dataPin = 5;                                                   // data pin defined at digital I/O pin 5 on Arduino
byte leds = 0;


const int ledPinYellow = 4;                                        //LED Disabled
const int ledPinGreen = 2;                                         //LED IDLE
const int ledPinRed = 3;                                            //LED Error
const int ledPinBlue = 1;                                           //LED Running


void setup() {
  Serial.begin(9600);                                               //setup normal delay

 
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
  pinMode(ledPinYellow,OUTPUT);
  pinMode(ledPinGreen,OUTPUT);
  pinMode(ledPinRed,OUTPUT);
  pinMode(ledPinBlue,OUTPUT);
  
}

void loop() {

  leds = 0;                                                         // keep LEDs off until button is pressed
  updateShiftRegister();                                            // reflect the state of the LEDs
 
  buttonState = digitalRead(buttonPin);                             //read push button for on and off
 
 
  waterLevel = analogRead(Spin);                                    //reading the pin into the variable
  Serial.println(waterLevel);                                       //printing the water level to console

  /*if(waterLevel < 80)                                               //80 is arbitrary, will find better value after testing
  {
    lcd.setCursor(1,0);
    lcd.println("THE WATER LEVEL IS LOW");
  }
  */

  //Setup for the LCD display and Humidity/Temperature
  int chk = DHT.read11(dht11);
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");

  
  // Motor threshold test code mock up
  if(DHT.temperature > 20)                                         //when the temperature is above 68 degress fahrenheit, motor will pick up speed
  {
    digitalWrite(motor, 200);
    digitalWrite(motorIN_1, HIGH);
    digitalWrite(motorIN_2, LOW);
  }
  else                                                              //else it wil idle around 100
  {
    digitalWrite(motor, 100);
    digitalWrite(motorIN_1, HIGH);
    digitalWrite(motorIN_2, LOW);
  }

  if(buttonState == LOW)                      //DISABLED
  {
    bitSet(leds, ledPinYellow);                                       // Turn on Yellow LED for disabled until button is pressed
    bitClear(leds);                                                   // Turn off other LEDs
    updateShiftRegister();                                            // reflect the state of the LEDs
    digitalWrite(motorIN_1, LOW);                                     // turn off motor
    digitalWrite(motorIN_2, LOW);
  }
  else                                        //IDLE
  {
    bitSet(leds, ledPinGreen);                                        // Turn on Green LED for Idle until cooler starts running
    bitClear(leds);                                                   // Turn off other LEDs
    updateShiftRegister();                                            // reflect the state of the LEDs
  }
  
  if(waterLevel < 40)                         //ERROR                //not sure what the condition should be for error state
  {
         bitSet(leds, ledPinRed);                                       // Turn on Red LED for error
         bitClear(leds);                                                   // Turn off other LEDs
         updateShiftRegister();                                            // reflect the state of the LEDs

         digitalWrite(motor, 0);                                         //0 is the motor not moving, change up to 255 to switch the speed
         digitalWrite(motorIN_1, LOW);
         digitalWrite(motorIN_2, LOW);
         
         lcd.setCursor(0,1);
         lcd.print("ERROR: WATER LEVEL IS LOW");
  }

  if(DHT.temperature < 8)                   //RUNNING
  {
     bitSet(leds, ledPinBlue);                                       // Turn on Blue LED for running
     bitClear(leds);                                                 // Turn off other LEDs
     updateShiftRegister();                                          // reflect the state of the LEDs
  }
 
  
  delay(1000);                                                      //setup normal delay
  
}

void updateShiftRegister()                                        // function utilized from the Elegoo LED tutorial (Lesson 24)
{
 digital Write(latch Pin,  LOW);                                    // sends data to the 74HC595 IC to change the state of the LEDs
 shiftOut(dataPin, clockPin, LSBFIRST, leds);
 digital Write(latch Pin, HIGH);
}
