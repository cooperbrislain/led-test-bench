#include "FastLED.h"

#define NUM_LEDS 120
#define DATA_PIN 7
#define CLOCK_PIN 6
//#define BRIGHTNESS_PIN 0
//#define SPEED_PIN 1
#define MIN_BRIGHTNESS 32
#define MAX_BRIGHTNESS 255

CRGB leds[NUM_LEDS];

int index;
int index2;
int count;
int color;
int brightness;
int speed;

void fade(){
    for(int i=0; i<NUM_LEDS; i++){ 
         leds[i].fadeToBlackBy( 10 );
    }
}

void fadeby(int b){
    for(int i=0; i<NUM_LEDS; i++){ 
         leds[i].fadeToBlackBy( b );
    }
}

void fade_out_by(int b){
    if(brightness > 0) {
        brightness -= b;
        FastLED.setBrightness(brightness);
    }
}

void setup() {
    //Serial.begin(9600);
    FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;  
    }
    FastLED.show();
    count=0;
    randomSeed(analogRead(A0)); 
    index = 0;
    index2 = 0;
    speed = 25;
}

void all_on() {
    for(int i=0; i<NUM_LEDS;i++) {
        leds[i] = CRGB::Blue;
    }
}

void chase_white() {
    for(int i=0; i<NUM_LEDS;i++){
        if((i+count)%10==0) {
            leds[i] += CHSV(color+(((count+count/25)/10)%2*128)%255, 255, 85);
        }
    }
}

void count_leds() {
    color = 40;
    for (int i=0; i<500; i++) {
        leds[i] = CHSV(color,255,255);
        if(i%5==0) {
            leds[i] = CHSV((color+128)%255,255,255);
            delay(1000);
        }
        delay(512);
    }
}

void loop() {
    if(count%100) {
        color=(color+1)%255;
    }
    chase_white();
    fadeby(1);
    FastLED.show();
    count++;
    delay(speed);
}