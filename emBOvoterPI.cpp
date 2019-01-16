#include <stdio.h>
#include <sys/time.h>
#include <wiringPi.h>

// Which GPIO pin we're using
#define GREATPIN 8
#define BADPIN 25
// How much time a change must be since the last in order to count as a change
#define IGNORE_CHANGE_BELOW_USEC 10000

// Current state of the pin
static volatile int state;
// Time of last change
struct timeval last_change;

// Handler for interrupt
void great(void) {
    struct timeval now;
    unsigned long diff;

    gettimeofday(&now, NULL);

    // Time difference in usec
    diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);

    // Filter jitter
    if (diff > IGNORE_CHANGE_BELOW_USEC) {
        if (state) {
            printf("Falling\n");
        }
        else {
            printf("Rising\n");
        }

        state = !state;
    }

    last_change = now;
}

void bad(void) {
    struct timeval now;
    unsigned long diff;

    gettimeofday(&now, NULL);

    // Time difference in usec
    diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);

    // Filter jitter
    if (diff > IGNORE_CHANGE_BELOW_USEC) {
        if (state) {
            printf("Falling\n");
        }
        else {
            printf("Rising\n");
        }

        state = !state;
    }

    last_change = now;
}


int main(void) {
    // Init
    wiringPiSetup();
    // Set pin to output in case it's not
    pinMode(GREATPIN, OUTPUT);
    pinMode(BADPIN, OUTPUT);


    // Time now
    gettimeofday(&last_change, NULL);

    // Bind to interrupt
    wiringPiISR(GREATPIN, INT_EDGE_BOTH, &great);
    wiringPiISR(BADPIN, INT_EDGE_BOTH, &bad);

    // Get initial state of pin
    state = digitalRead(PIN);

    if (state) {
        printf("Started! Initial state is on\n");
    }
    else {
        printf("Started! Initial state is off\n");
    }

    // Waste time but not CPU
    for (;;) {
        sleep(1);
    }
}