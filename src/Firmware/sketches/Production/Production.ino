#include <NonDelayNeoPixelAnimations.h>
#include <SimpleMPU6050A.h>
#include <IRremote.h>

#define PIN_NEOPIXELS 10
#define NEOPIXELS_COUNT 25
#define NEOPIXELS_BRIGHTNESS 255

#define PIN_IRRECEIVER 2

NonDelayNeoPixelAnimations neoPixels = NonDelayNeoPixelAnimations(PIN_NEOPIXELS, NEOPIXELS_COUNT, NEOPIXELS_BRIGHTNESS, &onNeoPixelAnimationComplete);
SimpleMPU6050A motionSensor = SimpleMPU6050A();
IRrecv irReceiver = IRrecv(PIN_IRRECEIVER);

decode_results irReceiverResults;

void onNeoPixelAnimationComplete()
{
    // Not needed
}

void setup()
{
    Serial.begin(115200);

    setupNeoPixels();
    setupIrReceiver();
    setupMotionSensor();
}

void setupNeoPixels()
{
    Serial.println("setupIrReceiver(): Setup NeoPixels...");
    neoPixels.setup();
    Serial.println("setupIrReceiver(): NeoPixels ready.");

    // Startup scene
    neoPixels.setState(TWOCOLOR);
}

void setupIrReceiver()
{
    Serial.println("setupIrReceiver(): Setup IR receiver...");
    irReceiver.enableIRIn();
    Serial.println("setupIrReceiver(): IR receiver is ready.");
}

void setupMotionSensor()
{
    Serial.println("setupMotionSensor(): Calibrating MPU6050 gyro and accelaration sensor...");
    // Calibrate only
    motionSensor.calibrate();

    // Setup registers etc.
    motionSensor.setup();

    Serial.println("setupMotionSensor(): Calibration done. Sensor is ready.");
}

void loop()
{
   neoPixels.update();
   motionSensor.update();

    if (irReceiver.decode(&irReceiverResults))
    {
        const int decodedValue = irReceiverResults.value;
        Serial.println(decodedValue, HEX);

        switch(decodedValue)
        {
            case 0xFFFF:
                neoPixels.setState(RAINBOW);
                break;

            default:
                neoPixels.setState(TWOCOLOR);
                break;
        }

        irReceiver.resume();
    }
}
