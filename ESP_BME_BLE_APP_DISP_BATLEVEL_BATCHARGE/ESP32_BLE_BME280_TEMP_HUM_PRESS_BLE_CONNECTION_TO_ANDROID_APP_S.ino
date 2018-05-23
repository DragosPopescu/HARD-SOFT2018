/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#include <Wire.h>
#include <SPI.h>
#include <BME280I2C.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
BME280I2C bme; 
#define BatteryLevel 34
#define BatteryChargeLevel 35

HardwareSerial Serial2(2); // pin 16=RX, pin 17=TX


BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int txValue = 0;
int txValue1 = 0;
int txValue2 = 0 ;
const int readPin = 32; // Use GPIO number. See ESP32 board pinouts
const int LED = 2; // Could be different depending on the dev board. I used the DOIT ESP32 dev board.

//std::string rxValue; // Could also make this a global var to access it in loop()

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }

        Serial.println();

        // Do stuff based on the command received from the app
        if (rxValue.find("A") != -1) { 
          Serial.print("Turning ON!");
          digitalWrite(LED, HIGH);
        }
        else if (rxValue.find("B") != -1) {
          Serial.print("Turning OFF!");
          digitalWrite(LED, LOW);
        }

        Serial.println();
        Serial.println("*********");
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  pinMode(LED, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("ESP32 TIMISOARA 2"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");


  while(!Serial) {} // Wait

  Wire.begin();

  while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

  // bme.chipID(); // Deprecated. See chipModel().
  switch(bme.chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor! No Humidity available.");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }
  
Serial.println("Start");
}


float batteryLevel;
float chargeLevel;

void loop() {
      float temperature,humidity,presure,newpresure;
   float temp(NAN), hum(NAN), pres(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);

 
   temperature = temp;
   humidity = hum;
   presure= pres;
   newpresure=presure*750/100000;

    chargeLevel = (3.3 * analogRead(BatteryChargeLevel) / 4096) * 99 / 2.17;
    batteryLevel = (3.3 * analogRead(BatteryLevel) / 4096) * 99 / 1.4;

    if(chargeLevel > 99) chargeLevel = 99;
    if(batteryLevel > 99) batteryLevel = 99;
    
       Serial.println(int(batteryLevel));
       Serial.println(int(chargeLevel));

    Serial2.print("B"); Serial2.print(int(batteryLevel));
    delay(50);
    Serial2.print("C"); Serial2.print(int(chargeLevel));
    delay(50);
    Serial2.print("T"); Serial2.print(int(temperature));
    delay(50);
    Serial2.print("H"); Serial2.print(int(humidity));
    delay(50);
    Serial2.print("P"); Serial2.print(int(newpresure));

       
  if (deviceConnected) {
  // return temp;
  //  txValue = temperature; // This could be an actual sensor reading!
   // txValue1 = humidity;
   // txValue2 = pres;
    
    // Let's convert the value to a char array:
    char txString[2],txString1[2],txString2[2]; // make sure this is big enuffz
    dtostrf(temperature, 1, 2, txString); // float_val, min_width, digits_after_decimal, char_buffer
    dtostrf(humidity, 1, 2, txString1);
    dtostrf(presure, 1, 2, txString2);
    
    
char sendBuffer[128];



sprintf(sendBuffer,"%d,%d,%d",int(temperature),int(humidity),int(newpresure));
//sprintf(sendBuffer,"T:%d°C\nH:%d%%\nP:%dPa dddd\n",int(temperature),int(humidity),int(presure/100));
//Serial.println(sendBuffer);
//bleSerial.print(sendBuffer);
//    pCharacteristic->setValue(&txValue, 1); // To send the integer value
//    pCharacteristic->setValue("Hello!"); // Sending a test message
    pCharacteristic->setValue(sendBuffer);
    //pCharacteristic->setValues(txString1);
    
    pCharacteristic->notify(); // Send the value to the app!
    Serial.print("*** Sent Value: ");
    Serial.print(txString);
    Serial.print(txString1);
    Serial.print(txString2);
    Serial.println(" ***");

    // You can add the rxValue checks down here instead
    // if you set "rxValue" as a global var at the top!
    // Note you will have to delete "std::string" declaration
    // of "rxValue" in the callback function.
//    if (rxValue.find("A") != -1) { 
//      Serial.println("Turning ON!");
//      digitalWrite(LED, HIGH);
//    }
//    else if (rxValue.find("B") != -1) {
//      Serial.println("Turning OFF!");
//      digitalWrite(LED, LOW);
//    }
  }
  delay(1000);
}

/*
void printBME280Data
(
   Stream* client
)
{
   float temp(NAN), hum(NAN), pres(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);

   client->print("Temp: ");
   client->print(temp);
   client->print("°"+ String(tempUnit == BME280::TempUnit_Celsius ? 'C' :'F'));
   client->print("\t\tHumidity: ");
   client->print(hum);
   client->print("% RH");
   client->print("\t\tPressure: ");
   client->print(pres);
   client->println(" Pa");

   delay(1000);
  // return temp;
}
*/
