/*
 * InfluxClient.h - Library for writing and querying a influxDB
 * with InfluxQL. No authentication and TLS implemented
 *
 * Copyright Martin Steinbach, 2021.
 *
 * Released into the public domain.
*/

#define SEALEVELPRESSURE_HPA (1013.25)

#ifndef InfluxClient_h
#define InfluxClient_h
#include "Arduino.h"
#include <HTTPClient.h>

typedef struct {
        String field;
        float value;
}DataPoint;

class Influx {
    public:
        Influx(String serverURL, String database, String measurement);
        void sendMeasurement(DataPoint dataPoints[], uint8_t len);
        void setTag(String tag, String tagValue);
        void setMeasurement(String measurement);
        float getMeanValueFromInfluxDB(String measurement, unsigned int minutes);
        DataPoint getLastValueFromInfluxDB(String measurement);
        String errString;
   private:
        String getValuesFromResponse(String response);
        String getFieldFromPayload(unsigned int fieldNumber, String payload);
        void writeData(String httpData);
        DataPoint readData(String requestData);
        String _database;
        String _serverURL;
        String _baseQueryURL;
        String _baseWriteURL;
        String _tag;
        String _tagValue;
        String _measurement;
};

#endif
