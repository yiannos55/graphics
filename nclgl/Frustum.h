#pragma once
#include "Plane.h"
#include "Matrix4.h"
#include "../Graphics For Games/7) Scene Management/SceneNode.h"

class Frustum{
public:
	Frustum(void) {};
	~Frustum(void) {};
	void FromMatrix(const Matrix4& mvp);
	bool InsideFrustum(SceneNode& n);

protected:
	Plane planes[6];
};

