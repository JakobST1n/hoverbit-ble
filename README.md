# HOVER:BIT Bluetooth Controller software
This is software for controlling the HOVER:BIT kit with a progressive web app.

The web app lives in the [controller](https://github.com/JakobST1n/hoverbit-ble/tree/controller) branch, you can access it [here](https://github.com/JakobST1n/hoverbit-ble). It works the best if you install it on your device, usually you should get a prompt to install it when accessing the site.

## Usage
### Flashing the micro:bit
Get the last [hex file](https://github.com/JakobST1n/hoverbit-ble/releases/) for the Releases, and flash your micro:bit with this file. 
To do this, plug the micro:bit into your computer. The micro:bit will then appear as a removable flash drive, then simply copy (drag-drop)
the hex-file into the flash-drive folder.

### Controlling the hover:bit
Navigate to [the progressive web app](https://jakobst1n.github.io/hoverbit-ble/) on a phone, and press connect.
Press `arm` when you are connected, then you can use the throttle on the right, and rudder on the bottom to controll
the hover:bit.

## Compiling the hex-file yourself
Clone or fork this repo locally, and navigate to that folder in your terminal of choice. At this point you have two choises:
### Docker image
I have been using the docker image provided by [Lancaster university](https://github.com/lancaster-university/microbit-v2-samples/blob/master/Dockerfile), because it is rather convenient and a lot less work to get up and running. Just build the container, and run it with a volume as a bind-mount to the code-directory.
```sh
# Build the image on your machine
docker build -t microbit-tools .
# Build the hex-file
docker run -v $(pwd):/app --rm microbit-tools
```
### Manually
[Refer to this repo's readme](https://github.com/lancaster-university/microbit-v2-samples). This is in theory not hard, but if you like me have a lot of toolchains for compiling to other architectures already installed, it could be a bit of work to make function properly.

## Known issues
- [ ] Webapp is a bit ugly.
- [ ] Things often stop working if you have been connected, drop the connection and then reconnect. I don't know exacly why, to circumvent this you can restart the webapp, and boot the micro:bit.
- [ ] There is no failsafe-mode currently, this should be implemented according to the spektsat protocol.
- [X] No multitouch support on webapp yet, this makes it difficult to use it, to make it a bit easier you can enable `Sticky controls`.

## Credits
- The [micro:bit Web Bluetooth](https://github.com/thegecko/microbit-web-bluetooth) library is used by the webapp for bluetooth communication with the MICRO:BIT.
- Joysticks on webapp is from the [nipplejs](https://yoannmoi.net/nipplejs/) library.