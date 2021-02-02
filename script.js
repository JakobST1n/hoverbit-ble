/* Register service worker */
if (navigator.serviceWorker) {
    navigator.serviceWorker.register('/hoverbit-ble/sw.js', {scope: '/hoverbit-ble/'});
}

const hoverControlModule = () => {
    let _throttle = 0;
    let _throttleAcc = 0;
    let _rudder = 0;
    let _rudderAcc = 0;
    let _arm = false;
    let _armAcc = false;

    const acc = (accString) => {
        accString.match(/[A-Z][-,0-9]+/g).forEach((item, i) => {
            switch (item.substring(0, 1)) {
                case "T":
                    _throttleAcc = parseInt(item.substring(1, item.length));
                    break;
                case "R":
                    _rudderAcc = parseInt(item.substring(1, item.length));
                    break;
                case "A":
                    _armAcc = parseInt(item.substring(1, item.length)) == 1;
                    if (_armAcc) {
                        document.body.classList.add("armed");
                    } else {
                        document.body.classList.remove("armed");
                    }
                    break;
                case "S":
                    break;
                default:
                    console.log(`Unkown acc: ${item}`);
            }
        });
        document.querySelector(".acc-string pre").innerHTML = `T: ${_throttleAcc}, R: ${_rudderAcc}`;
    }

    const reset = () => {
        setArm(0);
        setThrottle(0);
        setRudder(0);
    }

    const setThrottle = (throttle) => {
        if (!_armAcc) { return; }
        if (throttle > 100) { throttle = 100; }
        if (throttle < 0) { throttle = 0; }
        _throttle = throttle;
    }
    const getThrottle = () => _throttle;

    const setRudder = (rudder) => {
        if (!_armAcc) { return; }
        if (rudder > 90) { rudder = 90; }
        if (rudder < -90) { rudder = -90; }
        _rudder = rudder;
    }
    const getRudder = () => _rudder;

    const setArm = (arm) => {
        _arm = arm;
        if (!_arm) {
            _throttle = 0;
            _rudder = 0;
        }
    }
    const getArm = () => _arm;

    return {
        acc,
        reset,
        setThrottle,
        getThrottle,
        setRudder,
        getRudder,
        setArm,
        getArm
    };
}

/* Define and initialize things */
let hoverControl = hoverControlModule();
let joystickLeft = nipplejs.create({
    zone: document.querySelector(".joystick-left"),
    size: 200,
    position: {left: '50%', bottom: '50%'},
    mode: "static",
    lockX: true
});
let joystickRight = nipplejs.create({
    zone: document.querySelector(".joystick-right"),
    size: 200,
    position: {left: '50%', bottom: '50%'},
    mode: "static",
    lockY: true
});
let bDev;

/* Setup event_listeners */
joystickLeft.on("move", (evt, data) => {
    rudder = ((data.distance * 90) / 100);
    if (data.angle.degree > 90) { rudder = rudder * -1; }
    hoverControl.setRudder(rudder);
});
joystickLeft.on("end", (evt, data) => {
    hoverControl.setRudder(0);
});

joystickRight.on("move", (evt, data) => {
    throttle = data.distance;
    if (data.angle.degree > 90) { throttle = 0; }
    hoverControl.setThrottle(throttle);
});
joystickRight.on("end", (evt, data) => {
    hoverControl.setThrottle(0);
});

document.getElementById("btn_arm").addEventListener("click", () => {
    hoverControl.setArm(true);
});

document.getElementById("btn_disarm").addEventListener("click", () => {
    hoverControl.setArm(false);
});

document.getElementById("btn_connect").onclick = async () => {
    const device = await microbit.requestMicrobit(window.navigator.bluetooth);
    bDev = device;

    let connCheckInterval = setInterval(() => {
        if (device) {
            if (device.gatt.connected) {
                document.body.classList.add("connected");
            } else {
                hoverControl.reset();
                document.body.classList.remove("connected");
                document.body.classList.remove("armed");
                // clearInterval(connCheckInterval);
                device.gatt.disconnect();
            }
        } else {
            clearInterval(connCheckInterval);
            alert("FATAL ERROR, Please reload page...");
            clearInterval(connCheckInterval);
        }
    }, 500);

    if (device) {
        hoverControl.reset();
        const services = await microbit.getServices(device);

        document.querySelector("#btn_disconnect").addEventListener("click", () => {
            hoverControl.reset();
            device.gatt.disconnect();
        });

        if (services.deviceInformationService) {
            // logJson(await services.deviceInformationService.readDeviceInformation());
        }

        if (services.uartService) {
            services.uartService.addEventListener("receiveText", (event) => {
                var elm = document.querySelector(".ping i");
                var newone = elm.cloneNode(true);
                elm.parentNode.replaceChild(newone, elm);

                if ((event.detail).indexOf(":") != -1) {
                    let parts = (event.detail).split(":");
                    if (parts[0] == "B") {
                        document.querySelector(".battery-status").innerHTML = parts[1] + "mV";
                    } else if (parts[0] == "ACC") {
                        console.log(parts[1]);
                        hoverControl.acc(parts[1]);
                    } else { console.log(parts); }
                } else {
                    console.log(`Received unknown: ${event.detail}`);
                }
            });

            let sendCommands = setInterval(async() => {
                if (device) {
                    if (device.gatt.connected) {
                        let command =
                            "T" + hoverControl.getThrottle().toString() +
                            "R" + hoverControl.getRudder().toString() +
                            "A" + (hoverControl.getArm() ? "1" : "0") +
                            "S0" +
                            ":";
                        await services.uartService.sendText(command);
                    } else {
                        // clearInterval(sendCommands);
                        device.gatt.disconnect();
                    }
                }
            }, 70);

        }
    }
}
