# Humidor Sensor
Temperature and humidity data is collected and displayed on a simple OLED
interface. Provides current reading as well as rolling averages over a period
of time. This project also experiments with graphical animations, and includes
an LED for simple red/yellow/green temperature/humidity levels.

![humidor](assets/humidor.jpeg)
![averages](assets/averages.jpeg)

## Components
- Adafruit Feather M0 Basic Proto
- Adafruit 128x64 OLED FeatherWing
- Adafruit SHT40 Sensirion Temperature & Humidity Sensor
- NeoPixel 8mm Through-Hole LED (x2)

## Construction

By using a FeatherWing Doubler, wiring was quite simple an allowed for battery
back-up to be included with the set-up. A small 400mA battery fit perfectly
beneath the interface.

## Diagrams

## Libraries

Written for Arduino (C++).

- Adafruit SHT4x
- FastLED

## Programming

Although M0 Basic Proto is listed as the board, I was determined to make a
humidor that could carry a small enough memory footprint to function with a
Atmega328P chip (with its infamously compact 32k Flash, 2k SRAM).

The OLED interface included three buttons, which I programmed to:

A. Display the current temperature and humidity.
B. Display min, mean, max temperature and humidity over a rolling period of
   time.
C. Display a graphical animation.

Though the animation would not fit on Atmega328P, it does fit quite comfortably
on other Adafruit M0 (ATSAMD21G18) or M4 (ATSAMD51) boards.

Calculating averages proved to be a quirky challenge given the space
limitations. One could store every value for a period of time, and calculate
the min, mean, and max as necessary. However, when wanting to display the
averages for a day or longer at 1 second granularity, this simply does not fit
with Atmega328P's 2k SRAM, or even 32k of more powerful chips.

Calculating the mean does not actually require all the values. Instead, we
store just the minValue, maxValue, the lastValue, and a count. The minValue and
maxValue only need to be compared and replaced if necessary with each incoming
reading. The lastValue represents the last average that was calculated, and the
count represents the number of values that make up the average. These two can
be applied to any incoming value to get an accurate new average.

```
count = (count + 1)
lastValue = ((lastValue * count) + newValue) / count
```

Consider these data points:
```
# Storing All Values
data = [5, 7, 3]
average = (5+7+3) / 3 = 5

# Storing count and lastValue
count = 3
lastValue = 5
```

We then add a fourth value to each algorithm:
```
# Storing All Values
data = [5, 7, 3] + [9]
average = (5+7+3+9) / 4 = 6

# Storing count and lastValue
count = (3 + 1) = 4
lastValue = ((5 * 3) + 9) / 4 = 6
```

And a fifth:
```
# Storing All Values
data = [5, 7, 3, 9] + [6]
average = (5+7+3+9+6) / 5 = 5

# Storing count and lastValue
count = (4 + 1) = 5
lastValue = ((6 * 4) + 6) / 5 = 6
```

This tecnique/algorithm has one limitation for the scope of this project: since
it does not know the values it has stored, it cannot calculate rolling averages
(that is, averages over the past 24 hours rather than averages since it was
turned on). For the nature of this project (averages at a glance), I opted for
a compression based on count. Basically at some point (1.5 days), the count is
reduced by a day (to 0.5 days). Calculations then continue to be made as
normal. This makes new incoming data more influential immediately following
compression, but where the lastValue (average) is most influenced by the past
day of data. It is not perfect, but at a glance with a screen that outputs 0.01
resolution on a sensor that has 0.02 C temperature accuracy and 1.8% relative
humidity accuracy, it felt perfectly fine for keeping the project simple and
memory efficient while still delivering meaningful and fairly accurate metrics.

## Demo
