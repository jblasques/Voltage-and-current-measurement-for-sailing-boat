# Voltage-and-current-measurement-for-sailing-boat
Status as of May 2024: Ongoing testing on the boat
This is a project of an equipment to measure Voltage and Current in a small (35 feet) sailboat with a 12VDC electrical system.
It uses an "Arduino Uno R4 Minima" board with an LCD display connected via I2C (4wires).
A resistor voltage divider converts the 12VDC to a bit less than 5VDC that is fed to the Arduino A3 analog input.
Current is measured by a 50Amp "Hall split core current sensor" model HSTS016Lv, manufactured by YHDC, a chinese company.
This current sensor is powered by 5VDC from the Arduino the two outputs (measure and reference) are fed directly to the Arduino A1 and A2 analog inputs.
Additionaly there are an LED light and a buzzer for low voltage alarm and one pushbutton for alarm cancelation and testing. 
