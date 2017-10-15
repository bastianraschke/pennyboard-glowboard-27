#include <NonDelayNeoPixelAnimations.h>
#include <SimpleMPU6050A.h>
#include <IRremote.h>

#define PIN_NEOPIXELS 10
#define NEOPIXELS_COUNT 25
#define NEOPIXELS_BRIGHTNESS 255

#define PIN_IRRECEIVER 2

// Timeout in milliseconds:
#define ANIMATION_TIMEOUT_IDLE 2000
#define ANIMATION_TIMEOUT_OTHER 500

#define MOTIONSENSOR_THRESHOLD_MOVEMENT 3500
#define MOTIONSENSOR_THRESHOLD_PUSHING 10000

// Enable only for testing/setup because it dramatically slows down loop cycle and animations!
#define PRINT_SENSOR_DATA 0

enum MovementState
{
    IDLE,
    MOVING,
    PUSHING,
    CARRYING
};

NonDelayNeoPixelAnimations neoPixels = NonDelayNeoPixelAnimations(PIN_NEOPIXELS, NEOPIXELS_COUNT, NEOPIXELS_BRIGHTNESS, &onNeoPixelAnimationComplete);
IRrecv irReceiver = IRrecv(PIN_IRRECEIVER);
decode_results irReceiverResults;

SimpleMPU6050A motionSensor = SimpleMPU6050A();

bool remoteControlOverrideActive = false;

unsigned long previousAnimationUpdateTimestamp = 0;
MovementState previousMovementState = IDLE;

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
        Serial.println(F("printSensorDataCounter;getRawAccX;getRawAccY;getRawAccZ;getGyroX;getGyroY;getGyroZ"));
    #endif
}

void setupNeoPixels()
{
    Serial.println(F("setupIrReceiver(): Setup NeoPixels..."));
    neoPixels.setup();
    Serial.println(F("setupIrReceiver(): NeoPixels ready."));

    // Startup scene
    neoPixels.setState(LASERSCANNER);
}

void setupIrReceiver()
{
    Serial.println(F("setupIrReceiver(): Setup IR receiver..."));
    irReceiver.enableIRIn();
    Serial.println(F("setupIrReceiver(): IR receiver is ready."));
}

void setupMotionSensor()
{
    Serial.println(F("setupMotionSensor(): Calibrating MPU6050 gyro and accelaration sensor..."));
    motionSensor.calibrate();
    motionSensor.setup();
    Serial.println(F("setupMotionSensor(): Calibration done."));
}

void loop()
{
    // Only update Neopixels if the IR is idling to avoid interrupt caused timing issues
    if (isIRReceicerIdle())
    {
        neoPixels.update();
    }

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
                remoteControlOverrideActive = true;
                break;

            // Button 'On'
            case 0xF7C03F:
                neoPixels.setState(TWOCOLOR);
                remoteControlOverrideActive = true;
                break;

            // Button 'Brighter'
            case 0xF700FF:
                neoPixels.setBrightnessHigh();
                remoteControlOverrideActive = true;
                break;

            // Button 'Darker'
            case 0xF7807F:
                neoPixels.setBrightnessLow();
                remoteControlOverrideActive = true;
                break;

            // Button 'Flash'
            case 0xF7D02F:
                neoPixels.setState(RAINBOWCYCLE);
                remoteControlOverrideActive = true;
                break;

            // Button 'Strobe'
            case 0xF7F00F:
                neoPixels.setState(RAINBOW);
                remoteControlOverrideActive = true;
                break;

            // Button 'Fade'
            case 0xF7C837:
                break;

            // Button 'Smooth'
            case 0xF7E817:
                break;
        }

        irReceiver.resume();
    }

    if (!remoteControlOverrideActive)
    {
        const MovementState currentMovementState = getCurrentMotionState();

        const bool animationTimeoutIsOver = (millis() - previousAnimationUpdateTimestamp) > ANIMATION_TIMEOUT_OTHER;
        const bool immediatelyChangeAnimationDetected = currentMovementState == PUSHING || currentMovementState == CARRYING;
        const bool animationChangeIsAllowed = animationTimeoutIsOver || immediatelyChangeAnimationDetected;

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
        else if ((millis() - previousAnimationUpdateTimestamp) > ANIMATION_TIMEOUT_IDLE && currentMovementState == IDLE)
        {
            neoPixels.setState(LASERSCANNER);
            previousAnimationUpdateTimestamp = millis();

            Serial.println(F("No movement detected since timeout - switched to idle."));
        }
    }
}

bool isIRReceicerIdle()
{
    return (irReceiver.decode(&irReceiverResults) || irReceiverResults.rawlen == 0);
}

MovementState getCurrentMotionState()
{
    motionSensor.update();

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

    MovementState currentMovementState;

    // const bool boardIsBeingCarried = isNotInRange(motionSensor.getGyroY(), 86 - 20, 86 + 20) && isNotInRange(motionSensor.getGyroZ(), -30, 30);
    const bool boardIsBeingCarried = false;

    if (boardIsBeingCarried)
    {
        // Serial.print("getGyroY = ");
        // Serial.println(motionSensor.getGyroY());

        // Serial.print("getGyroZ = ");
        // Serial.println(motionSensor.getGyroZ());

        Serial.println(F("The board seems being carried."));
        currentMovementState = CARRYING;
    }
    else
    {
        unsigned long rawAccelarationDataY = abs(motionSensor.getRawAccY());

        // Serial.print("rawAccelarationDataY = ");
        // Serial.println(rawAccelarationDataY);

        if (rawAccelarationDataY > MOTIONSENSOR_THRESHOLD_PUSHING)
        {
            Serial.println(F("Pushing on Y axis was detected."));
            currentMovementState = PUSHING;
        }
        else if (rawAccelarationDataY > MOTIONSENSOR_THRESHOLD_MOVEMENT)
        {
            Serial.println(F("Movement on Y axis was detected."));
            currentMovementState = MOVING;
        }
        else
        {
            currentMovementState = IDLE;
        }
    }

    return currentMovementState;
}

bool isNotInRange(const double value, const double min, const double max)
{
    return (value <= min || value >= max);
}
