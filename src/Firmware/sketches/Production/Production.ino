#include <NonDelayNeoPixelAnimations.h>
#include <IRremote.h>
#include "I2Cdev.h"
#include "MPU6050.h"

#define PIN_NEOPIXELS 10
#define NEOPIXELS_COUNT 25
#define NEOPIXELS_BRIGHTNESS 255

#define PIN_IRRECEIVER 2

NonDelayNeoPixelAnimations neoPixels = NonDelayNeoPixelAnimations(PIN_NEOPIXELS, NEOPIXELS_COUNT, NEOPIXELS_BRIGHTNESS, &onNeoPixelAnimationComplete);
//SimpleMPU6050A motionSensor = SimpleMPU6050A();
IRrecv irReceiver = IRrecv(PIN_IRRECEIVER);

decode_results irReceiverResults;

MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;

bool mot_detect, zero_detect; 
bool XnegMD, XposMD, YnegMD, YposMD, ZnegMD, ZposMD;

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

    //Serial.println("nr;getRawAccX;getRawAccY;getRawAccZ;getRawGyroX;getRawGyroY;getRawGyroZ;getGyroX;getGyroY;getGyroZ;");
    //Serial.println("nr;getGyroX;getGyroY;getGyroZ;");

    //Serial.println("nr;getRawAccX;getRawAccY;getRawAccZ;getGyroX;getGyroY;getGyroZ;");
    Serial.println("nr;val");

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

double initialAccX;
double initialAccY;
double initialAccZ;

long initialacc;

void setupMotionSensor()
{
    // Serial.println("setupMotionSensor(): Calibrating MPU6050 gyro and accelaration sensor...");

    // // Calibrate only
    // motionSensor.calibrate();

    // // Setup registers etc.
    // motionSensor.setup();

    // motionSensor.update();
    // initialAccX = motionSensor.getRawAccX();
    // initialAccY = motionSensor.getRawAccY();
    // initialAccZ = motionSensor.getRawAccZ();

    // Serial.println("setupMotionSensor(): Calibration done. Sensor is ready.");

    Wire.begin();

    accelgyro.initialize();

    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    accelgyro.setAccelerometerPowerOnDelay(3); // 3ms delay (default)

    //accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);

    accelgyro.setIntMotionEnabled(true); // Generate interupt when motion detected
    accelgyro.setMotionDetectionThreshold(2); // 1 LSB = 2mg
    accelgyro.setMotionDetectionDuration(1); // 1 LSB = 1ms

    accelgyro.setIntZeroMotionEnabled(true); // Generate interupt when zero motion detected
    accelgyro.setZeroMotionDetectionThreshold(2); // 1 LSB = 2mg
    accelgyro.setZeroMotionDetectionDuration(2); // 1 LSB = 64ms

    accelgyro.setDHPFMode(MPU6050_DHPF_5); // 5Hz low-pass filter



    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    initialacc = ax + ay + az;
    initialacc = abs(initialacc); 

}

bool isIRReceicerIdle()
{
  return (irReceiver.decode(&irReceiverResults) || irReceiverResults.rawlen == 0);
}

long counter = 1;

bool isNotInRange(const double value, const double min, const double max) {
    return (value <= min || value >= max);
}

long mergedacc;

void loop()
{
    // Only update Neopixels if the IR is idling to avoid interrupt caused timing issues
    if (isIRReceicerIdle())
    {
        neoPixels.update();
    }

    // accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // mergedacc = ax + ay + az;
    // mergedacc = abs(mergedacc); 

    // if (isNotInRange(mergedacc, initialacc - 1000, initialacc + 1000)) {
    //     Serial.println("Movement detected");
    // } else {
    //     Serial.println("NO Movement detected");
    // }

    // Serial.print("mergedacc = ");
    // Serial.println(mergedacc);

    // Serial.print("a/g:\t");
    // Serial.print(ax); Serial.print("\t");
    // Serial.print(ay); Serial.print("\t");
    // Serial.print(az); Serial.print("\t");
    // Serial.print(gx); Serial.print("\t");
    // Serial.print(gy); Serial.print("\t");
    // Serial.println(gz);

    // XposMD = accelgyro.getXPosMotionDetected();
    // XnegMD = accelgyro.getXNegMotionDetected();
    // YposMD = accelgyro.getYPosMotionDetected();
    // YnegMD = accelgyro.getYNegMotionDetected();
    // ZposMD = accelgyro.getZPosMotionDetected();
    // ZnegMD = accelgyro.getZNegMotionDetected();

    // mot_detect = accelgyro.getIntMotionStatus();
    // zero_detect = accelgyro.getIntZeroMotionStatus();
    
    // if (XposMD || XnegMD) {
    //     Serial.println("Motion detected on X axis!");
    // }

    // if (YposMD || YnegMD) {
    //     Serial.println("Motion detected on Y axis!");
    // }

    // if (ZposMD || ZnegMD) {
    //     Serial.println("Motion detected on Z axis!");
    // }

    // if (accelgyro.getZeroMotionDetected()) {
    //     Serial.println("Zero Motion detected!");
    // }

    // Serial.print("DieTemp:\t");Serial.println(temp);
    
    // Serial.print("ZeroMotion(97):\t");  
    // Serial.print(zero_detect); Serial.print("\t");
    // Serial.print("Count: \t");Serial.print(count); Serial.print("\t");
    // Serial.print(XnegMD); Serial.print("\t");
    // Serial.print(XposMD); Serial.print("\t");
    // Serial.print(YnegMD); Serial.print("\t");
    // Serial.print(YposMD); Serial.print("\t");
    // Serial.print(ZnegMD); Serial.print("\t");
    // Serial.println(ZposMD);

    //delay(80);

    // motionSensor.update();

    // Serial.print(counter);
    // Serial.print(";");


    // Serial.println(mergedacc);

    // Serial.print(motionSensor.getRawAccX());
    // Serial.print(";");
    // Serial.print(motionSensor.getRawAccY());
    // Serial.print(";");
    // Serial.print(motionSensor.getRawAccZ());
    // Serial.print(";");

    // // Serial.print(motionSensor.getRawGyroX());
    // // Serial.print(";");
    // // Serial.print(motionSensor.getRawGyroY());
    // // Serial.print(";");
    // // Serial.print(motionSensor.getRawGyroZ());
    // // Serial.print(";");

    // Serial.print(motionSensor.getGyroX());
    // Serial.print(";");
    // Serial.print(motionSensor.getGyroY());
    // Serial.print(";");
    // Serial.print(motionSensor.getGyroZ());
    // Serial.print(";");

    // Serial.println();

    // counter++;

    // if (isNotInRange(motionSensor.getRawAccX(), 15000.0, 17000.0)  && motionSensor.getGyroY() > 40.0) {
    //     neoPixels.setState(OFF);
    // } else {
    //     neoPixels.setState(RAINBOWCYCLE);
    // }


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
