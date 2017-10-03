#include <Adafruit_NeoPixel.h>

enum State
{
    OFF,
    RAINBOWCYCLE,
    RAINBOW,
    TWOCOLOR,
    LASERSCANNER
};

enum Direction
{
    LEFT,
    RIGHT
};

class NonDelayNeoPixelAnimations
{
public:
    void (*onCompleteCallback)();

    NonDelayNeoPixelAnimations(const uint8_t neopixelPin, const uint8_t neopixelCount, const uint8_t brightness, void (*onCompleteCallback)())
    {
        this->neopixelPin = neopixelPin;
        this->neopixelCount = neopixelCount;
        this->brightness = brightness;
        this->onCompleteCallback = onCompleteCallback;
    }

    void setup()
    {
        neopixelStrip = Adafruit_NeoPixel(neopixelCount, neopixelPin, NEO_GRB + NEO_KHZ800);
        neopixelStrip.begin();
    }

    void setBrightnessLow()
    {
        neopixelStrip.setBrightness(brightness / 3);
    }

    void setBrightnessHigh()
    {
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
                direction = (direction == LEFT) ? RIGHT : LEFT;

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

        switch(state)
        {
            case RAINBOWCYCLE:
                updateInterval = 12;
                break;

            case RAINBOW:
                updateInterval = 25;
                break;

            case LASERSCANNER:
                updateInterval = 2;
                break;
        }
    }

private:
    Adafruit_NeoPixel neopixelStrip;
    uint8_t neopixelPin;
    uint8_t neopixelCount;
    uint8_t brightness;

    unsigned long updateInterval = 10;
    unsigned long lastUpdate = 0;
    uint16_t currentIndex = 0;
    uint16_t totalSteps = 256;

    State state;
    Direction direction = LEFT;

    void onUpdate()
    {
        switch (state)
        {
            case OFF:
                allOff();
                break;

            case RAINBOWCYCLE:
                rainbowCycle();
                break;

            case RAINBOW:
                rainbow();
                break;

            case TWOCOLOR:
                twoColors(0x00AEFF, 0xA92CCE);
                break;

            case LASERSCANNER:
                laserScanner(0x00AEFF);
                break;
        }
    }

    void allOff()
    {
        for (int i = 0; i < neopixelCount; i++)
        {
            neopixelStrip.setPixelColor(i, 0x000000);
        }

        neopixelStrip.show();
    }

    void rainbowCycle()
    {
        for (int i = 0; i < neopixelCount; i++)
        {
            neopixelStrip.setPixelColor(i, getCurrentWheelColor(((i * 256 / neopixelCount) + currentIndex) & 255));
        }

        neopixelStrip.show();
    }

    void rainbow()
    {
        for (int i = 0; i < neopixelCount; i++)
        {
            neopixelStrip.setPixelColor(i, getCurrentWheelColor((i + currentIndex) & 255));
        }

        neopixelStrip.show();
    }

    void twoColors(const uint32_t color1, const uint32_t color2)
    {
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

    void laserScanner(const uint32_t primaryColor)
    {
        const uint8_t scannerPrimaryWidth = 2;

        const uint16_t minIndex = 0;
        const uint16_t maxIndex = neopixelCount - 1;

        int lLimit;
        int rLimit;

        if (direction == LEFT)
        {
            lLimit = minIndex + scannerPrimaryWidth;
            rLimit = maxIndex - scannerPrimaryWidth;
        }
        else
        {
            lLimit = maxIndex - scannerPrimaryWidth;
            rLimit = minIndex + scannerPrimaryWidth;
        }

        const uint16_t currentLimitedIndex = round(map(currentIndex, 0, 255, lLimit, rLimit));

        const uint32_t secondaryColorR = (primaryColor >> 16 & 0xFF);
        const uint32_t secondaryColorG = (primaryColor >>  8 & 0xFF);
        const uint32_t secondaryColorB = (primaryColor >>  0 & 0xFF);

        // Use the half of primary color as secondary color
        const uint32_t secondaryColor = (
            (secondaryColorR > 0 ? (secondaryColorR / 2) : 0x00) << 16 |
            (secondaryColorG > 0 ? (secondaryColorG / 2) : 0x00) <<  8 |
            (secondaryColorB > 0 ? (secondaryColorB / 2) : 0x00) <<  0
        );

        for (int i = 0; i < neopixelCount; i++)
        {
            const int l = currentLimitedIndex - scannerPrimaryWidth;
            const int r = currentLimitedIndex + scannerPrimaryWidth;

            if (i == l)
            {
                neopixelStrip.setPixelColor(i, secondaryColor); 
            }
            else if (i > l && i < r)
            {
                neopixelStrip.setPixelColor(i, primaryColor); 
            }
            else if (i == r)
            {
                neopixelStrip.setPixelColor(i, secondaryColor); 
            }
            else
            {
                neopixelStrip.setPixelColor(i, 0x000000);
            }
        }

        neopixelStrip.show();
    }

    void scrobeEffect(const uint32_t color)
    {

    }
};
