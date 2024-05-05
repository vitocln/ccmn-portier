#include <SoftwareSerial.h>

const char BASMALA[] = "بسم الله الرحمن الرحيم";
const char VERSION[] = "1.0";

const unsigned char PHONE_MODULE_RX_PIN = 7;
const unsigned char PHONE_MODULE_TX_PIN = 8;

const unsigned long ASNWERING_CALL_DELAY = 2000;
const unsigned long CODE_TYPING_DELAY = 200;

SoftwareSerial phoneSerial(PHONE_MODULE_RX_PIN, PHONE_MODULE_TX_PIN);

void setup() {
  // SIM module begins at 115200 bauds by default. However it's too fast for the arduino's pins 7 and 8.
  // We are going to downshift it in the main loop
  phoneSerial.begin(115200);
  Serial.begin(9600);
}

void loop() {
  Serial.println();
  Serial.println(BASMALA);
  Serial.print("-- CCMN Gland - Portier v");
  Serial.println(VERSION);

  // Downgrade the baud rate and re-init the software serial
  sendCommand("AT+IPR=9600");
  phoneSerial.begin(9600);

  phoneCallLoop();
}

bool phoneCallLoop() {
  while (1) {
    Serial.println("Waiting for new phone calls...");
    while (phoneSerial.available()) {
      switch (analyse_response(2, "RING", "MISSED_CALL")) {
        case 1:
          Serial.println("New phone call received");
          if (answerCall()) {
            delay(ASNWERING_CALL_DELAY);
            if (!openDoor()) Serial.println("Failed to open the door");
            delay(ASNWERING_CALL_DELAY);
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
  bool res = false;
  sendCommand("ATA");
  switch (analyse_response(3, "CONNECT", "OK", "CARRIER")) {
    case 1:
      Serial.println("Data call OK");
      res = true;
      break;
    case 2:
      Serial.println("Voice call OK");
      res = true;
      break;
    case 3:
      Serial.println("Error no connection");
      break;
    default:
      Serial.println("Unexpected response while answering call");
  }
  return res;
}

// Send code #61
bool openDoor() {
  bool res = false;
  sendCommand("AT+VTS=#");
  delay(CODE_TYPING_DELAY);
  sendCommand("AT+VTS=6");
  delay(CODE_TYPING_DELAY);
  sendCommand("AT+VTS=1");
  switch (analyse_response(2, "OK", "ERROR")) {
    case 1:
      Serial.println("Code #61 sent successfully.");
      res = true;
      break;
    case 2:
      Serial.println("Error door code");
      break;
    default:
      Serial.println("Unexpected response while opening the door");
  }
  return res;
}

void hangupCall() {
  sendCommand("ATH");
  switch (analyse_response(1, "OK")) {
    case 1:
      Serial.println("Hang up call OK");
      break;
    default:
      Serial.println("Unexpected response while hanging up the call");
  }
}

void sendCommand(String cmd) {
  Serial.print("Sending the AT command: ");
  Serial.println(cmd);
  phoneSerial.println(cmd);
}

/// @brief  Read the Serial Softwre input, compare it to the expected provided list of response and return the matching response's position.
/// @param n nomber of arguments provided
/// @param ... Unlimited amount of exepcted responses
/// @return matching response's position (starting from 1). 0 means nothing matches.
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
  return ret;
}
