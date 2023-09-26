/*
Copyright 2017 Ben Eater
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
*/

#define EEPROM_A0 A0
#define EEPROM_A5 A5
#define EEPROM_D0 2
#define EEPROM_D7 9
#define WRITE_EN 11
#define OUTPUT_EN 12

/*
 * Output the address bits and outputEnable signal.
 */
void setAddress(int address, bool outputEnable) {
  if(outputEnable == true){
    digitalWrite(OUTPUT_EN, LOW);
  }
  byte baddress = (byte) address;
  for (int pin = EEPROM_A0; pin <= EEPROM_A5; pin += 1) {
    digitalWrite(pin, baddress & 1);
    baddress = baddress >> 1;
  }
  if(outputEnable == false){
    digitalWrite(OUTPUT_EN, HIGH);
  }
}


/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */

void printContents(){
  for (int address = 0; address < 64; address++){
    if(address == 16 || address == 32 || address == 48 ){
      Serial.println();
    }
    Serial.print(address); Serial.print(": ");
    byte val = readEEPROM(address);
    for (int i = 0; i < 8; i++){
        bool b = val & 0x80;
        Serial.print(b);
        val = val << 1;
    }
    Serial.println();
  }
}


// Data
// byte data[] = {
// P1/decoded comands               
// P2
// P3
// P4
//};


byte data[] = { 
//P1
B11111111, B00000001, B01000100, B10000011, B01100010, B10000011,B11111111, B11111111,
B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
//P2
B00000000, B01000001, B10000011, B11100000, B11010001, B11111111, B11111111, B11111111,
B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
//P3
B11111111, B00000001, B10100001, B11111111, B11000001, B00011111, B01000001, B10100001,
B11010011, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
//P4
B00000000, B10100000, B11111111, B00000101, B10100001, B11000000, B00011111, B11000001,
B01011111, B10100000, B11000001, B00011111, B11100000, B01100001, B10100001, B11010101
};


void setup() {
  // put your setup code here, to run once:
  for (int pin = EEPROM_A0; pin <= EEPROM_A5; pin += 1) {
    pinMode(pin, OUTPUT);
  }
  digitalWrite(OUTPUT_EN, LOW);
  pinMode(OUTPUT_EN, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);


  // Erase entire EEPROM
  Serial.print("Erasing EEPROM");
  for (int address = 0; address < 64; address += 1) {
    writeEEPROM(address, 0xff);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");


  // Program data bytes
  Serial.print("Programming EEPROM");
  for (int address = 0; address < sizeof(data); address += 1) {
    writeEEPROM(address, data[address]);
      Serial.print(".");
  }
  Serial.println(" done");


  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
}


void loop() {
  // put your main code here, to run repeatedly:

}
