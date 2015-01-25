#include <AFMotor.h>

AF_DCMotor motor2(2, MOTOR12_1KHZ); // create motor #2, 64KHz pwm
AF_DCMotor motor3(3, MOTOR12_1KHZ);

int momentarySwitch = 0; //holds voltage reading of momentary switch
unsigned int fanTimerFifthSecs = 0; //5ths of a second remaining til fan turns off
unsigned int fanHumTimerFifthSecs = 0;
int state = 0; //hold what state we're in in the loop
int fanState = 0;
int buttonHoldFifthSecs = 0; // number of tenths of seconds button has been held down

void setup() {
    motor2.setSpeed(255);     // set the speed to max to eliminate hum
    motor3.setSpeed(255); 
}

void start_fans() {
    motor2.run(FORWARD);      // turn it on going forward
    motor3.run(FORWARD);
}

void stop_fans() {
    motor2.run(RELEASE);      // stop
    motor3.run(RELEASE);
}

void loop() {
    momentarySwitch = analogRead(A0);

    switch (state) {
        case 0: //looking for button press
            if (momentarySwitch > 100) {
                state = 1;
                buttonHoldFifthSecs = 1;
            }
            break;

        case 1: //button was pressed, wait until unpressed
            if (momentarySwitch < 100) {
                state = 2;
                buttonHoldFifthSecs++;
            }
            else if (buttonHoldFifthSecs >= 10) {
                //button has been held for 2 seconds, turn off the fans then wait for a release
                state = 3;
            }
            else {
                buttonHoldFifthSecs++;
            }
            break;

        case 2: //button was released in less than 2 seconds, turn the fans on longer!
            fanTimerFifthSecs += 30*60*5; //30 minutes * 60 secs/mins * 5 fifths

            //about 3 button presses/90 mins
            if (fanTimerFifthSecs > 30*60*5 * 3) {
                fanTimerFifthSecs = 30*60*5 * 3; //put 90 minutes on the clock
            }

            state = 0; //go back to start!
            break;

        case 3: //button was held for 2 seconds, turn off fans and wait for button to be released
            fanTimerFifthSecs = 0; //stop fans
            if (momentarySwitch < 100)
                state = 0; //go back to start!
            break;
    }
    
    switch (fanState) {
        case 0: //fan off, waiting for turn on condition
            if (fanTimerFifthSecs > 0) {
                motor2.setSpeed(220);     // set the speed slower to make some hum
                motor3.setSpeed(220); 
                fanHumTimerFifthSecs = 10; //hum the fans for 2 seconds
                start_fans();

                fanState = 1;
            }
            break;

        case 1: //fans spinning up, humming
            if (fanHumTimerFifthSecs <= 0) {
                motor2.setSpeed(255);     // set the speed make to stop hum
                motor3.setSpeed(255); 
                fanState = 2;
            }
            else {
                fanHumTimerFifthSecs--;
            }
            break;

        case 2: //fans running at full speed, waiting for stop condition
            if (fanTimerFifthSecs <= 0) {
                stop_fans();
                fanState = 0;
            }
            else {
                fanTimerFifthSecs--; //decrement fan on counter
            }

    }

    
    
    delay(200); //run loop 5 times per second
}
