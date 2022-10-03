

#ifndef COMMANDS_H_
#define COMMANDS_H_

/*#include<iostream>
#include <string.h>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include <math.h>
#include <sys/types.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cstring>
#include <map>
#include <fstream>
*/
#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
protected:
	ofstream out;
	ifstream in;
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

	// you may add additional methods here
};

// you may add here helper classes
class StandardIO : public DefaultIO {

public:
	StandardIO(string inputFile, string outputFile) {
		in.open(inputFile);
		out.open(outputFile);
	}

	string read() {
		string str;
		in >> str;
		return str;
	}

	void write(string text) {
		out << text;
	}

	void write(float f) {
		out << f;
	}

	void read(float *f) {
		in >> *f;
	}
};

class SocketIO : public DefaultIO {
	int userID;
public:
	SocketIO(int userID) {
		this->userID = userID;
	}

	virtual string read() {
		string serverInput = "";
		char c = 0;
		::read(userID, &c, sizeof(char));
		while (c != '\n') {
			serverInput += c;
			::read(userID, &c, sizeof(char));
		}
		return serverInput;
	}

	virtual void write(string text) {
		::write(userID, text.c_str(), text.length());
	}

	virtual void write(float f) {
		string strF = to_string(f);
		::write(userID, strF.c_str(), to_string(f).length());
	}

	virtual void read(float *f) {
	}
};

class AnomalyDetectorSharedObjectGenerator {
	HybridAnomalyDetector theDetector;
	vector <AnomalyReport> reportsArray;
public:
	string trainFile, testFile;
	AnomalyDetectorSharedObjectGenerator() {
		theDetector = HybridAnomalyDetector();
	}
	HybridAnomalyDetector getDetector() {
		return theDetector;
	}

	vector<AnomalyReport> getAnomaly() {
		return reportsArray;
	}

	void setAnomalyReport(const TimeSeries &tsTrain, const TimeSeries &tsTest) {
		theDetector.learnNormal(tsTrain);
		reportsArray = theDetector.detect(tsTest);
	}

	float getThreshold() {
		return theDetector.getThreshold();
	}

	void setNewThreshold(float f) {
		theDetector.setNewThreshold(f);
	}

};
// you may edit this class
class Command{
protected:
	DefaultIO* dio;
	AnomalyDetectorSharedObjectGenerator* theSharedDetector;
	const string commandDescription;
public:
	Command(DefaultIO* dio, AnomalyDetectorSharedObjectGenerator* theSharedDetector, const string description) : dio(dio), theSharedDetector(theSharedDetector), 
	commandDescription(description){}
	virtual void execute()=0;
	virtual ~Command(){}
	string getDescription() {
		return commandDescription;
	}
	void upload(string file, string serverOutput) {
		ofstream newFile(file);
		dio->write(serverOutput);
		string line1 = "";
		while (line1 != "done") {
			line1 = dio->read();
			newFile << line1 << endl;
		}
		dio->write("Upload complete.\n");
		newFile.close();
	}
};

// implement here your command classes
class UploadData : public Command {
public:
	UploadData(DefaultIO* dio, AnomalyDetectorSharedObjectGenerator* theSharedDetector) : Command(dio, theSharedDetector, "1.upload a time series csv file\n") {
	}
		void execute() {
			string trainFile = "trainFile.csv";
			string testFile = "testFile.csv";
			string trainCmd = "Please upload your local train CSV file.\n";
			string testCmd = "Please upload your local test CSV file.\n";
			upload(trainFile, trainCmd);
			theSharedDetector->trainFile = trainFile;
			//upload test file.
			upload(testFile, testCmd);
			theSharedDetector->testFile = testFile;
		}
};

class Threshold : public Command {
public:
	Threshold(DefaultIO* dio, AnomalyDetectorSharedObjectGenerator* theSharedDetector) : Command(dio, theSharedDetector, "2.algorithm settings\n") {
	}
	void execute() {
		float threshold = theSharedDetector->getThreshold();
		string corr = to_string(threshold);
		//find if the threshold is 1.000 and delete redundant 000 and .
		size_t indexOfOne = corr.find_first_of('1');
		size_t zero = 0;
		if (zero == indexOfOne) {
			corr.erase(1);
		}
		// if threshold equals 0.94300 delete redundant 0.
		if (corr.length() > 2) {
			size_t indexOfZero = (corr.find_last_not_of('0') + 1);
			corr.erase(indexOfZero);
			if (corr.length() > 5) {
				corr.erase(5);
			}
		}
		dio->write("The current correlation threshold is " + corr + "\n" + "Type a new threshold\n");
		string userType = dio->read();
		float newThreshold = std::stof(userType);
		while (newThreshold <= 0 || newThreshold >= 1) {
			dio->write("Please choose a value between 0 to 1.\n");
			dio->write("The current correlation threshold is " + corr + "\n"
				+ "Type a new threshold \n");
			userType = dio->read();
			newThreshold = std::stof(userType);
		}
		theSharedDetector->setNewThreshold(newThreshold);
	}
};

class DetectAnomalies : public Command {
public:
	DetectAnomalies(DefaultIO* dio, AnomalyDetectorSharedObjectGenerator* theSharedDetector) : Command(dio, theSharedDetector, "3.detect anomalies\n") {
	}
	void execute() {
		TimeSeries trainPath = TimeSeries(theSharedDetector->trainFile.c_str());
		TimeSeries testPath = TimeSeries(theSharedDetector->testFile.c_str());
		theSharedDetector->setAnomalyReport(trainPath, testPath);
		dio->write("anomaly detection complete.\n");
	}
};

class ResultDisplayer : public Command {
public:
	ResultDisplayer(DefaultIO* dio, AnomalyDetectorSharedObjectGenerator* theSharedDetector) : Command(dio, theSharedDetector, "4.display results\n") {
	}
	void execute() {
		for (AnomalyReport report : theSharedDetector->getAnomaly()) {
			dio->write(to_string(report.timeStep) + " " + report.description + "\n");
		}
		dio->write("Done. \n");
	}
};

class ResultsAnalyser : public Command {
public:
	ResultsAnalyser(DefaultIO* dio, AnomalyDetectorSharedObjectGenerator* theSharedDetector) : Command(dio, theSharedDetector, "5.upload anomalies and analyze results\n") {
	}
	string deleteRedundant(string str) {
		//check for last number/point that is not 0 after the '.'
		std::size_t lastNum = str.find_last_not_of('0') + 1;
		//earase everything to the right of the last number/point
		string retStr = str.erase(lastNum);
		//find point index
		std::size_t pointIndex = retStr.find_first_of('.');
		//determine whether the point was the last charcter that is not 0
		if (retStr[pointIndex] == retStr[retStr.length() - 1]) {
			return retStr.erase(pointIndex);
		}
		return retStr;
	}

	bool isIntersect(long startUser, long endUser, long startMapped, long endMapped) {
		if (startMapped > endUser || startUser > endMapped) {
			return false;
		}
		return true;
	}

	map<string, vector<long>> readUserReports(string txt) {
		map<string, vector<long>> userReport;
		string line;
		ifstream userFile(txt);
		if (!userFile.is_open()) {
			throw runtime_error("Could not open file");
		}
		while (getline(userFile, line)) {
			if (line == "done") {
				break;
			}
			stringstream stream(line);
			vector<string> times;
			string str;
			//separate the range to start and end, without the ','. start is at 0, and end is at 1. then we clear times for it to be for
			//every iteration
			while (getline(stream, str, ',')) {
				times.push_back(str);
			}
			userReport["startOfAnomalies"].push_back(stol(times.at(0)));
			userReport["endOfAnomalies"].push_back(stol(times.at(1)));
			times.clear();
		}
		return userReport;
	}

	map<string, vector<long>> MappingTheAnomalies(vector<AnomalyReport> arrayOfReports) {
		map<string, vector<long>> mapOfBeginningAndEndingOfAnomaly;
		int i = 0;
		mapOfBeginningAndEndingOfAnomaly["startOfAnomalies"].push_back(theSharedDetector->getAnomaly().at(i).timeStep);
		mapOfBeginningAndEndingOfAnomaly["endOfAnomalies"].push_back(theSharedDetector->getAnomaly().at(i).timeStep);
		string currentDescription = theSharedDetector->getAnomaly().at(i).description;
		//we run here across the array of reports and do two checks here. if the next iteration in the array is differnt then the previous,
		//it means it's a differnt anomaly, hence we can update the start and end of an anomaly. In the other case, we have a long anomaly
		//in the same correlation, meaning we update the end to be the iteration in which the correlation is changed
		for (i = 1; i < arrayOfReports.size(); ++i) {
			if (currentDescription != (theSharedDetector->getAnomaly().at(i).description)) {
				currentDescription = theSharedDetector->getAnomaly().at(i).description;
				mapOfBeginningAndEndingOfAnomaly["startOfAnomalies"].push_back(theSharedDetector->getAnomaly().at(i).timeStep);
				mapOfBeginningAndEndingOfAnomaly["endOfAnomalies"].push_back(theSharedDetector->getAnomaly().at(i).timeStep);

			}
			else {
				mapOfBeginningAndEndingOfAnomaly["endOfAnomalies"].pop_back();
				mapOfBeginningAndEndingOfAnomaly["endOfAnomalies"].push_back(theSharedDetector->getAnomaly().at(i).timeStep);
			}
		}
		return mapOfBeginningAndEndingOfAnomaly;

	}


	void execute() {
		float P, N, sizeOfAnomalies;
		float TP = 0, FP = 0;
		TimeSeries ts(theSharedDetector->testFile.c_str());
		float n = ts.getMatrix().begin()->second.size(); //size of the given features
		map<string, vector<long>> mappedRangesOfAnomalies = MappingTheAnomalies(theSharedDetector->getAnomaly());
		upload("Reports.csv", "Please upload your local anomalies file.\n");
		map<string, vector<long>> userReports = readUserReports("Reports.csv");
		P = userReports["startOfAnomalies"].size(); //number of anomalies ranges
		sizeOfAnomalies = mappedRangesOfAnomalies["startOfAnomalies"].size();
		float x = 0;
		for (int i = 0; i < P; ++i) {
			x += (userReports["endOfAnomalies"][i] - userReports["startOfAnomalies"][i]);
		}
		N = n - x;
		bool fpFlag = false;
		for (int i = 0; i < sizeOfAnomalies; ++i) {
			for (int j = 0; j < P; ++j) {
				// checks if there is an intersection between the report and the anomaly.
				bool itOverlaps = isIntersect(userReports["startOfAnomalies"][j], userReports["endOfAnomalies"][j],
					mappedRangesOfAnomalies["startOfAnomalies"][i], mappedRangesOfAnomalies["endOfAnomalies"][i]);
				if (itOverlaps) {
					TP++;
					fpFlag = true;
				}
			}
			if (!fpFlag)
				FP++;
			fpFlag = false;
		}
		TP = TP / P;
		FP = FP / N;
		string strTP = to_string(TP);
		string strFP = to_string(FP);
		if (strTP.length() > 5) {
			strTP.erase(5);
		}
		strTP = deleteRedundant(strTP);
		if (strFP.length() > 5) {
			strFP.erase(5);
		}
		strFP = deleteRedundant(strFP);
		dio->write("True Positive Rate: " + strTP + "\n");
		dio->write("False Positive Rate: " + strFP + "\n");
	}
};

class ExitCommand : public Command {
public:
	ExitCommand(DefaultIO* dio, AnomalyDetectorSharedObjectGenerator* theSharedDetector) : Command(dio, theSharedDetector, "6.exit\n") {
	}
	void execute() {}
};


#endif /* COMMANDS_H_ */
