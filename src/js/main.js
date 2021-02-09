import nipplejs from 'nipplejs';
import { requestMicrobit, getServices } from 'microbit-web-bluetooth';
import hoverControlModule from './hoverControlModule';
import { notif_alert, notif_warn, notif_info, notif_success } from './notification';

let sw = "service-worker.js";
if (navigator.serviceWorker) {
    navigator.serviceWorker.register(
        sw, {scope: '/hoverbit-ble/'}
    ).then(registration => {
        registration.onupdatefound = () => {
            const installingWorker = registration.installing;
            if (installingWorker == null) { return; }
            installingWorker.onstatechange = () => {
                if (installingWorker.state === "installed") {
                    if (navigator.serviceWorker.controller) {
                        notif_info("New content is available, relaunch the app to install it.");
                    } else {
                        notif_success("Content is cached for offline use.");
                    }
                }
            };
        };
    }).catch(error => {
        notif_alert("Could not install service worker...");
        console.error("Error during service worker registration:", error);
    });
}

document.getElementById("btn_ignore_landscape_warning").addEventListener("click", () => {
    document.body.classList.add("ignore-landscape-warning");
});

/* Define and initialize things */
let hoverControl = new hoverControlModule();
let bluetoothDevice;
let bluetoothDeviceServices;

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

/* Setup event_listeners */
joystickLeft.on("move", (evt, data) => {
    let rudder = ((data.distance * 90) / 100);
    if (data.angle.degree > 90) { rudder = rudder * -1; }
    hoverControl.setRudder(rudder);
});
joystickLeft.on("end", (evt, data) => {
    hoverControl.setRudder(0);
});

joystickRight.on("move", (evt, data) => {
    let throttle = data.distance;

    if (data.angle.degree > 90) {
        throttle = 0;
        if (data.distance >= 100) {
            hoverControl.reset();
            if (hoverControl.getArm()) {
                notif_alert("Detected panicy reducing of throttle, dearming...");
            }
        }
    }
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

document.querySelector("#btn_disconnect").addEventListener("click", () => {
    hoverControl.reset();
    bluetoothDevice.gatt.disconnect();
});

let intervalConnectionChecker = setInterval(() => {
    if (bluetoothDevice !== undefined && bluetoothDevice) {
        if (bluetoothDevice.gatt.connected) {
            document.body.classList.add("connected");
        } else {
            document.body.classList.remove("connected");
            document.body.classList.remove("armed");
        }
    } else if (bluetoothDevice !== undefined) {
        bluetoothDevice.gatt.reconnect();
    }
}, 500);

let intervalSendCommands = setInterval(async() => {
    if (bluetoothDevice !== undefined && bluetoothDevice) {
        if (bluetoothDevice.gatt.connected && bluetoothDeviceServices.uartService) {
            let command =
                "T" + hoverControl.getThrottle().toString() +
                "R" + hoverControl.getRudder().toString() +
                "A" + (hoverControl.getArm() ? "1" : "0") +
                "S0" +
                ":";
            await bluetoothDeviceServices.uartService.sendText(command);
        }
    }
}, 70);

function receiveText(event) {
    /* Just make the ping symbol reappear. */
    var elm = document.querySelector(".ping i");
    var newone = elm.cloneNode(true);
    elm.parentNode.replaceChild(newone, elm);

    /* Actually handle received text. */
    if ((event.detail).indexOf(":") != -1) {
        let parts = (event.detail).split(":");

        if (parts[0] == "B") {
            document.querySelector(".battery-status").innerHTML = parts[1] + "mV";
        } else if (parts[0] == "ACC") {
            hoverControl.acc(parts[1]);
        } else {
            console.log(parts);
        }
    } else {
        notif_warn("Received weird data from MICRO:BIT...");
        console.log(`Received unknown: ${event.detail}`);
    }
}

document.getElementById("btn_connect").onclick = async () => {
    if (bluetoothDevice !== undefined && bluetoothDevice.gatt.connected) {
        bluetoothDevice.disconnect();
    }

    const device = await requestMicrobit(window.navigator.bluetooth);
    bluetoothDevice = device;

    if (device) {
        hoverControl.reset();
        const services = await getServices(device);
        bluetoothDeviceServices = services;

        if (bluetoothDeviceServices.deviceInformationService) {
            // logJson(await services.deviceInformationService.readDeviceInformation());
        }

        if (services.uartService) {
            services.uartService.addEventListener("receiveText", receiveText);
        }
    }
}
