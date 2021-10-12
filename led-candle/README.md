# LED Candle
Flickers using a heat color spectrum. Factors in temperature from a sensor that
affects flame color (warmer favors oranges and reds, colder favors whites and
greens).

Powered by Micro USB or Lithium Ion Battery with switch.

![led candle](assets/candle.jpeg)

## Components
- Adafruit Trinket M0
- Adafruit Switched JST-PH Breakout Board
- TMP36 Temperature Sensor
- NeoPixel 8mm Through-Hole LED (x2)

## Construction
I reused some old metal candleholders I had, cutting out a piece of wood that
the unit could comfortably sit on. I used Adafruit protoboard, feeding the
wiring through to house the battery beneath.

## Diagrams
![schematics on protoboard](assets/diagram.jpg)

## Libraries

Written for Arduino (C++).

- FastLED

## Programming

A simple weekend project, it was enjoyable experimenting and coming up with a
simple algorithm that aimed to create a "flickering" effect. The temperature
sensor input was factored in to affect color, all of which relied on
`FastLED.HeatColor` to render.

I ended on two independent patterns:

- Brightness would randomly (but very gradually and incrementally) increase
  until hitting a maximum. It would then immediately drop to a random value
  between the minimum and maximum.
- Color, or Heat, would randomly (gradually and incrementally) decay until
  hitting a minimum, before jumping back to a value between the minimum and
  maximum. Rather than being fixed (as with brightness), the minimum and
  maximum fluxuate based on the temperature sensor, where a warmer temperature
  would allow for more oranges and reds, and a colder temperature would allow
  for more greens and whites.


## Demo
![working prototype](assets/demo.gif)
