
#include "SimpleAnomalyDetector.h"
#define LINE_REG_THRESHOLD 0.9
#define CIRCLE_REG_THRESHOLD 0.5
#define NO_FALSE_ALARMS_THRESHOLD 1.1

SimpleAnomalyDetector::SimpleAnomalyDetector() {
	// TODO Auto-generated constructor stub

}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
	// TODO Auto-generated destructor stub
}

bool SimpleAnomalyDetector::isThereNormalCorrelation(float correlation) {
	if (correlation >= LINE_REG_THRESHOLD) {
		return true;
	}
	else {
		return false;
	}
}

bool SimpleAnomalyDetector::noPreviousCorrelation(const string &feat1, const string &feat2) {
	//scan the vector of correlated objects.
	for (auto iter = cf.begin(); iter != cf.end(); ++iter) {
		if (iter->feature1 == feat2 && iter->feature2 == feat1) {
			return false;
		}
	}
	return true;
}

correlatedFeatures SimpleAnomalyDetector::buildingCorrelation(Point** totalPoints, float curCor, size_t size, string feature1, string feature2) {
	correlatedFeatures corrF;
	corrF.feature1 = feature1;
	corrF.feature2 = feature2;
	corrF.corrlation = curCor;
	corrF.threshold = 0;
	//create the line regression for this struct.
	corrF.lin_reg = linear_reg(totalPoints, (int)size);
	// find the maximum deviation from the line.
	float maxDeviation = 0.0;
	for (size_t i = 0; i < size; ++i) {
		float dev1 = dev(*totalPoints[i], corrF.lin_reg);
		if (dev1 > maxDeviation) {
			maxDeviation = dev1;
		}
	}
	corrF.threshold = maxDeviation * (float)NO_FALSE_ALARMS_THRESHOLD;
	return corrF;
}

void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts){
	for (auto const& it1 : ts.getFeatureNames()) {
		string feature1 = it1;
		string feature2;
		// the current max correlation between two features.
		float curCor = 0;
		for (auto const& it2 : ts.getFeatureNames()) {
			if (it1 != it2) {
				// fill the arrays data from the timeseries vectors.
				auto value1 = ts.getValues(it1);
				auto value2 = ts.getValues(it2);
				float* x = new float[value1.size()];
				float* y = new float[value2.size()];
				for (size_t i = 0; i < value2.size(); ++i) {
					x[i] = value1[i];
					y[i] = value2[i];
				}
				float p = fabs(pearson(x, y, (int)value2.size()));
				//if the pearson value is at least 0.9
				// and the pearson bigger then the last max correlation which checked.
				if (isThereNormalCorrelation(p) && p > curCor) {
					feature2 = it2;
					curCor = p;
				}
				delete [] x;
				delete [] y;
			}
		}
		// if there is a value in max and no previous correlation between feat1 and 2
		if (curCor > 0 && noPreviousCorrelation(feature1, feature2)) {
			// fill correlatedfeatures struct.
			size_t size = ts.getValues(it1).size();
			float* xPoints = new float[size];
			float* yPoints = new float[size];
			Point** totalPoints = new Point*[size];
			//create arrays to convert them to points.
			for (size_t j = 0; j < size; ++j) {
				xPoints[j] = ts.getValues(feature1)[j];
				yPoints[j] = ts.getValues(feature2)[j];
				totalPoints[j] = new Point(xPoints[j], yPoints[j]);
			}
			correlatedFeatures corrF = buildingCorrelation(totalPoints, curCor, size, feature1, feature2);
			//push the object that has made to threshold vector.
			cf.push_back(corrF);
			for (size_t j = 0; j < size; ++j)
			{
				delete totalPoints[j];
			}
			delete[] totalPoints;
			delete[] yPoints;
			delete[] xPoints;
		}
	}
}

//check if there is deviation between the point to thr line regression
bool SimpleAnomalyDetector::isThereDev(Point point, correlatedFeatures corrF) {
	float newDev = dev(point, corrF.lin_reg);
	// this condition tell us if there is there is bigger deviation.
	if (newDev > corrF.threshold) {
		return true;
	}
	return false;
}


vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
	vector<AnomalyReport> anomalyArray;
	for (correlatedFeatures corrF : cf)
	{
		string feat1 = corrF.feature1;
		string feat2 = corrF.feature2;
		size_t size = ts.getValues(feat1).size();
		for (size_t i = 0; i < size; i++)
		{
			// we now get the dev of every point and check whether it's an anomaly!
			Point currPoint(ts.getValues(feat1)[i], ts.getValues(feat2)[i]);
			if (isThereDev(currPoint, corrF))
			{
				//adding the point to the anomaly array via a new report
				AnomalyReport anomalyReport(feat1 + "-" + feat2, (long)(i + 1));
				anomalyArray.push_back(anomalyReport);
			}
		}
	}
	return anomalyArray;
}

