#include "CurieIMU.h"
#include "CurieBLE.h"


BLEService batonService("123C");
BLEUnsignedCharCharacteristic batonBeatChar("123C", BLERead | BLENotify);

void setup() {
    Serial.begin(9600); // initialize Serial communication
    pinMode(13, OUTPUT); // setup LED pin
    
    // initialize device
    Serial.println("Initializing IMU device...");
    CurieIMU.begin();
    CurieIMU.setAccelerometerRange(2); // Set the accelerometer range to 2G

    BLE.begin();
    BLE.setLocalName("AcapellaBot Baton");
    BLE.setAdvertisedService(batonService);             // add the service UUID
    batonService.addCharacteristic(batonBeatChar);    // add the battery level characteristic
    BLE.addService(batonService);                       // Add the BLE Battery service
    batonBeatChar.setValue(0);            // initial value for this characteristic
    BLE.advertise();
}

float peakVal = 0.0;
int sig = 0;
unsigned int sigStart = 0;
unsigned int sigEnd = 0;
unsigned int lastSig = 0;

void loop() {

    BLE.setLocalName("AcapellaBot Baton");

    BLEDevice central = BLE.central();

    if (central) {
        Serial.print("Connected to central: ");
        // print the central's MAC address:
        Serial.println(central.address());

        while (central.connected()) {
    
            float ax, ay, az;   //scaled accelerometer values
            
            // read accelerometer measurements from device, scaled to the configured range
            CurieIMU.readAccelerometerScaled(ax, ay, az);
            float vlen = sqrt(ax*ax + ay*ay + az*az) - 1;
            if (vlen < 0.8) {
                peakVal = 0;
            } else {
                if (vlen >= peakVal) {
                    peakVal = vlen;
                } else {
                    
                } 
            }
        
            if (peakVal > 0) {
                if (sigStart == 0) {
                    if (millis() - sigEnd > 30) {
                        if (millis() - lastSig > 201) {
                            Serial.println(millis() - lastSig);
                            batonBeatChar.setValue(millis() - lastSig);
                            lastSig = millis();
                            sigStart = millis();
                            sig = 1;
                        } else {
                            sig = 0;
                        }
                    } else {
                        sig = 0;
                    }
                } else {
                    sig = 0;
                }
            } else {
                if (sigStart != 0) {
                    sigEnd = millis();
                    sigStart = 0;
                }
                sig = 0;
            }
        
            digitalWrite(13, sig);

        }

        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }

    
    
    //Serial.println(sig);
}
