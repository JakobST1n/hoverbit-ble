# HOVER:BIT Bluetooth Controller software




## Usage
First you need to flash your micro:bit with the hex-file on the releases-page, or you can
compile it yourself. I have been using the docker image provided by [Lancaster university](https://github.com/lancaster-university/microbit-v2-samples/blob/master/Dockerfile), because it is rather convenient.

```
    # Build the image on your machine
    docker build -t microbit-tools .
    # Build the hex-file
    docker run -v $(pwd):/app --rm microbit-tools
```
