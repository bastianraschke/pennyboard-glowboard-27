#include <Adafruit_NeoPixel.h>

enum State
{
    RAINBOW,
    TWOCOLOR   
};

class NonDelayNeoPixelAnimations
{
public:
    void (*onCompleteCallback)();
    
    NonDelayNeoPixelAnimations(const uint8_t neopixelPin, const uint8_t pixelCount, const uint8_t brightness, void (*onCompleteCallback)())
    {
        this->neopixelPin = neopixelPin;
        this->pixelCount = pixelCount;
        this->brightness = brightness;
        this->onCompleteCallback = onCompleteCallback;
    }

    void setup()
    {
        Serial.println("Setup Neopixels...");

        neopixelStrip = Adafruit_NeoPixel(pixelCount, neopixelPin, NEO_GRB + NEO_KHZ800);
        neopixelStrip.begin();
        neopixelStrip.setBrightness(brightness);
    }

    void update()
    {
        if ((millis() - lastUpdate) > updateInterval)
        {
            lastUpdate = millis();
            onUpdate();

            currentIndex++;

            if (currentIndex >= totalSteps)
            {
                currentIndex = 0;

                if (onCompleteCallback != NULL)
                {
                    onCompleteCallback();
                }
            }
        }
    }

    void setState(const State currentState)
    {
        state = currentState;
    }

private:
    Adafruit_NeoPixel neopixelStrip;
    uint8_t neopixelPin;
    uint8_t pixelCount;
    uint8_t brightness;

    unsigned long updateInterval = 10;
    unsigned long lastUpdate = 0;
    uint16_t currentIndex = 0;
    uint16_t totalSteps = 255;

    State state;

    void onUpdate()
    {
        switch(state)
        {
            case RAINBOW:
                rainbowCycle();
                break;

            case TWOCOLOR:
                twoColors(0xff0000, 0x00ff00);
                break;
        }
    }

    void rainbowCycle()
    {
        const uint16_t neopixelCount = neopixelStrip.numPixels();

        for (int i = 0; i < neopixelCount; i++)
        {
            neopixelStrip.setPixelColor(i, getCurrentWheelColor(((i * 256 / neopixelCount) + currentIndex) & 255));
        }

        neopixelStrip.show();
    }

    void twoColors(const uint32_t color1, const uint32_t color2)
    {
        const uint16_t neopixelCount = neopixelStrip.numPixels();

        // Split first color to R, B, G parts
        const uint8_t color1_r = (color1 >> 16) & 0xFF;
        const uint8_t color1_g = (color1 >>  8) & 0xFF;
        const uint8_t color1_b = (color1 >>  0) & 0xFF;

        // Split second color to R, B, G parts
        const uint8_t color2_r = (color2 >> 16) & 0xFF;
        const uint8_t color2_g = (color2 >>  8) & 0xFF;
        const uint8_t color2_b = (color2 >>  0) & 0xFF;

        for (int i = 0; i < neopixelCount; i++)
        {
            float percentage = mapPixelCountToPercentage(i, neopixelCount);

            // Calculate the color of this iteration
            // see: https://stackoverflow.com/questions/27532/ and https://stackoverflow.com/questions/22218140/
            const uint8_t r = (color1_r * percentage) + (color2_r * (1 - percentage));
            const uint8_t g = (color1_g * percentage) + (color2_g * (1 - percentage));
            const uint8_t b = (color1_b * percentage) + (color2_b * (1 - percentage));

            const uint32_t currentColor = neopixelStrip.Color(r, g, b);

            neopixelStrip.setPixelColor(i, currentColor);
        }

        neopixelStrip.show();
    }

    float mapPixelCountToPercentage(uint16_t i, float count)
    {
        const float currentPixel = (float) i;
        const float neopixelCount = (float) count;

        const float min = 0.0f;
        const float max = 1.0f;

        return (currentPixel - 0.0f) * (max - min) / (neopixelCount - 0.0f) + min;
    }

    uint32_t getCurrentWheelColor(uint8_t wheelPosition)
    {
        uint32_t currentWheelColor;
        wheelPosition = 255 - wheelPosition;

        if (wheelPosition < 85)
        {
            currentWheelColor = neopixelStrip.Color(255 - wheelPosition * 3, 0, wheelPosition * 3);
        }
        else if (wheelPosition < 170)
        {
            wheelPosition -= 85;
            currentWheelColor = neopixelStrip.Color(0, wheelPosition * 3, 255 - wheelPosition * 3);
        }
        else {
            wheelPosition -= 170;
            currentWheelColor = neopixelStrip.Color(wheelPosition * 3, 255 - wheelPosition * 3, 0); 
        }

        return currentWheelColor;
    }
};
