#include <AFMotor.h>

AF_DCMotor motor2(2, MOTOR12_1KHZ); // create motor #2, 64KHz pwm
AF_DCMotor motor3(3, MOTOR12_1KHZ);

int momentarySwitch = 0; //holds voltage reading of momentary switch
int fanTimerTenthSecs = 0; //10ths of a second remaining til fan turns off
int state = 0; //hold what state we're in in the loop
int buttonHoldTenthSecs = 0; // number of tenths of seconds button has been held down

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
                buttonHoldTenthSecs = 1;
            }
            break;

        case 1: //button was pressed, wait until unpressed
            if (momentarySwitch < 100) {
                state = 2;
                buttonHoldTenthSecs++;
            }
            else if (buttonHoldTenthSecs >= 20) {
                //button has been held for 2 seconds, turn off the fans then wait for a release
                state = 3;
            }
            else {
                buttonHoldTenthSecs++;
            }
            break;

        case 2: //button was released in less than 2 seconds, turn the fans on longer!
            fanTimerTenthSecs += 15*60*10; //15 minutes * 60 secs/mins * 10 tenths
            state = 0; //go back to start!
            break;

        case 3: //button was held for 2 seconds, turn off fans and wait for button to be released
            fanTimerTenthSecs = 0; //stop fans
            if (momentarySwitch < 100)
                state = 0; //go back to start!
            break;
    }
    
    if (fanTimerTenthSecs > 0) {
        start_fans();
        fanTimerTenthSecs--;
    }
    else {
        stop_fans();
    }

    //catch an integer overflow making the value negative, about 3 button presses/45 mins
    if (fanTimerTenthSecs < 0) {
        fanTimerTenthSecs = 15*60*10 * 3; //put 45 minutes on the clock
    }
    
    delay(100); //run loop 10 times per second
}
