# HOVER:BIT Bluetooth Controller software
This is software for controlling the HOVER:BIT kit with a progressive web app.

The web app lives in the `controller` branch, it uses the [micro:bit Web Bluetooth](https://github.com/thegecko/microbit-web-bluetooth) library.

## Usage
First you need to flash your micro:bit with the hex-file on the releases-page, or you can
compile it yourself. I have been using the docker image provided by [Lancaster university](https://github.com/lancaster-university/microbit-v2-samples/blob/master/Dockerfile), because it is rather convenient.

```
    # Build the image on your machine
    docker build -t microbit-tools .
    # Build the hex-file
    docker run -v $(pwd):/app --rm microbit-tools
```
