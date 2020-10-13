/*
 * InfluxClient.cpp - Library for writing and querying a InfluxDB
 * with InfluxQL. No authentication and TLS implemented.
 *
 * Created by Martin Steinbach, October 12, 2020.
 *
 * Released into the public domain.
*/

#include "Arduino.h"
#include "InfluxClient.h"
#include <HTTPClient.h>

Influx::Influx(String serverURL, String database){
    _database = database;
    _serverURL = serverURL;
    _baseQueryURL = serverURL + "/query?db=" + _database + "&epoch=s";
    _baseWriteURL = serverURL + "/write?db=" + _database + "&precision=s";
    _tag = "tag";
    _tagValue = "somevalue";
}

/*
 * Sets the tags for the next data points to send or receive. The default value is
 * "tag=somevalue"
 */
void Influx::setTag(String tag, String tagValue) {
    _tag = tag;
    _tagValue = tagValue;
}

/*
 * Unlike sendMeasurementToInfluxDB this  method sends more than one measurement
 * per http request to the InfluxDB. Therefore it uses the data structure
 * DataPoint without checking validity.
 *
 */
void Influx::sendDataPointsToInfluxDB(DataPoint dataPoints[], int len) {

    String httpData;

    for (int i = 0 ; i < len ; i++) {
        httpData.concat(getDataPointString(
                    dataPoints[i].measurement,dataPoints[i].value));
        httpData.concat("\n");
    }

    writeData(httpData);
}

/***** build the http data string ******/
String Influx::getDataPointString(String measurement, float value) {

    String httpData; // sample: "temperature,location=keller value=18.89999999999";
    httpData.concat(measurement);
    httpData.concat("," + _tag + "=");
    httpData.concat(_tagValue);
    httpData.concat(" value=");
    httpData.concat(String(value));

    return httpData;

}

/*
 * Sends the previously created data point(s) to the server.
 */
void Influx::writeData(String httpData) {

    HTTPClient http;

    /****** set up URL and header ******/

    http.begin(_baseWriteURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    /****** send data ******/

    int responseCode = http.POST(httpData);
    if (responseCode == 204) {
        Serial.printf("HTTP response code (writing data): %d\n", responseCode);
    } else {
        Serial.print("Error while writing values to server - HTTP-Code: ");
        Serial.println(responseCode);
    }

    http.end();
}

/*
 * Sends a single data point to the InfluxDB
 */
void Influx::sendMeasurementToInfluxDB(String measurement, float value) {

    /***** build data string ******/

    String httpData = getDataPointString(measurement, value);

    writeData(httpData);

 }


/*************************
 * READING FROM DATABASE *
 *************************/


/*
 *  Does a http get request and returns the reponse as a String object e.g.:
 *
 * "name,tags,time,raum,value
 * temperature,,1588053087,keller,15.5"
 *
 * Asking InfluxDB only for the value (SELECT value from ...),
 * it returns the whole string above anyway.
 *
 */

/*
 * getting mean value for specific series/measurement (e.g. temperature) from InfluxDB
 * for a specific time in minutes.
 */
float Influx::getMeanValueFromInfluxDB(String measurement, unsigned int minutes) {

    String requestData = "q=SELECT mean(value) FROM " + measurement
		+ " WHERE time > now() - " + minutes + "m AND " + _tag + "='" + _tagValue + "'";

    DataPoint dataPoint =  readData(requestData);
    return dataPoint.value;
}


/*
 * getting the youngest value for specific series/measurement (e.g. temperature)
 * from InfluxDB.
 */
DataPoint Influx::getLastValueFromInfluxDB(String measurement) {

    String requestData = "q=SELECT last(value) FROM " + measurement
		+ " WHERE " + _tag + "='" + _tagValue + "'";

    DataPoint dp = readData(requestData);
    dp.measurement = measurement;
    return dp;

}

DataPoint Influx::readData(String requestData) {

    HTTPClient http;

    http.begin(_baseQueryURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Accept", "application/csv");

    int responseCode = http.POST(requestData);

    if (responseCode == 200) {
        Serial.printf("HTTP response code (reading data): %d\n", responseCode);
        String payload = http.getString();
        http.end();

        DataPoint dataPoint;
        dataPoint.currTime = getFieldFromPayload(2, payload).toInt();
        dataPoint.value = getFieldFromPayload(3, payload).toFloat();

        return dataPoint;


    } else {
        Serial.print("Error while getting values from Server - HTTP-Code: ");
        Serial.println(responseCode);
    }
}

/*
 * Returns the requested field (first field is number 1) as a string
 */

String Influx::getFieldFromPayload(unsigned int fieldNumber, String payload) {

   unsigned int  sepLimit = fieldNumber--;

    // get the line with the results
    String line = getValuesFromResponse(payload);

    // search the starting position of the requested field
    int startPos = 0;
    int sepCount = 0;

    for(int i = 0; i < line.length(); i++ ) {

        if (line.charAt(i) == ',') {sepCount++;}

        if (sepCount == sepLimit) {
            startPos = i + 1;
            break;
        }
    }

    // cut the string at the srating position
    line = line.substring(startPos);

    // search the right border of the field (if exists)
    int endPos = 0;

    for(int i = 0; i < line.length(); i++ ) {

        if (line.charAt(i) == ',') {
            endPos = i;
            break;
        }
    }

    if (endPos == 0) { // already the last fiel
        return line;
    } else {
        return line.substring(0,endPos);
    }
}

// splits the comma separated value string from the header
String Influx::getValuesFromResponse(String response){
    unsigned int i;
    unsigned int pos = 0;

    for ( i = 1; i < response.length(); i++) {
        if (response.charAt(i) == '\n') {
          //Serial.printf("found newline at pos: %d", i);
          pos = i;
          break;
        }
    }

	if (pos == 0) {
        Serial.println("ERROR: no values in response");
        return String(errString);
    } else {
        String csvValues = response.substring(pos);
        //Serial.println(csvValues);
        return csvValues;
    }
}
