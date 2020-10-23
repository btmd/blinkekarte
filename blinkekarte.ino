#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

//#define CLK_PIN 14
#define LED_PIN 1
#define COLOR_ORDER GRB
// you need to define the right LED strip here or you run into the weirdest things
#define LED_TYPE    WS2812 //APA102//SK6812 - WS2812
#define NUM_LEDS    12

int BRIGHTNESS = 100;   //up to 255
CRGB leds[NUM_LEDS];

// Set to 20 timers manually in Library!!
// Default is 16
BlynkTimer timer;

boolean aLightWasToggled = false;

struct Lights {
  std::string        name;
  unsigned int       ledstripnumber;
  CRGB               color;
  std::vector <int>  indentification;
  unsigned long      current_index;
  unsigned long      startupDelay;// so hat not all lights start blinking at the same time
};


static Lights LightsArray[] = {

// Blinking Lights for Antigua (BA2046)
//  //  Name                NrInRow  Color        identif.    curr_index,  startupDelay (must not be 0)
  {"Iso.G",     0,  CRGB::Green,  {1000, 1000}, 0,  random16(10, 4000)},
  {"FL.R.4s",   1,  CRGB::Red,    {800, 3200}, 0,  100},
  {"FL.R.2s",   2,  CRGB::Red,    {800, 1200}, 0,  100},  
  {"Q.G",       3,  CRGB::Green,  {600, 1000}, 0,  random16(10, 4000)},
  {"F.G",       4,  CRGB::Green,  {5000}, 0,  random16(10, 4000)},
  {"Mo(A).5s",  5,  CRGB::Blue,   {500, 500, 1500, 2500}, 0,  random16(10, 4000)}, // Morsetonne
  {"Fl.10s",    6,  CRGB::White,  {1000, 9000}, 0,  random16(10, 4000)},
  {"Fl.R.7s",   7,  CRGB::Red,    {1000, 6000}, 0,  random16(10, 4000)},
  {"Fl.G.7s",   8,  CRGB::Green,  {1000, 6000}, 0,  random16(10, 4000)},
  {"Iso.R",     9,  CRGB::Red,    {3000, 3000}, 0,  100},
  {"Iso.R",     10, CRGB::Red,    {3000, 3000}, 0,  100},
  {"Q",         11, CRGB::White,  {700, 1000}, 0,  random16(10, 4000)}
// Blinking lights for Helgoland (D1311)
//  {"ODAS",                  0,  CRGB::Yellow, {1000, 1500, 1000, 1500, 1000, 1500, 1000, 1500, 1000, 9000}, 0,  random16(10, 4000)},
//  {"Helgoland-O",           1,  CRGB::White,  {1000, 200,  1000, 200, 1000, 6600}, 0, random16(10, 4000)},
//  {"Duene-S",               2,  CRGB::White,  {900,  200,  900,  200, 900,  200,  900,  200,  900,  200,  900,  200, 2000, 6400}, 0, random16(10, 4000)},
//  {"Green_1",               3,  CRGB::Green,  {1500, 500,  1500, 500}, 0, random16(10, 8000)},
//  {"Red_6",                 4,  CRGB::Red,    {1500, 500,  1500, 5500}, 0, random16(10, 8000)},
//  {"Suedhafen_Port",        5,  CRGB::Red,    {5000, 500, 500, 500, 5500}, 0, random16(10, 8000)},
//  {"Dock_1_2",              6,  CRGB::White,  {3000, 500, 3000, 500, 3000}, 0, random16(10, 4000)},
//  {"Suedhafen_Starboard",   7,  CRGB::Green,  {5500, 500}, 0, random16(10, 4000)},
//  {"Hohe Duene Sued White", 8,  CRGB::White,  {2000, 2000}, 0, 1000},
//  {"Hohe Duene Sued Green", 9,  CRGB::Green,  {2000, 2000}, 0, 1000},
//  {"Hohe Duene Sued Red",   10, CRGB::Red,    {2000, 2000}, 0, 1000},
//  {"Helgoland Nordhafen",   11, CRGB::Red,    {5500, 500}, 0, random16(10, 4000)},
//  {"Leuchtturm Helgoland",  12, CRGB::White,  {1000, 4000}, 0, random16(10, 4000)},
//  {"Hafen Hohe Duene",      13, CRGB::Red,    {4000}, 0, random16(10, 4000)},
//  {"Hohe Duene Nord",       14, CRGB::Green,  {2000, 2000}, 0, 1000},
//  {"Hohe Duene Nord",       15, CRGB::Red,    {2000, 2000}, 0, 1000},
//  {"Hohe Duene Nord",       16, CRGB::White,  {2000, 2000}, 0, 1000},
//  {"N6",                    17, CRGB::White,  {900,  200,  900,  200, 900,  200,  900,  200,  900,  200,  900,  200, 2000, 6400}, 0, random16(10, 4000)},
//  {"N3",                    18, CRGB::White,  {900, 100}, 0, random16(10, 4000)},
//  {"Duene-N",               19, CRGB::White,  {900, 100}, 0, random16(10, 8000)}

};

void Blink(void* args); // stupid C shit, warum kann der compiler nicht nen bisschen weiter unten gucken, maaan


void startAllLightsWithOffset(Lights array[]) {

  int length = 12;//sizeof(array);

  Serial.print("LightsArray Length: "); Serial.println(length);

  for (int i = 0; i < length; ++i) {
    timer.setTimeout(array[i].startupDelay, Blink, (void*)&array[i]);
  }
}


void setup() {
  Serial.begin(115200);
  delay(3000);

  Serial.println("setup");
  Serial.print("MAX_TIMERS: "); Serial.println(timer.MAX_TIMERS);

  random16_add_entropy( random(255) );

  //FastLED.addLeds<LED_TYPE, LED_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  startAllLightsWithOffset(LightsArray);
}

void loop() {
  while (true) {
    Blynk.run();
    timer.run();
    FastLED.show();
    yield();
  }
}


void Blink(void* args)
{
  struct Lights* thisLight = (struct Lights*)args;
  Serial.print("Tonne Strip Nr.: "); Serial.println(thisLight->ledstripnumber);
  if (thisLight->current_index % 2 == 0) {                            //gerade = AN
    leds[thisLight->ledstripnumber] = thisLight->color;
    Serial.print(" an "); Serial.println(thisLight->indentification[thisLight->current_index]);
  } else {                                                            //ungerade = AUS
    leds[thisLight->ledstripnumber] = CRGB::Black;
    Serial.print(" aus "); Serial.println(thisLight->indentification[thisLight->current_index]);
  }
  timer.setTimeout(thisLight->indentification[thisLight->current_index], Blink, (void *)thisLight);
  thisLight->current_index++;

  if (thisLight->current_index >= thisLight->indentification.size()) { //overflow
    thisLight->current_index = 0;
  }

}
