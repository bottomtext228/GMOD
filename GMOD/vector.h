#define M_PI 3.14159265358979323846f

class CVector
{
public:
	float x, y, z;
	inline void Init(float ix, float iy, float iz)
	{
		x = ix; y = iy; z = iz;
	}
	CVector() { x = 0; y = 0; z = 0; };
	CVector(float X, float Y, float Z) { x = X; y = Y; z = Z; };

	float operator[](int i) const { if (i == 1) return x; if (i == 2) return y; return z; };
	float& operator[](int i) { if (i == 1) return x; if (i == 2) return y; return z; };

	bool operator==(const CVector& v) { 
		return (v.x == x && v.y == y && v.z == z);
	}

	bool operator!=(const CVector& v) { 
		return (v.x != x || v.y != y || v.z != z);
	}
	CVector& operator=(const CVector& v) {
		x = v.x; y = v.y; z = v.z;
		return *this;
	}
	CVector toAngle() {
		return CVector(atan2(-z, hypot(x, y)) * (180 / M_PI),
			atan2(y, x) *(180 / M_PI),
			0.0f);
	}


	inline CVector operator-(const CVector& v) { return CVector(x - v.x, y - v.y, z - v.z); }
	inline CVector operator+(const CVector& v) { return CVector(x + v.x, y + v.y, z + v.z); }
	inline CVector operator*(const int n) { return CVector(x * n, y * n, z * n); }
	inline CVector operator*(const float n) { return CVector(x * n, y * n, z * n); }
	inline CVector operator/(const int n) { return CVector(x / n, y / n, z / n); }
	inline CVector operator/(const float n) { return CVector(x / n, y / n, z / n); }

	inline CVector operator-() { return CVector(-x, -y, -z); }
	inline CVector& operator+=(const CVector& v) { x += v.x; y += v.y; z += v.z; return *this; }
	inline CVector& operator-=(const CVector& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	float LengthSqr(void) { return (x * x + y * y + z * z); }
	float DistanceTo(const CVector& v) {
		return sqrtf(powf(x - v.x, 2) + powf(y - v.y, 2) + powf(z - v.z, 2));
	}
};


class CVector4D {
public:
	float x, y, z, w;
};

class CVector2D {
public:
	int x, y;
	float DistanceTo(const CVector2D& v) {
		return sqrtf(powf((float)(x - v.x), 2) + powf((float)(y - v.y), 2));
	}
};

class CVector2DF {
public:
	float x, y;
	inline CVector2DF operator-(const CVector2DF& v) {
		return CVector2DF{ x - v.x, y - v.y };
	}
	float LengthSqr() { 
		return sqrt(x * x + y * y);
	}
	void Normalize() {
		float length = LengthSqr();
		if (length > 0) {
			x = x / length;
			y = y / length;
		}
		else {
			x = 0; 
			y = 0;
		}	
	}
	float DistanceTo(const CVector2DF& v) {
		return sqrtf(powf(x - v.x, 2) + powf(y - v.y, 2));
	}
};

