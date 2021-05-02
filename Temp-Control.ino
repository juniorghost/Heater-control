// Salim El Atache
// lab3 using the dht22

#include <Wire.h> // I2C library
#include <Adafruit_SSD1306.h> // OLED display library
#define OLED_ADDR   0x3C    // I2C address
Adafruit_SSD1306 display(-1);   // no reset pin


#include "DHT.h"   //dht library
#define DHTPIN 2     // pin that connects to the second pin from the left on the sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);


#define Relay 3 //digital 3
int Case = 1;
unsigned long delayStart = 100000; // the time the delay starts at 10 sec

int HL = 0;
float DeadBand = 1; //chosen deadband is 1 for a half degree difference between the setpoint
int SetPoint = 0;
int Flag = 0;
const int analogInPin = A15; //potentiometer
float sensorValue = 0;


void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  dht.begin();
  pinMode(Relay, OUTPUT);  //declare pin type
  digitalWrite(Relay, LOW); // turn Relay off

}

void loop() {



  // DHT 22 and OLED display
  display.clearDisplay(); // clear display
  display.setTextColor(WHITE);
  float tempC = dht.readTemperature();// read the temp in degrees Celcius
  display.setCursor(0, 0);//set the position
  display.setTextSize(2); // bigger text
  display.println(tempC, 1);  // print the temps
  display.setCursor(52, 0);//set the position
  display.print((char)247); // print the degrees symbol
  display.println("C");
  display.setCursor(90, 0);//set the position
  display.setTextSize(1); // bigger text
  display.print("Set"); // print the degrees symbol
  display.setCursor(85, 7);//set the position
  display.print("point"); // print the degrees symbol
  display.setTextSize(2); // bigger text
  display.setCursor(90, 16);//set the position
  display.println(SetPoint);

  display.setCursor(0, 16);//set the position
  display.setTextSize(2); // bigger text
  display.println("Case:"); // print the humidity
  display.setCursor(61, 16);//set the position
  display.print(Case); // print the humidity
  display.display();  // update display
  
  

  //Fan Triggering
  sensorValue = analogRead(analogInPin);
  SetPoint = map(sensorValue, 0, 1023, 20, 30);
  HL = SetPoint + DeadBand / 2;

  if (tempC >= HL) {
    tempC = dht.readTemperature(); //read the temp sensor value
    delayStart = millis();//reset the timer
  }

  Fan_ON(); // calling the function


}


void Fan_ON() {

  //case 1
  if (  (Case == 1) && ((millis() - delayStart) <= 15000) ) {
    digitalWrite(Relay, HIGH); // turn Relay on
    Flag = 1;
  }
  //case 2
  else if ((Case == 2) && ((millis() - delayStart) <= 10000)) {
    digitalWrite(Relay, HIGH); // turn Relay on
    Flag = 2;
  }
  //case 3
  else if ((Case == 3) && ((millis() - delayStart) <= 5000)) {
    digitalWrite(Relay, HIGH); // turn Relay on
    Flag = 3;
  }

  //once the timer reaches the limit of the cases, we check which case was running and we add one,
  //if it's case n3 we reset it to 1.
  else {
    digitalWrite(Relay, LOW); // turn Relay on
    if ((Flag == 1) && (Case == 1)) {
      Case = 2;
    }
    else if ((Flag == 2) && (Case == 2)) {
      Case = 3;
    }
    else if ((Flag == 3) && (Case == 3)) {
      Case = 1;
      delayStart =  millis() + 15000;
      // so it doesn't start the first case, since we have 10 sec left on the meter and case =1,
      //nothing stops the first case from happening till we add 15 seconds on the timer.

    }
  }
}
