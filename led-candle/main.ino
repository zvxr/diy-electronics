/* LED Candle
 *  Flickers using a heat color spectrum. Factors in temperature
 *  from a sensor that affects flame color (warmer favors
 *  oranges and reds, colder favors whites and greens).
 *  
 *  Powered by Micro USB or Lithium Ion Battery with switch.
 *
 *  Components:
 *    Adafruit Trinket M0
 *    Adafruit Switched JST-PH Breakout Board
 *    TMP36 Temperature Sensor
 *    NeoPixel 8mm Through-Hole LED (x2)
  */

#include "FastLED.h"

#define LED_PIN           0
#define NUM_LEDS          2
#define FRAMES_PER_SECOND 60

#define BASE_HEAT_MIN     80
#define BASE_HEAT_MAX     140

#define MIN_BRI           80
#define MAX_BRI           200

#define VOLTAGE           3.3
#define TEMP_PIN          3

#define DEBUG_MODE        false
/*
 ██████  ██       ██████  ██████   █████  ██      ███████ 
██       ██      ██    ██ ██   ██ ██   ██ ██      ██      
██   ███ ██      ██    ██ ██████  ███████ ██      ███████ 
██    ██ ██      ██    ██ ██   ██ ██   ██ ██           ██ 
 ██████  ███████  ██████  ██████  ██   ██ ███████ ███████ 
 */
CRGB leds[NUM_LEDS];
uint8_t temp;
uint8_t minHeat;
uint8_t maxHeat;

/*
  ███████ ███████ ████████ ██    ██ ██████  
  ██      ██         ██    ██    ██ ██   ██ 
  ███████ █████      ██    ██    ██ ██████  
       ██ ██         ██    ██    ██ ██      
  ███████ ███████    ██     ██████  ██                                              
 */
void setup() {
  delay( 2000); // sanity delay

  FastLED.addLeds<NEOPIXEL, LED_PIN>( leds, NUM_LEDS);
  FastLED.setBrightness( MIN_BRI);
}

/*
  ██       ██████   ██████  ██████  
  ██      ██    ██ ██    ██ ██   ██ 
  ██      ██    ██ ██    ██ ██████  
  ██      ██    ██ ██    ██ ██      
  ███████  ██████   ██████  ██      
 */
void loop() {
  updateHeatTemp();
  updateColor();
  updateBrightness();

  FastLED.show();
  FastLED.delay( 1000 / FRAMES_PER_SECOND);
  if ( DEBUG_MODE ){
    Serial.print( "temp = ");
    Serial.print( temp);
    Serial.print( " minHeat = ");
    Serial.print( minHeat);
    Serial.print( " maxHeat = ");
    Serial.println( maxHeat);
  }
}

/*
  ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████ 
  ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██      
  █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████ 
  ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██ 
  ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████ 
 */

// Updates the global brightness. This will increment slowly.
// If it surpasses the MAX_BRI, it instead sets it to a random value
// between MIN_BRI and the half way point (between MIN_BRI and MAX_BRI).
void updateBrightness() {
  static byte bri;

  bri = FastLED.getBrightness() + random8( 0, 1);
  if ( bri > MAX_BRI ){
    bri = random8( MIN_BRI, avg8( MIN_BRI, MAX_BRI));
  }
  FastLED.setBrightness( bri);
}

// Updates the color using FastLED.HeatColor function.
// Gradually will reduce heat. If it goes lower than minHeat, it
// instead will set it to a random value between the half way point
// (between minHeat and maxHeat) and maxHeat.
void updateColor() {
  static byte heat[NUM_LEDS];

  for( int i = 0; i < NUM_LEDS; i++) {
    heat[i] -= random8( 1, 3);
    if( heat[i] < minHeat + temp ) {
      heat[i] = sub8( heat[i], random8( avg8( minHeat, maxHeat), maxHeat));
    }
  }

  for( int j = 0; j < NUM_LEDS; j++) {
    CRGB color = HeatColor( heat[j]);
    leds[j] = color;
  }
}

// Updates temp, minHeat, maxHeat.
// Applies a simple formula to minHeat using BASE_MIN_HEAT plus double the
// temperature, and maxHeat using BASE_MAX_HEAT plus triple the temperature.
// All values are constrained between 0 and 255.
void updateHeatTemp() {
  float voltage = analogRead( TEMP_PIN) * VOLTAGE;
  voltage /= 1024.0;

  temp = (voltage - 0.5) * 100;
  minHeat = constrain( BASE_HEAT_MIN + (temp * 2), 0, 255);
  maxHeat = constrain( BASE_HEAT_MAX + (temp * 3), 0, 255);
}
