/*
 * InfluxClient.cpp - Library for writing and querying a InfluxDB
 * with InfluxQL. No authentication and TLS implemented.
 *
 * Created by Martin Steinbach, October 12, 2020.
 *
 * Released into the public domain.
*/

#include "InfluxClient.h"
#include <esp32-hal-log.h>

Influx::Influx(String serverURL, String database, String measurement){
    _database = database;
    _serverURL = serverURL;
    _measurement = measurement;

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

/***** build the http data string and sends it ******/
void Influx::sendMeasurement(DataPoint dataPoints[], uint8_t len) {

    // create string for fields
    String fields;
    DataPoint currData;
    for (uint8_t i = 0 ; i < len; i++) {
        currData = dataPoints[i];
        fields.concat(currData.field + "=" + String(currData.value));
        if (i < len - 1)
            fields.concat(",");
    }

    log_d("Measurement: %s", _measurement.c_str());
    log_d("TAG: %s=%s", _tag.c_str(), _tagValue.c_str());
    log_d("Fields: %s", fields.c_str());

    String httpData; // sample: "temperature,location=keller value=18.89999999999";
    httpData.concat(_measurement);
    httpData.concat("," + _tag + "=");
    httpData.concat(_tagValue);
    httpData.concat(" ");
    httpData.concat(fields);

    writeData(httpData);
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
        log_d("HTTP response code (writing data): %d", responseCode);
    } else {
        log_e("Error while writing values to server - HTTP-Code: %d", responseCode);
    }

    http.end();
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
DataPoint Influx::getLastValueFromInfluxDB(String field) {

    String requestData = "q=SELECT last(" + field + ") FROM " + _measurement
		+ " WHERE " + _tag + "='" + _tagValue + "'";

    DataPoint dp = readData(requestData);
    dp.field = field;
    return dp;

}

DataPoint Influx::readData(String requestData) {

    HTTPClient http;

    http.begin(_baseQueryURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Accept", "application/csv");

    int responseCode = http.POST(requestData);

    if (responseCode == 200) {
        log_d("HTTP response code (reading data): %d", responseCode);
        String payload = http.getString();
        http.end();

        DataPoint dataPoint;
        //dataPoint.currTime = getFieldFromPayload(2, payload).toInt();
        dataPoint.value = getFieldFromPayload(3, payload).toFloat();

        return dataPoint;

    } else {
        log_d("Error while getting values from Server - HTTP-Code: %d", responseCode);
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
        log_e("ERROR: no values in response");
        return String(errString);
    } else {
        String csvValues = response.substring(pos);
        //Serial.println(csvValues);
        return csvValues;
    }
}
