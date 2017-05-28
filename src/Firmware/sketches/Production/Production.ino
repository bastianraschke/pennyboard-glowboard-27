#include <Adafruit_NeoPixel.h>

#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

#define PIN_NEOPIXELS 10
#define NEOPIXELS_COUNT 25
#define NEOPIXELS_BRIGHTNESS 150

enum State
{
    RAINBOW,
    TWOCOLOR   
};

class NeoPatterns
{
    public:

    void (*onComplete)();  // Callback on completion of pattern
    
    NeoPatterns(void (*callback)())
    {
        onComplete = callback;
        _setupNeopixels();
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

                if (onComplete != NULL)
                {
                    onComplete();
                }
            }
        }
    }

    void setState(State currentState) {
        state = currentState;
    }

    void onUpdate() {
        switch(state) {
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
        for (int i = 0; i < NEOPIXELS_COUNT; i++)
        {
            neopixelStrip.setPixelColor(i, _getCurrentWheelColor(((i * 256 / NEOPIXELS_COUNT) + currentIndex) & 255));
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
            float percentage = _mapPixelCountToPercentage(i, neopixelCount);

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

    float _mapPixelCountToPercentage(uint16_t i, float count)
    {
        const float currentPixel = (float) i;
        const float neopixelCount = (float) count;

        const float min = 0.0f;
        const float max = 1.0f;

        return (currentPixel - 0.0f) * (max - min) / (neopixelCount - 0.0f) + min;
    }

    private:

    Adafruit_NeoPixel neopixelStrip;

    unsigned long updateInterval = 10;
    unsigned long lastUpdate = 0;
    uint16_t currentIndex = 0;
    uint16_t totalSteps = 255;

    State state;

    void _setupNeopixels()
    {
        Serial.println("Setup Neopixels...");

        neopixelStrip = Adafruit_NeoPixel(NEOPIXELS_COUNT, PIN_NEOPIXELS, NEO_GRB + NEO_KHZ800);
        neopixelStrip.begin();
        neopixelStrip.setBrightness(NEOPIXELS_BRIGHTNESS);
    }

    uint32_t _getCurrentWheelColor(uint8_t wheelPosition)
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

int j = 0;

NeoPatterns np = NeoPatterns(&onComplete);

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

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;


void setup()
{
    Serial.begin(115200);
    Serial.println("setup()");


    Wire.begin();


    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

}

void loop()
{



    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);


    if (ay < -1000) {
        np.setState(TWOCOLOR);
    } else {
        np.setState(RAINBOW);
    }

    np.update();


    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);

    // Serial.println();
    
    //delay(50);
}
