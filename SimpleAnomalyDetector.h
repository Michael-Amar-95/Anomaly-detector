

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include "minCircle.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures{
	string feature1, feature2;  // names of the correlated features
	float corrlation;
	Line lin_reg;
	float threshold;
	Circle reg_circle;

	correlatedFeatures() : reg_circle({ 0.0, 0.0 }, 0.0) {}
};


class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
	vector<correlatedFeatures> cf;
public:
	SimpleAnomalyDetector();
	virtual ~SimpleAnomalyDetector();
	virtual bool isThereNormalCorrelation(float correlation);
	virtual bool noPreviousCorrelation(const string &feat1, const string &feat2);
	virtual correlatedFeatures buildingCorrelation(Point** totalPoints, float curCor, size_t size, string feature1, string feature2);
	virtual void learnNormal(const TimeSeries& ts);
	virtual bool isThereDev(Point point, correlatedFeatures corrF);
	virtual vector<AnomalyReport> detect(const TimeSeries& ts);

	vector<correlatedFeatures> getNormalModel(){
		return cf;
	}

};



#endif /* SIMPLEANOMALYDETECTOR_H_ */
