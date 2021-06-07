# HOVER:BIT Bluetooth Controller software
This is software for controlling the HOVER:BIT kit with a progressive web app.

The web app lives in the [controller](https://github.com/JakobST1n/hoverbit-ble/tree/controller) branch, you can access it [here](http://jakobst1n.github.io/hoverbit-ble/). It works the best if you install it on your device, usually you should get a prompt to install it when accessing the site.

> Only tested on __Google Chrome__! Will __NOT__ work on Safari (as apple doesn't allow web bluetooth). I have not tested on IOS, but it should work with chrome there as well.

The Micro:bit code for this project is not the most efficient, object oriented and readable code is highly prioritized instead.

## Usage
### Flashing the micro:bit
Get the last [hex file](https://github.com/JakobST1n/hoverbit-ble/releases/latest) from Releases, and flash your micro:bit with this file.
To do this, plug the micro:bit into your computer. The micro:bit will then appear as a removable flash drive, then simply copy (drag-drop)
the hex-file into the flash-drive folder.

### Controlling the hover:bit
Navigate to [the progressive web app](https://jakobst1n.github.io/hoverbit-ble/) on a phone, and press connect.
Press `arm` when you are connected, then you can use the throttle on the right, and rudder on the left to controll
the hover:bit.

## Compiling the hex-file yourself
Clone or fork this repo locally, and navigate to that folder in your terminal of choice. At this point you have two choises:
### Docker image
I have used a docker image to compile the source. The [Dockerfile](https://github.com/JakobST1n/hoverbit-ble/blob/master/Dockerfile) is in the root of this repo. Using a docker container is rather convenient and a lot less work to get up and running. Just build the container, and run it with a volume as a bind-mount to the code-directory.
```sh
# Build the image on your machine
docker build -t microbit-tools .
# Build the hex-file
docker run -v $(pwd):/app --rm microbit-tools
```
### Manually
[Refer to this repo's readme](https://github.com/lancaster-university/microbit-v2-samples). This is in theory not hard, but if you like me have a lot of toolchains for compiling to other architectures already installed, it could be a bit of work to make function properly.

## Versions
There is no connection between the version of the app and the version of the MICRO:BIT code, other than that major versions is supposed to be compatible. Meaning that if you come over some problems, and don't find a [issue](https://github.com/JakobST1n/hoverbit-ble/issues) describing your problem. You should first check that the major version of both the app and microbit hex is the same (e.g. app `1.0.8-alpha.0`and MICRO:BIT `1.0.1`, where the first number is the major).

To see what version the micro:bit is running, press `A` and `B` simultaneously, and the version number should scroll over the screen.

## Known issues
Take a look at [active issues](https://github.com/JakobST1n/hoverbit-ble/issues). Submit a issue there if find any problems.

## Credits
- Joysticks on webapp is from the [nipplejs](https://yoannmoi.net/nipplejs/) library.
- [micro:bit V2 Runtime (Docs)](https://rneacy.dev/mbv2/ble/)
