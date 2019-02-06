import atexit
import json
import struct
from os import environ
from sys import stdin, stdout

import uinput

EVENTS = [
    # action pad buttons
    'BTN_NORTH',
    'BTN_WEST',
    'BTN_SOUTH',
    'BTN_EAST',
    # d-pad buttons
    'BTN_DPAD_UP',
    'BTN_DPAD_LEFT',
    'BTN_DPAD_DOWN',
    'BTN_DPAD_RIGHT',
    # left analog stick
    'ABS_X',
    'ABS_Y',
    # right analog stick
    'ABS_RX',
    'ABS_RY',
    # trigger buttons
    'BTN_TL',
    'BTN_TR',
    'BTN_TL2',
    'BTN_TR2',
    # menu buttons
    'BTN_SELECT',
    'BTN_START',
    'BTN_MODE',
]
REGISTERED_EVENTS = {event: vars(uinput.ev)[event] for event in EVENTS}
EVENT_INT16_MAP = {event: EVENTS.index(event) for event in EVENTS}
INT16_EVENT_MAP = {EVENTS.index(event): event for event in EVENTS}
DEVICE_PREFIX = "controlloid"
BUS_VIRTUAL = 0x06


def get_device_name(addr):
    return "-".join([DEVICE_PREFIX, addr])


def device_consume_message(device, message):
    for i in range(len(message) // 4):
        ev_int16, ev_value = struct.unpack('hh', message[i * 4:i * 4 + 4])
        ev_code = REGISTERED_EVENTS[INT16_EVENT_MAP[ev_int16]]
        device.emit(ev_code, ev_value, syn=False)
    device.syn()


def socket_read_loop(device):
    stdout.buffer.write(json.dumps(EVENT_INT16_MAP).encode())
    stdout.buffer.flush()
    while True:
        message = stdin.buffer.read1()
        device_consume_message(device, message)


def ws_handler():
    addr = environ['REMOTE_ADDR']
    device = uinput.Device(REGISTERED_EVENTS.values(), get_device_name(addr), BUS_VIRTUAL)
    atexit.register(device.destroy)
    socket_read_loop(device)


if __name__ == '__main__':
    try:
        ws_handler()
    except KeyboardInterrupt:
        pass
