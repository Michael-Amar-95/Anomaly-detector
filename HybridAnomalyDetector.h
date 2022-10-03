

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
public:
	float cliThreshold = 0.9;
	HybridAnomalyDetector() {}
	virtual ~HybridAnomalyDetector();
    virtual bool isThereDev(Point p, correlatedFeatures corrF);
	virtual correlatedFeatures buildingCorrelation(Point** totalPoints, float curCor, size_t size, string feature1, string feature2);
	virtual bool isThereNormalCorrelation(float correlation);
	float dist(const Point& a, const Point& b);
	float getThreshold() {
		return cliThreshold;
	}
	void setNewThreshold(float newValue) {
		cliThreshold = newValue;
	}
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */




