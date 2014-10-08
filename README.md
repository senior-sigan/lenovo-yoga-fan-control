lenovo-yoga-fan-control
=======================

Program for Linux OS to control Fan Speed on Lenovo Yoga 13. 

Inside the Lenovo Yoga is an embedded controller (EC), which is responsible for some of the Yoga's low-level tasks. One of them is Fan controling.

There is [NBFC](http://www.computerbase.de/forum/showthread.php?t=1070494) programm for controlling Fan on windows in internet.

## Temperature Monitoring and Fan Control

There is a CPU temperature sensor connected to the EC via SMBus; note that this SMBus is not accessible by the CPU, and so the Linux i2c drivers cannot see any devices on this bus.

The fan is connected to a fan controller in the EC, which allows the fan speed to be varied (using PWM) between 0% and 100% duty cycle. By default, the EC sets the fan speed very strange. All time works on 60% of maximum fan speed.

This program works in automatic mode. It's controling Fan with my custom temp table. You should run it as root in background. I plane to make linux kernel with this functioanlity. 
