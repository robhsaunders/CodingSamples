#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MotorShield.h>

SoftwareSerial Bluetooth(10, 9); // RX, TX
DS3231 rtc(SDA, SCL);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

int LED = 8; // LED
String Data; // the data received
//bool ReadFromEEPROMOnStartUp = 0;
int Hour;//Input from Bluetooth and EEPROM
int Minute;//Input from Bluetoothand EEPROM
int Revolutions;//Input from Bluetooth and EEPROM
int RPM;//Input from Bluetooth and EEPROM
int REVStepper;//Calculated from Revolutions
int BTRecOption;
char cData;//Bluetooth Read
String sOption;//Bluetooth Read
String sValue;//Bluetooth Read
String sBluetooth;//Bluetooth Read
String Option;//Option compare
String Value;//Data for Option
Time t; 
int CurrentHour;
int CurrentMinute;
bool PumpTimeStart=0;

void setup() {
  //Load from EEPROM----------------------------------------------
  Revolutions = EEPROM.read(1);
  Hour = EEPROM.read(2);
  Minute = EEPROM.read(3);
  RPM = EEPROM.read(4);
  
  rtc.begin();
  lcd.begin();
  lcd.backlight();
  AFMS.begin();
  Bluetooth.begin(9600);
  Serial.begin(9600);
  
  myMotor->setSpeed(RPM); //RPM.  200 is good.
  myMotor->release();

  Serial.println("Waiting for command...");
  pinMode(LED,OUTPUT);




  
  //Reset Options
  Option = "0";
  
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(15, 37, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014
}
 
void loop() {

  //Read Bluetooth Section----------------------------------------
  while (Bluetooth.available()) {
    delay(10);  //small delay to allow input buffer to fill
    cData = Bluetooth.read();  //gets one byte from serial buffer
    sBluetooth += cData; 
    sOption = sBluetooth.substring(0,1);
    sValue = sBluetooth.substring(2);
    digitalWrite(LED,1);    
  } 
  if (sOption.length() >0) {
    Serial.println("sOption ");
    Serial.println(sOption); //prints string to serial port out
    Option=sOption;
    sOption=""; //clears variable for new input
    sBluetooth = "";
    digitalWrite(LED,0);
  }
  if (sValue.length() >0) {
    Serial.println("sValue ");
    Serial.println(sValue); //prints string to serial port out
    Value=sValue;
    sValue=""; //clears variable for new input
  }

  ////Option Section-------------------------------
  if (Option.length() >0) {
    Serial.println("Option ");
    Serial.println(Option); //prints string to serial port out
    //Option=""; //clears variable for new input
  }
  if (sBluetooth.length() >0) {
    Serial.println("sBluetooth ");
    Serial.println(sBluetooth); //prints string to serial port out
    //Option=""; //clears variable for new input
  }  

    if(Option=="?"){
      Serial.println("Running Option ?");
      Bluetooth.println(" ");
      Bluetooth.println(" ");
      Bluetooth.println("-----------Plant Waterer------------.");
      Bluetooth.println("Send 'Option:Value'.");
      Bluetooth.println(" ");
      Bluetooth.println("? Show Options");
      //Bluetooth.println("* Reset Option");
      Bluetooth.println("0 Release Motor");
      Bluetooth.println("1 Manual Motor On");
      Bluetooth.println("2:? Pump Revolutions");
      Bluetooth.println("3:? Pump On Hour");
      Bluetooth.println("4:? Pump On Minute");
      Bluetooth.println("5:? Pump RPM");

      Bluetooth.println(" ");
      Bluetooth.print("Pump Revolutions:  ");
      Bluetooth.println(Revolutions);
      Bluetooth.print("On Hour: ");
      Bluetooth.println(Hour);
      Bluetooth.print("On Minute: ");
      Bluetooth.println(Minute);
      Bluetooth.print("RPM: ");
      Bluetooth.println(RPM);
      Bluetooth.println(" ");
      
      //Serial.println(Option);//debug..........................
      Option="";
    }
    if(Option=="0"){
       digitalWrite(LED,0);
       Serial.println("Motor Released");
       Bluetooth.println("Motor Released");
       myMotor->release();      
       lcd.setCursor(0,1);
       lcd.print("Motor Released");
       Option="";
      
    }
    else if(Option=="1"){  
      digitalWrite(LED,1);
      Serial.println("Pump On");
      Bluetooth.println("Pump On");
      lcd.setCursor(0,1);
      lcd.print("Pumping");
      
      myMotor->setSpeed(RPM); //RPM.  200 is good.
      REVStepper = Revolutions * 200;
      myMotor->step(REVStepper, FORWARD, DOUBLE);
       
      //Program halts here until motor has completed revs
      //Set Option to '0' to release pump
      Option = "0";

    }

    else if(Option=="2"){
      Revolutions = Value.toInt();
      Bluetooth.println("Revolutions:  Saving to EEPROM");
      EEPROM.write(1,Revolutions);//(addr, val);
      Option="";
    }
    else if(Option=="3"){
      Hour = Value.toInt();
      Bluetooth.println("Hour:  Saving to EEPROM");
      EEPROM.write(2,Hour);//(addr, val);
      Option="";
    }
    else if(Option=="4"){
      Minute = Value.toInt();
      Bluetooth.println("Minute:  Saving to EEPROM");
      EEPROM.write(3,Minute);//(addr, val);
      Option="";
    }   
    else if(Option=="5"){
      RPM = Value.toInt();
      Bluetooth.println("RPM:  Saving to EEPROM");
      EEPROM.write(4,RPM);//(addr, val);
      Option="";
    }      
    else if(Option=="8"){
      Bluetooth.println("Reading EEPROM values............");
      Bluetooth.print("Revolution:  ");
      Bluetooth.println(EEPROM.read(1));
      Bluetooth.print("Hour:  ");
      Bluetooth.println(EEPROM.read(2));
      Bluetooth.print("Minute:  ");    
      Bluetooth.println(EEPROM.read(3));
      Bluetooth.print("RPM:  ");    
      Bluetooth.println(EEPROM.read(4));
      Option="";
    }
    else{;}
 

  //Serial.println(rtc.getTimeStr());
  //Serial.print(rtc.getTemp());
  //Serial.println(" C");

  lcd.setCursor(0,0);
  lcd.print(rtc.getTimeStr());
  lcd.setCursor(10,0);
  lcd.print(((rtc.getTemp()*9/5)+32));
  lcd.setCursor(15,0);
  lcd.print("F");

  t = rtc.getTime();
//  Serial.println(t.hour,DEC);
//  Serial.println(t.min,DEC);

  CurrentHour = t.hour;
  CurrentMinute = t.min;

  if(CurrentHour == Hour){
    if(CurrentMinute == Minute){
      if(PumpTimeStart == 0){
        Serial.println("Pump Timer Start");
        Bluetooth.println("Pump Timer Start");
        PumpTimeStart = 1;
        Option = 1;
      }
    }
  }
  if(CurrentMinute != Minute){
    PumpTimeStart = 0;
  }
  
delay(1000);
}
