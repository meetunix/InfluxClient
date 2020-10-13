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

If you want to write more than one data point at a time (and use only one http-request)
you can use the `sendDataPointsToInfluxDB` method.

```
DataPoint dataPoints[LEN];

for (int m = 0 ; m < LEN ; m++ ) {
    dataPoints[m].measurement = getMeasurementString(m); // 'temperature'
    dataPoints[m].value = sensorValues[m]; // 18.899999
}

flux.sendDataPointsToInfluxDB(dataPoints, LEN)
```

### reading

#### a single data point

At first set the tag you want to filter. Then read the value from server identified by
the measurement/series (e.g. `humidity`).

```
flux.setTag("location", "attic");

DataPoint dp = flux.getLastValueFromInfluxDB('humidity');
Serial.println(dp.value + " from " + dp.currTime);
```

#### average data

At first set the tag you want to filter. Then read the value from server identified by
the measurement/series (e.g. `humidity`) and the period of time in minutes over
which the average should be formed.

```
flux.setTag("location", "attic");

float val = flux.getMeanValueFromInfluxDB('humidity', 10);
```
