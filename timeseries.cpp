/*
 * Michael Amar
 */

#include "timeseries.h"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <istream>
#include <string>
#include <string.h>

/*
 * This class contain the data from the file
 */
TimeSeries::TimeSeries(const char *CSVfileName) {
    //initialize the map
    setName(CSVfileName);
    //extract the data from the file.
    loadDataFromCSVFile(CSVfileName);
}

void TimeSeries::setName(const char *name) {
    char *fileName = new char[strlen(name) + 1];
    strcpy(fileName, name);
	csvName = fileName;
}

const char *TimeSeries::getName() {
    return csvName;
}


void TimeSeries::loadDataFromCSVFile(const char *CSVfileName) {
    ifstream CSVfile(CSVfileName);
    if (!CSVfile.is_open()) {
        throw runtime_error("couldn't open this file.");
    }
    //in this vector I save all the lines.
    vector<string> values;
	string line;
    //getting the first line from the file (the names of the features).
    getline(CSVfile, line);
    //creating vector with the name of the features.
    featuresNames = dataSeparation(line, ',');
    //getting all the lines with values.
    while (getline(CSVfile, line)) {
		if (strcmp(line.c_str(), "done") == 0) {
			break;
		}
        values = dataSeparation(line, ',');
        // add the date to the map
        add(line, values);
    }
    CSVfile.close();
}

void TimeSeries::add(string line, vector<string> vec) {
    for (size_t i = 0; i < featuresNames.size(); ++i) {
        //pushing this value to its suitable vector according to the name of the feature.
        matrix[featuresNames[i]].push_back(stof(vec[i]));
    }
}

vector<string> TimeSeries::dataSeparation(string line, char delim) {
    stringstream s(line);
    string str;
    vector<string> vec;
    //separate the line into word without punctuation.
    while (getline(s, str, delim)) {
        vec.push_back(str);
    }
    return vec;
}

vector<float> TimeSeries::getValues(string str) const {
    return getMatrix()[str];
}

const vector <string> TimeSeries::getFeatureNames() const{
    return this->featuresNames;
}

map<string, vector<float>> TimeSeries::getMatrix() const {
        return matrix;
}

TimeSeries::~TimeSeries() {
	delete csvName;
}