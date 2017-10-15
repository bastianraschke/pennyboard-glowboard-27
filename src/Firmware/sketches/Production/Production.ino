#include <NonDelayNeoPixelAnimations.h>
#include <SimpleMPU6050A.h>
#include <IRremote.h>

#define PIN_NEOPIXELS 10
#define NEOPIXELS_COUNT 25
#define NEOPIXELS_BRIGHTNESS 255

#define PIN_IRRECEIVER 2

// Unit: sensor raw sensor output
#define MERGED_SENSOR_DATA_THRESHOLD 500

// In milliseconds
#define IDLE_ANIMATION_TIMEOUT 2000

// Enable only for testing/setup because it dramatically slows down loop cycle and animations!
#define PRINT_SENSOR_DATA 0

NonDelayNeoPixelAnimations neoPixels = NonDelayNeoPixelAnimations(PIN_NEOPIXELS, NEOPIXELS_COUNT, NEOPIXELS_BRIGHTNESS, &onNeoPixelAnimationComplete);
IRrecv irReceiver = IRrecv(PIN_IRRECEIVER);
decode_results irReceiverResults;

SimpleMPU6050A motionSensor = SimpleMPU6050A();

double initialGyroX;
double initialGyroY;
double initialGyroZ;

double initialAccX;
double initialAccY;
double initialAccZ;

long printSensorDataCounter = 1;

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

    #if PRINT_SENSOR_DATA == 1
        Serial.println("printSensorDataCounter;getRawAccX;getRawAccY;getRawAccZ;getGyroX;getGyroY;getGyroZ");
    #endif
}

void setupNeoPixels()
{
    Serial.println("setupIrReceiver(): Setup NeoPixels...");
    neoPixels.setup();
    Serial.println("setupIrReceiver(): NeoPixels ready.");

    // Startup scene
    neoPixels.setState(LASERSCANNER);
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
    motionSensor.calibrate();
    motionSensor.setup();
    Serial.println("setupMotionSensor(): Calibration done.");
}

enum MovementState
{
    IDLE,
    MOVING,
    PUSHING,
    CARRYING
};


long previousRawAccelarationDataY = 0;

unsigned long previousMergedRawAccelarationData = 0;
unsigned long previousAnimationUpdateTimestamp = 0;

MovementState previousMovementState = IDLE;

void loop()
{
    // Only update Neopixels if the IR is idling to avoid interrupt caused timing issues
    if (isIRReceicerIdle())
    {
        neoPixels.update();
    }

    motionSensor.update();

    MovementState currentMovementState;

    // if (isNotInRange(motionSensor.getGyroY(), 86 - 20, 86 + 20) && isNotInRange(motionSensor.getGyroZ(), -30, 30)) {

    //     currentMovementState = CARRYING;

    //     Serial.println("The board seems being carried.");

    //     // Serial.print("getGyroY = ");
    //     // Serial.println(motionSensor.getGyroY());

    //     // Serial.print("getGyroZ = ");
    //     // Serial.println(motionSensor.getGyroZ());

    // } else {

        unsigned long rawAccelarationDataY = abs(motionSensor.getRawAccY());

        Serial.print("rawAccelarationDataY = ");
        Serial.println(rawAccelarationDataY);

        if (rawAccelarationDataY > 10000)
        {
            // Serial.println("Pushing on Y axis was detected.");
            currentMovementState = PUSHING;
        }
        else if (rawAccelarationDataY > 3500)
        {
            // Serial.println("Movement on Y axis was detected.");
            currentMovementState = MOVING;
        }
        else
        {
            currentMovementState = IDLE;
        }
    // }

    const bool animationTimeoutIsOver = (millis() - previousAnimationUpdateTimestamp) > 500;
    const bool immediatelyChangeAnimationDetected = currentMovementState == PUSHING || currentMovementState == CARRYING;
    const bool animationChangeIsAllowed = animationTimeoutIsOver || immediatelyChangeAnimationDetected;

    // Serial.print("previousMovementState != currentMovementState = ");
    // Serial.println(previousMovementState != currentMovementState);

    // Serial.print("                        previousMovementState = ");

    // switch(previousMovementState) {
    //     case IDLE:
    //         Serial.println("IDLE");
    //         break;
    //     case MOVING:
    //         Serial.println("MOVING");
    //         break;
    //     case PUSHING:
    //         Serial.println("PUSHING");
    //         break;
    //     case CARRYING:
    //         Serial.println("CARRYING");
    //         break;
    // }

    // Serial.print("                         currentMovementState = ");

    // switch(currentMovementState) {
    //     case IDLE:
    //         Serial.println("IDLE");
    //         break;
    //     case MOVING:
    //         Serial.println("MOVING");
    //         break;
    //     case PUSHING:
    //         Serial.println("PUSHING");
    //         break;
    //     case CARRYING:
    //         Serial.println("CARRYING");
    //         break;
    // }

    // Serial.print("                     animationChangeIsAllowed = ");
    // Serial.println(animationChangeIsAllowed);

    // Serial.println();

    if (previousMovementState != currentMovementState && animationChangeIsAllowed)
    {
        switch(currentMovementState)
        {
            case PUSHING:
                neoPixels.setState(STROBE);
                previousAnimationUpdateTimestamp = millis();
                break;

            case MOVING:
                neoPixels.setState(RAINBOW);
                previousAnimationUpdateTimestamp = millis();
                break;

            case CARRYING:
                neoPixels.setState(OFF);
                previousAnimationUpdateTimestamp = millis();
                break;
        }

        previousMovementState = currentMovementState;
    }
    else if ((millis() - previousAnimationUpdateTimestamp) > IDLE_ANIMATION_TIMEOUT && currentMovementState == IDLE)
    {
        neoPixels.setState(LASERSCANNER);
        previousAnimationUpdateTimestamp = millis();

        Serial.println("No movement detected since timeout - switching to idle.");
    }







    #if PRINT_SENSOR_DATA == 1
        Serial.print(printSensorDataCounter);
        Serial.print(";");

        Serial.print(motionSensor.getRawAccX());
        Serial.print(";");
        Serial.print(motionSensor.getRawAccY());
        Serial.print(";");
        Serial.print(motionSensor.getRawAccZ());
        Serial.print(";");

        Serial.print(motionSensor.getGyroX());
        Serial.print(";");
        Serial.print(motionSensor.getGyroY());
        Serial.print(";");
        Serial.print(motionSensor.getGyroZ());
        Serial.print(";");

        Serial.println();
        printSensorDataCounter++;
    #endif

    // if (irReceiver.decode(&irReceiverResults))
    // {
    //     const long decodedValue = irReceiverResults.value;
    //     Serial.println(decodedValue, HEX);

    //     // Important: No default because of busy codes of IR receiver
    //     switch (decodedValue)
    //     {
    //         // Button 'Off'
    //         case 0xF740BF:
    //             neoPixels.setState(OFF);
    //             break;

    //         // Button 'On'
    //         case 0xF7C03F:
    //             neoPixels.setState(TWOCOLOR);
    //             break;

    //         // Button 'Brighter'
    //         case 0xF700FF:
    //             neoPixels.setBrightnessHigh();
    //             break;

    //         // Button 'Darker'
    //         case 0xF7807F:
    //             neoPixels.setBrightnessLow();
    //             break;

    //         // Button 'Flash'
    //         case 0xF7D02F:
    //             neoPixels.setState(RAINBOWCYCLE);
    //             break;

    //         // Button 'Strobe'
    //         case 0xF7F00F:
    //             neoPixels.setState(RAINBOW);
    //             break;

    //         // Button 'Fade'
    //         case 0xF7C837:
    //             neoPixels.setState(COLORWIPE);
    //             break;
        
    //         // Button 'Smooth'
    //         case 0xF7E817:
    //             break;
    //     }

    //     irReceiver.resume();
    // }
}

bool isIRReceicerIdle()
{
    return (irReceiver.decode(&irReceiverResults) || irReceiverResults.rawlen == 0);
}

bool isNotInRange(const double value, const double min, const double max)
{
    return (value <= min || value >= max);
}
