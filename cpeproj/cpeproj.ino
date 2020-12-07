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
  - get dht.h header file to work , might need to just add it into the library
  - be able to adjust the output vent
  - make sure the right leds turn on during each state
  - make a clock that tracks the time

*/

int waterLevel = 0;                                                 //water level initialization
int Spin = A5;                                                      //water level sensor pin

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);                              // setting up LCD display
dht DHT;                                                            //setting up the LCD
int dht11 = 7;                                                      //dht temperature and humidifier pin 7 

int motor = 3;                                                      //pwm analog pins

const int buttonPin = 2;                                            //push button pin
int buttonState = 0;                                        
const int ledPinYellow = 12;                                        //LED Disabled
const int ledPinGreen = 13;                                         //LED IDLE
const int ledPinRed = 8;                                            //LED Error
const int ledPinBlue = 7;                                           //LED Running


void setup() {

  lcd.begin(16, 2);                                                 //setup the LCD display

  pinMode(motor, OUTPUT);                                           //setup motor

  pinMode(buttonPin, INPUT);                                        //LED setup
  pinMode(ledPinYellow,OUTPUT);
  pinMode(ledPinGreen,OUTPUT);
  pinMode(ledPinRed,OUTPUT);
  pinMode(ledPinBlue,OUTPUT);
  
  Serial.begin(9600);                                               //setup normal delay

}

void loop() {

  buttonState = digitalRead(buttonPin);                             //read push button for on and off

  waterLevel = analogRead(Spin);                                    //reading the pin into the variable
  Serial.println(waterLevel);                                       //printing the water level to console

  if(waterLevel < 80)                                               //80 is arbitrary, will find better value after testing
  {
    lcd.setCursor(1,0);
    lcd.println("THE WATER LEVEL IS LOW");
  }

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
  if(DHT.tempterature > 20)                                         //when the temperature is above 68 degress fahrenheit, motor will pick up speed
  {
    analogWrite(motor, 200);
  }
  else                                                              //else it wil idle around 100
  {
    analogWrite(motor, 100);
  }

  /*CODE MIGHT NEED TO BE COMBINED FOR FUNCTIONALITY LATER - STILL NEEDS WORK FOR IMPLEMENTATION*/
  if(buttonState == LOW)                      //DISABLED
  {
    digitalWrite(ledPinYellow, HIGH);
    digitalWrite(ledPinRed, LOW);
    digitalWrite(ledPinBlue, LOW);
    digitalWrite(ledPinGreen, LOW);
  }
  else                                        //IDLE
  {
    digitalWrite(ledPinGreen, HIGH);
    digitalWrite(ledPinRed, LOW);
    digitalWrite(ledPinBlue, LOW);
    digitalWrite(ledPinYellow, LOW);
  }
  
  if(waterLevel < 40)                         //ERROR                //not sure what the condition should be for error state
  {
         digitalWrite(ledPinRed, HIGH);
         digitalWrite(ledPinYellow, LOW);
         digitalWrite(ledPinBlue, LOW);
         digitalWrite(ledPinGreen, LOW);

         analogWrite(motor, 0);                                         //0 is the motor not moving, change up to 255 to switch the speed
         
         lcd.setCursor(0,1);
         lcd.print("IN THE ERROR STATE");
  }

  if(DHT.tempterature < 8)                   //RUNNING
  {
     digitalWrite(ledPinBlue, HIGH);
     digitalWrite(ledPinYellow, LOW);
     digitalWrite(ledPinRed, LOW);
     digitalWrite(ledPinGreen, LOW)
  }
 
  
  delay(1000);                                                      //setup normal delay
  
}
