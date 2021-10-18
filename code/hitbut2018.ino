#include <FastLED.h>
#include <ADCTouch.h>

#define NUM_LEDS 7
#define EYE 1
#define DATA_PIN 4
#define TOUCHPIN A3

const bool code[125] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0,
                        1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 
                        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0,
                        1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0,
                        0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0,
                        1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1,
                        1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0, 0};

CRGB leds[NUM_LEDS];

int ref;
int mode = 0;
int rippleTime = 4000;
unsigned long startOfPress = millis();

int frame = 0;
float rFrame = 0;

float fSinLong;

float fSinW[4];
float fSinH[4];
float fSinQ = 0;
float fSinE = 0;

float offset[3] = {0.5, 1, 1.5};

float scale = .75;
float longScale = 0.5;

int h[7];
int s[7];
int v[7];

int minBright = 0;
int maxBright = 120;

int randAbove = 0;

int randPattern = 8;
int randColor = 8;

int newAnim = 0;
int newColor = 0;
int currentColor = 0;
int currentPattern = 0;

int eyeIndex = 0;

void frameCalc(int frame);
void mirrorV();
void mirrorH();
void allRandom();
void colorOne(int frame);
void colorTwo(int frame);
void colorThree(int frame);
void brightnessLoop(int frame, float freq, int ledStart, int ledEnd);
void patternOne(int frame);
void patternTwo(int frame);
void patternThree(int frame);
void patternFour(int frame);
void patternFive(int frame);
void patternSix(int frame);

void setup() { 
  randomSeed(analogRead(0));
  
  pinMode(EYE, OUTPUT);
  ref = ADCTouch.read(TOUCHPIN);
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
}

void loop() { 

  //calculate sine values from fra`me
  frameCalc(frame);

  //select current pattern
  if (currentPattern == 0) { patternOne(frame); }
  if (currentPattern == 1) { patternTwo(frame); }
  if (currentPattern == 2) { patternThree(frame); }
  if (currentPattern == 3) { patternFour(frame); }
  if (currentPattern == 4) { patternFive(frame); }
  if (currentPattern == 5) { patternSix(frame); }

  //select current color scheme
  if (currentColor == 0) { colorOne(frame); }
  if (currentColor == 1) { colorTwo(frame); }
  if (currentColor == 2) { colorOne(frame); }

  //get touch
  const int touch = ADCTouch.read(TOUCHPIN) - ref;
  
  if (touch > 30 && millis() - startOfPress > 500) {
    if (mode == 0) {
      mode = 1;
      eyeIndex = 0;
    } 
    startOfPress = millis();
  }
  if (millis() - startOfPress > rippleTime) {
    mode = 0;
  }

  //new pattern
  if (floor(fSinH[0]*1000) == 999) { 
    newAnim++;
    if (newAnim > randPattern) { 
      newAnim = 0;
      currentPattern = random(0, 6);
    }
  } 

  //new color
  if (floor(fSinW[0]*1000) == 999) { 
    newColor++;
    if (newColor > randColor) { 
      newColor = 0;
      currentColor = random(0, 3);
    }
  }
   
  //output eye
  if (floor(fSinQ*1000) == 999) { 
    eyeIndex++;
    if (eyeIndex == 125) { eyeIndex = 0; }
  } 
  digitalWrite(EYE, code[eyeIndex]);

  if (mode == 1) {
    patternSix(frame);
  }
  for (int i = 0;i < 7;i++) {
      leds[i] = CHSV(h[i], 220, v[i]);
    }
    FastLED.show();
  
  frame = millis()/(16+(2/3));
}

void frameCalc(int frame) {
  rFrame = (frame*(3.14/180));

  fSinLong = 0.5*sin((scale*longScale)*rFrame)+0.5;
  fSinW[0] = 0.5*sin(scale*rFrame)+0.5;
  fSinW[1] = 0.5*sin(scale*(rFrame + offset[0]))+0.5;
  fSinW[2] = 0.5*sin(scale*(rFrame + offset[1]))+0.5;
  fSinW[3] = 0.5*sin(scale*(rFrame + offset[2]))+0.5;
  
  fSinH[0] = 0.5*sin((scale*2)*rFrame)+0.5;
  fSinH[1] = 0.5*sin((scale*2)*(rFrame + offset[0]))+0.5;
  fSinH[2] = 0.5*sin((scale*2)*(rFrame + offset[1]))+0.5;
  fSinH[3] = 0.5*sin((scale*2)*(rFrame + offset[2]))+0.5;
  
  fSinQ = 0.5*sin((scale*4)*rFrame)+0.5;
  fSinE = 0.5*sin((scale*8)*rFrame)+0.5;
}

void mirrorV() {
  v[0] = v[3];
  v[1] = v[2];
  v[6] = v[4];
}

void mirrorH() {
  h[0] = h[3];
  h[1] = h[2];
  h[6] = h[4];
}

void allRandom() {
  for (int i = 0;i<7;i++) {
    h[i] = random(0, 255);
  }
}

void colorOne(int frame) {
  for (int i = 0;i < 7;i++) {
    h[i] = fSinW[0]*255;
  }
}

void colorTwo(int frame) {
  int b = random(0, 255);
  
  for (int i = 0;i<7;i++) {
      h[i] = b;
    }
  
}

void colorThree(int frame) {
  allRandom();
}

void brightnessLoop(int frame, float freq, int ledStart, int ledEnd) {
  for (int i = ledStart;i < ledEnd;i++) {
    v[i] = minBright + (freq*maxBright);
  }
}

void patternOne(int frame) {
   brightnessLoop(frame, fSinH[0], 0,4);
   brightnessLoop(frame, fSinQ, 4,5);
   brightnessLoop(frame, fSinE, 5,6);
   brightnessLoop(frame, fSinQ, 6,7);
}

void patternTwo(int frame) {
  brightnessLoop(frame, fSinH[0], 2,3);
  brightnessLoop(frame, fSinH[1], 3,4);
  brightnessLoop(frame, fSinH[2], 4,5);
  brightnessLoop(frame, fSinH[3], 5,6);

  if (random(0,4) > 2) { mirrorH(); }
  if (random(0,4) > 2) { mirrorV(); }
  
}

void patternThree(int frame) {
  brightnessLoop(frame, fSinH[0], 0,7);
}

void patternFour(int frame) {
  brightnessLoop(frame, fSinH[0], 0,4);
  brightnessLoop(frame, fSinH[3], 4,7);
}

void patternFive(int frame) {
  brightnessLoop(frame, fSinH[0], 1,3);
  brightnessLoop(frame, fSinH[0], 4,5);

  brightnessLoop(frame, fSinE, 3,4);
  brightnessLoop(frame, fSinE, 5,6);
  mirrorV();
  
}

void patternSix(int frame) {
  int a = asin(fSinH[0])*((360*2)/3.14);

  if (a < 0) { a = 360 + a; }
  int led = floor(a/(360/4)) + 2;
  for (int i = 0;i < 7;i++) {
    if (i == led) {
      brightnessLoop(frame, fSinE, i, i+1);
    } else {
      v[i] = 0;
    }
  }
  mirrorV();
}
