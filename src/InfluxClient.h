/*
 * InfluxClient.h - Library for writing and querying a influxDB
 * with InfluxQL. No authentication and ZLS implemented
 *
 * Created by Martin Steinbach, October 12, 2020.
 *
 * Released into the public domain.
*/

#define SEALEVELPRESSURE_HPA (1013.25)

#ifndef InfluxClient_h
#define InfluxClient_h
#include "Arduino.h"

typedef struct {
        String measurement;
        float value;
        int currTime;
}DataPoint;

class Influx {
    public:
        Influx(String serverURL, String database);
        float getMeanValueFromInfluxDB(String measurement, unsigned int minutes);
        DataPoint getLastValueFromInfluxDB(String measurement);
        void sendMeasurementToInfluxDB(String measurement, float value);
        void sendDataPointsToInfluxDB(DataPoint dataPoints[], int len);
        String getDataPointString(String measurement, float value);
        void setTag(String tag, String tagValue);
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
};

#endif
