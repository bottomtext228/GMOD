#define  Assert( _exp ) ((void)0)
constexpr float MAX_TRACE_LENGTH = 56754.0f; // https://developer.valvesoftware.com/wiki/UTIL_TraceLine

#include "masks.h"
class __declspec(align(16))VectorAligned : public CVector
{
public:
	VectorAligned& operator=(const CVector& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}
	float w;
};
struct Ray_t
{
	VectorAligned  m_Start;    // starting point, centered within the extents
	VectorAligned  m_Delta;    // direction + length of the ray
	VectorAligned  m_StartOffset;    // Add this to m_Start to get the actual ray start
	VectorAligned  m_Extents;    // Describes an axis aligned box extruded along a ray
	//const matrix3x4_t *m_pWorldAxisTransform;
	bool    m_IsRay;    // are the extents zero?
	bool    m_IsSwept;    // is delta != 0?

	void Init(CVector& vecStart, CVector& vecEnd)
	{
		m_Delta = vecEnd - vecStart;

		m_IsSwept = (m_Delta.Length() != 0);

		m_Extents.x = m_Extents.y = m_Extents.z = 0.0f;
		m_IsRay = true;

		m_StartOffset.x = m_StartOffset.y = m_StartOffset.z = 0.0f;

		m_Start = vecStart;
	}


private:
};

struct trace_t
{
	CVector start;
	CVector end;
	BYTE plane[20];
	float flFraction;
	int contents;
	WORD dispFlags;
	bool allSolid;
	bool startSolid;
	float fractionLeftSolid;
	BYTE surface[8];
	int hitGroup;
	short physicsBone;
	CPed* ped;
	int hitbox;
};
enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};
class ITraceFilter
{
public:
	virtual bool			ShouldHitEntity(void* pEntity, int mask) = 0;
	virtual TraceType_t            GetTraceType() const = 0;
	virtual int Unknown() const = 0;
};


class CTraceFilter : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip1);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	int Unknown() const {
		return 0;
	}

	void* pSkip1;
};

class CTrace
{
public:
	void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* filter, trace_t* trace)
	{
		typedef void(__thiscall* fn)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
		VMT.getvfunc<fn>(this, 4)(this, ray, fMask, filter, trace);
	}
	trace_t TraceRayWrapper(CVector from, CVector to,  void* pSkip) {
		trace_t trace; 
		Ray_t ray;
		CTraceFilter tracefilter;
		tracefilter.pSkip1 = pSkip;
		ray.Init(from, to);
		TraceRay(ray, MASK_SHOT, &tracefilter, &trace);
		
		return trace;
	}
};