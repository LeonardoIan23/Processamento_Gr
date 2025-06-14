#include "ltMath.h"
#include <cmath>

float calculateTriangleArea(float x1, float y1, float x2, float y2, float x3, float y3) {
    return fabs((x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2)) / 2.0f);
}

bool triangleCollidePoint2D(float* abc, float* point) {
    float Ax = abc[0], Ay = abc[1];
    float Bx = abc[2], By = abc[3];
    float Cx = abc[4], Cy = abc[5];

    float Px = point[0], Py = point[1];

    float areaABC = calculateTriangleArea(Ax, Ay, Bx, By, Cx, Cy);

    float areaPBC = calculateTriangleArea(Px, Py, Bx, By, Cx, Cy);
    float areaPAC = calculateTriangleArea(Ax, Ay, Px, Py, Cx, Cy);
    float areaPAB = calculateTriangleArea(Ax, Ay, Bx, By, Px, Py);

    return (areaABC >= (areaPBC + areaPAC + areaPAB - 0.0001f)) &&
           (areaABC <= (areaPBC + areaPAC + areaPAB + 0.0001f));
}