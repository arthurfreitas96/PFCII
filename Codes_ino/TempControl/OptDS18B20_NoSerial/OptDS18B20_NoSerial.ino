#include <OneWire.h>

OneWire myds(2);
float temp[4];
byte resolution;
uint8_t dsaddr1[8] = { 0x28, 0xD4, 0x2D, 0x79, 0xA2, 0x01, 0x03, 0xB6 }; // defining adresses manually
uint8_t dsaddr2[8] = { 0x28, 0xA2, 0xA5, 0x79, 0xA2, 0x00, 0x03, 0xAD };
uint8_t dsaddr3[8] = { 0x28, 0xA6, 0xED, 0x79, 0xA2, 0x01, 0x03, 0x62 };
uint8_t dsaddr4[8] = { 0x28, 0x0B, 0x84, 0x79, 0xA2, 0x00, 0x03, 0x52 };

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
  pinMode(3,OUTPUT);
}

void loop(void) {
  dsconvertcommand(myds);
  delay(500); // 94 for 9bit res. / 188 for 10bit res. / 375 for 11bit res. / 750 for 12bit res.
  temp[0] = dsreadtemp(myds, dsaddr1)-0.75;
  temp[1] = dsreadtemp(myds, dsaddr2);
  temp[2] = dsreadtemp(myds, dsaddr3);
  temp[3] = dsreadtemp(myds, dsaddr4);
  if(temp[0] >= 28 || temp[1] >= 28 || temp[2] >= 28 || temp[3] >= 28){
    digitalWrite(3,HIGH);
  }
  else{
    digitalWrite(3,LOW);}
}
