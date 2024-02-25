#include <Wire.h>
#include <RTClib.h>
#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h> 

RTC_DS1307 rtc;
int oraCurenta = 0;
int minutCurent = 0;
int ok = 0;
 
const byte ROWS = 4;
const byte COLS = 4;
 
char hexaKeys[ROWS][COLS] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'}
};
 
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

LiquidCrystal_I2C lcd(0x27, 16, 2);
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
 
int hourValue = 0;
int minuteValue = 0;
int nrcifre = 0;
String formattedTime = "";
String oraSiMinut = "";
bool enteringHour = true;
 
Servo myservo;
int servoMoving = 1;
 
void setup() {
  Serial.begin(9600);
  lcd.init(); // initialize the lcd
  lcd.backlight();
  
  myservo.attach(11);
  delay(500);
  myservo.write(120);  // Set servo angle to 90 degrees
 
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  if (!rtc.isrunning()) {
    // Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}
 
void loop() {
 
   
  //rtc
  DateTime now = rtc.now();
  oraCurenta = now.hour();
  minutCurent = now.minute();
 
  oraSiMinut = String(oraCurenta) + ':' + (minutCurent < 10 ? "0" : "") + String(minutCurent);
 
 
  //keypad
  char customKey = customKeypad.getKey();
 
  if (customKey) {
    if (customKey >= '0' && customKey <= '9') {
      if (enteringHour) {
        if (nrcifre < 2) {
          hourValue = (hourValue * 10 + (customKey - '0'));
          if (hourValue > 23) {
            hourValue = 23;
          }
          nrcifre++;
        } else {
          enteringHour = false;
          nrcifre = 0;
        }
      } else {
        if (nrcifre < 2) {
          minuteValue = (minuteValue * 10 + (customKey - '0'));
          if (minuteValue > 59) {
            minuteValue = 59;
          }
          nrcifre++;
        }
      }
    } else if (customKey == '#') {
      formattedTime = String(hourValue) + ":" + (minuteValue < 10 ? "0" : "") + String(minuteValue);
 
      hourValue = 0;
      minuteValue = 0;
      enteringHour = true;
      nrcifre = 0;
 
    } else if (customKey == 'C') {
      hourValue = 0;
      minuteValue = 0;
      enteringHour = true;
      nrcifre = 0;
    } else if (customKey == '*') {
      enteringHour = !enteringHour;
      nrcifre = 0;
    }
  }
 
  verificare();
}
 
bool servoMoved = false;
 
void verificare() {
 
  if (!oraSiMinut.equals(formattedTime)) {
//lcd.clear();  
lcd.setCursor(0, 0);
lcd.print("CTime:");
lcd.print(oraSiMinut);

lcd.setCursor(0, 1);
lcd.print(" FTime:");
lcd.print(formattedTime);


  

    servoMoved = false;
  } else if (oraSiMinut.equals(formattedTime) && servoMoved == false) {
    
      myservo.write(90);
      delay(1000);
      myservo.write(120);
      
      servoMoved = true;
    
  }
 
}
