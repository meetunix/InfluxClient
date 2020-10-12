# InfluxClient

InfluxClient is a tiny client for reading and writing simple data points to and from the  InfluxDB 1.x REST-API.

The client is able to write data points in the following scheme:

`'temperature,location=attic value=18.899999'`

## installation

Clone the repository to the library directory inside your Arduino directory, often
located at  `~/Arduino/libraries/`.

InfluxClient depends on the Arduino http client. The http client can be
installed using the Arduino IDE.

## using

Create an instance of the class Influx by providing the
server **url** and the **database name**.

```
#include <InfluxClient.h>

Influx flux("http://192.0.2.254:8086","sensordb");
```

### writing

Set the **tag** for your data point you want to write or read and send a single
data point to the server.

```
flux.setTag("location", "attic");

flux.sendMeasurementToInfluxDB("temperature", 18.899999);
```

If you want to write more than one data points at a time (only one http-request)
you can use the `sendDataPointsToInfluxDB` method.

```
DataPoint dataPoint[LEN];

for (int m = 0 ; m < LEN ; m++ ) {
    dataPoints[m].measurement = getMeasurementString(m); // 'temperature'
    dataPoints[m].value = sensorValues[m]; // 18.899999
}

flux.sendDataPointsToInfluxDB(dataPoints, LEN)
```

### reading

At first set the tag you want to filter and read the value from server. Currently
the mean value from the last 10 minutes will be returned. I will change that later.

```
flux.setTag("location", "attic");

float value = flux.getValueFromInfluxDB('humidity');
```
