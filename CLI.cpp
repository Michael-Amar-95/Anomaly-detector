#include "CLI.h"

CLI::CLI(DefaultIO *dio1) {
    dio = dio1;
    theSharedDetector = new AnomalyDetectorSharedObjectGenerator;
    //we now upload a shared (single) object to every command, so that a change in it in one command, would be shown via it to other commands
    uploadDataCommand = new UploadData(dio, theSharedDetector);
    thresholdUserCommand = new Threshold(dio, theSharedDetector);
    detectAnomaliesCommand = new DetectAnomalies(dio, theSharedDetector);
    displayResultsCommand = new ResultDisplayer(dio, theSharedDetector);
    analyseResultsCommand = new ResultsAnalyser(dio, theSharedDetector);
    exit = new ExitCommand(dio, theSharedDetector);
    //the order of every Push is important!
    commandsArray.push_back(uploadDataCommand);
    commandsArray.push_back(thresholdUserCommand);
    commandsArray.push_back(detectAnomaliesCommand);
    commandsArray.push_back(displayResultsCommand);
    commandsArray.push_back(analyseResultsCommand);
    commandsArray.push_back(exit);
}

void CLI::start() {
    int userChoice = 0;
    while (userChoice != 6) {
        dio->write("Welcome to the Anomaly Detection Server.\n");
        dio->write("Please choose an option:\n");
        for (int i = 0; i < commandsArray.size(); i++) {
            dio->write(commandsArray.at(i)->getDescription());
        }
        userChoice = atoi(dio->read().c_str());
        if (userChoice > 0 && userChoice < 6) {
            commandsArray.at(userChoice - 1)->execute();
        }
    }
}


CLI::~CLI() {
    delete exit;
    delete analyseResultsCommand;
    delete displayResultsCommand;
    delete detectAnomaliesCommand;
    delete thresholdUserCommand;
    delete uploadDataCommand;
    delete theSharedDetector;
}

