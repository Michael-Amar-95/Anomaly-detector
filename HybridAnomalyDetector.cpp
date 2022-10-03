
#include "HybridAnomalyDetector.h"

#define CIRCLE_REG_THRESHOLD 0.5
#define LINE_REG_THRESHOLD 0.9
#define NO_FALSE_ALARMS_THRESHOLD 1.1

HybridAnomalyDetector::~HybridAnomalyDetector() {
	// TODO Auto-generated destructor stub
}

//making a distinction between the Hybrid detector to the Simple.
bool HybridAnomalyDetector::isThereNormalCorrelation(float p) {
    if (p < LINE_REG_THRESHOLD && p > CIRCLE_REG_THRESHOLD) {
        return true;
    } else {
        return SimpleAnomalyDetector::isThereNormalCorrelation(p);
    }
}

float HybridAnomalyDetector::dist(const Point& a, const Point& b)
{
	return sqrt(pow(a.x - b.x, (float)2)
		+ pow(a.y - b.y, (float)2));
}

//check if this point is inside the circle and return false if it isn't.
bool HybridAnomalyDetector::isThereDev(Point p, correlatedFeatures corrF) {
    if (corrF.corrlation < LINE_REG_THRESHOLD && corrF.corrlation > CIRCLE_REG_THRESHOLD) {
        //build circle from center point and the threshold(radius*1.1).
        Circle c(corrF.reg_circle.center, corrF.threshold);
		float distanceBetweenPointAndCenter = dist(c.center, p);
        if (distanceBetweenPointAndCenter > corrF.threshold) {
            //there is deviation!
            return true;
        }
        return false;
    }
    return SimpleAnomalyDetector::isThereDev(p, corrF);
}

correlatedFeatures HybridAnomalyDetector::buildingCorrelation(Point** totalPoints, float curCor, size_t size, string feature1,
	string feature2) {                                                 
    //if this condition occur wh have to create corrF with the minCircle class.
	if (curCor < LINE_REG_THRESHOLD && curCor > CIRCLE_REG_THRESHOLD) {
        // fill correlatedfeatures struct.
		correlatedFeatures corrF;
		corrF.feature1 = feature1;
		corrF.feature2 = feature2;
		corrF.threshold = 0.0;
		corrF.corrlation = curCor;
        corrF.reg_circle = findMinCircle(totalPoints, size);
        corrF.threshold = (float)(corrF.reg_circle.radius * (NO_FALSE_ALARMS_THRESHOLD));
        return corrF;
    } else {
        //if i >= the min corrlation which had set.
	    return SimpleAnomalyDetector::buildingCorrelation(totalPoints, curCor, size, feature1, feature2);
    }
}