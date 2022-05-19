class IClientRenderable
{
public:
	bool SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) {
		typedef bool(__thiscall* fn)(void*, matrix3x4_t*, int, int, float);
		return VMT.getvfunc<fn>(this, 16)(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
	/*
	modelInfo* GetModel() {
		typedef modelInfo*(__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 9)(this);
	}
	*/
};