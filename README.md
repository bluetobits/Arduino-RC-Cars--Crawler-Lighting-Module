# Arduino-RC-Cars-Crawler-Lighting-Module
Arduino control for indicators, headlights, brake lights etc for radio controlled vehicles.

Adruino Pro Mini ATMega328p 5v 16MHz
Currently for USA tail lights 

  Front headlights*:  default pin 6 Permenant dip when in throttle is off for more than 4 seconds
                        Full beam when throttle is forwards
  Rear right tail*:   default pin 5 Permenant full when throttle is off
                        Flash full when turning right and throttle is off
                        Stop flashing 2 seconds after steering is netural
  Rear left tail*:    default pin 3 Same as above
  Front left ind:     default pin 10 Flash full when turning right and throttle is off
                        Stop flashing 2 seconds after steering is netural
  Front right ind:    default pin4 Same as above
  Reverse:            default pin 7 Whenever throttle is reversed
  Spotlight array*:   default pin 9 Directly following channel 4 input, off/dim/bright
  Ancillairy LED:     default pin 8 Directly following channel 3 input, off/on
  
  
  RX data:
  Each RX channel outputs a square wave pulse of between 1000 to 2000 microseonds
  the 'servo' centre point is a pulse width of approx 1500 microseconds
  each RX channel pulse repeats approx every 180 milliseconds 
  The falling edge of each channel pulse coincides with the rising edge of the next channel pulse
   
  Pulse detection
  A pin 2 interrupt (RX channel 1) sets a flag  and a time-stamp  
  Stream comprises each channel in sequence follows pulse 
  1st pulse of between 850 to 1800 microseconds for channel 1 (steering)
  falling edge of channel 1 pulse triggers rising edge of channel 2 pulse
  
  General Code Description
  Digital Input pin 2 - set as an interrupt to flag the rise of the first pulse of 6 sequential servo pulses 
 
 * headlights, tail lights and array lights require PWM capable pin.
 Pin allocations are set as global constants 

Basic Operation
Power is taken directly from the "6V" RX power.  Measured at 5.77v and slightly higher than Arduino 5.5 max. Monitoring this. 
signals taken receiver from servo control outputs for channel 1 (steering) 2, ESC , 3 three position switch, 4, toggle on/off.  
original conception had a lighting spot bar with a servo controlled single rotating spotlight
Headlights: always on dip. 
 
 Throttle is off >4                    Headlights dip                    1.
  Throttle is off                       Tail lights full
 ***Throttle is retarding               Brake lights full**
  Throttle is reverse                   Reverse lights on
                                        Tail lights dip
  Throttle is forward                   Headlights full
                                        Tail lights dip
  Throttle is off & turning right       Right tail light flash full       2.
                                        Right indicator flash full
  Throttle is off & turning left        Left tail light flash full        2.
                                        Left insicator flash full
  Steering is neutral after 2 seconds   Cancel left indicator
                                        Right indicator off
                                        Left indicator off
                                        Right tail light flash off
                                        Left tail light flash off
  3-position switch is off              Spots off
  3-position switch is mid              Spots dip
  3-position switch is high             Spots full
  Toggle is off                         Rotating spot off
                                        Servo is detached
  Toggle is on                          Rotating spot is on
                                        Servo is attached
 


*/
