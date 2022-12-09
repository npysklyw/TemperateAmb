#define TIMER_A9_BASE 0xFFFEC600

// Structure for the timer
typedef struct _a9_timer
{
    int load;
    int count;
    int control;
    int status;
} a9_timer;

// pointers to hardware
volatile a9_timer *const timer_1 = (a9_timer *)TIMER_A9_BASE;

// Initialize the timer
void initTimer()
{
    // interval for counting 5 s
    int interval = 1000000000;
    // Set inteval
    // initialize timer for 5 s interval // assumes 100 MHz rate
    timer_1->load = interval;
    // start timer for continuous counting // 3 is (0b011) for control
    // (1 << 8) is for prescaler
    timer_1->control = 3 + (1 << 8);
    timer_1->status = 1;
}

// Check if the timer has timed out
void checkTimeOut()
{

    // If a timeout occurs
    if (timer_1->status == 1)
    {
        // Do the tings
        // timer_1->status = 1;
        // timer_1->control = 0;
        initTimer();
    }
}


void main()
{

    // blank out LEDs
    initTimer();
    // main loop
    while (1)
    {
        checkTimeOut();
    }
}