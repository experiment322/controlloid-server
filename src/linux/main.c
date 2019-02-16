#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

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
        {.type=EV_ABS, .code=ABS_X, .name="ANALOG_LX"},
        {.type=EV_ABS, .code=ABS_Y, .name="ANALOG_LY"},
        // right analog stick
        {.type=EV_ABS, .code=ABS_RX, .name="ANALOG_RX"},
        {.type=EV_ABS, .code=ABS_RY, .name="ANALOG_RY"},
        // trigger buttons
        {.type=EV_KEY, .code=BTN_TL, .name="L1"},
        {.type=EV_KEY, .code=BTN_TL2, .name="L2"},
        {.type=EV_KEY, .code=BTN_TR, .name="R1"},
        {.type=EV_KEY, .code=BTN_TR2, .name="R2"},
        // action pad buttons
        {.type=EV_KEY, .code=BTN_NORTH, .name="TRIANGLE"},
        {.type=EV_KEY, .code=BTN_SOUTH, .name="CROSS"},
        {.type=EV_KEY, .code=BTN_WEST, .name="SQUARE"},
        {.type=EV_KEY, .code=BTN_EAST, .name="CIRCLE"},
        // d-pad buttons
        {.type=EV_KEY, .code=BTN_DPAD_UP, .name="UP"},
        {.type=EV_KEY, .code=BTN_DPAD_DOWN, .name="DOWN"},
        {.type=EV_KEY, .code=BTN_DPAD_LEFT, .name="LEFT"},
        {.type=EV_KEY, .code=BTN_DPAD_RIGHT, .name="RIGHT"},
        // menu buttons
        {.type=EV_KEY, .code=BTN_SELECT, .name="SELECT"},
        {.type=EV_KEY, .code=BTN_START, .name="START"},
};

static struct libevdev_uinput *controller = NULL;

static struct libevdev_uinput *controller_create() {
    struct libevdev *device = libevdev_new();

    char controller_name[BUFFER_SIZE] = {0};
    strncat(controller_name, "controlloid-", STR_FREE_SPACE(controller_name));
    strncat(controller_name, getenv("REMOTE_ADDR"), STR_FREE_SPACE(controller_name));
    libevdev_set_name(device, controller_name);

    struct input_absinfo absinfo = {
            .flat = 0,
            .fuzz = 0,
            .value = 0,
            .minimum = -32767,
            .maximum = +32767,
            .resolution = 0,
    };
    for (int i = 0; i < ARR_LENGTH(REGISTERED_EVENTS); ++i) {
        unsigned int event_type = REGISTERED_EVENTS[i].type;
        unsigned int event_code = REGISTERED_EVENTS[i].code;
        libevdev_enable_event_code(device, event_type, event_code, event_type == EV_ABS ? &absinfo : NULL);
    }

    struct libevdev_uinput *controller = NULL;
    libevdev_uinput_create_from_device(device, LIBEVDEV_UINPUT_OPEN_MANAGED, &controller);

    libevdev_free(device);

    return controller;
}

static void controller_emit_event(struct libevdev_uinput *controller, int event_index, short int event_value) {
    unsigned int event_type = REGISTERED_EVENTS[event_index].type;
    unsigned int event_code = REGISTERED_EVENTS[event_index].code;
    libevdev_uinput_write_event(controller, event_type, event_code, event_value);
    libevdev_uinput_write_event(controller, EV_SYN, SYN_REPORT, 0);
}

static void controller_destroy(struct libevdev_uinput *controller) {
    libevdev_uinput_destroy(controller);
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
        fread(&event_index, sizeof(event_index), 1, stdin);
        fread(&event_value, sizeof(event_value), 1, stdin);

        if (ferror(stdin)) break;

        if (event_index >= 0 && event_index < ARR_LENGTH(REGISTERED_EVENTS)) {
            controller_emit_event(controller, event_index, event_value);
        }
    }
}

static void int_handler(int signal) {
    fprintf(stderr, "signal(%d): halting execution...\n", signal);
    controller_destroy(controller);
    fclose(stdout);
    fclose(stdin);

    exit(EXIT_SUCCESS);
}

int main() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    signal(SIGINT, int_handler);

    if (getenv("REMOTE_ADDR") == NULL) {
        fputs("error: environment variable REMOTE_ADDR not found\n", stderr);
        exit(EXIT_FAILURE);
    }

    controller = controller_create();
    if (controller == NULL) {
        fputs("error: failed to create virtual controller\n", stderr);
        exit(EXIT_FAILURE);
    }

    send_events_mapping();
    receive_events();

    return 0;
}
