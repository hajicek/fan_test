#include <AFMotor.h>

AF_DCMotor motor2(2, MOTOR12_1KHZ); // create motor #2, 64KHz pwm
AF_DCMotor motor3(3, MOTOR12_1KHZ);

int momentarySwitch = 0; //holds voltage reading of momentary switch
int fanTimerSecs = 0; //second remaining til fan turns off
int fanHumTimerSecs = 0;
int buttonState = 0; //hold what state we're in in the loop
int fanState = 0;
int buttonHoldHundredthSecs = 0; // number of tenths of seconds button has been held down
int buttonHoldSecs = 0;

int defaultFanTimeSecs = 90*60;

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
    int buttonLoop = 1;
    buttonState = 0;

    while(buttonLoop == 1) { //initial press loop
        momentarySwitch = analogRead(A0);


        switch (buttonState) {
            case 0: //looking for button press
                if (momentarySwitch > 100) {
                    buttonState = 1;
                    buttonHoldHundredthSecs = 1;
                }
                break;

            case 1: //button was pressed, wait until unpressed

                if (momentarySwitch < 100) {
                    //released too soon, go try again
                    buttonState = 2;
                    buttonHoldHundredthSecs++;
                }
                else if (buttonHoldHundredthSecs >= 50) {
                    //button has been held for .050 seconds, turn on the fans
                    fanTimerSecs = defaultFanTimeSecs;
                    buttonState = 3;
                }
                else {
                    buttonHoldHundredthSecs++;
                }
                break;

            case 2: //button was released in less than 0.050 seconds, start over

                buttonState = 0; //go back to start!
                break;

            case 3: //button was held for 0.050 seconds
                //break out of this loop
                buttonState = 0;
                buttonLoop = 0;
                break;

            delay(10);
        }

    }
    
    int fanLoop = 1;
    fanState = 0;

    while(fanLoop == 1) {
        momentarySwitch = analogRead(A0);

        switch (fanState) {
            case 0: //fan off, waiting for turn on condition
                if (fanTimerSecs > 0) {
                    motor2.setSpeed(220);     // set the speed slower to make some hum
                    motor3.setSpeed(220); 
                    fanHumTimerSecs = 2; //hum the fans for 2 seconds
                    start_fans();

                    fanState = 1;
                }
                else {
                    fanLoop = 0; //get out of this while loop
                }
                break;

            case 1: //fans spinning up, humming
                if (fanHumTimerSecs <= 0) {
                    motor2.setSpeed(255);     // set the speed make to stop hum
                    motor3.setSpeed(255); 
                    fanState = 2;
                }
                else {
                    fanHumTimerSecs--;
                }
                break;

            case 2: //check for button press, or if fan timer expires
                if (fanTimerSecs <= 0) { //stop the fans
                    motor2.setSpeed(255);     // set the speed faster to stop hum
                    motor3.setSpeed(255); 
                    delay(2000);
                    stop_fans();
                    delay(1000); //give time for the fans to spin down
                    fanState = 0;

                    break;

                }
                else {
                    fanTimerSecs--; //decrement fan on counter
                }


                if (momentarySwitch > 100) {
                    fanState = 3;
                    buttonHoldSecs = 1;
                }
                break;

            case 3: //button was pressed, wait until unpressed
                //add some time since the button was pressed
                motor2.setSpeed(220);     // set the speed slower to make some hum
                motor3.setSpeed(220); 
                

                if (momentarySwitch < 100) {
                    //released too soon, reset the timer to full
                    motor2.setSpeed(255);     // set the speed faster to stop hum
                    motor3.setSpeed(255); 
                    fanTimerSecs = defaultFanTimeSecs;
                    fanState = 2;
                }
                else if (buttonHoldSecs >= 2) {
                    //button has been held for 2 seconds, turn off the fans
                    fanTimerSecs = 0;
                    fanState = 2; //the if loop will catch the 0 to turn off the fans
                }
                else {
                    buttonHoldSecs++;
                }
                break;

        }

        delay(900); //1 second loops
    }

    
}
