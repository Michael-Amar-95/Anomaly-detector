/*
 * Michael Amar
 */

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

using namespace std;

class TimeSeries{

private:
    //the file his name
    const char *csvName;
    //vector of features names.
    vector<string> featuresNames;
    //save the data as map. Each key is the name of the feature and the values are the vectors that
    //contain the parameters/
    map<string, vector<float>> matrix;

public:

    //constructor
	TimeSeries(const char* CSVfileName);

    //set the name of the file
    void setName(const char *name);

    //get the name of the file
    const char *getName();

    //load the data from the CSV file
    void loadDataFromCSVFile(const char *CSVfileName);

    //add values to the map.
    void add(string line, vector<string> values);

    //this function separate the line into a words and return them as vector.
    // delim is the the char ','.
    vector<string> dataSeparation(string line, char delim);

    //get a vector according to his name of feature.
    vector<float> getValues(string str) const;

    //get the features names
    const vector<std::string> getFeatureNames() const;

    //get all the data from the map.
    map<string, vector<float>> getMatrix() const;

    //delete the pointer that contain the name of the file.
    ~TimeSeries();

};

#endif /* TIMESERIES_H_ */
