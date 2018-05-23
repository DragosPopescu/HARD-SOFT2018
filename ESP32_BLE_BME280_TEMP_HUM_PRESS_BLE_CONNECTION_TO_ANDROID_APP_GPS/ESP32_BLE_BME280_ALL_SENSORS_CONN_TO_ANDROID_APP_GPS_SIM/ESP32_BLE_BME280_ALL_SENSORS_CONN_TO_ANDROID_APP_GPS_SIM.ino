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

#include <Adafruit_GPS.h>

BME280I2C bme; 
//#include <SoftwareSerial.h>
/* Create object named Serial2 of the class SoftwareSerial */
//SoftwareSerial Serial2(8, 7);


HardwareSerial Serial2(2); // pin 16=RX, pin 17=TX



#define GPSECHO  true


//HardwareSerial Serial1(1);
//HardwareSerial Serial2(2);
// Choose two free pins
//#define SERIAL1_RXPIN 12
//#define SERIAL1_TXPIN 13
//Adafruit_GPS GPS(&Serial1);





BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int txValue = 0;
int txValue1 = 0;
int txValue2 = 0 ;
const int readPin = 32; // Use GPIO number. See ESP32 board pinouts
//const int LED = 2; // Could be different depending on the dev board. I used the DOIT ESP32 dev board.

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
//          digitalWrite(LED, HIGH);
        }
        else if (rxValue.find("B") != -1) {
          Serial.print("Turning OFF!");
//          digitalWrite(LED, LOW);
        }

        Serial.println();
        Serial.println("*********");
      }
    }
};


typedef enum
{
  GSM__nenTemp1,
  GSM__nenTemp2,
  GSM__nenTemp3,
  GSM__nenHum1,
  GSM__nenHum2,
  GSM__nenHum3,
  GSM__nenPres1,
  GSM__nenPres2,
  GSM__nenPres3,
  GSM__nenLong1,
  GSM__nenLong2,
  GSM__nenLong3,
  GSM__nenLat1,
  GSM__nenLat2,
  GSM__nenLat3,
  GSM__nenAlt1,
  GSM__nenAlt2,
  GSM__nenAlt3
} GSM_enStates;





typedef struct 
{
  GSM_enStates GSM__Temperature;
  GSM_enStates GSM__Humidity;
  GSM_enStates GSM__Pressure;
  GSM_enStates GSM__Long;
  GSM_enStates GSM__Lat;
  GSM_enStates GSM__Alt;
  uint32_t Timer1;
  uint32_t Timer2;
  uint32_t Timer3;
  uint32_t Timer4;
  uint32_t Timer5; 
}GSM_States;

GSM_States GSM_tStates;




void setup() {
  
  Serial.begin(115200);
  //Serial1.begin(9600, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
  Serial2.begin(4800);  // pin 16=RX, pin 17=TX

  BLEDevice::init("ESP32 TIMISOARA 2");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

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
  
  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  while(!Serial) {} // Wait

  Wire.begin();

  while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

   bme.chipID(); // Deprecated. See chipModel().
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

  //GPS.begin(9600);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);  
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  //GPS.sendCommand(PGCMD_ANTENNA);
  //delay(1000);

  Serial2.begin(9600);  /* Define baud rate for software serial communication */
  //Serial.begin(9600); /* Define baud rate for serial communication */

  Serial2.println("AT"); /* Check Communication */
  delay(1000);
  Serial2.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  /* Connection type GPRS */
  delay(1000);   
  Serial2.println("AT+SAPBR=3,1,\"APN\",\"net\""); /* APN of the provider */
  delay(1000);  
  Serial2.println("AT+SAPBR=1,1"); /* Open GPRS context */
  delay(1000);  
  Serial2.println("AT+SAPBR=2,1"); /* Query the GPRS context */
  delay(1000);  
  Serial2.println("AT+HTTPINIT");  /* Initialize HTTP service */
  delay(1000);   
  Serial2.println("AT+HTTPPARA=\"CID\",1");  /* Set parameters for HTTP session */
  delay(1000);  
  Serial2.println("AT+HTTPPARA=\"URL\",\"api.thingspeak.com/update\"");  /* Set parameters for HTTP session */
  delay(1000);

  
  //GSM_tStates.Timer1=0;
  //GSM_tStates.Timer2=0;
  //GSM_tStates.Timer3=0;
  //GSM_tStates.Timer4=0;
  //GSM_tStates.Timer5=0;
}

uint32_t timer = millis();
uint32_t lastSendBT = millis();
uint32_t t1,t2,t3;
uint32_t lastvalue1=0,lastvalue2=0,lastvalue3=0,lastvalue4=0,lastvalue5=0,lastvalue6=0,lastvalue7=0,lastvalue8=0,lastvalue9=0;

void loop() {

  //char c = GPS.read();
 //delay(1);
 // if (GPS.newNMEAreceived()) {
 //   if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
 //     return;  // we can fail to parse a sentence in which case we should just wait for another
 // }
  
 // if (timer > millis())  timer = millis();

 // if (millis() - timer > 2000) { 
//    timer = millis(); // reset the timer
    
    
  //  Serial.print("Fix: "); Serial.println((int)GPS.fix);
 
    
 // }

  
  if (deviceConnected )//&& millis()-lastSendBT > 1000)
  {

    //lastSendBT = millis();
    float temperature,humidity,presure,newpresure;
   float temp(NAN), hum(NAN), pres(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);


  // if (GPS.fix) {
      
    //  Serial.print("Location (in degrees, works with Google Maps): ");
     // Serial.print(GPS.latitudeDegrees, 4);
     // Serial.print(", "); 
     // Serial.println(GPS.longitudeDegrees, 4);
     // Serial.print("Altitude: "); Serial.println(GPS.altitude);

    


 
     temperature = temp;
     humidity = hum;
     presure= pres;

    
     newpresure=presure*750/100000;
    
     char sendBuffer[16];
    sprintf(sendBuffer,"%d,%d,%d",int(temperature),int(humidity),int(newpresure));

    pCharacteristic->setValue(sendBuffer);
    
    
    pCharacteristic->notify(); // Send the value to the app!

  }

  
  
       float temperature,humidity,presure,newpresure;
       float temp(NAN), hum(NAN), pres(NAN);
       BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
       BME280::PresUnit presUnit(BME280::PresUnit_Pa);
       bme.read(pres, temp, hum, tempUnit, presUnit);
switch(GSM_tStates.GSM__Temperature)
{
      
     // GSM_tStates.Timer1 = 1000;
     //  float temperature,humidity,presure,newpresure;
      // float temp(NAN), hum(NAN), pres(NAN);
      // BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
      // BME280::PresUnit presUnit(BME280::PresUnit_Pa);
      // bme.read(pres, temp, hum, tempUnit, presUnit);
  case GSM__nenTemp1:
       {
       if(millis()-lastvalue1 > 1000)
       {
       lastvalue1 = millis();   
       Serial2.println("AT+HTTPDATA=35,10000");  /* POST data of size 33 Bytes with maximum latency time of 10seconds for inputting the data*/  
       //GSM_tStates.Timer2 = 500;
       GSM_tStates.GSM__Temperature = GSM__nenTemp2;
       }
       }
       break;
  case GSM__nenTemp2:
       {   
       if(millis()-lastvalue2 > 500)
       {
       lastvalue2 = millis();
       Serial2.print("api_key=Z3OXBGE62M1FS4H0&field1=");
       Serial2.println(temp); 
       //GSM_tStates.Timer3 = 5500;
       GSM_tStates.GSM__Temperature = GSM__nenTemp3;
       } 
       }
       break;

  case GSM__nenTemp3:
       {
       if(millis()-lastvalue3 > 5500)
       {
       lastvalue3=millis();
       Serial2.println("AT+HTTPACTION=1");  /* Start POST session */
       }
       }
       break;
}


switch(GSM_tStates.GSM__Humidity)
{
       case GSM__nenHum1:
       {
       if(millis()-lastvalue4 > 1000)
       {
       lastvalue4 = millis();   
       Serial2.println("AT+HTTPDATA=35,10000");  /* POST data of size 33 Bytes with maximum latency time of 10seconds for inputting the data*/  
       //GSM_tStates.Timer2 = 500;
       GSM_tStates.GSM__Temperature = GSM__nenHum2;
       }
       }
       break;
  case GSM__nenHum2:
       {   
       if(millis()-lastvalue5 > 500)
       {
       lastvalue5 = millis();
       Serial2.print("api_key=Z3OXBGE62M1FS4H0&field1=");
       Serial2.println(hum); 
       //GSM_tStates.Timer3 = 5500;
       GSM_tStates.GSM__Temperature = GSM__nenHum3;
       } 
       }
       break;
  case GSM__nenHum3:
       {
       if(millis()-lastvalue6 > 5500)
       {
       lastvalue6=millis();
       Serial2.println("AT+HTTPACTION=1");  /* Start POST session */
       }
       }
       break;
       }
}

switch(GSM_tStates.GSM__Pressure){
       case GSM__nenPres1:
       {
       if(millis()-lastvalue7 > 1000)
       {
       lastvalue7 = millis();   
       Serial2.println("AT+HTTPDATA=35,10000");  /* POST data of size 33 Bytes with maximum latency time of 10seconds for inputting the data*/  
       //GSM_tStates.Timer2 = 500;
       GSM_tStates.GSM__Temperature = GSM__nenPres2;
       }
       }
       break;
  case GSM__nenPres2:
       {   
       if(millis()-lastvalue8 > 500)
       {
       lastvalue8 = millis();
       Serial2.print("api_key=Z3OXBGE62M1FS4H0&field1=");
       Serial2.println(pres); 
       //GSM_tStates.Timer3 = 5500;
       GSM_tStates.GSM__Temperature = GSM__nenPres3;
       } 
       }
       break;
  case GSM__nenPres3:
       {
       if(millis()-lastvalue9 > 5500)
       {
       lastvalue9=millis();
       Serial2.println("AT+HTTPACTION=1");  /* Start POST session */
       }
       }
       break;
       }
}

  
 
   
  }
  
//void Pressure()
//{
  
//}

void ShowSerialData()
{
  while(Serial2.available()!=0)  /* If data is available on serial port */
  Serial.write(char (Serial2.read())); /* Print character received on to the serial monitor */
}

/*
void ConsumeTimers(){
    if (GSM_tStates.Timer1 > 0)
  {
    GSM_tStates.Timer1 --;
  }
  if (GSM_tStates.Timer2 > 0)
  {
    GSM_tStates.Timer2 --;
  }
  if (GSM_tStates.Timer3 > 0)
  {
    GSM_tStates.Timer3 --;
  }
  if (GSM_tStates.Timer4 > 0)
  {
    GSM_tStates.Timer4 --;
  }
  if (GSM_tStates.Timer5 > 0) 
  {
    GSM_tStates.Timer5 --;
  }
}
*/
