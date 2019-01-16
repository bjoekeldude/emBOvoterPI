#include <stdio.h>
#include <sys/time.h>
#include <wiringPi.h>

// Which GPIO pin we're using
#define GREATPIN 27
#define BADPIN 25
// How much time a change must be since the last in order to count as a change
#define IGNORE_CHANGE_BELOW_USEC 10000

// Current state of the pin
static volatile int greatstate;
static volatile int badstate;
// Time of last change
struct timeval last_change;

void PrintTime ()
{
    struct timeval tv;
    struct tm* ptm;
    char time_string[40];
    long milliseconds;

    /* Obtain the time of day, and convert it to a tm struct. */
    gettimeofday (&tv, NULL);
    ptm = localtime (&tv.tv_sec);
    /* Format the date and time, down to a single second. */
    strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
    /* Compute milliseconds from microseconds. */
    milliseconds = tv.tv_usec / 1000;
    /* Print the formatted time, in seconds, followed by a decimal point
      and the milliseconds. */
    printf ("%s.%03ld\n", time_string, milliseconds);
}

// Handler for interrupt
void great(void) {
    struct timeval now;
    unsigned long diff;

    gettimeofday(&now, NULL);

    // Time difference in usec
    diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);

    // Filter jitter
    if (diff > IGNORE_CHANGE_BELOW_USEC) {
        PrintTime();
        printf("- This was good!\n");
        greatstate = !greatstate;
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
        PrintTime();
        printf("- This was bad!\n");
        badstate = !badstate;
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
    wiringPiISR(GREATPIN, INT_EDGE_FALLING, &great);
    wiringPiISR(BADPIN, INT_EDGE_FALLING, &bad);

    // Get initial state of pin
    greatstate  =   digitalRead(GREATPIN);
    badstate    =   digitalRead(BADPIN);


    printf("Started!\n "
           "initial greatstate is %d\n"
           "initial  badstate  is %d\n",greatstate,badstate);

    // Waste time but not CPU
    for (;;) {
        nanosleep(1000);
    }
}