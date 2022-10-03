/*
 * anomaly_detection_util.cpp
 *
 * Michael Amar
 */

#include "anomaly_detection_util.h"
#include <math.h>
#include <cmath>

float expectedValue(float *x, int size) {
    float sum = 0;
    for (int i = 0; i < size; ++i) {
        sum = sum + x[i];
    }
    return sum / (float) size;
}

float var(float *x, int size) {
    float var = 0;
    float expected_Value = expectedValue(x, size);
    for (int i = 0; i < size; ++i) {
        var = var + pow(x[i], 2);
    }
    var = var / float(size);
    var = var - pow(expected_Value,2);
    return var;
}

float cov(float *x, float *y, int size) {
    float y_expectedValue = expectedValue(y, size);
    float x_expectedValue = expectedValue(x, size);
    float sum = 0;
    for (int i = 0; i < size; i++) {
        sum += (x[i] - x_expectedValue) * (y[i] - y_expectedValue);
    }
    float cov_x_y;
    cov_x_y = sum / (float)size;
    return cov_x_y;
}

float pearson(float *x, float *y, int size) {
    float numerator = cov(x, y, size);
    float denominator = sqrt(var(x,size)) * sqrt(var(y,size));
    return numerator/denominator;

}

// we create an array of x's from points
static float *from_point_to_x(Point **points, int size)
{
    float *fromPointToFloat = new float[size];
    for (int i = 0; i < size; i++) {
        fromPointToFloat[i] = points[i]->x;
    }
    return fromPointToFloat;
}

// we create an array of y's from points
static float *from_point_to_y(Point **points, int size)
{
    float *fromPointToFloat = new float[size];
    for (int i = 0; i < size; i++) {
        fromPointToFloat[i] = points[i]->y;
    }
    return fromPointToFloat;
}

// performs a linear regression and return s the line equation
Line linear_reg(Point **points, int size)
{
    float *fromPointToX = from_point_to_x(points, size);
    float *fromPointToY = from_point_to_y(points, size);
    //float xAverage = expectedValue(fromPointToX, size);
    //float yAverage = expectedValue(fromPointToY, size);
    //float varA = (cov(fromPointToX, fromPointToY, size)) / (var(fromPointToX, size));
    //float varB = (yAverage) - (varA * xAverage);
    float a,b;
    a = cov(fromPointToX, fromPointToY, size) / var(fromPointToX, size);
    b = expectedValue(fromPointToY, size) - (a * expectedValue(fromPointToX, size));
    //delete[] fromPointToY;
    //delete[] fromPointToX;
    Line lineReg(a, b);
    return lineReg;
}

// returns the deviation between point p and the line equation of the points
float dev(Point p, Point **points, int size)
{
	Line line = linear_reg(points, size);
	double y = (line.a * p.x) + line.b;
	double dev1 = std::abs(p.y - y);
	return (float)dev1;

//    Line theLineFromThePoints = linear_reg(points, size);
//    if (theLineFromThePoints.f(p.x) == p.y)
//    {
//        return 0;
//    } else
//    {
//        if ((theLineFromThePoints.f(p.x) - p.y) > 0)
//        {
//            return (theLineFromThePoints.f(p.x) - p.y);
//        }
//        else
//        {
//            return (p.y - (theLineFromThePoints.f(p.x)));
//        }
//    }
}

// returns the deviation between point p and the line
float dev(Point p, Line l)
{
	double y = (p.x * l.a) + l.b;
	double dev1 = std::abs(y - p.y);
	return (float)dev1;
//    if (l.f(p.x) == p.y)
//    {
//        return 0;
//    }
//    else
//    {
//        if ((l.f(p.x) - p.y) > 0)
//        {
//            return (l.f(p.x) - p.y);
//        }
//        else
//        {
//            return (p.y - (l.f(p.x)));
//        }
//    }
}