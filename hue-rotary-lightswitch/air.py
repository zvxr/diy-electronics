# adafruit_requests usage with an esp32spi_socket
import board
import busio
from digitalio import DigitalInOut

from adafruit_esp32spi import (
    adafruit_esp32spi,
    adafruit_esp32spi_wifimanager,
)

import neopixel


from secrets import SSID, PASSWORD


# If you are using a board with pre-defined ESP32 Pins:
esp32_cs = DigitalInOut(board.ESP_CS)
esp32_ready = DigitalInOut(board.ESP_BUSY)
esp32_reset = DigitalInOut(board.ESP_RESET)

# If you have an externally connected ESP32:
# esp32_cs = DigitalInOut(board.D9)
# esp32_ready = DigitalInOut(board.D10)
# esp32_reset = DigitalInOut(board.D5)

spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
esp = adafruit_esp32spi.ESP_SPIcontrol(spi, esp32_cs, esp32_ready, esp32_reset)
status_light = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=0.2)
wifi = None


def connect_wifi(force=False):
    global wifi
    if not wifi or force:
        secrets = {"ssid": SSID, "password": PASSWORD}
        wifi = adafruit_esp32spi_wifimanager.ESPSPI_WiFiManager(esp, secrets, status_light)


connect_wifi()
