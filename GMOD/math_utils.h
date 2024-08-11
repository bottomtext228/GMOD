#pragma once

#define M_PI 3.14159265358979323846f
#define RAD2DEG(radians) (radians * (180 / M_PI))
#define DEG2RAD(degrees) (degrees * (M_PI / 180))

class Quaternion {
public:
	float x, y, z, w;
};

struct matrix3x4_t {
	float m[3][4];
	float* operator[](int i) {
		return m[i];
		
	}
};


void MatrixAngles(matrix3x4_t& matrix, float* angles) {

	float forward[3];
	float left[3];
	float up[3];

	//
	// Extract the basis CVectors from the matrix. Since we only need the Z
	// component of the up CVector, we don't get X and Y.
	//

	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

	// enough here to get angles?
	if (xyDist > 0.001f)
	{
		// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
		angles[1] = RAD2DEG(atan2f(forward[1], forward[0]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

		// (roll)	z = ATAN( left.z, up.z );
		angles[2] = RAD2DEG(atan2f(left[2], up[2]));
	}
	else	// forward is mostly Z, gimbal lock-
	{
		// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
		angles[1] = RAD2DEG(atan2f(-left[0], left[1]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

		// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
		angles[2] = 0;
	}
}

inline float DotProductAbs(CVector& v0, float* v1) {
	return fabsf(v0.x * v1[0]) + fabsf(v0.y * v1[1]) + fabsf(v0.z * v1[2]);
}

inline float DotProduct(CVector& a, float* b) {

	return(a.x * b[0] + a.y * b[1] + a.z * b[2]);
}

void VectorTransform(CVector& in1, matrix3x4_t& in2, CVector& out) {
	out.x = DotProduct(in1, in2[0]) + in2[0][3];
	out.y = DotProduct(in1, in2[1]) + in2[1][3];
	out.z = DotProduct(in1, in2[2]) + in2[2][3];
}

void TransformAABB(matrix3x4_t& transform, CVector& vecMinsIn, CVector& vecMaxsIn, CVector& vecMinsOut, CVector& vecMaxsOut) {
	CVector localCenter = vecMinsIn + vecMaxsIn;
	localCenter *= 0.5f;

	CVector localExtents = vecMaxsIn - localCenter;

	CVector worldCenter;
	VectorTransform(localCenter, transform, worldCenter);

	CVector worldExtents;

	worldExtents.x = DotProductAbs(localExtents, transform[0]);
	worldExtents.y = DotProductAbs(localExtents, transform[1]);
	worldExtents.z = DotProductAbs(localExtents, transform[2]);

	vecMinsOut = worldCenter - worldExtents;
	vecMaxsOut = worldCenter + worldExtents;
}


CVector GetAABBCenter(matrix3x4_t& transform, CVector& vecMinsIn, CVector& vecMaxsIn) {
	CVector localCenter = vecMinsIn + vecMaxsIn;
	localCenter *= 0.5f;

	CVector localExtents = vecMaxsIn - localCenter;

	CVector worldCenter;
	VectorTransform(localCenter, transform, worldCenter);
	return worldCenter;
}

