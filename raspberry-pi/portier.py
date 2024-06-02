import serial
import time
import logging

VERSION = "1.0"

PHONE_MODULE_PORT = '/dev/ttyUSB2'
PHONE_MODULE_BAUD = 115200

OPEN_DOOR_DELAY = 1
WAITING_RESPONSE_DELAY = 2
WAITING_LOOP_PERIOD = 2

logging.basicConfig(
    format='%(asctime)s %(levelname)-8s %(message)s',
    level=logging.INFO,
    datefmt='%Y-%m-%d %H:%M:%S')

phoneSerial = serial.Serial(
    PHONE_MODULE_PORT, PHONE_MODULE_BAUD, timeout=1)


def handle_new_call():
    while True:
        if phoneSerial.in_waiting > 0:
            response = analyse_response("RING")
            if response == 1:
                logging.info("New phone call received")
                if answer_call():
                    time.sleep(OPEN_DOOR_DELAY)
                    open_door()
                    time.sleep(OPEN_DOOR_DELAY)
                    hangup_call()
            else:
                logging.error(
                    "Unexpected incoming message while waiting for new calls.")
        time.sleep(WAITING_LOOP_PERIOD)


def answer_call():
    send_command("ATA")
    response = analyse_response("OK", "BEGIN")
    if response in (1, 2):
        logging.info("Call answered successfully")
        return True
    else:
        logging.error("Failed to answer call")
        return False


def open_door():
    logging.info("اللهم افتحهم أبواب رحمتك")
    send_command('AT+VTS="#,6,1"')
    response = analyse_response("OK")
    if response == 1:
        logging.info("Code sent successfully.")
        return True
    else:
        logging.error("Error sending door code")
        return False


def hangup_call():
    logging.info("ٱلسلام عليكم ورحمة ٱلله وبركاته")
    send_command("AT+CHUP")
    response = analyse_response("OK", "END")
    if response in (1, 2):
        logging.info("Call hang up successfully")
    else:
        logging.error("Failed to hang up call")


def send_command(cmd):
    phoneSerial.write((cmd + '\r').encode())


def analyse_response(*expected_responses):
    time.sleep(WAITING_RESPONSE_DELAY)
    incoming = phoneSerial.read(phoneSerial.in_waiting).decode()
    logging.debug(f"Message received: {incoming.strip()}")

    for i, option in enumerate(expected_responses, start=1):
        if option in incoming:
            logging.debug(f"Answer contains: {option}")
            return i
    return 0


if __name__ == "__main__":
    logging.info("بسم الله الرحمن الرحيم")
    logging.info(f"-- CCMN Gland - Portier v{VERSION}")
    while True:
        logging.info("Waiting for new phone calls...")
        handle_new_call()
