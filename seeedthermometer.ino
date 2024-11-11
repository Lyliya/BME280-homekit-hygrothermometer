#include <HomeSpan.h>
#include <Adafruit_BME280.h>
#include "define.h"

const float TEMPERATURE_OFFSET = -2.0f;

Adafruit_BME280 bme;

struct TempSensor : Service::TemperatureSensor
{
  SpanCharacteristic *temp;
  
  TempSensor() : Service::TemperatureSensor()
  {
    temp=new Characteristic::CurrentTemperature(0);
    temp->setRange(-40, 85);
  }

  void loop()
  {
    if(temp->timeVal()>5000)
    {
      bme.takeForcedMeasurement();
      float temperature = bme.readTemperature() + TEMPERATURE_OFFSET;
      Serial.printf("Temperature: %f\n", temperature);
      temp->setVal(temperature);
    }
  }
};

struct HumSensor : Service::HumiditySensor
{
  SpanCharacteristic *humidity;
  
  HumSensor() : Service::HumiditySensor()
  { 
    humidity=new Characteristic::CurrentRelativeHumidity(0);
  }

  void loop()
  {
    if(humidity->timeVal()>5000)
    {
      bme.takeForcedMeasurement();
      float humidityValue = bme.readHumidity();
      Serial.printf("Humidity: %f\n", humidityValue);
      humidity->setVal(humidityValue);
    }
  }
};

void setup()
{
  Serial.begin(115200);

  homeSpan.setApSSID(AP_SSID);
  homeSpan.setApPassword(AP_PASSWORD);
  homeSpan.enableAutoStartAP();
  homeSpan.begin(Category::Sensors, "Hygro-thermometre");

  if (!bme.begin(0x76)) {
    while (1);
  }
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::FILTER_OFF,
    Adafruit_BME280::STANDBY_MS_1000);

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Manufacturer("Lyliya");
      new Characteristic::SerialNumber("2-THERMOMETER");
      new Characteristic::Model("BME280-Thermo-Hygro");
      new Characteristic::FirmwareRevision("1.0");

      
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Capteur de Temperature");
    new TempSensor();

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Capteur d'humidité");
    new HumSensor();
}

void loop()
{
  homeSpan.poll();
}
