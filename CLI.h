

#ifndef CLI_H_
#define CLI_H_

#include "stdlib.h"
#include <vector>
#include <string.h>
#include "commands.h"

using namespace std;

class CLI {
	DefaultIO* dio;
	AnomalyDetectorSharedObjectGenerator* theSharedDetector;
	UploadData* uploadDataCommand;
	Threshold* thresholdUserCommand;
	DetectAnomalies* detectAnomaliesCommand;
	ResultDisplayer* displayResultsCommand;
	ResultsAnalyser* analyseResultsCommand;
	ExitCommand* exit;
	vector<Command*> commandsArray;
	// you can add data members
public:
	CLI(DefaultIO* dio);
	void start();
	virtual ~CLI();
};

#endif /* CLI_H_ */
