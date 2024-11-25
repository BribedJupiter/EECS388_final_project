#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document
    int uart0 = devid; //set devid 
    uint16_t dist = 0; //set distance variable
    if ('Y' == ser_read(uart0) && 'Y' == ser_read(uart0)) { //check data readiness double byte
        char DistLow = ser_read(uart0); //read first byte
        char DistHigh = ser_read(uart0); //read second distance byte
        
        dist = (DistHigh << 8) + DistLow; //combine distance data

        //convert distance data to led output
        if (dist > 200){ //safe distance = green
            gpio_write(GREEN_LED, ON);
            gpio_write(RED_LED, OFF);
            gpio_write(BLUE_LED, OFF);
            }
        else if(dist > 100) //close = red and green
        {
            gpio_write(GREEN_LED, ON);
            gpio_write(RED_LED, ON);
            gpio_write(BLUE_LED, OFF);
            }
        else if (dist > 60){ //very close = red 
            gpio_write(GREEN_LED, OFF);
            gpio_write(RED_LED, ON);
            gpio_write(BLUE_LED, OFF);
        }
        else{ //Too close = Blinking Red
            gpio_write(GREEN_LED, OFF);
            gpio_write(RED_LED, ON); //only red on 
            gpio_write(BLUE_LED, OFF);
            delay(100); //on for 100 ms 

            gpio_write(RED_LED, OFF);
            delay(100); //off for 100ms
        }
    }
}

int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.

    //uart0 = lidar
    //uart1 = pi
    char data[5];
    int angle;
    static int lastAngle;

    if (ser_isready(devid)) {
        ser_readline(devid, 5, data);
        lastAngle = angle
        return angle;
    }
    else{
        return lastAngle;
    }
}

void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task

    // Using PWM
    int usDegreeLength = (2400 - 544) / 180; // microseconds (us) to turn 1 degree: (max pulse length - min pulse length / 180
    int usSignalLength = usDegreeLength * pos + 544; // length of time to go 1 degree * # of degrees to turn + min pulse length
    int leftoverPeriod = 20000 - usSignalLength; // leftover time for 1 period

    gpio_write(gpio, ON);
    delay_usec(usSignalLength);
    gpio_write(gpio, OFF);
    delay_usec(leftoverPeriod);
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        printf("\nangle=%i", angle) 
        int gpio = PIN_19; 
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            // steering(gpio, angle);
        }

    }
    return 0;
}
