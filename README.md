# EE109_Project
* Speed Measurement: Two phototransistor light detectors detect when an object enters and exits a fixed distance. Timer interrupts measure the elapsed time between sensors. Speed is calculated based on the known distance between sensors.
* LCD Display: Prints measured time (ms) and calculated speed (cm/s) and shows the configurable speed threshold.
* Adjustable Speed Threshold: A rotary encoder allows the user to increase or decrease the speed threshold. The threshold is stored in EEPROM so it persists after power cycles.
* Feedback System: Servo motor indicates speed level. Buzzer plays a tone when speed exceeds the threshold. RGB LED indicates relative speed comparison with another device.
* Serial Communication (UART): Transmits measured speed in a formatted message. Receives speed data from another system and compares the results.

video: https://drive.google.com/file/d/15Dl_ujMHG98AdKpPSSr9SUC5X92ZXilq/view?usp=sharing
