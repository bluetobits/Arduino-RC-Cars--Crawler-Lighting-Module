Arduino control for indicators, headlights, brake lights etc for radio controlled vehicles.

THIS IS AN UNFINISHED WORK IN PROGRESS PROJECT

Adruino Pro Mini ATMega328p 5v 16MHz
Facility  for USA and REST OF WORLD  tail lights
Number in brackets (2*) refer to default pin number.  The * denotes a PWM capable output pin is required.
FOR USA tail lights Bridge USA pin (12) to ground
For Rest Of World tail lights leave USA (12) open (pulled up)  and link rear indicators to respective front indicstors

Headlights (6*): 	Normally dipped. Full when moving forward or stationary for  less than 4 seconds after moving forward
Reversing Lights (7):	Normally off. Full when reversing or stationary for  less than 4 seconds after reversing
Left Indicators (10) :  Normally off. Always off whenever turning right .
Flashing when: 	(turning left and stationary) or  (already indicating left and within 2 seconds of straight steering ) or (continuing turning left from stationary and moving).
Right Indicators  (4) :  Normally off. Always off whenever turning left .
Flashing when: 	(turning right and stationary) or  (already indicating right and within 2 seconds of straight steering ) or (continuing turning right from stationary and moving). 
For USA connect 2 LEDs to each front indicator or change series resistor value.
Rear Left Tail Light (3*) : Normally (dim when moving) or  (Full when stationary)
USA ONLY  Flash off /  full when (turning left and stationary) or  (already indicating left and within 2 seconds of straight steering ) or (continuing turning left from stationary and moving). 
Rear Right Tail Light (5*) : Normally (dim when moving) or  (Full when stationary)
USA ONLY  Flash off /  full when (turning right and stationary) or  (already indicating right and within 2 seconds of straight steering ) or (continuing turning right from stationary and moving). 
4 Array LEDs  - eg spotlight bar (9*): following RX channel 4 (off/mid/high)
1 LED (3): RX channel 3 (on/off)


Inputs from RX are rx1 to rx4  to pins 2, 10,11,12

  
  RX data:
  Each RX channel outputs a square wave pulse of between 1000 to 2000 microseconds
  the 'servo' centre point is a pulse width of approx. 1500 microseconds
  each RX channel pulse repeats approx. every 10 milliseconds 
  The falling edge of each channel pulse coincides with the rising edge of the next channel pulse
   
  Pulse detection
The lights do not require a fast response time. The channel pulses are only interrogated once every 180ms leaving ample processing time for other operations. 

A pin 2 rising edge  interrupt (RX channel 1) sets a flag   
Loop checks for next flag raise  after the 180ms TX has timed out .
Duration [txchannel] is set for  each channel 1-4
Function (fun) for each channel is set to ‘S’tationary , ‘L’ow or ‘H’igh:
L = <1400us, S=>1400 &<1600us   H=>1600us 

Basic Operation
Power is taken directly from the "6V" RX power.  Measured at 5.77v and slightly higher than Arduino 5.5 max. Monitoring this. 
signals taken receiver from servo control outputs for channel 1 (steering) 2, ESC , 3 three position switch, 4, toggle on/off.  
original conception had a lighting spot bar with a servo controlled single rotating spotlight

