/*
Copyright JBlasques 2024
Checked 28March2024  This revision is good for display of DC voltage and 
alarm if voltage below 12VDC and DC current from 0 to 50 or 0 to 100 Amps
depending on sensor.
This is the code ("sketch" in Arduino jargon) that runs in a microcontroller
"Arduino UNO R4 Minima". Based on DC Hall Current Sensor by Solarduino but
highly transformed by João Blasques of Lisbon, Portugal.
It measures service battery (11 to 14.5 Volt) and current (0 to 50Amps or 0
to 100Amps) and displays values in a LCD screen.
Low voltage alarm (set to 12VDC) 
It will be installed in a small sailing boat.
The voltage supply must not exceed 15 VDC. It may burn the electronic
circuit. There is no overvoltage protection.
Apparently an overcurrent is not a problem but accuracy will be lost above
50 or 100 Amps, depending on the sensor used.
Note :  The voltage sensor is a simple voltage divider 47K + 22k resistors
with a 100nf capacitor in parallel with the 22k resistor.
Note :  The DC Current Sensor Model is a HSTS016L Hall effect split core
current transformer manufactured by YHDC. 
Beijing Yaohuadechang Electronic Co., Ltd.  http://en.yhdc.com/intro/
This Hall Sensor has 4 wires-Red:+5VDC,Black:GND,Yellow:current signal,
White:current reference.
Note :  The values shown in Serial Monitor / LCD Display are refreshed every
second.
Note :  The unit provides reasonable accuracy but is not comparable with
other commercial products like digital multimiters.
Note: Pin A0, not used,is grounded via a 10k resistor to prevent interferences
between pins*/ 

/*  0- Declaration of libraries, constants and variables */
           //Correct as per 28March2024
/* 0.1 - Libraries. */
#include <Arduino.h>   // ???To be compatible with PlatformIO compiler ???
#include <Wire.h> // Wire library required for communication with I2C boards
#include <LiquidCrystal_I2C.h> // Not sure this is the best library. It works

/* 0.2 - Constants. Do not change */

LiquidCrystal_I2C lcd(0x27, 20, 4); // Sets the LCD address, clumns and rows.
const int decimalPrecision = 2;     // Decimal places for print values.
const int supplyVoltage = 5000; // Analog input pin maximum supply (mVolt).
const int buttonPin = 7;               // the pin number for the button
const int ledPin = 11;                 // the pin number for the led
const int buzzerPin = 5;               // the pin number for the buzzer
const int sensorRangePin = 2;          // Used to detect Hall Sensor amperage
const int voltagePin = A3;             // pin to measure voltage
const int calibrationPin = A2;         // pin to calibrate offset middle value
const int currentAnalogInputPin = A1;  // pin to measure Current Value
const int buttonInterval = 23; // number of millisecs between button readings
const int readInterval = 1000; // reading of voltage & current every second
const int numReadings = 20; // voltages & current measured 20 times per second
const float manualOffset = .65; // Key in value to offset initial value

/*
For "Hall-Effect" Current Transformers Model HSTS016L:
mVperAmp = maximum voltage range (in millivolt) / current rating of CT
For example, a 20A Hall-Effect Code HSTS016L rated at 20A, 2.5V +/- 0.625V:
mVperAmp will be 625 mV / 20A = 31.25mV/A
For example, a 50A Hall-Effect Code HSTS016L rated at 50A, 2.5V +/- 0.625V:
mVperAmp will be 625 mV / 50A = 12.5 mV/A
For example, a 100A Hall-Effect Code HSTS016L rated at 100A, 2.5V +/- 0.625V:
mVperAmp will be 625 mV / 100A = 6.25 mV/A
*/

/* 0.3 - Variables. Will change */
unsigned long previousMillis;          // 
unsigned long previousButtonMillis = 0; // used by "readButton()" function
byte ledState = LOW;;         // ledState used to set or read the external led
byte buzzerState = LOW;       // buzzerState used to set or read the buzzer
byte sensorRangeState = LOW;  // used to check sensor range, 50 or 100 Amp
int flag1 = 0;  // type int to allow OR (||) and AND (&&) logical operations
int flag2 = 0;  // type int to allow OR (||) and AND (&&) logical operations
int total;      // Sum of all analog readings in a one second cycle
int average;    // Used for voltage. If not an integer, value gets truncated 
                // (not rounded) to integer. Error is less than .02 Volts at 15V
int currentSampleRead = 0;
int currentSampleSum = 0;              // accumulation of sample readings
int currentSampleCount = 0;            // to count number of samples
int currentMean = 0;  // the average value from all samples, in analog values
float currentAMPS = 0;    // Only required for printing with decimalPrecision
float voltageVOLTS = 0;   // Only required for printing with decimalPrecision 
/*Regarding next line of code
For "Hall-Effect" Current Transformers Model HSTS016L:
mVperAmp = maximum voltage range (in millivolt) / current rating of CT
For example, a 20A Hall-Effect Model HSTS016L rated at 20A, 2.5V +/- 0.625V:
mVperAmp will be 625 mV / 20A = 31.25mV/A
For example, a 50A Hall-Effect Model HSTS016L rated at 50A, 2.5V +/- 0.625V:
mVperAmp will be 625 mV / 50A = 12.5 mV/A
For example, a 100A Hall-Effect Model HSTS016L rated at 100A, 2.5V +/- 0.625V:
mVperAmp will be 625 mV / 100A = 6.25 mV/A
*/
float mVperAmpValue = 0; //This is not a constant. Depends on sensor amperage 

//=======================================================

/*
Functions. For some obscure reasons all functions , except "loop()", must
be placed here, before "void setup()". If not there is a compilation error
*/

//=======================================================

void blinkAlarm() {
  
  /*  
  Blinks led and buzzer. Voltage below 12VDC. As this functin is called by
  "displayVoltAmps()" every second, blinking has a two second period 
  */
  
    // if the LED is off turn LED and Buzzer on and vice-versa:
    if (ledState == LOW)
     {
      ledState = HIGH;
      buzzerState = HIGH;
     } 
     else 
     {
      ledState = LOW;
      buzzerState = LOW;
     }
    // set the led and buzzer with the ledState and buzzerState :
    digitalWrite(ledPin, ledState);
    digitalWrite(buzzerPin, buzzerState);
  }

//=======================================================

void steadyAlarm() {
  /*led on, buzzer off. Alarm acknowledged by pushing button*/
    ledState = HIGH;
    buzzerState = LOW;
    digitalWrite(ledPin, ledState);          // set the led on
	digitalWrite(uzzerPin, buzzerState);    // and buzzer off
}
//=======================================================

void testAlarm() {b
  /*  led on, buzzer on. Push button continuosly pressed*/
  ledState = HIGH;
  buzzerState = HIGH;
  digitalWrite(ledPin, ledState);         // set the led on...
  digitalWrite(buzzerPin, buzzerState);   // and buzzer on
}

//=======================================================

void clearAlarm() {
  /*  led off, buzzer off. Voltage returned to above 12VDC*/
  ledState = LOW;
  buzzerState = LOW;
  digitalWrite(ledPin, ledState);  // set the led off ...
  digitalWrite(buzzerPin, buzzerState);  //and buzzer off
}

//=======================================================

 void checkAlarm(){
 if (digitalRead(buttonPin) == HIGH)
	 return;                       // return to void displayVoltAmps()
 if (average < 808 || flag1 == 1)  // Voltage below 12 OR alarm active          
    {
    flag1 = 1;                         // Alarm active
      if (average > 814 && flag2 == 1) //voltage over 12 AND alarm acknowledged 
	  {								   //clear flags and clear alarm
      flag1 = 0;
      flag2 = 0;
      clearAlarm();
      return;                       // return to void displayVoltAmps()
      }
      else if (flag2 == 0)      // voltage above 12 and alarm not acknowledged
      {
      blinkAlarm();             // Calls blinkAlarm  (blinking led and buzzer)
      } 
      else if (flag2 == 1)
      {
      steadyAlarm();            // Calls steadyAlarm (blinking led, buzzer off)
      }
    } 
 }

//========================================

void readButton()  // this function is called continuously by void loop()
					 
{ //if buttonInterval has not been reached returns to void loop()
  if (millis() - previousButtonMillis >= buttonInterval)
 {  //if buttonInterval has been reached (23ms) checks button state
	previousButtonMillis += buttonInterval;
    if (digitalRead(buttonPin) == HIGH)
    {
      if (flag1 == 1)
      {
      flag2 = 1;
      } 
      else                                      //testAlarm();
      {
      testAlarm();
      }
    }
    else                                         //clearAlarm();
    {
    clearAlarm();
    }
  }
 } 

//========================================

void displayVoltAmps()
/* 
This is the main function where all measurements, calculations and display
are made
Called continuously by void loop() it runs every second (readInterval = 1000ms)
*/
{      //if readInterval has not been reached returns to void loop()
  if (millis() - previousMillis >= readInterval)
 {      //if readInterval has been reached function runs to the end
    previousMillis += readInterval;

    /* 1- DC Voltage. Measurement and calculations*/
   
   /* Average of 20 readings per second.Integer from 0 to 1023
      Coef 3.0403 cheked on 15March2024 with 
	  digital meter Fluke true voltage 13,70 Volt 
	  Serial port was printing from 13.68 to 13.72 Volt*/
	
	total = 0;
    for (int i = 0; i < numReadings; i++)  // loops 20 times (as defined)
	{
      total = total + analogRead(voltagePin);
    }
    average = total / numReadings;                 
    voltageVOLTS = (average * 3.0403 * 5 / 1023);                         
   
   /* 1.1- DC Voltage. Check voltage alarm condition
            voltage less than 12VDC. (average 808 = 12,006 Volt).*/
    
    checkAlarm();

    /*  2- DC Current Measurement and calculations*/

    currentSampleSum = 0;
    for (int i = 0; i < numReadings; i++) {                                               
      currentSampleRead = analogRead(currentAnalogInputPin)  //continues ...
	  - analogRead(calibrationPin);  // read the sample value - offset value
       // accumulate total analog values for each sample reading:
	  currentSampleSum = currentSampleSum + currentSampleRead;                            
    }
    currentMean = currentSampleSum / numReadings; // average of 20 readings
    currentAMPS = currentMean * supplyVoltage / //continues on next line...
	mVperAmpValue / 1023 - manualOffset;  // calculate the final RMS current
    // if the current detected is less than 1% of sensor maximum:
	if (currentAMPS <= (625 / mVperAmpValue / 100))                                  
    {
    currentAMPS = 0;  // set current value to display to 0 Amp
   }
    
	/* 3. Prints Voltage and Current every second to LCD. Checked 15March2024*/
    
	lcd.setCursor(0, 0);
    lcd.print("Voltage = ");
    lcd.print(voltageVOLTS, decimalPrecision); // Prints Battery Voltage 
    lcd.print(" V");
    lcd.setCursor(0, 1);
    lcd.print("Current = ");
    lcd.print(currentAMPS, decimalPrecision); // Prints Service Battery Current
    lcd.print(" A");
     
    /* 4. Prints Voltage and Current every second to serial port. 
          This requires connection via USB to a computer 
		  Not used in normal operation.
		  Required for debuging
          WARNING: CONNECTION VIA USB REQUIRES PREVIOUS DISCONNECTION
		  OF POWER VIA SOCKET. Risk of voltage conflict */
    
    Serial.print("analogRead(currentAnalogInputPin) = "); //The average current
    Serial.println(analogRead(currentAnalogInputPin));    //0 to 1023 integer
    Serial.print("analogRead(calibrationPin) = ");
    Serial.println(analogRead(calibrationPin));
    Serial.print("(currentAMPS, decimalPrecision) = ");
    Serial.println(currentAMPS, decimalPrecision);  // Prints Battery current
    Serial.print("Voltage = ");
    Serial.print(average);  // The average voltage 0 to 1023 integer
    Serial.print(",");      // (808 corresponds to 12 VDC)
    Serial.println(voltageVOLTS, decimalPrecision);  // Prints Battery voltage
													 //	in volts
  }
}

//=======================================================

void setup() // Runs once
{
    Serial.begin(9600); //opens serial port to allow debugging
    lcd.init();         // initialize the lcd (display)
    lcd.backlight();    // Turns on the backlight
	//Note:To turn off the backlight, the command is "lcd.nobacklight()"	
	//Backlight shall be turned off after 30 seconds or so to reduce 
	//consumption (not relevant) and to increase life span of the lcd
	//I will do it one day, Inch Allah!
    pinMode(ledPin, OUTPUT);k
    pinMode(buzzerPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    
 //========
    /*These 7 lines of code may be removed. It is a laracha,
	displayed when equipment is switched on*/
    lcd.setCursor(0, 0);
    lcd.print("Ioannes Blasques");
    lcd.setCursor(0, 1);
    lcd.print("Olisiponensis");
    lcd.setCursor(0, 2);
    lcd.print("fecit.AD.MMXXIIII");
    delay(10000); //After 10 seconds the laracha is cleared and program starts
 //========

    lcd.clear();
    /*Range of connectd Hall Sensor is automatically detected on startup
	(either 50 or 100 Amp). This requires previous changes to sensor cable*/
    pinMode(sensorRangePin, INPUT_PULLUP);
    lcd.setCursor(0, 3);
    if (digitalRead(sensorRangePin) == HIGH)
    {
      lcd.print("50 Amp Hall Sensor ");
      mVperAmpValue = 12.5; //If sensor is 50Amp set mVperAmpValue to 12.5mV
    }    
    else
    {  
      lcd.print("100 Amp Hall Sensor");
      mVperAmpValue = 6.25;  //If sensor is 100Amp set mVperAmpValue to 6.25mV
    }
}

 //=======================================================

void loop() // Runs permanently
{
    displayVoltAmps();
    readButton();
}
