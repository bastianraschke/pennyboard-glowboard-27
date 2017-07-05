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

bool isIRReceicerIdle()
{
  return (irReceiver.decode(&irReceiverResults) || irReceiverResults.rawlen == 0);
}

void loop()
{
    // Only update Neopixels if the IR is idling to avoid interrupt caused timing issues
    if (isIRReceicerIdle())
    {
        neoPixels.update();
    }

    motionSensor.update();

    if (irReceiver.decode(&irReceiverResults))
    {
        const long decodedValue = irReceiverResults.value;
        Serial.println(decodedValue, HEX);

        // Important: No default because of busy codes of IR receiver
        switch (decodedValue)
        {
            // Button 'Off'
            case 0xF740BF:
                neoPixels.setState(OFF);
                break;

            // Button 'On'
            case 0xF7C03F:
                neoPixels.setState(TWOCOLOR);
                break;

            // Button 'Brighter'
            case 0xF700FF:
                neoPixels.setBrightnessHigh();
                break;

            // Button 'Darker'
            case 0xF7807F:
                neoPixels.setBrightnessLow();
                break;

            // Button 'Flash'
            case 0xF7D02F:
                neoPixels.setState(RAINBOWCYCLE);
                break;

            // Button 'Strobe'
            case 0xF7F00F:
                neoPixels.setState(RAINBOW);
                break;

            // Button 'Fade'
            case 0xF7C837:
                neoPixels.setState(COLORWIPE);
                break;
        
            // Button 'Smooth'
            case 0xF7E817:
                break;
        }

        irReceiver.resume();
    }
}
