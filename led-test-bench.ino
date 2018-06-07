#include "SPI.h"
#include "FastLED.h"
#include "LiquidCrystal.h"

#define IS_WS2812 0
#define IS_APA102 1

#define PRE_LEDS 0
#define POST_LEDS 1
#define NUM_LEDS 300
#define CLOCK_PIN 31
#define DATA_PIN 30
#define FRAME_DELAY 100
#define PI 3.1415

#define NUM_SPECS 7
#define NUM_MODES 5

#define CHASE_WHITE 0
#define FILL_WHITE 1
#define CHASE_COLOR 2
#define FILL_COLOR 3
#define TEST_STRIP 4

#define halt(s) { Serial.println(F( s )); while(1);  }

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;


// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold

 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 250)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 420)  return btnLEFT;
 if (adc_key_in < 700)  return btnSELECT;


 // For V1.0 comment the other threshold and use the one below:
/*
 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;
*/


    return btnNONE;  // when all others fail, return this...
}

int num_leds;

CRGB leds[NUM_LEDS];

unsigned int mode;
unsigned int index;
unsigned int spec_index;
unsigned int count;
unsigned int hue;
unsigned int brightness;
unsigned int pressed;
unsigned int pressed_time;

unsigned int specs[NUM_SPECS];

bool on;

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
    Serial.begin(115200);
    count=0;
    index = 0;
    brightness = 35;
    mode = 0;

    specs[4] = 27;
    specs[0] = 42;
    specs[1] = 74;
    specs[2] = 113;
    specs[3] = 143;
    specs[5] = 250;
    specs[6] = 500;

    spec_index = 0;
    num_leds = specs[spec_index];

    lcd.begin(16, 2);              // start the library
    lcd.setCursor(0,0);
    lcd.print("LED Field Tester"); // print a simple message
    Serial.println("Starting LED Test Bench");
    if(IS_APA102) {
        FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
    }
    if(IS_WS2812) {
        FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    }
    initialize();
}

void chase(CRGB color) {
    fade();
    leds[count%num_leds] = color;
}

void fill(CRGB color) {
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = color;
    }
}

void turn_on() {

}

void turn_off() {

}

void test_strip() {
    for(int i=0;i<num_leds;i++) {
        leds[i] = CHSV(((i+count)/4)%2==0? 40 : 160, 255, brightness);
    }
    FastLED.show();
    count++;
    delay(40);
}

void initialize() {
    for (int i=0; i<NUM_LEDS; i++) {
        fade();
        leds[i] = CRGB::White;
        FastLED.show();
    }
    while(leds[NUM_LEDS-1] != CRGB(0,0,0)) {
        fade();
        FastLED.show();
    }
    FastLED.show();
    count = 0;
}

void blink_end() {
    if (num_leds != NUM_LEDS) {
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
    int lcd_key = read_LCD_buttons();
    Serial.println(lcd_key);
    char buffer[4];
    if (pressed == lcd_key && lcd_key != btnNONE) {
        Serial.print("still pressed for ");
        Serial.print(pressed_time);
        Serial.println(" seconds");
        pressed_time++;
    } else if (lcd_key == btnNONE && pressed != btnNONE) {
        Serial.print("released after ");
        Serial.print(pressed_time);
        Serial.println(" seconds");
    } else if (lcd_key != btnNONE && pressed == btnNONE) {
        Serial.println("pressed");
        pressed = lcd_key;
        pressed_time = 0;
    }
    if (pressed == lcd_key && pressed_time > 3) {
        Serial.println("long press");
        if (pressed == lcd_key && pressed_time > 3) {
            switch (pressed) {
                case btnUP:
                    hue++;
                    lcd.setCursor(0,1);
                    lcd.print("Hue ");
                    lcd.print(" - ");
                    lcd.print(hue);
                    break;
                case btnDOWN:
                    hue--;
                    if (hue < 255) {
                        hue= 0;
                    }
                    lcd.setCursor(0,1);
                    lcd.print("Hue ");
                    lcd.print(" - ");
                    lcd.print(hue);
                    break;
                case btnLEFT:
                    spec_index++;
                    if(spec_index == NUM_SPECS) {
                        spec_index = 0;
                    }
                    num_leds = specs[spec_index];
                    lcd.setCursor(0,1);
                    lcd.print("spec ");
                    lcd.print(spec_index);
                    lcd.print(" - ");
                    lcd.print(num_leds);
                    lcd.print("px");
                    break;
            }
        }
        pressed_time++;
    }
    if (!pressed && lcd_key != btnNONE) {
        if (pressed_time < 3) { // short press on release
            switch (pressed) {
                case btnUP:
                    if (!on) {
                        on = 1;
                        lcd.setCursor(0,1);
                        lcd.print("On");
                    }
                    break;
                case btnDOWN:
                    if (on) {
                        on = 0;
                        lcd.setCursor(0,1);
                        lcd.print("Off");
                    }
                    break;
                default:
                    break;
            }
        }
    }
    switch (mode) {
        case CHASE_WHITE:
            chase(CRGB::White);
            break;
        case FILL_WHITE:
            fill(CRGB::White);
            break;
        case CHASE_COLOR:
            chase(CHSV(hue,255,255));
            break;
        case FILL_COLOR:
            fill(CHSV(hue,255,255));
            break;
        case TEST_STRIP:
            test_strip();
            break;
    }
    FastLED.show();
    count++;
    delay(FRAME_DELAY);
}


