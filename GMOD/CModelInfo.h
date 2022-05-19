
class CModelInfo
{
public:

	studiohdr_t* GetStudiomodel(modelInfo* model) {
		typedef studiohdr_t* (__thiscall* fn)(void*, modelInfo*);
		return VMT.getvfunc<fn>(this, 28)(this, model);
	}
};

