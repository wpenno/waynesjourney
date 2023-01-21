#include<LiquidCrystal.h>
#include<Servo.h>
Servo myServo;
LiquidCrystal lcd(12 ,11, 5, 4, 3, 2);
int tempbutton = 6;//Hi/Med/Lo setting button
int OP = 0;
int CL = 170;
const int sensorPin = A0;
int tempval = 0;
int sensorVal = 0;
float temptot;
float Ftempnew;
int sensortot;
int tempsetting;
int DamperPos;
int DamperPosNew;


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  myServo.attach(7);
  myServo.write(OP);//allow the damper to be opened by pressing reset button and temp set back to Medium of 220
  tempsetting = 220;
  float Ftempnew = 0;
  DamperPos = OP; 
  pinMode(tempbutton,INPUT_PULLUP);
  }

void loop() {

 //Set desired temp setting LO = 180. MD = 220, HI = 260, MX = 300
  if (digitalRead (tempbutton) == LOW){
    tempsetting = tempsetting + 40;
  }
  if (tempsetting > 300){
    tempsetting = 180;
  }

   //this loop averages 10 tempval's of 40 sensorvals average 
   temptot = 0;
    for(int tempval = 1; tempval<11; tempval++){
    sensortot = 0;

      //this loop is 40 sensorvals totalled
      for(int sensorval = 1; sensorval<41; sensorval++){ 
      sensorVal = analogRead(sensorPin);//temp reads accurate when plugged into 12volt external source, if using USB subtract 7
      sensortot = sensortot + sensorVal;
      }

   //this takes the average of all 40 sensorVal's
   sensorVal = sensortot/40.0;

   //convert ADC to voltage
   float voltage = (sensorVal/1024.0) * 5.0;

   //covert voltage to degC
   float tempVal = (voltage - 0.5) * 100;

   //return to line beginning of loop and repeat 10 times
   temptot = temptot + tempVal;
   }

//this takes the average of all 10 tempVal's = celcius temp 
float temperature = temptot/10.0;

//convert to F
float Ftemperature = (temperature * 1.8 + 32);

//Do not change temp unless there is more than a 1 degree change in either direction
if ((Ftemperature - Ftempnew) > 1){
  Ftempnew = Ftemperature;
 }
if ((Ftempnew - Ftemperature) > 1){
  Ftempnew = Ftemperature;
 }

//print temp in F on line 2
lcd.setCursor(0, 1);
lcd.print("Temp F = ");
lcd.print(Ftempnew);
lcd.print(" ");

//determine damper position
if (Ftempnew >= tempsetting) { //if new temp is more than set point then close damper 100% 
  DamperPosNew = CL;
}
if (Ftempnew < (tempsetting - 10.0)) {  //if new temp is more than 10 degrees below set point then set damper open at 0%
  DamperPosNew = OP;
}
if (Ftempnew > (tempsetting - 10.0) && Ftempnew < (tempsetting - 5.0)) {  //if new temp is between 10 and 5 degrees below set point set damper at 70% closed
  DamperPosNew = OP + 120;
}
if (Ftempnew > (tempsetting - 5.0) && Ftempnew < (tempsetting - 3.0)) {  //if new temp is between 5 and 3 degrees below setpoint set damper at 76% closed
  DamperPosNew = OP + 130;
}
if (Ftempnew > (tempsetting - 3.0) && Ftempnew < tempsetting) {  //if new temp is between 3 and 0 degrees below setpoint set damper at 82% closed 
  DamperPosNew = OP + 140;
}

//print damper position,SetPoint LO 180/MD 220/HI 260 on line 1
lcd.setCursor(0, 0);
lcd.print("DMPR ");
lcd.print(DamperPos * 0.589);//convert servo degrees to % of closed
lcd.setCursor(9,0);
lcd.print(":");
if (tempsetting == 180){
  lcd.print("LO 180");
}
if (tempsetting == 220){
  lcd.print("MD 220");
}
if (tempsetting == 260){
  lcd.print("HI 260");
}
//If no change in damper setting then dont set damper
  if (DamperPos != DamperPosNew){  //initially DamperPos was set to OP in setup
  //Position the damper
  DamperPos = DamperPosNew;
  myServo.write(DamperPosNew);
  delay(1000);//allow board voltage to re-settle and temp input to stabilize
}
//delay to allow pushbutton to pick up one push only
delay(200);
}

