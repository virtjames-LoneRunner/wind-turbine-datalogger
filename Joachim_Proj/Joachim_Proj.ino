#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <RTClib.h> // RTClib.h library (by Adafruit) for the DS3231 RTC
#include "HX711.h" // HX711 Arduino Library by Bogdan Necula

// Edit values in this file as needed
#include "variables.h"

#include "additional-functions.h"

// Real-time Clock
RTC_DS3231 rtc;

// Create object for Load Cell
HX711 scale;
// RPM Variables
unsigned long rpmtime;
float rpmfloat;
unsigned int rpm;
bool tooslow = 1;

// Loadcell variables
float load;

// CS Pin for SD Card Module
const int chipSelect = SDCARDCSPIN;

// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;
 
// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 
 
// Float for Reference Voltage
float ref_voltage = 5.0;
 
// Integer for ADC value
int adc_value = 0;

void setup() {
  Serial.begin(9600);
  rtc.begin();
  // Indicator LED
  pinMode(INDICATORLIGHTPIN, OUTPUT);

  // Setup SD Card
  pinMode(chipSelect, OUTPUT);

  // Initialize SD Card Module
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial) {
    ; 
  }
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    // Signal User that initialization failed
    digitalWrite(INDICATORLIGHTPIN, LOW);
    while (1);
  }
  Serial.println("card initialized.");
  // Signal User that initialization succeeded
  digitalWrite(INDICATORLIGHTPIN, HIGH);

  // Setup Tachometer
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS12); //Prescaler 256
  TIMSK1 |= (1 << TOIE1); //enable timer overflow
  pinMode(TACHOMETER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(TACHOMETER_PIN), RPM, FALLING);

  // Setup Load Cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  // Setup Power Sensing
  pinMode(CURRENT_SENSOR_PIN, INPUT);
  pinMode(VOLTAGE_SENSOR_PIN, INPUT);
}

void loop() {

  // Get RPM Value
  if (tooslow == 1) {
  }
  else {
    rpmfloat = 120 / (rpmtime/ 31250.00);
    rpm = round(rpmfloat);
  }

  // Get Load Cell Value
  load = scale.get_units();

  // Measure Power
  int adc = analogRead(CURRENT_SENSOR_PIN);
  float voltage = adc * 5 / 1023.0;
  float current = (voltage - 2.5) / 0.185;
  if (current < 0.16) {
    current = 0;
  }

  // Read the Analog Input
  adc_value = analogRead(VOLTAGE_SENSOR_PIN);
  // Determine voltage at ADC input
  adc_voltage  = (adc_value * ref_voltage) / 1024.0; 
  // Calculate voltage at divider input
  in_voltage = adc_voltage / (R2/(R1+R2));

  // Measure Windspeed
  float sensorValue = analogRead(ANEMOMETER_PIN);
  float anemometer_voltage = (sensorValue / 1023) * 5;
  float wind_speed = mapfloat(anemometer_voltage, 0.4, 2, 0, 32.4); // m/s

  DateTime rtcTime = rtc.now();

  int ss = rtcTime.second();
  int mm = rtcTime.minute();
  int hh = rtcTime.twelveHour();
  int DD = rtcTime.dayOfTheWeek();
  int dd = rtcTime.day();
  int MM = rtcTime.month();
  int yyyy = rtcTime.year();

  // Prepare Data to be printed
  String dataString = "";
  // Date and time
  dataString += MM;
  dataString += "-";
  dataString += dd;
  dataString += "-";
  dataString += yyyy;
  dataString += "---";
  dataString += hh;
  dataString += ":";
  dataString += mm;
  dataString += ":";
  dataString += ss;
  dataString += ",";

  // Data
  dataString += rpm;
  dataString += ",";
  dataString += load;
  dataString += ",";
  dataString += current;
  dataString += ",";
  dataString += in_voltage;
  dataString += ",";
  dataString += current * in_voltage;
  dataString += ",";
  dataString += wind_speed;
  // dataString += ",";


  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("Error opening datalog.csv, create file if it does not already exist.");
  }

  delay(DELAY_SECONDS * 1000);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Tachometer Functions
void RPM() {
  rpmtime = TCNT1;
  TCNT1 = 0;
  tooslow = 0;
}
ISR(TIMER1_OVF_vect) {
  tooslow = 1;
}