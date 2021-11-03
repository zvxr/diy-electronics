"""
Living Room + Outdoors Controller

Hardware:
- Adafruit M4 Express w/ AirLift ESP32 WiFi Co-Processor

Features:
- Potentiometer controls LED bar grid
- Rotary+ controls Neopixel
"""

import board
import digitalio
import neopixel
import random
import rotaryio
import time
from analogio import AnalogIn
 
import air   # auto-connects wifi
import hue


pixels = neopixel.NeoPixel(board.D0, 1, brightness=0.5) 

button = digitalio.DigitalInOut(board.D2)
button.direction = digitalio.Direction.INPUT
button.pull = digitalio.Pull.UP

# Controls brightness.
potentiometer = AnalogIn(board.A1)

# Bar LEDs, indicates brightness.
bar_leds = [
    digitalio.DigitalInOut(board.D13),
    digitalio.DigitalInOut(board.D12),
    digitalio.DigitalInOut(board.D11),
    digitalio.DigitalInOut(board.D10),
    digitalio.DigitalInOut(board.D9),
    digitalio.DigitalInOut(board.D8),
    digitalio.DigitalInOut(board.D7),
    digitalio.DigitalInOut(board.D6),
    digitalio.DigitalInOut(board.D5),
    digitalio.DigitalInOut(board.D4)
]
for led in bar_leds:
    led.direction = digitalio.Direction.OUTPUT

# Controls color.
encoder = rotaryio.IncrementalEncoder(board.D1, board.D3)

# Lights
# Integer IDs of all lights go here.
FOREGROUND_LIGHTS = tuple()
BACKGROUND_LIGHTS = tuple()

LIGHTS = FOREGROUND_LIGHTS + BACKGROUND_LIGHTS


def bars_led_demo():
    """
    Simple full increment, full decrement of LED bars.
    """
    for led in bar_leds:
        led.value = False

    for led in bar_leds:
        led.value = True
        time.sleep(0.1)

    for i in range(len(bar_leds) - 1, -1, -1):
        bar_leds[i].value = False
        time.sleep(0.1)


def button_pressed(potentiometer_score, encoder_score):
    """
    Applies the current scores to Hue lighting.
    """
    # Blink bar LEDs as acknowledgement.
    for _ in range(3):
        clear_bar_leds()
        time.sleep(0.1)
        show_bar_leds(potentiometer_score)
        time.sleep(0.1)

    # Foreground lights are set to a brighter state.
    for lid in FOREGROUND_LIGHTS:
        hue.set_light_state(
            lid,
            on=True,
            bri=potentiometer_score * 25,  # 25-250
            hue=get_hue(encoder_score),
            sat=250
        )

    # Background lights are set to a dimmer state.
    # A potentiometer_score of 1 or 2 will turn lights off.
    for lid in BACKGROUND_LIGHTS:
        if potentiometer_score < 3:
            hue.set_light_state(lid, on=False)
        else:
            hue.set_light_state(
                lid,
                on=True,
                bri=potentiometer_score * 10,  # 30-100
                hue=get_hue(encoder_score),
                sat=200
            )


def clear_bar_leds():
    """
    Simply set the entire bar to off.
    """
    for led in bar_leds:
        led.value = False


def get_hue(encoder_score):
    """
    Handwavey af method to get hue.
    Wheel is 0-255, so * 256 to get us near the max 65535.
    We apply random -4000 to +4000 value to give some (analogous) variation.
    """
    return (
        (
            encoder_score * 256
        ) + random.randint(-4000, 4000)
    ) % 65535


def get_potentiometer_score():
    """
    Read the potentiometer value and return a {1 - 10} score.
    This gives a bit of a buffer to higher values (indication lower score),
    as maxing out will cause sudden jumps in reported value.
    """
    if potentiometer.value < 1000:
        potentiometer_score = 10
    elif potentiometer.value < 4000:
        potentiometer_score = 9
    elif potentiometer.value < 8000:
        potentiometer_score = 8
    elif potentiometer.value < 12000:
        potentiometer_score = 7
    elif potentiometer.value < 16000:
        potentiometer_score = 6
    elif potentiometer.value < 20000:
        potentiometer_score = 5
    elif potentiometer.value < 24000:
        potentiometer_score = 4
    elif potentiometer.value < 28000:
        potentiometer_score = 3
    elif potentiometer.value < 32000:
        potentiometer_score = 2
    else:
        potentiometer_score = 1

    return potentiometer_score


def get_rgb(encoder_score):
    """
    Converts {0-255} score into a RGB tuple.
    """
    if encoder_score < 0 or encoder_score > 255:
        r = g = b = 0
    elif encoder_score < 85:  # red (green)
        r = int(255 - encoder_score * 3)
        g = int(encoder_score * 3)
        b = 0
    elif encoder_score < 170: # green (blue)
        r = 0
        g = int(255 - (encoder_score - 85) * 3)
        b = int((encoder_score - 85) * 3)
    else:                   # blue (red)
        r = int((encoder_score - 170) * 3)
        g = 0
        b = int(255 - (encoder_score - 170) * 3)

    return (r, g, b)


def show_bar_leds(potentiometer_score):
    """
    Light up the appropriate number of bar LEDs based on potentiometer score.
    """
    for led in bar_leds[:potentiometer_score]:
        led.value = True
    for led in bar_leds[potentiometer_score:]:
        led.value = False


def run():
    # A {0-255} score based on encoder reading.
    # Offset is counted periodically to update score.
    encoder_score = 200
    encoder_offset = encoder.position

    # Set the NeoPixel appropriately.
    pixels[0] = get_rgb(encoder_score)
    pixels.show()

    # A {1-10} score based on potentiometer reading.
    potentiometer_score = 0

    # Simple graphic for Bar LEDs. The POT value is fixed, so
    # don't bother setting the value as this will get picked up
    # and applied in first loop.
    bars_led_demo()

    while True:
        # Potentiometer controls Bar LEDs.
        potentiometer_score = get_potentiometer_score()
        show_bar_leds(potentiometer_score)

        # Encoder controls NeoPixel.
        offset = encoder.position - encoder_offset
        if offset:
            # Apply offset to encoder.
            encoder_score = (encoder_score + (offset * 6)) % 256
            encoder_offset = encoder.position

            # Update the NeoPixel.
            pixels[0] = get_rgb(encoder_score)
            pixels.show()

        # Check button for taking action.
        if not button.value:
            button_pressed(potentiometer_score, encoder_score)

        time.sleep(0.1)


run()
