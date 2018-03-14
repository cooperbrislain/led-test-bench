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
int num_leds;

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
    color = 80;
    for (int i=0; i<500; i++) {
        if(i%50==49) {
            leds[i] = CHSV((color+128)%255,255,128);
        }
        else if(i%10==9) {
            leds[i] = CHSV((color+96)%255,255,128);
        }
        else if(i%5==4) {
            leds[i] = CHSV((color+64)%255,255,64);
        } else {
            leds[i] = CHSV(color,255,40);
        }
        FastLED.show();
        delay(50*((i%50==49)+(i%10==9)+(i%5==4)+1)*2);
    }
    delay(5000);
}

void loop() {
    count_leds();
    if(count%100) {
        color=(color+1)%255;
    }
    //chase_white();
    fadeby(1);
    FastLED.show();
    count++;
    delay(speed);
}