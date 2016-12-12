
// npm install bme280-sensor si7021-sensor cayennejs

const BME280 = require('bme280-sensor');
const Si7021 = require('si7021-sensor');
var Cayenne = require('cayennejs');

const bme280 = new BME280({ i2cBusNo : 1, i2cAddress : 0x76 });
const si7021 = new Si7021({ i2cBusNo : 1, i2cAddress : 0x40 });

// Update every x seconds
const updateInterval = 60;

// Initiate Cayenne MQTT API
const cayenneClient = new Cayenne.MQTT({
  username: "a1ced9e0-b24e-11e6-bb76-1157597ded22",
  password: "0858f39268653283bf68bb08b165c07cd6cb1959",
  clientId: "bd6c2ab0-bd1e-11e6-9638-53ecf337e03f"
});


cayenneClient.connect((err , mqttClient) => {
	console.log('Cayenne connected')
})	


// Read BME280 sensor data, repeat
const readBME280SensorData = () => {
  bme280.readSensorData()
    .then((data) => {
      
      console.log(`BME280 data = ${JSON.stringify(data, null, 2)}`);

			// dashboard widget automatically detects datatype & unit
			cayenneClient.celsiusWrite (0, data.temperature_C.toFixed(1));
			cayenneClient.rawWrite(1, data.humidity.toFixed(0), "rel_hum" , "p" );
			cayenneClient.hectoPascalWrite (2, data.pressure_hPa.toFixed(0));

      setTimeout(readBME280SensorData, updateInterval*1000);
    })
    .catch((err) => {
      console.log(`BME280 read error: ${err}`);
      setTimeout(readBME280SensorData, updateInterval*1000);
    });
};

// Initialize the BME280 sensor
//
bme280.init()
  .then(() => {
    console.log('BME280 initialization succeeded');
    readBME280SensorData();
  })
  .catch((err) => console.error(`BME280 initialization failed: ${err} `));
	
	
const readSI7021SensorData = () => {
  si7021.readSensorData()
    .then((data) => {

			console.log(`SI7021 data = ${JSON.stringify(data, null, 2)}`);
			
			cayenneClient.celsiusWrite (4, data.temperature_C.toFixed(1));
			cayenneClient.rawWrite(5, data.humidity.toFixed(0), "rel_hum" , "p" );

      setTimeout(readSI7021SensorData, updateInterval*1000);
    })
    .catch((err) => {
      console.log(`Si7021 read error: ${err}`);
      setTimeout(readSI7021SensorData, updateInterval*1000);
    });
};

si7021.reset()
  .then(() => {
    console.log('SI7021 initialization succeeded');
		readSI7021SensorData();
	})
  .catch((err) => console.error(`Si7021 reset failed: ${err} `));
	