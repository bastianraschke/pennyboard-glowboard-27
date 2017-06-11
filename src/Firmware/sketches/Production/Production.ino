#include <NonDelayNeoPixelAnimations.h>
#include <SimpleMPU6050A.h>

#define PIN_NEOPIXELS 10
#define NEOPIXELS_COUNT 25
#define NEOPIXELS_BRIGHTNESS 255

NonDelayNeoPixelAnimations np = NonDelayNeoPixelAnimations(PIN_NEOPIXELS, NEOPIXELS_COUNT, NEOPIXELS_BRIGHTNESS, &onComplete);
SimpleMPU6050A as = SimpleMPU6050A();

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
    Serial.begin(38400); // 115200
    Serial.println("setup()");

    np.setup();
    np.setState(TWOCOLOR);

    as.setup();

    Serial.println("Calibrating MPU6050 gyro and accelaration sensor...");
    as.calibrate();
    Serial.println("Calibration done.");
}

void loop()
{
    np.update();

    as.update();


  if(Serial.available())
  {
    char rx_char;
    // dummy read
    rx_char = Serial.read();
    // we have to send data, as requested
    if (rx_char == '.'){
      Serial.print(as.getGyroX(), 2);
      Serial.print(", ");
      Serial.print(as.getGyroY(), 2);
      Serial.print(", ");
      Serial.println(as.getGyroZ(), 2);
    }
  }
}
