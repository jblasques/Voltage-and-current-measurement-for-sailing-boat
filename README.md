# Voltage-and-current-measurement-for-sailing-boat
Status as of May 2024: Ongoing testing in the boat.
Tests of equipment onshore were positive. I do not have a DC current supply for 50 Amps. 
The Arduino sketch was tested and is operating. This sketch should run in any Arduino Uno, with minor or no modifications.
This is a project of an equipment to measure battery Voltage and Current in a small (35 feet) sailboat with a 12VDC electrical system.
It uses an "Arduino Uno R4 Minima" board with an LCD display connected via I2C (4wires).
The power supply voltage to the equipment is the same that is measured. A resistor voltage divider converts the 12VDC to a bit less than 5VDC that is fed to the Arduino A3 analog input.
Current to be measured is the current from the service battery bank to the 12VDC switchboard. It is measured by a 50Amp "Hall split core current sensor" model HSTS016L, manufactured by YHDC, a chinese company. I knew about these sensors in a video from Solarduino. https://solarduino.com/how-to-measure-dc-and-ac-current-using-hsts016l-hall-effect-current-transformer/ 
This current sensor is powered by 5VDC from the Arduino, the two outputs (measure and reference) are fed directly to the Arduino A1 and A2 analog inputs.
Additionaly there are an LED light and a buzzer for low battery voltage alarm and one pushbutton for cancelation and testing the alarm.
If the equipment works in the boat it will be just a question of programing to calculate and display the rate of discharge of the battery and possibly other parameters.
