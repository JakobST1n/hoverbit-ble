import { notif_alert, notif_warn, notif_info, notif_success } from './notification';

let n = 0;

export default class hoverControlModule {
    #throttle = 0;
    #throttleAcc = 0;
    #rudder = 0;
    #rudderAcc = 0;
    #arm = 0;
    #armAcc = 0;

    constructor() {}

    acc(accString) {
        accString.match(/[A-Z][-,0-9]+/g).forEach((item, i) => {
            switch (item.substring(0, 1)) {
                case "T":
                    this.#throttleAcc = parseInt(item.substring(1, item.length));
                    break;
                case "R":
                    this.#rudderAcc = parseInt(item.substring(1, item.length));
                    break;
                case "A":
                    this.#armAcc = parseInt(item.substring(1, item.length)) == 1;
                    if (this.#armAcc) {
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
        document.querySelector(".acc-string pre").innerHTML = `T: ${this.#throttleAcc}, R: ${this.#rudderAcc}, ${n}`;
        n++;
    }

    reset() {
        this.setArm(0);
        this.setThrottle(0);
        this.setRudder(0);
    }

    setThrottle(throttle) {
        if (!this.#armAcc) { return; }
        if (throttle > 100) { throttle = 100; }
        if (throttle < 0) { throttle = 0; }
        this.#throttle = throttle;
    }
    getThrottle() {
        return this.#throttle;
    }

    setRudder(rudder) {
        if (!this.#armAcc) { return; }
        if (rudder > 90) { rudder = 90; }
        if (rudder < -90) { rudder = -90; }
        this.#rudder = rudder;
    }
    getRudder() {
        return this.#rudder;
    }

    setArm(arm) {
        this.#arm = arm;
        if (!this.#arm) {
            this.#throttle = 0;
            this.#rudder = 0;
        }
    }
    getArm() {
        return this.#arm;
    }
}
