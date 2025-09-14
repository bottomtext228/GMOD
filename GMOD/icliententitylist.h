class IClientEntityList
{
public:
	// Get IClientNetworkable interface for specified entity
	virtual IClientNetworkable* GetClientNetworkable(int entnum) = 0;
	virtual IClientNetworkable* GetClientNetworkableFromHandle(void* hEnt) = 0;
	virtual void* GetClientUnknownFromHandle(void* hEnt) = 0;

	// NOTE: This function is only a convenience wrapper.
	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
	virtual CPed* GetClientEntity(int entnum) = 0;
private:
#ifndef _WIN64
	virtual CPed* GetClientEntityFromHandle_(uint32_t hEnt) = 0;
#else
	virtual CPed* GetClientEntityFromHandle_(uint32_t* hEnt) = 0; // I don't know why it is so in x64
#endif
public:
	// Returns number of entities currently in use
	virtual int					NumberOfEntities(bool bIncludeNonNetworkable) = 0;

	// Returns highest index actually used
	virtual int					GetHighestEntityIndex(void) = 0;

	// Sizes entity list to specified size
	virtual void				SetMaxEntities(int maxents) = 0;
	virtual int					GetMaxEntities() = 0;

	CPed* GetClientEntityFromHandle(uint32_t hEnt) {
#ifdef _WIN64
		return GetClientEntityFromHandle_(&hEnt);
#else
		return GetClientEntityFromHandle_(hEnt);
#endif
	}
};

