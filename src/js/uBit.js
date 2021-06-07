/*
 * This code is written with a lot of help from these resources:
 * https://github.com/antefact/microBit.js/blob/master/src/microBit.js
 * https://gist.github.com/kotobuki/7c67f8b9361e08930da1a5cfcfb0653f
 * https://lancaster-university.github.io/microbit-docs/resources/bluetooth/bluetooth_profile.html
 */
const UART_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
const UART_TX_CHARACTERISTIC_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
const UART_RX_CHARACTERISTIC_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

export default class uBitBLE {

    constructor() {
        let device;

        this.onConnectCallback = function() {};
        this.onDisconnectCallback = function() {};
        this.onUartTxCallback = function() {};

        this.characteristic = {
            UART_RX: {},
            UART_TX: {}
          }
    }

    onConnect(callbackFunction) {
        this.onConnectCallback = callbackFunction;
    }

    onDisconnect(callbackFunction) {
        this.onDisconnectCallback = callbackFunction;
    }

    onUartTx(callbackFunction) {
        this.onUartTxCallback = callbackFunction;
    }

    sendUart(string) {
        if (this.isConnected() && this.characteristic.UART_RX) {
            let encoder = new TextEncoder();
            this.characteristic.UART_RX.writeValue(
                encoder.encode(string)
            ).catch(error => {
                console.log(error);
            });
        }
    }

    isConnected() {
        if (this.device) {
            return this.device.gatt.connected;
        } else {
            return false;
        }
    }

    disconnect() {
        if (this.isConnected()) {
            this.device.gatt.disconnect();
        }
    }

    searchDevice() {
        navigator.bluetooth.requestDevice({
            filters: [{namePrefix: "BBC micro:bit"}],
            optionalServices: [UART_SERVICE_UUID]
        })
        .then(device => {
            console.log('> Name:             ' + device.name);
            console.log('> Id:               ' + device.id);
            this.device = device;

            device.addEventListener('gattserverdisconnected', this.onDisconnectCallback);

            // Attempts to connect to remote GATT Server.
            return device.gatt.connect();
        })
        .then(server => {
            // Note that we could also get all services that match a specific UUID by
            // passing it to getPrimaryServices().
            this.onConnectCallback();
            console.log('Getting Services...');
            return server.getPrimaryServices();
        })
        .then(services => {
            console.log('Getting Characteristics...');
            let queue = Promise.resolve();
            services.forEach(service => {
                queue = queue.then(_ => service.getCharacteristics().then(characteristics => {
                    console.log('> Service: ' + service.uuid);
                    characteristics.forEach(characteristic => {
                        console.log('>> Characteristic: ' + characteristic.uuid + ' ' +
                        getSupportedProperties(characteristic));
                        switch (characteristic.uuid) {
                            case UART_RX_CHARACTERISTIC_UUID:
                                this.characteristic.UART_RX = characteristic;
                                break;
                            
                            case UART_TX_CHARACTERISTIC_UUID:
                                this.characteristic.UART_TX = characteristic;
                                this.characteristic.UART_TX.startNotifications();
                                this.characteristic.UART_TX.addEventListener(
                                    "characteristicvaluechanged",
                                    (event) => { this.onUartTxCallback(eventByteArrayToString(event)) }
                                );
                                break;

                            default:
                        }
                    });
                }));
            });
            return queue;
        })
        .catch(error => {
            console.log('Argh! ' + error);
        });
        }

}

function getSupportedProperties(characteristic) {
    let supportedProperties = [];
    for (const p in characteristic.properties) {
        if (characteristic.properties[p] === true) {
            supportedProperties.push(p.toUpperCase());
        }
    }
    return '[' + supportedProperties.join(', ') + ']';
}

function eventByteArrayToString(event) {
    let receivedData = [];
    for (var i = 0; i < event.target.value.byteLength; i++) {
        receivedData[i] = event.target.value.getUint8(i);
    }
    return String.fromCharCode.apply(null, receivedData);
}
