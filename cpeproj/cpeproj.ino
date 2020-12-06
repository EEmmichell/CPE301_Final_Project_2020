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
    - at some point an analog signal needs to be converted to digital
*/

int waterLevel = 0; //water level initialization
int Spin = A5; //water level sensor pin

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // setting up LCD display
dht DHT; //setting up the LCD
int dht11 = 7; //dht temperature and humidifier pin 7 

int motor = 3; //pwm analog pins

void setup() {

  lcd.begin(16, 2); //setup the LCD display

  pinMode(motor, OUTPUT);
  
  Serial.begin(9600); //setup normal delay

}

void loop() {

  waterLevel = analogRead(Spin); //reading the pin into the variable
  Serial.println(waterLevel); //printing the water level to console
  //make an alert when its low go to LCD display

  /* Temperature and Humidity Setup */
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

  analogWrite(motor, 0); //0 is the motor not moving, change up to 255 to switch the speed

  delay(1000);  //setup normal delay
  
}
