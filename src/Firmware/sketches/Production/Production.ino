#include <NonDelayNeoPixelAnimations.h>
#include <SimpleMPU6050A.h>
#include <IRremote.h>

#define PIN_NEOPIXELS 10
#define NEOPIXELS_COUNT 25
#define NEOPIXELS_BRIGHTNESS 255

#define PIN_IRRECEIVER 2

#define PRINT_SENSOR_DATA 1

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

long previousacc = 0;
long previousAnimationUpdateTimestamp = 0;
bool previousMovementWasDetected = false;

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
    neoPixels.setState(RAINBOWCYCLE);
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

    Serial.println("setupMotionSensor(): Calibration done.");

    motionSensor.update();

    initialGyroX = motionSensor.getGyroX();
    initialGyroY = motionSensor.getGyroY();
    initialGyroZ = motionSensor.getGyroY();

    initialAccX = motionSensor.getRawAccX();
    initialAccY = motionSensor.getRawAccY();
    initialAccZ = motionSensor.getRawAccZ();

    #if PRINT_SENSOR_DATA == 1
        Serial.print("setupMotionSensor(): initialGyroX = ");
        Serial.println(setupMotionSensor(): initialGyroX);
        
        Serial.print("setupMotionSensor(): initialGyroY = ");
        Serial.println(setupMotionSensor(): initialGyroY);

        Serial.print("setupMotionSensor(): initialGyroZ = ");
        Serial.println(setupMotionSensor(): initialGyroZ);

        Serial.print("setupMotionSensor(): initialAccX = ");
        Serial.println(setupMotionSensor(): initialAccX);
        
        Serial.print("setupMotionSensor(): initialAccY = ");
        Serial.println(setupMotionSensor(): initialAccY);

        Serial.print("setupMotionSensor(): initialAccZ = ");
        Serial.println(initialAccZ);
    #endif
}

void loop()
{
    // Only update Neopixels if the IR is idling to avoid interrupt caused timing issues
    if (isIRReceicerIdle())
    {
        neoPixels.update();
    }

    motionSensor.update();

    // TODO: Just use motion is drive axis
    // TODO: detect pushing
    // TODO: avoid move detection if gyro is changed

    bool movementWasDetected;

    // Serial.print("mergedacc = ");
    // Serial.println(mergedacc);

    long mergedacc = motionSensor.getRawAccX() + motionSensor.getRawAccY() + motionSensor.getRawAccZ();
    mergedacc = abs(mergedacc); 

    if (isNotInRange(mergedacc, previousacc - 500, previousacc + 500)) {
        previousacc = mergedacc;
        Serial.println("Movement detected");

        movementWasDetected = true;
    } else {
        movementWasDetected = false;
    }

    if (previousMovementWasDetected != movementWasDetected) {
        previousMovementWasDetected = movementWasDetected;

        if (movementWasDetected) {
            neoPixels.setState(RAINBOWCYCLE);
            previousAnimationUpdateTimestamp = millis();
        }

    } else if ((millis() - previousAnimationUpdateTimestamp) > 2000 && !movementWasDetected) {
        neoPixels.setState(LASERSCANNER);
        previousAnimationUpdateTimestamp = millis();

        Serial.println("B switching to IDLE");
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
