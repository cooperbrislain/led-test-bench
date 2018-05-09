#include "FastLED.h"
#include <LiquidCrystal.h>

#define IS_WS2812 0
#define IS_APA102 1

#define NUM_LEDS 200 // maximum ever; we won't use them all
#define TEST_LEDS 5
#define DATA_PIN 7
#define CLOCK_PIN 6
#define SPEED_PIN A0
#define FRAME_DELAY 100
#define BUTTON_PIN 4
#define NUM_SPECS 3
#define PI 3.1415
//#define BRIGHTNESS_PIN 0
//#define SPEED_PIN 1
#define MIN_BRIGHTNESS 32
#define MAX_BRIGHTNESS 255
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int lcd_key     = 0;
int adc_key_in  = 0;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

CRGB leds[NUM_LEDS];

int index;
int spec_index;
int count;
int color;
int brightness;
int speed;
int specs[NUM_SPECS];
specs[0] = 52;
specs[1] = 73;
specs[2] = 143;

int num_leds;

int read_lcd_buttons() {
    adc_key_in = analogRead(0);      // read the value from the sensor
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
    if (adc_key_in < 50)   return btnRIGHT;
    if (adc_key_in < 250)  return btnUP;
    if (adc_key_in < 380)  return btnDOWN;
    if (adc_key_in < 420)  return btnLEFT;
    if (adc_key_in < 700)  return btnSELECT;
    return btnNONE;  // when all others fail, return this...
}

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
    lcd.begin(16, 2);              // start the library
    lcd.setCursor(0,0);
    lcd.print("Velleman VMA203"); // print a simple message
    //Serial.begin(9600);
    if(IS_APA102) {
        FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    }
    if(IS_WS2812) {
        FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    }
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    count=0;
    spec_index = 0;
    num_leds = specs[spec_index];
    index = 0;
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

void randstrobe() {
    for(int i=0; i<10000; i++) {
        if(i%8==0) {
            for(int j=random(NUM_LEDS); j<random(NUM_LEDS)+random(NUM_LEDS/3); j++) {
                leds[j%NUM_LEDS] += CHSV(255,0,180);
            }
        }
        FastLED.show();
        fadeby(50);
        count++;
    }
}

void strobe() {
    for(int i=0; i<10000; i++) {
        if(i%4==0) {
            int which = random(4);
            for(i=which*143; i<which*143+143; i++) {
                leds[i]+= CHSV(0,0,188);
            }
        }
        FastLED.show();
        count++;
    }
}

void fill(CRGB color) {
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = color;
    }
}

void light_tests() {
    for(int i=0; i<TEST_LEDS-1; i++) {
        leds[i] = CRGB::Red;
        leds[TEST_LEDS+num_leds+TEST_LEDS-(i+1)] = CRGB::Blue;
        FastLED.show();
        delay(1000);
    }
    fill(CRGB::Green);
    FastLED.show();
}

void test_strip() {
    // assumes 5 LED test strip before, and after
    // num_leds is the number of LEDs in the strip to be tested
    for (int i=0; i<num_leds; i++) {
        CRGB color = CRGB::White;
        if ((i+1)%10 == 0) color = CRGB::Blue;
        if ((i+1)%100 == 0) color = CRGB::Red;
        leds[TEST_LEDS+i] = color;
        leds[TEST_LEDS+num_leds-(i+1)] = color;
        FastLED.show();
        fadeby(25);
        delay(3);
    }
    // as long as the first LED is still lit, keep fading out
    // (others shoudl not be lit by the time this one fades out)
    do {
        fadeby(25);
        FastLED.show();
        delay(3);
    } while (leds[TEST_LEDS]);
    delay(FRAME_DELAY);
}

void initialize() {
    fill(CRGB::Black);
    FastLED.show();
    count = 0;
    light_tests(num_leds);
}

void loop() {
    initialize(num_leds);
    test_strip(num_leds);
    if (int btn = read_lcd_buttons() != btnNONE) {
        switch( btn ) {
            case btnUP :
                if(spec_index > 0) {
                    spec_index--;
                    num_leds = specs[spec_index];
                }
                break;
            case btnDOWN :
                spec_index = (spec_index + 1)%NUM_SPECS;
                num_leds = specs[spec_index];
                break;
        }
    }
    delay(FRAME_DELAY);
    count++;
}