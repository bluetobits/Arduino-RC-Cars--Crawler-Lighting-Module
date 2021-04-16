//RC Crawler Lighting Controller - Creative Commons Non Commercial.
//
//Added USA/Non USA tail light option:
// FOR USA TAIL LIGHTS  link USAPIN (default 12) to GROUND
//FOR REST OF WORLD:
//    ALLOW PIN 12 TO FLOAT HIGH
//    REPLACE 200R resistors for 100R on front indicators (darlington pins 13 & 14)
//    Parallel connect rear intdicators with front intdicators.
//
//No link needed for seperate rear indicator option
//rear indicators wire to front indicators
/*
  Rear left tail*:    1 Red LED from Darlingtonpin 10 via 200R from D3 Permenant full when throttle is off
                        (USA OPTION Flash full when turning right and throttle is off
                        Stop flashing 2 seconds after steering is netural)
  Reverse:            2 white LED from  Darlington pin 11 via 60R triggered from D4 Whenever throttle is reversed
  Rear right tail*:   1 Red LED from Darlington pin 12  and 200R from D5 same as above
  Front headlights*:  2 white LED from  Darlington pin 13 via 60R triggered from D6 Permenant dip when in throttle is off for more than 4 seconds 
                        Full beam when throttle is forwards
  Front left ind:     1 Orange LED from Darlington pin 14 via 200R triggered from D7 Flash full when turning right and throttle is off
                        Stop flashing 2 seconds after steering is netural
  Front right ind:    1 Orange LED from Darlington pin 15 via 200R from D8 Same as above                        
 

  light bar           4 White LED from Darlington pin 16 and 30R from D9
                        with RX channel 3
Wiring:
Vcc to 6v from ESU
GND to 0v from ECU

Arduino Pins D2, D10 & D11 to receiver output channels 1,2 & 3 (throttle, steering & aux toggle switch)

ULN2003/ULN2004 pins 
  1, 2, 3, 4, 5, 6 & 7 to Adruino Pins D9, D8, D7, D6, D5, D4 & D3
  8 - GND, 
  9 not connected 10-16 as above
  connect Arduino pin D12 to ground for USA rear tail lights.
  Arduino pins D0,D1, D13 and A0 - A7 not connected.
  
  RX channel 1 and 2 to be parallel connected to steering servo and ESC


*/


static int debug = 1;



static const int NO_OF_CHANNELS = 3;
static int rxChannelPin[4] = {0, 2, 10, 11};// 0 not used RX1 Steering, RX2 Throttle, RX3 Toggle

static const int USAPIN = 12; //connect to ground  for USA tail lights
static const int REARLEFT = 3;
static const int REARRIGHT = 5;
static const int FRONTRIGHT = 8;
static const int FRONTLEFT = 7;
static const int REVERSE = 4;
static const int LIGHTBAR = 9;
static const int HEADLIGHT = 6;
static const int led = 13;
static unsigned long now;
static unsigned long  rxTout;
static const unsigned long  RXPOLL = 100;// time in ms between polling of RX channels
static unsigned long flashTout = 0;
static unsigned long flashToutDelay = 300;
static int aflash;
static unsigned long STimer[4];// straight/stationary timer for RX channels
static unsigned long duration[4];

static boolean startedTimerFlag[4] = {0, 0, 0, 0};
static boolean flash = 0;
static char statusCh[5];
static boolean stCancel[4];
static boolean USA = 0;

int dip = 55;
unsigned long rxChannel = 1;
volatile int STF;

int headlight;
int frontLeft;
int frontRight;
int rearRight;
int rearLeft;
int reverse;
int lightbar;

//============================== SETUP ==========================================
void setup() {

  if (debug == 1) {
    Serial.begin(250000);  // start serial for output
    Serial.println("Started");

  }
  for (int i = 1; i < 4; i++) {
    pinMode(rxChannelPin[i], INPUT_PULLUP);
  }
  pinMode (USAPIN, INPUT_PULLUP);

  pinMode(REARLEFT, OUTPUT);
  pinMode(REARRIGHT, OUTPUT);
  pinMode(REVERSE, OUTPUT);
  pinMode(LIGHTBAR, OUTPUT);
  pinMode(FRONTLEFT, OUTPUT);
  pinMode(FRONTRIGHT, OUTPUT);
  pinMode(HEADLIGHT, OUTPUT);
  digitalWrite(REVERSE, 0);
  analogWrite(HEADLIGHT, 0);
  digitalWrite(FRONTLEFT, 0);
  digitalWrite(FRONTRIGHT, 0);
  analogWrite(REARRIGHT, 0);
  analogWrite(REARLEFT, 0);
  analogWrite(LIGHTBAR, 0);


  attachInterrupt(digitalPinToInterrupt(2), initiate, RISING);
  rxChannel = 1;
  startedTimerFlag[1] = 0;
  USA = digitalRead(USAPIN);
}
//==============================INTIATE INTERRUPT ==========================================

void initiate() {

  STF = 1;

}

//================================ main loop =================================
void loop() {
  now = millis();

  // set flasher state for indicators
  if (now > flashTout) {
    flash = !flash;
    aflash = flash * 255;
    flashTout = now + flashToutDelay;
  }

  interrogate();
  output();
}




//================================ Interrogate =================================
void interrogate() {
  unsigned long fallTime;
  unsigned  long riseTime;


  if (now > rxTout) {// if the timeout has exceeded
    rxChannel = 1;
    rxTout = now + RXPOLL;//reset timeout timer
    STF = 0; //drop flag
    while (STF == 0) {};// waiting until channel 1 pulse rising edge raises the flag.
    riseTime = micros();//set timestamp for channel 1

    while (rxChannel <= NO_OF_CHANNELS) {
      while (digitalRead(rxChannelPin[rxChannel] ) == 1) {} //wait until rxChannelPin falls
      fallTime = micros();
      duration[rxChannel] = fallTime - riseTime;
      riseTime = fallTime; // timestamp riseTime for next channel pulse

      statusCh[rxChannel] = 'S';
      if (duration[rxChannel] < 1400) {
        statusCh[rxChannel] = 'L';
        STimer[rxChannel] = now;
      }
      if (duration[rxChannel] > 1600) {
        statusCh[rxChannel] = 'H';
        STimer[rxChannel] = now;
      }
      // setting the  timeout for stationary steering and throttle
      stCancel[rxChannel] = 0;
      if (STimer[rxChannel] > now - (rxChannel * 2000)) { //2000 for steer ch1 and 4000 for stationary ch2:  Not yet timed out
        stCancel[rxChannel] = 1;// within timeout
      }

      Serial.print(rxChannel);
      Serial.print(" ");
      Serial.print(statusCh[rxChannel]);

      Serial.print(stCancel[rxChannel]);
      Serial.print("     ");

      rxChannel++;
    }

    Serial.println();

  }
}
//============================== OUTPUT ==========================================
/*

*/
void output() {


  //output 0 = off, 1 for dip 2 flash 3= full

  // headlights
  // default to dip 
  //hi if moving forwards
  //hi if on and stationary is timing out and not reverse

  //reverse
  //default to off
  // hi when reversing an during timeout when stationary afterrevers not when forwards


  if ((statusCh[2] == 'H') || ((statusCh[2] != 'L') & (stCancel[2] == 1) & (headlight == 3 ))) {
    headlight = 3;
  } else {
    headlight = 1;
  }
  if (headlight == 3) {
    analogWrite (HEADLIGHT, 255);
  } else {
    analogWrite (HEADLIGHT, dip);
  }

  //reverse
  //default to off
  // hi when reversing and during timeout when stationary afterrevers not when forwards
  //    revsing       or (  not forwards    and  whithin delay and already showing reverse
  if ((statusCh[2] == 'L') || ((statusCh[2] != 'H') & (stCancel[2] == 1) & (reverse == 3 ))) {
    reverse = 3;
  } else {
    reverse = 0;
  }
  if (reverse == 3) {
    digitalWrite (REVERSE, 1);
  } else {
    digitalWrite (REVERSE, 0);
  }

  //lightbar

  if (statusCh[3] == 'H') {
    digitalWrite(LIGHTBAR, 1);
    digitalWrite(led, 1);

  }  else {
    analogWrite(LIGHTBAR, 0);
    digitalWrite(led, 0);
  }




  //FrontLeft
  // default is off
  //     never turning right and (already ind left  and within turning delay) or  (turning left   and  (stationary    or     already ind left an moving))
  if ((statusCh[1] != 'L') && ((frontLeft == 2 && stCancel[1] == 1) || (statusCh[1] == 'H') && ((statusCh[2] == 'S') || ((statusCh[2] != 'S') && (frontLeft == 2))))) {
    //   (not turning Right) && ((  in timer and indicating left) or (turning L & stationary) or (not stationary and indicating left)
    frontLeft = 2;
    digitalWrite(FRONTLEFT, flash);
  } else {
    frontLeft = 0;
    digitalWrite(FRONTLEFT, 0);
  }
  //FrontRight
  // default is off
  //     never turning leftt and (already ind right  and within turning delay) or  (turning right   and  (stationary    or     already ind right an moving))
  if ((statusCh[1] != 'H') && (frontRight == 2 && stCancel[1] == 1) || (statusCh[1] == 'L') && ((statusCh[2] == 'S') || ((statusCh[2] != 'S') && (frontRight == 2)))) {
    frontRight = 2;
    digitalWrite(FRONTRIGHT, flash);
  } else {
    frontRight = 0;
    digitalWrite(FRONTRIGHT, 0);
  }
  if (USA == 0) { // if Not USA wire rear indicators to front indicators

    //reartLeft
    // default is off

    //     never turning right and (already ind left  and within turning delay) or  (turning left   and  (stationary    or     already ind left an moving))
    if ((statusCh[1] != 'L') && (rearLeft == 2 & stCancel[1] == 1) || (statusCh[1] == 'H') && ((statusCh[2] == 'S') || ((statusCh[2] != 'S') && (rearLeft == 2)))) {
      rearLeft = 2;
      analogWrite(REARLEFT, aflash);
    } else {
      rearLeft = 0;
      if (statusCh[2] == 'S') {
        analogWrite(REARLEFT, 255);
      } else {
        analogWrite(REARLEFT, dip);
      }
    }
    //     never turning left and (already ind right  and within turning delay) or  (turning right   and  (stationary    or     already ind right an moving))
    if ((statusCh[1] != 'H') && (rearRight == 2 & stCancel[1] == 1) || (statusCh[1] == 'L') && ((statusCh[2] == 'S') || ((statusCh[2] != 'S') && (rearRight == 2)))) {
      rearRight = 2;
      analogWrite(REARRIGHT, aflash);
    } else {
      rearRight = 0;
      if (statusCh[2] == 'S') {
        analogWrite(REARRIGHT, 255);
      } else {
        analogWrite(REARRIGHT, dip);
      }
    }
  } else { // not USA TAIL LIGHTS
    if (statusCh[2] == 'S') {
      analogWrite(REARRIGHT, 255);
      analogWrite(REARLEFT, 255);
    } else {
      analogWrite(REARRIGHT, dip);
      analogWrite(REARLEFT, dip);
    }
  }


}
