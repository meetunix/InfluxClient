# InfluxClient

InfluxClient is a tiny client for writing simple data points to and from the
InfluxDB 1.x REST-API.

## installation

Clone the repository to the library directory inside your Arduino directory, often
located at  `~/Arduino/libraries/`.

InfluxClient depends on the Arduino http client. The http client can be
installed using the Arduino IDE.

## using

Create an instance of the class Influx by providing the
server **url**, the **database name** and the **measurement**.

```
#include <InfluxClient.h>

Influx flux("http://192.0.2.254:8086","sensordb", "measurement");
```

### writing

Set the **tag** for your data point you want to write to the server.

```
flux.setTag("location", "attic");
```

Send one or more data points in a single http-request to the database:

```
void sendMeasurements(float sensorValues[]) {

    DataPoint dataPoints[MEASUREMENTS];

    for (int m = 0 ; m < MEASUREMENTS ; m++ ) {
        dataPoints[m].field = getFieldString(m);
        dataPoints[m].value = sensorValues[m];          
    }

    // sending all sensor values in one http request to InfluxDB
    flux.sendMeasurement(dataPoints, MEASUREMENTS);
}


```

### reading

ToDo

