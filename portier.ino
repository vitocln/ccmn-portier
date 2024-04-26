#include <SoftwareSerial.h>
SoftwareSerial mySerial(7, 8);
char msg;
char call;
const size_t RESPONSE_BUFFER_SIZE = 1000;

void setup() {
  mySerial.begin(115200);  // Setting the baud rate of GSM Module
  Serial.begin(115200);    // Setting the baud rate of Serial Monitor (Arduino)
  delay(100);
}

void loop() {
  Serial.println("Press any touch.");
  while (Serial.available() == 0)
    ;                              // wait for user input
  String s = Serial.readString();  // Read the inout to flush

  // SendCommand("AT+CGMI");
  // SendCommand("AT+CGMN");
  // SendCommand("AT+CGMR");
  // SendCommand("AT+CGSN");
  // SendCommand("ATA");
  // SendCommand("AT+CSQ");
  // SendCommand("AT+CCID");
  // SendCommand("AT+CREG?");
  // SendCommand("AT+CREG=2");
  // SendCommand("AT+CREG?");

  SendCommand("AT+CMGF=1");
  SendCommand("AT+CMGS=\"+41782388448\"");
  SendCommand("Nhabek barcha zinouba.");
  mySerial.println(0x1A);
}

void ReadResponse() {
  Serial.println("DBG -- Read the response");
  char buf[RESPONSE_BUFFER_SIZE] = { '\0' };
  int i = 0;
  while (mySerial.available() > 0 && i < RESPONSE_BUFFER_SIZE) {
    buf[i] = mySerial.read();
    i++;
  }
  Serial.print(buf);
}

void SendCommand(String cmd) {
  Serial.println("DBG -- Sending the AT command: ");
  Serial.println(cmd);
  mySerial.println(cmd);
  delay(1000);
  ReadResponse();
  delay(1000);
}
