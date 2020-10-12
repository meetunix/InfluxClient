/*
 * InfluxClient.h - Library for writing and querying a influxDB
 * with InfluxQL. No authentication and ZLS implemented
 * 
 * Created by Martin Steinbach, Juli 5, 2020.
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
}DataPoint;

class Influx {
    public:
		/* constructor:
	     *
		 * serverURL: "http://IPADDR:PORT" - database:  "db_name"
		 */
        Influx(String serverURL, String database);
        float getValueFromInfluxDB(String measurement);
        void sendMeasurementToInfluxDB(String measurement, float value);
        void sendDataPointsToInfluxDB(DataPoint dataPoints[], int len);
        String getDataPointString(String measurement, float value);
        void writeData(String httpData);
        void setTag(String tag, String tagValue);
        String getMeasurementString(int type);
		String errString;
   private:
        String getValuesFromResponse(String response);
        float getValueFromValues(String values);
        String _database;
        String _serverURL;
        String _baseQueryURL;
        String _baseWriteURL;
        String _tag;
        String _tagValue;
};


#endif
