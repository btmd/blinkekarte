# blinkekarte
blinkekarte

# Hardware
Controller: ESP8266
LED strips: WS2812B or WS2812

# Project description
This project illuminates nautical charts. Bouys blink in there designed pattern. This is the digital part for it.
You still have to buy a nice chart, print it on something solid, drill holes, cut LED strip, glue LEDs, wire all of them together and to the ESP.

# blinking patterns
for now you have to hardcode the different patterns. If you hook it to Blynk you can set parameters like brightness from your mobile phone!
One ESP can only adress so many LEDs, we had to modify the timer lib to have more timers available. We need timers == amount of LEDs.

For charts like "Port of Hamburg" we run into a hard obstacle. We might not have enough timers even if we tweak the timer library...


# How To
load into arduino ide, load onto your ESP, wire the strip check the patterns



# TODO
* Support a gazilion LEDs (see Port of Hamburg issue #1)

