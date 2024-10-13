import serial
import time
import logging
from datetime import datetime
from pathlib import Path

VERSION = "1.1"
PHONE_MODULE_PORT = '/dev/ttyUSB2'
PHONE_MODULE_BAUD = 115200

OPEN_DOOR_DELAY = 1
WAITING_RESPONSE_DELAY = 2
WAITING_LOOP_PERIOD = 2
CALL_RENEW_PERIOD_IN_DAYS = 90
LAST_CALL_FILE = Path.home() / '.portier_last_call_date'
PHONE_NUMBER = "+41800700700" # Salt Mobile Support
CALL_DURATION = 5

logging.basicConfig(
    format='%(asctime)s %(levelname)-8s %(message)s',
    level=logging.INFO,
    datefmt='%Y-%m-%d %H:%M:%S')

phoneSerial = serial.Serial(
    PHONE_MODULE_PORT, PHONE_MODULE_BAUD, timeout=1)


def get_device_time():
    send_command("AT+CCLK?")
    time.sleep(WAITING_RESPONSE_DELAY)
    response = phoneSerial.read(phoneSerial.in_waiting).decode()
    logging.debug(f"reponse of AT+CCLK?: {response}")
    if response:
        # Example: +CCLK: "24/10/13,15:36:06+08" \n OK
        date_str = response.split('"')[1].split(',')[0]
        logging.debug(f"date_str after split manipulation: {date_str}")
        device_time = datetime.strptime(date_str, '%y/%m/%d')
        logging.info(f"Current device time from phone module: {device_time}")
        return device_time
    else:
        logging.error("Failed to retrieve time from device.")
        return None


def make_periodic_call():
    last_call_date = load_last_call_date()
    logging.info(f"The last refresh call date was the {last_call_date}")
    current_time = get_device_time()

    if last_call_date and current_time:
        # Check if 3 months have passed
        if (current_time - last_call_date).days >= CALL_RENEW_PERIOD_IN_DAYS:
            logging.warning("3 MONTHS HAVE PASSED, MAKING THE PERIODIC CALL TO REFRESH THE SIM...")
            if make_call():
                store_last_call_date(current_time)
        else:
            logging.info("Less than 3 months have passed since the last call.")
    elif current_time:
        logging.info("No previous call date found. Making initial call...")
        if make_call():
            store_last_call_date(current_time)


def load_last_call_date():
    try:
        with open(LAST_CALL_FILE, 'r') as file:
            date_str = file.read().strip()
            return datetime.strptime(date_str, '%Y-%m-%d')
    except FileNotFoundError:
        logging.info("No last call date found, making the first call.")
        return None
    except Exception as e:
        logging.error(f"Error reading last call date: {e}")
        return None


def store_last_call_date(date):
    try:
        with open(LAST_CALL_FILE, 'w') as file:
            file.write(date.strftime('%Y-%m-%d'))
        logging.info(f"Stored last call date: {date}")
    except Exception as e:
        logging.error(f"Error storing last call date: {e}")


def make_call():
    logging.info(f"Calling {PHONE_NUMBER}...")
    send_command(f"ATD{PHONE_NUMBER};")
    response = analyse_response("BEGIN", delay=5)
    if response == 1:
        logging.info(f"{PHONE_NUMBER} has answered the call. Wait 5 seconds now...")
        time.sleep(CALL_DURATION)
        logging.info("hang up")
        hangup_call()
        return True
    else:
        logging.error("Failed to send call command")

    return False


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


def analyse_response(*expected_responses, delay=WAITING_RESPONSE_DELAY):
    time.sleep(delay)
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

    make_periodic_call()

    while True:
        logging.info("Waiting for new phone calls...")
        handle_new_call()
