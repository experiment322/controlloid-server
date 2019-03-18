#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vjoy/public.h>
#include <vjoy/interface.h>

#define AXIS 0x01
#define BUTTON 0x02
#define BUFFER_SIZE 256
#define ARR_LENGTH(ARR) (sizeof(ARR) / sizeof(*ARR))
#define STR_FREE_SPACE(STR) (sizeof(STR) - strlen(STR) - 1)

struct controller_event {
    unsigned int type;
    unsigned int code;
    const char *name;
};

static const struct controller_event REGISTERED_EVENTS[] = {
        // left analog stick
        {.type=AXIS, .code=HID_USAGE_X, .name="ANALOG_LX"},
        {.type=AXIS, .code=HID_USAGE_Y, .name="ANALOG_LY"},
        // right analog stick
        {.type=AXIS, .code=HID_USAGE_RX, .name="ANALOG_RX"},
        {.type=AXIS, .code=HID_USAGE_RY, .name="ANALOG_RY"},
        // trigger buttons
        {.type=BUTTON, .code=7, .name="L1"},
        {.type=BUTTON, .code=5, .name="L2"},
        {.type=BUTTON, .code=8, .name="R1"},
        {.type=BUTTON, .code=6, .name="R2"},
        // action pad buttons
        {.type=BUTTON, .code=1, .name="TRIANGLE"},
        {.type=BUTTON, .code=3, .name="CROSS"},
        {.type=BUTTON, .code=4, .name="SQUARE"},
        {.type=BUTTON, .code=2, .name="CIRCLE"},
        // d-pad buttons
        {.type=BUTTON, .code=13, .name="UP"},
        {.type=BUTTON, .code=14, .name="DOWN"},
        {.type=BUTTON, .code=15, .name="LEFT"},
        {.type=BUTTON, .code=16, .name="RIGHT"},
        // menu buttons
        {.type=BUTTON, .code=9, .name="SELECT"},
        {.type=BUTTON, .code=10, .name="START"},
};

static unsigned int controller;

static unsigned int get_vjoy_device() {
    int vjd_no;
    GetNumberExistingVJD(&vjd_no);

    for (unsigned int i = 1; i <= vjd_no; ++i) {
        if (GetVJDStatus(i) == VJD_STAT_FREE &&
            GetVJDAxisExist(i, HID_USAGE_X) > 0 &&
            GetVJDAxisExist(i, HID_USAGE_Y) > 0 &&
            GetVJDAxisExist(i, HID_USAGE_RX) > 0 &&
            GetVJDAxisExist(i, HID_USAGE_RY) > 0 &&
            GetVJDButtonNumber(i) >= 16) {
            return i;
        }
    }

    return 0;
}

static int vjoy_configuration_ok() {
    if (!vJoyEnabled()) {
        fprintf(stderr, "error: vjoy driver not installed\n");
        return 0;
    }

    unsigned short DllVer, DrvVer;
    if (!DriverMatch(&DllVer, &DrvVer)) {
        fprintf(stderr, "error: vjoy driver (%04x) not matching library driver (%04x)\n", DllVer, DrvVer);
        return 0;
    }

    unsigned int vjd_id = get_vjoy_device();
    if (!vjd_id) {
        fprintf(stderr, "error: no vjoy device available or none with valid configuration (16 BTN, X, Y, RX, RY)\n");
        return 0;
    }

    return 1;
}

static void controller_emit_event(int event_index, short int event_value) {
    unsigned int event_type = REGISTERED_EVENTS[event_index].type;
    unsigned int event_code = REGISTERED_EVENTS[event_index].code;

    switch (event_type) {
        case AXIS:
            SetAxis(((long int) event_value + 32767) / 2, controller, event_code);
            return;
        case BUTTON:
            SetBtn(event_value ? 1 : 0, controller, (unsigned char) event_code);
            return;
        default:
            return;
    }
}

static void send_events_mapping() {
    char json_entry[BUFFER_SIZE] = {0};
    char json_data[BUFFER_SIZE * ARR_LENGTH(REGISTERED_EVENTS)] = {0};

    strncat(json_data, "{", STR_FREE_SPACE(json_data));
    for (int i = 0; i < ARR_LENGTH(REGISTERED_EVENTS); ++i) {
        snprintf(json_entry, sizeof(json_entry), "%s\"%s\":%d", i ? "," : "", REGISTERED_EVENTS[i].name, i);
        strncat(json_data, json_entry, STR_FREE_SPACE(json_data));
    }
    strncat(json_data, "}", STR_FREE_SPACE(json_data));

    fputs(json_data, stdout);
}

static void receive_events() {
    short int event_index;
    short int event_value;
    for (;;) {
        if (ferror(stdin) || feof(stdin)) {
            break;
        }

        if (fread(&event_index, sizeof(event_index), 1, stdin) != 1 ||
            fread(&event_value, sizeof(event_value), 1, stdin) != 1) {
            continue;
        }

        if (event_index >= 0 && event_index < ARR_LENGTH(REGISTERED_EVENTS)) {
            controller_emit_event(event_index, event_value);
        }
    }
}

int main(void) {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    _setmode(_fileno(stdin), _O_BINARY);

    if (getenv("REMOTE_ADDR") == NULL) {
        fprintf(stderr, "error: environment variable REMOTE_ADDR not found\n");
        return -1;
    }

    if (!vjoy_configuration_ok()) {
        return -1;
    }

    controller = get_vjoy_device();
    if (!AcquireVJD(controller)) {
        fprintf(stderr, "error: failed to create virtual controller\n");
        return -1;
    }

    send_events_mapping();
    receive_events();

    RelinquishVJD(controller);
    ResetVJD(controller);

    return 0;
}
