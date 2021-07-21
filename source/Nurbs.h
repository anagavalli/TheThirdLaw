#ifndef __NURBS_H_
#define __NURBS_H_

#include "cugl/cugl.h"

class Nurbs {
private:
	std::vector<float> _knotVec;
	std::vector<cugl::Vec4> _ctrlPts;
	int _p; //degree


	cugl::Vec3 deBoor(float u);
	bool floatEquals(float a, float b);

public:

	Nurbs(std::vector<float> &knotVec, std::vector<cugl::Vec4> &ctrlPts, float degree);
	~Nurbs() {}

	bool checkParameters(std::vector<float> &knotVec, std::vector<cugl::Vec4> &ctrlPts, float degree);

	cugl::Vec3 get3DPt(float t);
	cugl::Vec2 get2DPt(float t);
	float get1DPt(float t);
};


#endif
