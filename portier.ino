const char BASMALA[] = "بسم الله الرحمن الرحيم";

const char VERSION[] = "0.0.1";

#include <SoftwareSerial.h>

const unsigned char PHONE_MODULE_RX_PIN = 7;
const unsigned char PHONE_MODULE_TX_PIN = 8;
const size_t RESPONSE_BUFFER_SIZE = 1000;


SoftwareSerial phoneSerial(PHONE_MODULE_RX_PIN, PHONE_MODULE_TX_PIN);

void setup() {
  // SIM module begins at 115200 by default, however it is too fast for the arduino pins 7, 8.
  // We are going to downshift it in the main loop
  phoneSerial.begin(115200);
  Serial.begin(9600);
  delay(100);
}

void loop() {
  Serial.println();
  Serial.println(BASMALA);
  Serial.print("-- CCMN Gland - Portier v");
  Serial.println(VERSION);
  // Serial.println("Press any touch.");
  // while (Serial.available() == 0)
  //   ;                              // wait for user input
  // String s = Serial.readString();  // Read the inout to flush

  // Downgrade the baud rate and re-init the software serial
  sendCommand("AT+IPR=9600");
  phoneSerial.begin(9600);


  // while (1) {
  //   while (phoneSerial.available()) {
  //     Serial.write(phoneSerial.read());  //if Serial received data, output it via mySerial.
  //   }
  //   while (Serial.available()) {
  //     phoneSerial.write(Serial.read());  //if myserial received data, output it via Serial.
  //   }
  // }

  // Blocking call
  Serial.println("WAINTING FOR CALLS...");
  handleNewCall();

  // sendCommand("AT+CMGF=1");
  // sendCommand("AT+CMGS=\"+41782388448\"");
  // sendCommand("Nhabek barcha zinouba.");
  // phoneSerial.println(0x1A);
}

void readResponse() {
  Serial.println("Reading the response");
  char buf[RESPONSE_BUFFER_SIZE] = { '\0' };
  int i = 0;
  while (phoneSerial.available() > 0 && i < RESPONSE_BUFFER_SIZE) {
    buf[i] = phoneSerial.read();
    i++;
  }
  Serial.print(buf);
}

bool handleNewCall() {
  while (1) {
    while (phoneSerial.available()) {
      switch (analyse_response(2, "RING", "MISSED_CALL")) {
        case 1:
          Serial.println("NEW CALL RECEIVED !");
          if (answerCall()) {
            Serial.println("Answering a call.");
            if (!openDoor()) Serial.println("Failed to open the door");
            delay(2000);
            hangupCall();
          }
          break;
        case 2:
          Serial.println("Missed call.");
          break;
        default:
          Serial.println("Unexpected incomming message while waiting for new calls.");
      }
    }
  }

  return true;
}

bool answerCall() {
  phoneSerial.println("ATA");
  delay(1000);
  {
    String call = phoneSerial.readString();
    Serial.print(call);
  }
  return true;
}

// Send code #61
// AT+VTS
bool openDoor() {
  sendCommand("AT+VTS=\"#,6,1\"");
  return true;
}

void hangupCall() {
  phoneSerial.println("ATH");
}

void sendCommand(String cmd) {
  Serial.print("Sending the AT command: ");
  Serial.println(cmd);
  phoneSerial.println(cmd);
  delay(1000);
  readResponse();
  delay(1000);
}

unsigned char analyse_response(int n, ...) {
  va_list args;
  va_start(args, n);
  int ret = 0;

  if (phoneSerial.available() > 0) {
    String incomming = phoneSerial.readString();
    Serial.print("message received: ");
    Serial.println(incomming);

    for (int i = 1; i <= n; ++i) {
      if (strstr(incomming.c_str(), va_arg(args, char *)) != NULL) {
        // Returns the matching option's position
        ret = i;
        break;
      }
    }
  }

  va_end(args);

  // Nothing matches
  return ret;
}
