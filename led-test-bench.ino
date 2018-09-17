#include "FastLED.h"

#define DEBUG 0

#define IS_WS2812 1
#define IS_APA102 0
#define IS_LPD8806 0

#define DATA_PIN 7
#define CLOCK_PIN 6

#define PRE_LEDS 0
#define POST_LEDS 0
#define NUM_LEDS 20
#define FRAME_DELAY 13
#define PI 3.1415

#define NUM_SPECS 7
#define NUM_MODES 5

#define CHASE_WHITE 0
#define FILL_WHITE 1
#define CHASE_COLOR 2
#define FILL_COLOR 3
#define TEST_STRIP 4

#define FADE_AMT 5
#define FADE_RATE 3

int num_leds;

CRGB leds[NUM_LEDS];

unsigned int mode = 0;
unsigned int index1 = 0;
unsigned int index2 = 0;
unsigned int index3 = 0;
unsigned int spec_index = 0;
unsigned int count = 0;
unsigned int hue1 = 0;
unsigned int hue2 = 0;
unsigned int hue3 = 0;
unsigned int brightness = 150;
unsigned int saturation = 77;
unsigned int specs[NUM_SPECS];
unsigned int step1 = 0;
unsigned int step2 = 0;
unsigned int step3 = 0;
unsigned int max_brightness = 255;

unsigned int brightness1 = brightness;
unsigned int brightness2 = brightness;
unsigned int brightness3 = brightness;

void fade(int amount){
    if (count%FADE_RATE == 0) {
        for(int i=0; i<NUM_LEDS; i++){
             //leds[i]--;
             leds[i].fadeToBlackBy(amount);
        }
    }
}

void cap(){
    for(int i=0; i<NUM_LEDS; i++){
         leds[i]|=max_brightness;
    }
}

void setup() {
    count=0;
    mode = 0;

    specs[4] = 27;
    specs[0] = 42;
    specs[1] = 74;
    specs[2] = 113;
    specs[3] = 143;
    specs[5] = 250;
    specs[6] = 500;

    spec_index = 5;
    //num_leds = specs[spec_index];
    num_leds = NUM_LEDS;

    if(IS_APA102) {
        FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    }
    if(IS_WS2812) {
        FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    }
    if(IS_LPD8806) {
        FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    }
    initialize();
}

void chase_specs(CRGB color) {
    CRGB color2 = CHSV((hue1+80)%255, 355, brightness );
    fade(FADE_AMT);
    leds[count%num_leds] = color;
    for (int i = 0; i<NUM_SPECS; i++) {
        leds[specs[i]-1] = color2;
    }
}

void chase(CRGB color) {
    leds[count%num_leds] = color;
}

void fill(CRGB color) {
    for(int i=0; i<num_leds; i++) {
        leds[i] = color;
    }
}

void test_strip() {
    for(int i=0;i<num_leds;i++) {
        leds[i] = CHSV(((i+count)/4)%2==0? 40 : 160, 255, brightness);
    }
    FastLED.show();
    count++;
    delay(40);
}

void randpulse() {
    if (count%79 == 0) {
        hue2 = hue3;
        hue1 = (hue1 + (100 + random(30) - 15))%255;
        hue3 = (hue2 + 128)%255;
        step1 = random(3)+1;
        step2 = random(3)+1;
        step3 = random(3)+1;
    }
    if (count%47 == 0) {
        index1 = (index1+step1)%NUM_LEDS;
        index2 = (index2+step2)%NUM_LEDS;
        index3 = (index3+step3)%NUM_LEDS;
    }
    /*if (count%88 == 0) {
        brightness1 = random(brightness) + brightness/2;
        brightness2 = random(brightness) + brightness/2;
        brightness3 = random(brightness) + brightness/2;
    }*/
    CRGB color = CHSV(hue1, saturation, brightness1);
    CRGB color2 = CHSV(hue2, saturation, brightness2);
    CRGB color3 = CHSV(hue3, saturation, brightness3);
    leds[index1] += color;
    leds[index2] += color2;
    leds[index3] += color3;
}

void strobe() {
    index1 = random(NUM_LEDS);
    leds[index1] = (count%2==0? CRGB::Aquamarine: CRGB::DarkViolet);
}


void initialize() {
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    count = 0;
}

void blink_end() {
    if (num_leds < NUM_LEDS) {
        if (count%25 == 0) {
            if (leds[num_leds] != CRGB(0,0,0)) {
                leds[num_leds] = CRGB::Black;
            } else {
                leds[num_leds] = CRGB::White;
            }
        }
    }
}

void loop() {
    fade(FADE_AMT);
    //randpulse();
    if (count%5 > 3)
        chase(CHSV(228, 255, 180));
    if (count%23 == 0)
        strobe();
    FastLED.show();
    count++;
    delay(FRAME_DELAY);
}


