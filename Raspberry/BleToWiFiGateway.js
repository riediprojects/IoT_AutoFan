const noble = require("@abandonware/noble");
const fanServiceUuid = "400f";
const fanStateCharacteristicUuid = "4b00";
const serviceUuids = [fanServiceUuid];
const characteristicUuids = [fanStateCharacteristicUuid];

var oldTemperatur = 0;
var temperatur = 0;
var temperaturAsInt = 0;

const https = require("https");
const options = {
    hostname: "api.thingspeak.com",
    path: "https://api.thingspeak.com/channels/1249641/fields/1.json?api_key=PW6OR5BS0O6KSQV5&results=1",
    method: "GET",
    port: 443,
};


//ThingSpeak Polling
function polling() {
    console.log("Try pull from ThingSpeak...");
    oldTemperatur = temperatur;

    const req = https.request(options, (res) => {
        let resData = "";
        res.on("data", (chunk) => {
            resData += chunk;
        });

        res.on("end", () => {
            //console.log(resData);
            var jsonParsed = JSON.parse(resData);
            temperatur = jsonParsed.feeds[0].field1;
            console.log("Read ThingSpeak Temp: " + temperatur);

            if (oldTemperatur !== temperatur) {
                console.log("Temperatur has changed");
                temperaturAsInt = Math.round(temperatur * 100);
                sendTemp_to_FanController();
            } else {
                console.log("Temperatur not changed");
            }

        });
    });
    req.on("error", (err) => {
        console.log("Error: " + err.message);
    });
    req.end();

}

const createPolling = setInterval(polling, 9000);


//Connect to BLE Fan Controller
function sendTemp_to_FanController() {
    console.log("Scanning Fan Controllers...");
    noble.startScanning(serviceUuids, false); // true = allow duplicates
}

noble.on("discover", (peripheral) => {
    noble.stopScanning();
    console.log(
        peripheral.address + ", " +
        peripheral.advertisement.localName);
    peripheral.connect((err) => {
        console.log("connected");
        peripheral.discoverServices(serviceUuids, (err, services) => {
            services.forEach((service) => {
                console.log("found service:", service.uuid);
                service.discoverCharacteristics(characteristicUuids, (err, characteristics) => {
                    characteristics.forEach((characteristic) => {
                        console.log("found characteristic:", characteristic.uuid);

                        //Write to Fan_Controller
                        const data = Buffer.alloc(2);
                        data.writeUInt16LE(temperaturAsInt, 0);
                        characteristic.write(data, true);

                        //Read from Fan_Controller
                        characteristic.read((error, data) => {
                            var tempOnFan = (data.readUInt16LE(0) / 100);
                            if (tempOnFan == temperatur) {
                                console.log("Set " + tempOnFan + "° on Fan_Controller successful");
                            } else {
                                console.log("Set " + temperatur + "° on Fan_Controller failed");
                            }

                            peripheral.disconnect((err) => {
                                console.log("Disconnected from Fan_Controller");
                                console.log("==========================================");
                            });
                        });
                    });
                });
            });
        });
    });
});