MSG_COMMAND = 2

CMD = {
    "ping": 1,
    "memory dump": 2,
    "set led": 3,
    "set telemetry": 4,
    "set debug mode": 5,
    "set mode": 6,
    "set motor": 7,
    "set max speed": 8,
    "set qti threshold": 9,
    "change robot id": 10,

    "set target": 100,

    "data dump": 200,
    "read qti": 201,
    "read battery": 202,
    "ultra sweep": 203,
    "ultrasound": 204,
    "rfid value": 205,
    "motor value": 206,
}

MODE = {
    "run": 0,
    "test": 1,
    "idle": 2,
}

robot_id = 1

def onoff(value: str) -> int:
    if value.lower() in ("off", "0", "false"):
        return 0
    else:
        return 1


def translate_command(text: str, ) -> str | None:
    global robot_id 

    text = text.strip().lower()
    if not text:
        return None

    if ";" in text:
        return text

    parts = text.split()

    try:
        if text == "ping":
            return f"{robot_id};{MSG_COMMAND};{CMD['ping']};0"

        if text == "memory dump":
            return f"{robot_id};{MSG_COMMAND};{CMD['memory dump']};0"

        if text == "data dump":
            return f"{robot_id};{MSG_COMMAND};{CMD['data dump']};0"

        if text == "read qti":
            return f"{robot_id};{MSG_COMMAND};{CMD['read qti']};0"

        if text == "read battery":
            return f"{robot_id};{MSG_COMMAND};{CMD['read battery']};0"

        if text == "ultra sweep":
            return f"{robot_id};{MSG_COMMAND};{CMD['ultra sweep']};0"

        if text == "ultrasound":
            return f"{robot_id};{MSG_COMMAND};{CMD['ultrasound']};0"

        if text == "rfid value":
            return f"{robot_id};{MSG_COMMAND};{CMD['rfid value']};0"

        if text == "motor value":
            return f"{robot_id};{MSG_COMMAND};{CMD['motor value']};0"

        if text.startswith("set target"):
            value = int(parts[-1])
            if 1 <= value <= 255:
                robot_id = value
                return f"LOCAL:{robot_id};{MSG_COMMAND};{CMD['set target']};{value}"
        
        if text.startswith("set telemetry"):
            value = onoff(parts[-1])
            return f"{robot_id};{MSG_COMMAND};{CMD['set telemetry']};{value}"

        if text.startswith("set debug mode"):
            value = onoff(parts[-1])
            return f"{robot_id};{MSG_COMMAND};{CMD['set debug mode']};{value}"

        if text.startswith("set mode"):
            mode = parts[-1]
            if mode in MODE:
                return f"{robot_id};{MSG_COMMAND};{CMD['set mode']};{MODE[mode]}"

        if text.startswith("set led"):
            led = int(parts[2])
            state = onoff(parts[3])

            if 1 <= led <= 127:
                value = (led << 1) | state
                return f"{robot_id};{MSG_COMMAND};{CMD['set led']};{value}"

        if text.startswith("set motor"):
            value = int(parts[2])
            value2 = int(parts[3])
            value3 = int(parts[4])
            if 1300 <= value <= 1700 and 1300 <= value2 <= 1700 and 1 <= value3 <= 65535:
                return f"{robot_id};{MSG_COMMAND};{CMD['set motor']};{value};{value2};{value3}"

        if text.startswith("set max speed"):
            value = int(parts[3])
            if 0 <= value <= 200:
                return f"{robot_id};{MSG_COMMAND};{CMD['set max speed']};{value}"

        if text.startswith("set qti threshold"):
            value = int(parts[3])
            if 0 <= value <= 1023:
                return f"{robot_id};{MSG_COMMAND};{CMD['set qti threshold']};{value}"

        if text.startswith("change robot id"):
            new_id = int(parts[3])
            if 0 < new_id <= 255:
                packet = f"{robot_id};{MSG_COMMAND};{CMD['change robot id']};{new_id}"
                robot_id = new_id
                return packet

        if text == "quit":
            return "QUIT"

    except:
        return None

    return None