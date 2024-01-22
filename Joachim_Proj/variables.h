#define CURRENT_SENSOR_PIN A0
#define VOLTAGE_SENSOR_PIN A1
#define ANEMOMETER_PIN A2
#define TACHOMETER_PIN 3
#define LOADCELL_SCK_PIN  2                 // or CLK
#define LOADCELL_DOUT_PIN  4                // or DT
#define INDICATORLIGHTPIN 8
#define SDCARDCSPIN 10

// Torquemeter Parameters
#define CALIBRATION_FACTOR -7050.0          //This value is obtained using the SparkFun_HX711_Calibration.
// Pin for IR Sensor Output
// Choose from either 2 or 3 (hardware pin interrupts)

#define GENERATOR_EFFICIENCY 0.9

// Sets the delay after each measurement
#define DELAY_SECONDS 1
