if (navigator.serviceWorker) {
    navigator.serviceWorker.register('/hoverbit-ble/sw.js', {scope: '/hoverbit-ble/'});
}

let StickyControls = false;
let throttleElement = document.querySelector(".throttle");
let rudderElement = document.querySelector(".rudder");

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
                    document.querySelector(".throttle-indicator").style["bottom"] = _throttleAcc + "%";
                    break;
                case "R":
                    _rudderAcc = parseInt(item.substring(1, item.length));
                    document.querySelector(".rudder-indicator").style.left = (((_rudderAcc+90) * (100)) / (180)) + "%";
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

        document.querySelector(".throttle-thumb").style.height = throttle + "%";
    }
    const getThrottle = () => _throttle;

    const setRudder = (rudder) => {
        if (!_armAcc) { return; }
        if (rudder > 90) { rudder = 90; }
        if (rudder < -90) { rudder = -90; }
        _rudder = rudder;

        let rudderThumb = document.querySelector(".rudder-thumb");
        rudderThumb.style.width = ((Math.abs(_rudder) / (90 / 50))) + "%";
        if (_rudder > 0) {
            rudderThumb.style.backgroundColor = "yellow";
        } else {
            rudderThumb.style.backgroundColor = "red";
            rudderThumb.style.left = (50 - ((Math.abs(_rudder) / (90 / 50)))) + "%";
        }
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
let hoverControl = hoverControlModule();

window.onload = () => {
    addEventListener("touchstart", (touch) => {
        addEventListener("touchmove", (touch => {
            console.log(touch);
        }), true);
    }, false);

    // throttleElement.addEventListener('mousedown', on_pointer_hold_throttle,false);
    // document.body.addEventListener('mouseup', on_pointer_release_throttle, false);
    //
    // throttleElement.addEventListener('touchstart', on_pointer_hold_throttle,false);
    // document.body.addEventListener('touchend', on_pointer_release_throttle, false);
    //
    // rudderElement.addEventListener('mousedown', on_pointer_hold_rudder,false);
    // document.body.addEventListener('mouseup', on_pointer_release_rudder, false);
    //
    // rudderElement.addEventListener('touchstart', on_pointer_hold_rudder,false);
    // document.body.addEventListener('touchend', on_pointer_release_rudder, false);
};

function on_pointer_hold_throttle() {
    throttleElement.addEventListener('mousemove',(on_mouse_move_throttle),true);
    document.body.addEventListener('mousemove',on_mouse_move_throttle,true);

    throttleElement.addEventListener('touchmove',on_touch_move_throttle,true);
    document.body.addEventListener('touchmove',on_touch_move_throttle,true);
}

function on_mouse_move_throttle(event) {
    let throttle = Math.round(100 - (event.clientY) / (throttleElement.clientHeight / 100));
    hoverControl.setThrottle(throttle);
}

function on_touch_move_throttle(touch) {
    let throttle = Math.round(100 - (touch.touches[0].clientY) / (throttleElement.clientHeight / 100));
    hoverControl.setThrottle(throttle);
}

function on_pointer_release_throttle() {
	throttleElement.removeEventListener('mousemove',on_mouse_move_throttle,true);
	document.body.removeEventListener('mousemove',on_mouse_move_throttle,true);

	throttleElement.removeEventListener('touchmove',on_touch_move_throttle,true);
	document.body.removeEventListener('touchmove',on_touch_move_throttle,true);

    if (!StickyControls) {
        hoverControl.setThrottle(0);
    }
}

function on_pointer_hold_rudder() {
    rudderElement.addEventListener('mousemove',on_mouse_move_rudder,true);
    document.body.addEventListener('mousemove',on_mouse_move_rudder,true);

    rudderElement.addEventListener('touchmove',on_touch_move_rudder,true);
    document.body.addEventListener('touchmove',on_touch_move_rudder,true);
}

function on_mouse_move_rudder(event) {
    let rudder = Math.round(((event.clientX * (90 - (-90))) / (rudderElement.clientWidth)) + (-90));
    hoverControl.setRudder(rudder);
}

function on_touch_move_rudder(touch) {
    let rudder = Math.round(((touch.touches[0].clientX * (90 - (-90))) / (rudderElement.clientWidth)) + (-90));
    hoverControl.setRudder(rudder);
}

function on_pointer_release_rudder() {
	rudderElement.removeEventListener('mousemove',on_mouse_move_rudder,true);
	document.body.removeEventListener('mousemove',on_mouse_move_rudder,true);

	rudderElement.removeEventListener('touchmove',on_touch_move_rudder,true);
	document.body.removeEventListener('touchmove',on_touch_move_rudder,true);

    if (!StickyControls) {
        hoverControl.setRudder(0);
    }
}

const logEl = document.getElementById("logEl");
const log = (message) => logEl.innerHTML = `${message}\n${logEl.innerHTML}`;
const logJson = (message) => log(JSON.stringify(message, null, 2));
const eventHandler = event => log(`${event.type}: ${JSON.stringify(event.detail, null, 2)}`);
let bDev;

document.getElementById("btn_arm").addEventListener("click", () => {
    hoverControl.setArm(true);
});

document.getElementById("btn_disarm").addEventListener("click", () => {
    hoverControl.setArm(false);
});

document.getElementById("btn_sticky").addEventListener("click", () => {
    StickyControls = !StickyControls;
    if (StickyControls) {
        document.getElementById("btn_sticky").style.backgroundColor = "green";
    } else {
        hoverControl.setThrottle(0);
        hoverControl.setRudder(0);
        document.getElementById("btn_sticky").style.backgroundColor = null;
    }
});

document.getElementById("btn_connect").onclick = async () => {
    const device = await microbit.requestMicrobit(window.navigator.bluetooth);
    bDev = device;

    let connCheckInterval = setInterval(() => {
        if (device) {
            if (device.gatt.connected) {
                document.body.classList.add("connected");
                // document.querySelector(".connection-status").innerHTML = "CONNECTED";
            } else {
                hoverControl.reset();
                document.body.classList.remove("connected");
                document.body.classList.remove("armed");
                // document.querySelector(".connection-status").innerHTML = "DISCONNECTED";
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
            hoverControl.setArm(0);
            hoverControl.setThrottle(0);
            hoverControl.setRudder(0);

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
