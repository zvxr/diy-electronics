/* Audio Matrix
 *  
 *  Reads and displays audio input on a color cycling spectrum.
 *  Input through a 3.5 audio jack, output to a 16x16 LED matrix.
 *
 * Components:
 *  - M4 Airlift Lite (NOTE: any Arduino R3 compat M0/M4: Airlift only for future features).
 *  - Adafruit DotStar Matrix 16x16
 *  - SparkFun Spectrum Shield
 */

#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

#define STROBE 4
#define RESET 5
#define DC_LEFT A0
#define DC_RIGHT A1
#define DATA_PIN 11
#define CLOCK_PIN 13
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16

Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
 MATRIX_WIDTH, MATRIX_HEIGHT,
 DATA_PIN, CLOCK_PIN,
 DS_MATRIX_TOP + DS_MATRIX_LEFT +
 DS_MATRIX_COLUMNS + DS_MATRIX_ZIGZAG,
 DOTSTAR_BGR);

int frequenciesLeft[7];
int frequenciesRight[7];

void setup() {
  // Matrix Set-up
  matrix.begin();
  matrix.setBrightness(4);
  matrix.show();

  //Set spectrum Shield pin configurations
  pinMode(STROBE, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(DC_LEFT, INPUT);
  pinMode(DC_RIGHT, INPUT);

  //Initialize Spectrum Analyzers
  digitalWrite(STROBE, LOW);
  digitalWrite(RESET, LOW);


  delay(20);
  Serial.begin(9600);
}

void loop() {
  uint16_t color;
  unsigned int colors[3];
  colors[0] = 255;
  colors[1] = 0;
  colors[2] = 0;

  // Cycle through colors.
  for( int dec = 0; dec < 3; dec += 1 ){
    int inc = dec == 2 ? 0 : dec + 1;

    // cross-fade two selected colors.
    for( int i = 0; i < 255; i += 1 ){
      colors[dec] -= 1;
      colors[inc] += 1;

      // read and display frequencies in the selected color.
      color = matrix.Color(colors[0], colors[1], colors[2]);
      readFrequencies();
      displayFrequencies(color);
      delay(20);
    }
  }
}


void readFrequencies() {
  static int freq_amp;
  digitalWrite(RESET, HIGH);
  delayMicroseconds(200);
  digitalWrite(RESET, LOW);
  delayMicroseconds(200);

  //Read frequencies for each band
  for( freq_amp = 0; freq_amp < 7; freq_amp++ ){
    digitalWrite(STROBE, HIGH);
    delayMicroseconds(50);
    digitalWrite(STROBE, LOW);
    delayMicroseconds(50);

    frequenciesLeft[freq_amp] = analogRead(DC_LEFT);
    frequenciesRight[freq_amp] = analogRead(DC_RIGHT);
  }
}

void displayFrequencies(uint16_t color){
  matrix.fillScreen(0);
  static uint8_t weight;
  static uint8_t index;

  // draw left
  for( int row=0; row<7; row++ ){
    // max range 1024 / number of cells 16.
    // We also allow any value >1000 to indicate all cells.
    if( frequenciesLeft[row] > 1000 ){
      weight = 16;
    } else {
      weight = (frequenciesLeft[row] / 64);
    }
    for( int col=0; col<weight; col++ ){
      // double up on first cell (0 and 1).
      if( row == 0 ){
        index = row;
        matrix.drawPixel(index, col, color);
      }
      index = row * 2 + 1;
      matrix.drawPixel(index, col, color);
    }
  }

  // draw right
  for( int row=0; row<7; row++ ){
    // max range 1024 / number of cells 16.
    // We also allow any value >1000 to indicate all cells.
    if( frequenciesRight[row] > 1000 ){
      weight = 16;
    } else {
      weight = (frequenciesRight[row] / 64);
    }

    for( int col=0; col<weight; col++ ){
      index = row * 2 + 2;
      matrix.drawPixel(index, col, color);
      // double up on last cell (14 and 15).
      if( row == 6 ){
        index = row * 2 + 3;
        matrix.drawPixel(index, col, color);
      }
    }
  }
  Serial.println();

  matrix.show();
}
