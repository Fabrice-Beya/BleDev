#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>

#define SERVICE_UUID "e9861d7e-7853-11ea-bc55-0242ac130003"
#define TEMP_UUID "e986209e-7853-11ea-bc55-0242ac130003"
#define HUM_UUID "e9862274-7853-11ea-bc55-0242ac130003"
#define CMD_UUID "e9862404-7853-11ea-bc55-0242ac130003"

BLECharacteristic *pTemperatureCharacteristic;
BLECharacteristic *pHumidityCharacteristic;
BLECharacteristic *pCommandCharacteristic;

#define DHTPIN 27     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 112

DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS = 2000;

void updateTemperature()
{
  float temperature = dht.readTemperature();
  if (isnan(temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C ");

    char buffer[10];
    sprintf(buffer, "%2.2f", temperature);
    pTemperatureCharacteristic->setValue(buffer);
    pTemperatureCharacteristic->notify(true);
  }
}

void updateHumidity()
{
  float temperature = dht.readTemperature();
  if (isnan(temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C ");

    char buffer[10];
    sprintf(buffer, "%2.2f", temperature);
    pTemperatureCharacteristic->setValue(buffer);
    pTemperatureCharacteristic->notify(true);
  }
}

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCommandCharacteristic->getValue();

    if (rxValue.length() > 0)
    {
      Serial.println("*********");
      Serial.print("Received Value: ");

      for (int i = 0; i < rxValue.length(); i++)
      {
        Serial.print(rxValue[i]);
      }

      Serial.println();

      // Do stuff based on the command received from the app
      if (rxValue.find("A") != -1)
      {
        Serial.print("Received command A");
      }
      else if (rxValue.find("B") != -1)
      {
        Serial.print("Received Command B");
      }

      Serial.println();
      Serial.println("*********");
    }
  }
};

void updateReadings()
{
}

void setup()
{
  Serial.begin(115200);
  dht.begin();
  Serial.println("Starting BLE work!");

  BLEDevice::init("Photon-BLE");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTemperatureCharacteristic = pService->createCharacteristic(
      TEMP_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  pHumidityCharacteristic = pService->createCharacteristic(
      HUM_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  pCommandCharacteristic = pService->createCharacteristic(
      CMD_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pTemperatureCharacteristic->setValue("0.0");
  pHumidityCharacteristic->setValue("0.0");
  pCommandCharacteristic->setValue("0.0");

  pCommandCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  updateTemperature();
  updateHumidity();
}

void loop()
{
  delay(delayMS);
  updateTemperature();
  updateHumidity();
}