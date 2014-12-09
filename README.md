lenovo-yoga-fan-control
=======================

Program for Linux-based OS to control Fan Speed on Lenovo Yoga 13. 

Inside the Lenovo Yoga is an embedded controller (EC), which is responsible for some of the Yoga's low-level tasks. One of them is Fan controling.

There is 'notebook fan control'  programm for controlling Fan on windows in internet.

## Temperature Monitoring and Fan Control

There is a CPU temperature sensor connected to the EC via SMBus; note that this SMBus is not accessible by the CPU, and so the Linux i2c drivers cannot see any devices on this bus.

The fan is connected to a fan controller in the EC, which allows the fan speed to be varied (using PWM) between 0% and 100% duty cycle. By default, the EC sets the fan speed very strange. All time works on 60% of maximum fan speed.

This program works in automatic mode. It's controling Fan with my custom temp table. You should run it as root in background. I plane to make linux kernel with this functioanlity. 

### HowTo use

You can compile program from source.

Compiling from sourse:
```bash
sudo apt-get install gcc-4.9-base cmake git
git clone https://github.com/blan4/lenovo-yoga-fan-control.git
cd lenovo-yoga-fan-control
cmake CMakeLists.txt
make
sudo ./yoga_fan
```

For running script at runtime do this as ROOT:
```bash
sudo -s
make install
update-rc.d yoga_fan_daemon defaults
```

If you use system.d you should copy service file yoga_fan.service to /etc/systemd/system/ directory. After that enable service by ``sudo systemctl enable yoga_fan.service``

### Useful links

[Ec specification](http://wiki.laptop.org/go/Ec_specification)

[Yoga TempTable Bios patch for old versions](https://forums.lenovo.com/t5/Idea-Windows-based-Tablets-and/Yoga-13-Fan-Noise/ta-p/1065093)

[Linux IO programming](http://www.tldp.org/HOWTO/IO-Port-Programming.html)

[Kernel module for Asus eeePc](https://code.google.com/p/eeepc-linux/wiki/EmbeddedController)

[NBFC](http://www.computerbase.de/forum/showthread.php?t=1070494)


###TODO:
- [x] add daemon for ubuntu
- [x] add daemon for systemd based
- [ ] add config file
- [ ] widget with fan speed and temperature
- [ ] add fan device output and input to /sys/class/hwmon
