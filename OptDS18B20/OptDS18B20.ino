#include <OneWire.h>

#define SERIAL_BAUD   9600

OneWire myds(2);
byte resolution;
unsigned long starttime;
unsigned long elapsedtime;
uint8_t dsaddr1[8] = { 0x28, 0xD4, 0x2D, 0x79, 0xA2, 0x01, 0x03, 0xB6 }; // defining adresses manually
uint8_t dsaddr2[8] = { 0x28, 0xA2, 0xA5, 0x79, 0xA2, 0x00, 0x03, 0xAD };
uint8_t dsaddr3[8] = { 0x28, 0xA6, 0xED, 0x79, 0xA2, 0x01, 0x03, 0x62 };
uint8_t dsaddr4[8] = { 0x28, 0x0B, 0x84, 0x79, 0xA2, 0x00, 0x03, 0x52 };

void dssetresolution(OneWire myds, byte addr[8], byte resolution) {

  // Get byte for desired resolution
  byte resbyte = 0x1F;
  if (resolution == 12) {
    resbyte = 0x7F;
  }
  else if (resolution == 11) {
    resbyte = 0x5F;
  }
  else if (resolution == 10) {
    resbyte = 0x3F;
  }

  // Set configuration
  myds.reset();
  myds.select(addr);
  myds.write(0x4E);         // Write scratchpad
  myds.write(0);            // TL
  myds.write(0);            // TH
  myds.write(resbyte);         // Configuration Register

  myds.write(0x48);         // Copy Scratchpad
}

void dsconvertcommand(OneWire myds) {
  myds.reset();
  myds.write(0xCC, 1);        // send Skip ROOM command, next command will be for all devices, see datasheet
  myds.write(0x44, 1);        // send Convert T command, start conversion
}



float dsreadtemp(OneWire myds, byte addr[8]) {
  byte present = 0;
  int i;
  byte data[12];
  byte type_s;
  float celsius;

  switch (addr[0]) {
    //case 0x10:
      //for old DS18S20
      //type_s = 1;
      //break;
    case 0x28:
      //for DS18B20
      type_s = 0;
      break;
    default:
      Serial.println(F("Device is not a DS18x20 family device."));
  }

  present = myds.reset();
  myds.select(addr);
  myds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = myds.read();
  }
  
  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    } else {
      byte cfg = (data[4] & 0x60);
      if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
      // default is 12 bit resolution, 750 ms conversion time
    }
  }
  celsius = (float)raw / 16.0;
  return celsius;
}

void setup(void) {
  Serial.begin(SERIAL_BAUD);
  resolution = 10; // min 9 max 12, if you change this, make sure to change the delay in loop too
  dssetresolution(myds, dsaddr1, resolution);
  dssetresolution(myds, dsaddr2, resolution);
  dssetresolution(myds, dsaddr3, resolution);
  dssetresolution(myds, dsaddr4, resolution);
  Serial.println("S1,S2,S3,S4,t");
}

void loop(void) {
  starttime = millis();
  dsconvertcommand(myds);
  delay(500); // 94 for 9bit res. / 188 for 10bit res. / 375 for 11bit res. / 750 for 12bit res.
  Serial.print(dsreadtemp(myds, dsaddr1)-0.75); // some offset for an excentric DS18B20 unit
  Serial.print(",");
  Serial.print(dsreadtemp(myds, dsaddr2));
  Serial.print(",");
  Serial.print(dsreadtemp(myds, dsaddr3));
  Serial.print(",");
  Serial.print(dsreadtemp(myds, dsaddr4));
  Serial.print(",");
  Serial.println(elapsedtime);
  Serial.println();
  //delay(1000);
}
