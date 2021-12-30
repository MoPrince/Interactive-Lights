
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <EEPROM.h>
#include <WiFi.h>
#include<FastLED.h>
#include<MegunoLink.h>
#include<Filter.h>
#include <arduinoFFT.h>
#include <BlynkSimpleEsp32.h>



#define WLAN_SSID       ""             // Your SSID
#define WLAN_PASS       ""        // Your password

/***********************
** Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  ""
#define AIO_KEY       ""

/************ Global Var *************************/
#define ADC_PIN 36 //Mic Pin

#define N_PIXELS  150 //Number of pixels

#define LED_PIN   13 // labeled "D6" on the board
#define LED_TYPE  WS2812B // 
#define BRIGHTNESS  255     // a little dim for recording purposes
#define COLOR_ORDER GRB


#define NUM_BANDS  8
#define READ_DELAY 50
#define USE_RANDOM_DATA false

#define SAMPLES 512              //Must be a power of 2
#define SAMPLING_FREQUENCY 20000 //Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT.


#define Auth "ZjEB-i6IVr7Hx8EqMgw7hl4p-KPBik24" //Blynk Authentication

// Use ADC1 so that WiFi stuff doesnt interfere with ADC measurements


int amplitude = 200;
unsigned int sampling_period_us;
unsigned long microseconds;
byte peak[] = {0, 0, 0, 0, 0, 0, 0, 0, 0 , 0};
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime, oldTime;
float zeroed;
int w = 0;
String next;
int test = 0;
int HALF_PIXEL = N_PIXELS / 2;

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/


// Setup a feed called 'onoff' for subscribing to changes.

/****************************** FeedName ************************************/
Adafruit_MQTT_Subscribe Mode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Mode");

arduinoFFT FFT = arduinoFFT();

//Control Using BLYNK application to control specific Color
BLYNK_WRITE(V2)
{
  int R = param[0].asInt();
  int G = param[1].asInt();
  int B = param[2].asInt();
  Serial.println(R);
  Serial.println(G);
  Serial.println(B);
  for (int i = 0; i < 150; i++) {

    setPixel(i, R, G, B);

    showStrip();
  }
}



// declare the LED array
CRGB leds[N_PIXELS];

void displayBand(int band, int dsize)
{
  int dmax = amplitude;
  if (dsize > dmax)
    dsize = dmax;
  if (dsize > peak[band])
  {
    peak[band] = dsize;
  }
}

void ConnectionCheck(void * parameter) {
  for (;;) {
    MQTT_connect();
    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(2))) {

      if (subscription == &Mode) {
        Serial.print("Mode > ");
        Serial.print(F("Got: "));
        Serial.println((char *)Mode.lastread);
        byte Mode1_state = atoi((char *)Mode.lastread);
        //analogWrite(0, Light2_State);
        //EEPROM.write(1, Mode_state);
        EEPROM.commit();
        next = ((char *)Mode.lastread);
        Serial.println("Mode Change: " + next);
        //Serial.println(EEPROM.read(1));
        //Serial.println(Light2_State);
      }
    }
  }
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  adcAttachPin(ADC_PIN);
  analogSetClockDiv(255); // 1338mS
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, N_PIXELS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  //  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Blynk.begin(Auth, WLAN_SSID, WLAN_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqtt.subscribe(&Mode);
  xTaskCreatePinnedToCore(
    ConnectionCheck,          // Function that should be called
    "Connection Check",     // Name of the task (for debugging)
    10000,              // Stack size (bytes)
    NULL,               // Parameter to pass
    1,                  // Task priority
    NULL,               // Task handle
    1                   // Core you want to run the task on (0 or 1)
  );

}

void loop() {

  // put your main code here, to run repeatedly:

  if (next == "music" && test != 20) test = 20;

  if ((next == "one") or (next == "1") or (next == "One"))  test = 1;
  if ((next == "two") or (next == "2") or (next == "Two"))  test = 2;
  if ((next == "three") or (next == "3") or (next == "Three"))  test = 3;
  if ((next == "Zero") or (next == "0") or (next == "Zero")) test = 0;
  if ((next == "four") or (next == "4") or (next == "Four"))  test = 4;
  if ((next == "five") or (next == "5") or (next == "Five"))  test = 5;
  if ((next == "six") or (next == "6") or (next == "Six"))  test = 6;
  if ((next == "seven") or (next == "7") or (next == "Seven")) test = 7;
  if ((next == "eight") or (next == "8") or (next == "Eight"))  test = 8;
  if ((next == "nine") or (next == "9") or (next == "Nine"))  test = 9;
  if ((next == "ten") or (next == "10") or (next == "Ten"))  test = 10;
  if ((next == "eleven") or (next == "11") or (next == "Eleven")) test = 11;
  if ((next == "twelve") or (next == "12") or (next == "Twelve"))  test = 12;
  if ((next == "thirteen") or (next == "13") or (next == "Thirteen")) test = 13;
  if ((next == "fourteen") or (next == "14") or (next == "Fourteen"))  test = 14;
  if ((next == "fifteen") or (next == "15") or (next == "Fifteen"))  test = 15;
  if ((next == "sixteen") or (next == "16") or (next == "Sixteen"))  test = 16;
  if ((next == "seventeen") or (next == "17") or (next == "Seventeen")) test = 17;
  if ((next == "eighteen") or (next == "18") or (next == "Eighteen"))  test = 18;
  if ((next == "nineteen") or (next == "19") or (next == "Nineteen"))  test = 19;



  switch ( test)
  {

    case 0  : {
        turnoff();
        break;
      }

    case 1  : {
        // FadeInOut - Color (red, green. blue)
        Serial.println("One");
        FadeInOut(0xff, 0x00, 0x00); // red
        FadeInOut(0xff, 0xff, 0xff); // white
        FadeInOut(0x00, 0x00, 0xff); // blue

        break;
      }

    case 2  : {
        // Strobe - Color (red, green, blue), number of flashes, flash speed, end pause
        Serial.println("Two");
        Strobe(0xff, 0xff, 0xff, 10, 50, 1000);
        break;
      }
    case 3 : {
        // meteorRain - Color (red, green, blue), meteor size, trail decay, random trail decay (true/false), speed delay
        meteorRain(0xff, 0xff, 0xff, 10, 64, true, 30);
        break;
      }

    case 4  : {
        // CylonBounce - Color (red, green, blue), eye size, speed delay, end pause
        CylonBounce(0xff, 0x00, 0x00, 4, 10, 50);
        break;
      }

    case 5  : {
        // NewKITT - Color (red, green, blue), eye size, speed delay, end pause
        NewKITT(0xff, 0x00, 0x00, 8, 10, 50);
        break;
      }

    case 6  : {
        // Twinkle - Color (red, green, blue), count, speed delay, only one twinkle (true/false)
        Twinkle(0xff, 0x00, 0x00, 10, 100, false);
        break;
      }

    case 7  : {
        // TwinkleRandom - twinkle count, speed delay, only one (true/false)
        TwinkleRandom(20, 100, false);
        break;
      }

    case 8  : {
        // Sparkle - Color (red, green, blue), speed delay
        Sparkle(0xff, 0xff, 0xff, 0);
        break;
      }

    case 9  : {
        // SnowSparkle - Color (red, green, blue), sparkle delay, speed delay
        SnowSparkle(0x10, 0x10, 0x10, 20, random(100, 1000));
        break;
      }

    case 10 : {
        // Running Lights - Color (red, green, blue), wave dealy
        RunningLights(0xff, 0x00, 0x00, 50); // red
        RunningLights(0xff, 0xff, 0xff, 50); // white
        RunningLights(0x00, 0x00, 0xff, 50); // blue
        break;
      }

    case 11 : {
        // colorWipe - Color (red, green, blue), speed delay
        colorWipe(0x00, 0xff, 0x00, 50);
        colorWipe(0x00, 0x00, 0x00, 50);
        break;
      }

    case 12 : {
        // rainbowCycle - speed delay
        rainbowCycle(20);
        break;
      }

    case 13 : {
        // theatherChase - Color (red, green, blue), speed delay
        theaterChase(0xff, 0, 0, 50);
        break;
      }

    case 14 : {
        // theaterChaseRainbow - Speed delay
        theaterChaseRainbow(50);
        break;
      }

    case 15 : {
        // Fire - Cooling rate, Sparking rate, speed delay
        Fire(55, 120, 15);
        break;
      }


    // simple bouncingBalls not included, since BouncingColoredBalls can perform this as well as shown below
    // BouncingColoredBalls - Number of balls, color (red, green, blue) array, continuous
    // CAUTION: If set to continuous then this effect will never stop!!!

    case 16 : {
        // mimic BouncingBalls
        byte onecolor[1][3] = { {0xff, 0x00, 0x00} };
        BouncingColoredBalls(1, onecolor, false);
        break;
      }

    case 17 : {
        // multiple colored balls
        byte colors[3][3] = { {0xff, 0x00, 0x00},
          {0xff, 0xff, 0xff},
          {0x00, 0x00, 0xff}
        };
        BouncingColoredBalls(3, colors, false);
        break;
      }
    case  18 : {
        // HalloweenEyes - Color (red, green, blue), Size of eye, space between eyes, fade (true/false), steps, fade delay, end pause
        HalloweenEyes(0xff, 0x00, 0x00,
                      1, 4,
                      true, random(5, 50), random(50, 150),
                      random(1000, 10000));
        HalloweenEyes(0xff, 0x00, 0x00,
                      1, 4,
                      true, random(5, 50), random(50, 150),
                      random(1000, 10000));
        break;
      }
    case 19  : {
        Blynk.run();
        break;
      }

  }
  if ( test == 20) {
    int max_ = 0;
    int avg = 0;
    uint8_t* returned = dynamiccolors();
    //  zeroed = 0;
    for (int i = 0; i < 8; i++) {
      zeroed += map(returned[i], 0, 200, 0, 18.75);
      if (returned[i] > max_) max_ = returned[i];
      avg += returned[i];
    }

    zeroed *= 2;

    avg = map(avg / 7, 0, 200, 0, 255);

    int x = map(returned[random(0, 2)], 0, 200, 0, 255);
    int y = map(returned[random(2, 4)], 0, 200, 0, 255);
    int z = map(returned[random(4, 7)], 0, 200, 0, 255);




    for (uint8_t i = 0; i <= HALF_PIXEL; i++) {
      // turn off LEDs above the current level
      if (i >= int(zeroed)) {
        leds[i + HALF_PIXEL] = CRGB(0, 0, 0);
        leds[HALF_PIXEL - i] = CRGB(0, 0, 0);
        //        leds[i] = CRGB(0,0,0);
      }
      // otherwise, turn them on!
      else {

        leds[HALF_PIXEL + i] = Wheel( map( i, 1, zeroed, 0, HALF_PIXEL ), random(avg, max_));
        leds[HALF_PIXEL - i] = Wheel( map(i, 1, zeroed, 0, HALF_PIXEL ), random(avg, max_) );
      }
    }

    FastLED.show();
    zeroed = 0;
  }
}

void MQTT_connect() {

  int8_t ret;

  // Stop if already connected.

  if (mqtt.connected()) {

    return;

  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected

    Serial.println(mqtt.connectErrorString(ret));

    Serial.println("Retrying MQTT connection in 5 seconds...");

    mqtt.disconnect();

    delay(5000);  // wait 5 seconds

    retries--;

    if (retries == 0) {

      // basically die and wait for WDT to reset me

      while (1);

    }

  }

  Serial.println("MQTT Connected!");

}


void ModeChange() {

}


CRGB Wheel(byte WheelPos, int Rand1) {
  // return a color value based on an input value between 0 and 255
  if (WheelPos < 85)
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else {
    WheelPos -= 170;
    return CRGB(255, WheelPos * 3, Rand1 - WheelPos * 3);
  }
}


uint8_t* dynamiccolors() {/*SAMPLING*/
  for (int i = 0; i < SAMPLES; i++)
  {
    newTime = micros() - oldTime;
    oldTime = newTime;
    vReal[i] = analogRead(ADC_PIN); // A conversion takes about 1mS on an ESP8266
    vImag[i] = 0;
    while (micros() < (newTime + sampling_period_us))
    {
      delay(0);
    }
  }
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  for (int i = 2; i < (SAMPLES / 2); i++)
  { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
    if (vReal[i] > 2000)
    { // Add a crude noise filter, 10 x amplitude or more
      //      if (i <= 2)
      //        displayBand(0, (int)vReal[i] / amplitude); // 125Hz
      if (i > 3 && i <= 5)
        displayBand(0, (int)vReal[i] / amplitude); // 250Hz
      if (i > 5 && i <= 7)
        displayBand(1, (int)vReal[i] / amplitude); // 500Hz
      if (i > 7 && i <= 15)
        displayBand(2, (int)vReal[i] / amplitude); // 1000Hz
      if (i > 15 && i <= 30)
        displayBand(3, (int)vReal[i] / amplitude); // 2000Hz
      if (i > 30 && i <= 53)
        displayBand(4, (int)vReal[i] / amplitude); // 4000Hz
      if (i > 53 && i <= 200)
        displayBand(5, (int)vReal[i] / amplitude); // 8000Hz
      if (i > 200)
        displayBand(6, (int)vReal[i] / amplitude); // 16000Hz
    }
  }

  if (millis() % 4 == 0)
  {
    for (byte band = 1; band < NUM_BANDS; band++)
    {
      if (peak[band] > 0)
        peak[band] /= 2;
    }
  } // Decay the peak
  return peak;
}

// *************************
// ** LEDEffect Functions **
// *************************

void turnoff() {
  setAll(0, 0, 0);
  showStrip();
}

void FadeInOut(byte red, byte green, byte blue) {
  float r, g, b;

  for (int k = 0; k < 256; k = k + 1) {
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }

  for (int k = 255; k >= 0; k = k - 2) {
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause) {
  for (int j = 0; j < StrobeCount; j++) {
    setAll(red, green, blue);
    showStrip();
    delay(FlashDelay);
    setAll(0, 0, 0);
    showStrip();
    delay(FlashDelay);
  }

  delay(EndPause);
}

void HalloweenEyes(byte red, byte green, byte blue,
                   int EyeWidth, int EyeSpace,
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause) {
  randomSeed(analogRead(0));

  int i;
  int StartPoint  = random( 0, N_PIXELS - (2 * EyeWidth) - EyeSpace );
  int Start2ndEye = StartPoint + EyeWidth + EyeSpace;

  for (i = 0; i < EyeWidth; i++) {
    setPixel(StartPoint + i, red, green, blue);
    setPixel(Start2ndEye + i, red, green, blue);
  }

  showStrip();

  if (Fade == true) {
    float r, g, b;

    for (int j = Steps; j >= 0; j--) {
      r = j * (red / Steps);
      g = j * (green / Steps);
      b = j * (blue / Steps);

      for (i = 0; i < EyeWidth; i++) {
        setPixel(StartPoint + i, r, g, b);
        setPixel(Start2ndEye + i, r, g, b);
      }

      showStrip();
      delay(FadeDelay);
    }
  }

  setAll(0, 0, 0); // Set all black

  delay(EndPause);
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {

  for (int i = 0; i < N_PIXELS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for (int i = N_PIXELS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);
}

void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

// used by NewKITT
void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = ((N_PIXELS - EyeSize) / 2); i >= 0; i--) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(N_PIXELS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(N_PIXELS - i - j, red, green, blue);
    }
    setPixel(N_PIXELS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// used by NewKITT
void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i <= ((N_PIXELS - EyeSize) / 2); i++) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(N_PIXELS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(N_PIXELS - i - j, red, green, blue);
    }
    setPixel(N_PIXELS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// used by NewKITT
void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i < N_PIXELS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// used by NewKITT
void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = N_PIXELS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(random(N_PIXELS), red, green, blue);
    showStrip();
    delay(SpeedDelay);
    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(SpeedDelay);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(random(N_PIXELS), random(0, 255), random(0, 255), random(0, 255));
    showStrip();
    delay(SpeedDelay);
    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(SpeedDelay);
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(N_PIXELS);
  setPixel(Pixel, red, green, blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel, 0, 0, 0);
}

void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(red, green, blue);

  int Pixel = random(N_PIXELS);
  setPixel(Pixel, 0xff, 0xff, 0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel, red, green, blue);
  showStrip();
  delay(SpeedDelay);
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position = 0;

  for (int i = 0; i < N_PIXELS * 2; i++)
  {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < N_PIXELS; i++) {
      // sine wave, 3 offset waves make a rainbow!
      //float level = sin(i+Position) * 127 + 128;
      //setPixel(i,level,0,0);
      //float level = sin(i+Position) * 127 + 128;
      setPixel(i, ((sin(i + Position) * 127 + 128) / 255)*red,
               ((sin(i + Position) * 127 + 128) / 255)*green,
               ((sin(i + Position) * 127 + 128) / 255)*blue);
    }

    showStrip();
    delay(WaveDelay);
  }
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for (uint16_t i = 0; i < N_PIXELS; i++) {
    setPixel(i, red, green, blue);
    showStrip();
    delay(SpeedDelay);
  }
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < N_PIXELS; i++) {
      c = Wheel(((i * 256 / N_PIXELS) + j) & 255);
      setPixel(i, *c, *(c + 1), *(c + 2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

// used by rainbowCycle and theaterChaseRainbow
byte * Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}

void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < N_PIXELS; i = i + 3) {
        setPixel(i + q, red, green, blue);  //turn every third pixel on
      }
      showStrip();

      delay(SpeedDelay);

      for (int i = 0; i < N_PIXELS; i = i + 3) {
        setPixel(i + q, 0, 0, 0);    //turn every third pixel off
      }
    }
  }
}

void theaterChaseRainbow(int SpeedDelay) {
  byte *c;

  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < N_PIXELS; i = i + 3) {
        c = Wheel( (i + j) % 255);
        setPixel(i + q, *c, *(c + 1), *(c + 2)); //turn every third pixel on
      }
      showStrip();

      delay(SpeedDelay);

      for (int i = 0; i < N_PIXELS; i = i + 3) {
        setPixel(i + q, 0, 0, 0);    //turn every third pixel off
      }
    }
  }
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[N_PIXELS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < N_PIXELS; i++) {
    cooldown = random(0, ((Cooling * 10) / N_PIXELS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = N_PIXELS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < N_PIXELS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void BouncingColoredBalls(int BallCount, byte colors[][3], boolean continuous) {
  float Gravity = -9.81;
  int StartHeight = 1;

  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
  boolean ballBouncing[BallCount];
  boolean ballsStillBouncing = true;

  for (int i = 0 ; i < BallCount ; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
    ballBouncing[i] = true;
  }

  while (ballsStillBouncing) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i] / 1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if ( Height[i] < 0 ) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if ( ImpactVelocity[i] < 0.01 ) {
          if (continuous) {
            ImpactVelocity[i] = ImpactVelocityStart;
          } else {
            ballBouncing[i] = false;
          }
        }
      }
      Position[i] = round( Height[i] * (N_PIXELS - 1) / StartHeight);
    }

    ballsStillBouncing = false; // assume no balls bouncing
    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i], colors[i][0], colors[i][1], colors[i][2]);
      if ( ballBouncing[i] ) {
        ballsStillBouncing = true;
      }
    }

    showStrip();
    setAll(0, 0, 0);
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  setAll(0, 0, 0);

  for (int i = 0; i < N_PIXELS + N_PIXELS; i++) {


    // fade brightness all LEDs one step
    for (int j = 0; j < N_PIXELS; j++) {
      if ( (!meteorRandomDecay) || (random(10) > 5) ) {
        fadeToBlack(j, meteorTrailDecay );
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ( ( i - j < N_PIXELS) && (i - j >= 0) ) {
        setPixel(i - j, red, green, blue);
      }
    }

    showStrip();
    delay(SpeedDelay);
  }
}

// used by meteorrain
void fadeToBlack(int ledNo, byte fadeValue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 256);

  strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[ledNo].fadeToBlackBy( fadeValue );
#endif
}

// *** REPLACE TO HERE ***



// ***************************************
// ** FastLed/NeoPixel Common Functions **
// ***************************************

// Apply LED color changes
void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < N_PIXELS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
