#include "FastLED.h"

#define IS_WS2812 0
#define IS_APA102 1

#define NUM_LEDS 180
#define DATA_PIN 7
#define CLOCK_PIN 6
#define SPEED_PIN A0
#define BUTTON_PIN 4
#define PI 3.1415
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

void countfade() {
    for (int i=0;i<count;i++) {
        if(i%10!=9) {
            leds[i].fadeToBlackBy(10);
        }
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
    if(IS_APA102) {
        FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    }
    if(IS_WS2812) {
        FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    }
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    count=0;
    num_leds = NUM_LEDS;
    //randomSeed(analogRead(A3));
    index = 0;
    index2 = 0;
    speed = 0;
}

void chase_white() {
    for(int i=0; i<NUM_LEDS;i++){
        if((i+count)%10==0) {
            leds[i] += CHSV(color+(((count+count/25)/10)%2*128)%255, 255, 85);
        }
    }
}

int count_leds() {
    color = 80;
    count = 0;
    for (int i=index; i<500; i++) {
        countfade();
        if(digitalRead(BUTTON_PIN) == HIGH) {
            num_leds = i-(i%10);
            break;
        }
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
        color+=3;
        FastLED.show();
        count++;
        delay(50*((i%50==49)+(i%10==9)+(i%5==4)+1)*2);
    }
    return count;
    delay(5000);
}

void sinewave(){
    for(int i=0;i<1000;i++){
        fade();
        leds[(num_leds+(int)(sin(i*PI/180)*num_leds/2+num_leds/2))%num_leds] = CRGB::White;
        FastLED.show();
        delay(10);
    }
}

void loop() {
    sinewave();
    num_leds = count_leds();
    if(count%100) {
        color=(color+1)%255;
    }
    fadeby(1);
    FastLED.show();
    count++;
    delay(speed);
}