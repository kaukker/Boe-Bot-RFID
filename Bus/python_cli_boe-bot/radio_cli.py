import serial
import threading
import time
import os

from command_parser import translate_command
from bus_logic import Bus, format_bus_report

SERIAL_PORT = "COM4"
BAUD_RATE = 9600
LOG_FILE = "radio_log.csv"

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)

log_lock = threading.Lock()
bus = Bus()

if not os.path.exists(LOG_FILE):
    with open(LOG_FILE, "w", encoding="utf-8") as f:
        f.write("sep=,\n")
        f.write("timestamp,direction,data\n")


def log(direction, message):
    timestamp = time.time()
    safe_message = str(message).replace("\n", " ").replace("\r", " ")

    print(f"{timestamp},{direction},{safe_message}")

    with log_lock:
        with open(LOG_FILE, "a", encoding="utf-8") as f:
            f.write(f"{timestamp},{direction},{safe_message}\n")


def reader():
    while True:
        try:
            line = ser.readline().decode("utf-8", errors="replace").strip()
            if line:
                log("RX", line)

                bus_result = bus.process_bus_arrival(line)
                if bus_result is not None:
                    print(format_bus_report(bus_result))
        except Exception as e:
            log("ERROR", f"{e}")
            break

def writer():
    while True:
        try:
            user_text = input().strip()
            if not user_text:
                continue

            packet = translate_command(user_text)

            if packet is None:
                print("Unknown command")
                continue

            if packet == "QUIT":
                break

            if packet.startswith("LOCAL:"):
                packet = packet[len("LOCAL:"):]
                log("LOCAL", packet)
                continue

            ser.write((packet + "\n").encode("utf-8"))
            log("TX", packet)

        except Exception as e:
            log("ERROR", f"{e}")
            break


t = threading.Thread(target=reader, daemon=True)
t.start()

writer()
ser.close()