
#include "Nurbs.h"

#define EPSILON 0.0001f


Nurbs::Nurbs(std::vector<float> &knotVec, std::vector<cugl::Vec4> &ctrlPts, float degree) {

	if (checkParameters(knotVec, ctrlPts, degree)) {
		for (int i = 0; i < knotVec.size(); i++) {
			_knotVec.push_back(knotVec[i]);
		}

		for (int i = 0; i < ctrlPts.size(); i++) {
			_ctrlPts.push_back(ctrlPts[i]);
		}

		_p = degree;
	}

}


// make sure we can construct spline based on ctrlpts, knotvec, and degree
bool Nurbs::checkParameters(std::vector<float> &knotVec, std::vector<cugl::Vec4> &ctrlPts, float degree) {
	int diff = (knotVec.size()-1) - (degree + ctrlPts.size());
	if (diff != 0) {
		CULog("Bad spline arg, excess %d\n", diff);
		return false;
	}
	else {
		return true;
	}

}


/*#https://www.cs.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/de-Boor.html
#assume s <= p
#idea is to increase multiplicity of a knot without changing the curve
#higher multiplicity knots have fewer control points affecting the curve value
#when the multiplicity of a knot equals the degree, then only one control point determines the value
#we can find this control point and say p(u) = p_i where u_i <= u < u_i+1 */
cugl::Vec3 Nurbs::deBoor(float u) {
	cugl::Vec3 out;

	// not defined at ends, so fix to control pts
	if (floatEquals(u,_knotVec[0])) {
		out.x = _ctrlPts[0].x;
		out.y = _ctrlPts[0].y;
		out.z = _ctrlPts[0].z;

		return out;
	}
	else if (floatEquals(u,_knotVec[_knotVec.size()-1])) {
		out.x = _ctrlPts[_ctrlPts.size() - 1].x;
		out.y = _ctrlPts[_ctrlPts.size() - 1].y;
		out.z = _ctrlPts[_ctrlPts.size() - 1].z;

		return out;
	}

	int h = 0; //how many times to "insert" $u as a knot
	int s = 0; //multplicity of $u in original spline
	int k = 0; //find $k such that u_k <= u < u_k+1

	for (int j = 0; j < _knotVec.size() - 1; j++) {
		if (_knotVec[j] <= u && u < _knotVec[j + 1]) {
			k = j;
		}
	}

	//$u is not a knot in the original spline
	if (!floatEquals(u,_knotVec[k])) {
		h = _p;
		s = 0;
	}
	//u is already a knot in the spline
	else {
		//find multiplicity of $u in original spline
		for (int j = 0; j < _knotVec.size(); j++) {
				if (floatEquals(_knotVec[j],u)) {
					s++;
				}
		}

		h = _p - s;
	}



	//collect affected control points
	//cugl::Vec4 p_[_p + 1][_p + 1];
	std::vector<std::vector<cugl::Vec4>> p_;
	p_.resize(k - s + 1);
	for (int j = 0; j < p_.size(); j++) {
		p_[j].resize(h+1);
	}

	for (int j = (k - s); j >= 0 && j >= (k - _p); j--) {
		float weight = _ctrlPts[j].w;
		p_[j][0].x = weight * _ctrlPts[j].x;
		p_[j][0].y = weight * _ctrlPts[j].y;
		p_[j][0].z = weight * _ctrlPts[j].z;
		p_[j][0].w = weight;
	}

	std::vector<std::vector<float>> a_;
	a_.resize((k - _p + h) + (k - s) - (k - _p) + 1);
	for (int j = 0; j < a_.size(); j++) {
		a_[j].resize(h+1);
	}

	for (int r = 1; r <= h; r++) {
		//column in tree
		for (int i = k - _p + r; i <= k - s; i++) {
			//row in tree
			a_[i][r] = (u - _knotVec[i]) / (_knotVec[i + _p - r + 1] - _knotVec[i]);

			//do x and y coordinates
			p_[i][r] = (1 - a_[i][r])*p_[i - 1][r - 1] + a_[i][r] * p_[i][r - 1];
		}
	}


	int kms = k - s;
	int pms = _p - s;

	// return point
	float weight = p_[kms][pms].w;
	out = cugl::Vec3(p_[kms][pms].x, p_[kms][pms].y, p_[kms][pms].z);
	out /= weight;
	return out;

}


bool Nurbs::floatEquals(float a, float b) {
	if (fabs(a - b) < EPSILON) {
		return true;
	}
	else {
		return false;
	}
}

cugl::Vec3 Nurbs::get3DPt(float t) {
	return deBoor(t);
}

cugl::Vec2 Nurbs::get2DPt(float t) {
	cugl::Vec3 db = deBoor(t);

	cugl::Vec2 ret = cugl::Vec2(db.x, db.y);
	return ret;
}

float Nurbs::get1DPt(float t) {
	cugl::Vec3 db = deBoor(t);

	float ret = db.x;
	return ret;
}