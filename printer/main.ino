/* Printer version 102
 *  2021-05: This is a STABLE build.
 *
 *  Adafruit Feather M0 Adalogger [ Board: Feather M0 ]
 */

#include <SPI.h>
#include <SD.h>

#include <Adafruit_NeoPixel.h>
#include "Adafruit_Thermal.h"

#include "FastLED.h"

#define LED_PIN     10  // neopixel

#define USB_LED_PIN 13  // red LED, next to the USB jack

#define CS_PIN      4   // MicroSD card CS (chip select) pin
#define CD_PIN      7   // MicroSD card CD (card detect) pin
#define SD_LED_PIN  8   // green LED, next to the SD card

#define BUTTON_PIN  12  // push button

/*
 ██████  ██       ██████  ██████   █████  ██      ███████ 
██       ██      ██    ██ ██   ██ ██   ██ ██      ██      
██   ███ ██      ██    ██ ██████  ███████ ██      ███████ 
██    ██ ██      ██    ██ ██   ██ ██   ██ ██           ██ 
 ██████  ███████  ██████  ██████  ██   ██ ███████ ███████ 
 */

CRGB leds[1];

File assetDir;
File assetFile;

Adafruit_Thermal printer( &Serial1);


/*
  ███████ ███████ ████████ ██    ██ ██████  
  ██      ██         ██    ██    ██ ██   ██ 
  ███████ █████      ██    ██    ██ ██████  
       ██ ██         ██    ██    ██ ██      
  ███████ ███████    ██     ██████  ██                                              
 */
void setup(){

  pinMode( USB_LED_PIN, OUTPUT);
  pinMode( SD_LED_PIN, OUTPUT);
  pinMode( BUTTON_PIN, INPUT);

  // Set-up NeoPixel.
  FastLED.addLeds<NEOPIXEL, LED_PIN>( leds, 1).setCorrection( TypicalSMD5050);
  leds[0] = CRGB::Orange;
  FastLED.show();

  // Set-up SD Card.
  if ( !SD.begin( CS_PIN) ){
    error( 2);
  }
  leds[0] = CRGB::Yellow;
  FastLED.show();

  // Set-up Printer.
  Serial1.begin( 9600);
  printer.begin();
  leds[0] = CRGB::Green;
  FastLED.show();

}

/*
  ██       ██████   ██████  ██████  
  ██      ██    ██ ██    ██ ██   ██ 
  ██      ██    ██ ██    ██ ██████  
  ██      ██    ██ ██    ██ ██      
  ███████  ██████   ██████  ██      
 */
void loop(){
  static int switchVal;

  switchVal = digitalRead( BUTTON_PIN);
  if ( switchVal == HIGH ){
    digitalWrite( SD_LED_PIN, HIGH);
    streamFileToPrinter();
    delay( 100);
    digitalWrite( SD_LED_PIN, LOW);
    delay( 1000);
  }

  cycleLeds();
  FastLED.delay( 32);  // 30fps
}

/*
  ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████ 
  ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██      
  █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████ 
  ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██ 
  ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████ 
 */

// Increment hue of neopixel LED and show.
 void cycleLeds(){
  static uint8_t hue = 0;
  leds[0] = CHSV( hue++, 255, 150);
  FastLED.show();
 }


// This will operate on the oldest file in asset directory, `v1/`.
// File is opened and streamed directly to printer. The first
// line is centered at Medium size. Remaining lines are left
// justified at Small size. File is permanently deleted after
// printing.
void streamFileToPrinter(){
  leds[0] = CRGB::SeaGreen;
  FastLED.show();

  assetDir = SD.open( "V1");
  assetFile = assetDir.openNextFile();
  if ( assetFile ){
    String filename = assetFile.name();

    printer.wake();
    printer.reset();
    printer.online();

    leds[0] = CRGB::Blue;
    FastLED.show();

    // Title: Center justified, medium
    String title = assetFile.readStringUntil( '\n');
    printer.feed( 1);
    printer.justify( 'C');
    printer.setSize( 'M');
    printer.println( title);
    delay( 300);

    leds[0] = CRGB::DarkBlue;
    FastLED.show();

    // Body: left justified, small
    printer.justify( 'L');
    printer.setSize( 'S');

    char byteRead;
    int availableBytes = assetFile.available();
    for( int i=0; i<availableBytes; i++ ){
      byteRead = assetFile.read();
      printer.print( byteRead);
    }

    printer.feed( 5);
    assetFile.close();

    leds[0] = CRGB::Purple;
    FastLED.show();

    String path = String( "V1/" + filename);
    SD.remove( path);
  } else {
    error( 3);
  }
}


// Blink out the appropriate error code, indefinitely.
// 2: Failure to access SD Card.
// 3: No files available.
void error( uint8_t errno){
  while( 1 ){
    uint8_t i;
    for ( i=0; i<errno; i++ ){
      digitalWrite( USB_LED_PIN, HIGH);
      delay( 100);
      digitalWrite( USB_LED_PIN, LOW);
      delay( 100);
    }
    for ( i=errno; i<10; i++ ){
      delay( 200);
    }
  }
}
