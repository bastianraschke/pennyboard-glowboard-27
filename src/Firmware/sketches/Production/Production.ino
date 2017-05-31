#include <NonDelayNeoPixelAnimations.h>
#include "Wire.h"

#define PIN_NEOPIXELS 10
#define NEOPIXELS_COUNT 25
#define NEOPIXELS_BRIGHTNESS 255

NonDelayNeoPixelAnimations np = NonDelayNeoPixelAnimations(PIN_NEOPIXELS, NEOPIXELS_COUNT, NEOPIXELS_BRIGHTNESS, &onComplete);

void onComplete()
{
    // j++;

    // if (j % 2 == 0) {
    //     np.setState(RAINBOW);
    // } else {
    //     np.setState(TWOCOLOR);
    // }
    // Serial.println("onComplete()");
}

void setup()
{
    Serial.begin(115200);
    Serial.println("setup()");
}

void loop()
{
    np.update();
}
