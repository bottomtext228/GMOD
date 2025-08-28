#pragma once


class IVRenderView
{
public:

	// Draw normal brush model.
	// If pMaterialOverride is non-null, then all the faces of the bmodel will
	// set this material rather than their regular material.
	virtual void			DrawBrushModel(
		CPed* baseentity,
		modelInfo* model,
		const CVector& origin,
		const void* angles,
		bool bUnused) = 0;

	// Draw brush model that has no origin/angles change ( uses identity transform )
	// FIXME, Material proxy CPed *baseentity is unused right now, use DrawBrushModel for brushes with
	//  proxies for now.
	virtual void			DrawIdentityBrushModel(void* pList, modelInfo* model) = 0;

	// Mark this dynamic light as having changed this frame ( so light maps affected will be recomputed )
	virtual void			TouchLight(struct dlight_t* light) = 0;
	// Draw 3D Overlays
	virtual void			Draw3DDebugOverlays(void) = 0;
	// Sets global blending fraction
	virtual void			SetBlend(float blend) = 0;
	virtual float			GetBlend(void) = 0;

	// Sets global color modulation
	virtual void			SetColorModulation(float const* blend) = 0;
	virtual void			GetColorModulation(float* blend) = 0;

	// Wrap entire scene drawing
	virtual void			SceneBegin(void) = 0;
	virtual void			SceneEnd(void) = 0;

	// Gets the fog volume for a particular point
	virtual void			GetVisibleFogVolume(const CVector& eyePoint, void* pInfo) = 0;

	// Wraps world drawing
	// If iForceViewLeaf is not -1, then it uses the specified leaf as your starting area for setting up area portal culling.
	// This is used by water since your reflected view origin is often in solid space, but we still want to treat it as though
	// the first portal we're looking out of is a water portal, so our view effectively originates under the water.
	virtual void** CreateWorldList() = 0;

	virtual void			BuildWorldLists(void* pList, void* pInfo, int iForceFViewLeaf, const void* pVisData = NULL, bool bShadowDepth = false, float* pReflectionWaterHeight = NULL) = 0;
	virtual void			DrawWorldLists(void* pList, unsigned long flags, float waterZAdjust) = 0;

	// Optimization for top view
	virtual void			DrawTopView(bool enable) = 0;
	virtual void			TopViewBounds(CVector2D const& mins, CVector2D const& maxs) = 0;

	// Draw lights
	virtual void			DrawLights(void) = 0;
	// FIXME:  This function is a stub, doesn't do anything in the engine right now
	virtual void			DrawMaskEntities(void) = 0;

	// Draw surfaces with alpha
	virtual void			DrawTranslucentSurfaces(void* pList, int sortIndex, unsigned long flags, bool bShadowDepth) = 0;

	// Draw Particles ( just draws the linefine for debugging map leaks )
	virtual void			DrawLineFile(void) = 0;
	// Draw lightmaps
	virtual void			DrawLightmaps(void* pList, int pageId) = 0;
	// Wraps view render sequence, sets up a view
	virtual void			ViewSetupVis(bool novis, int numorigins, const CVector origin[]) = 0;

	// Return true if any of these leaves are visible in the current PVS.
	virtual bool			AreAnyLeavesVisible(int* leafList, int nLeaves) = 0;

	virtual	void			VguiPaint(void) = 0;
	// Sets up view fade parameters
	virtual void			ViewDrawFade(int8_t* color, void* pMaterial) = 0;
	// Sets up the projection matrix for the specified field of view
	virtual void			OLD_SetProjectionMatrix(float fov, float zNear, float zFar) = 0;
	// Determine lighting at specified position
	virtual void* GetLightAtPoint(CVector& pos) = 0;
	// Whose eyes are we looking through?
	virtual int				GetViewEntity(void) = 0;
	// Get engine field of view setting
	virtual float			GetFieldOfView(void) = 0;
	// 1 == ducking, 0 == not
	virtual unsigned char** GetAreaBits(void) = 0;

	// Set up fog for a particular leaf
	virtual void			SetFogVolumeState(int nVisibleFogVolume, bool bUseHeightFog) = 0;

	// Installs a brush surface draw override method, null means use normal renderer
	virtual void			InstallBrushSurfaceRenderer(void* pBrushRenderer) = 0;

	// Draw brush model shadow
	virtual void			DrawBrushModelShadow(IClientRenderable* pRenderable) = 0;

	// Does the leaf contain translucent surfaces?
	virtual	bool			LeafContainsTranslucentSurfaces(void* pList, int sortIndex, unsigned long flags) = 0;

	virtual bool			DoesBoxIntersectWaterVolume(const CVector& mins, const CVector& maxs, int leafWaterDataID) = 0;

	virtual void			SetAreaState(
		unsigned char chAreaBits[1],
		unsigned char chAreaPortalBits[1]) = 0;

	// See i
	virtual void			VGui_Paint(int mode) = 0;

	// Push, pop views (see PushViewFlags_t above for flags)
	virtual void			Push3DView(const void* view, int nFlags, void* pRenderTarget, void* voidPlanes) = 0;
	virtual void			Push2DView(const void* view, int nFlags, void* pRenderTarget, void* voidPlanes) = 0;
	virtual void			PopView(void* voidPlanes) = 0;

	// Sets the main view
	virtual void			SetMainView(const CVector& vecOrigin, const CVector& angles) = 0;

	enum
	{
		VIEW_SETUP_VIS_EX_RETURN_FLAGS_USES_RADIAL_VIS = 0x00000001
	};

	// Wraps view render sequence, sets up a view
	virtual void			ViewSetupVisEx(bool novis, int numorigins, const CVector origin[], unsigned int& returnFlags) = 0;

	//replaces the current view void with a rhyming replacement of your choice
	virtual void			OverrideViewvoid(void* custom) = 0;

	virtual void			DrawBrushModelShadowDepth(CPed* baseentity, modelInfo* model, const CVector& origin, const void* angles, int DepthMode) = 0;
	virtual void			UpdateBrushModelLightmap(modelInfo* model, IClientRenderable* pRenderable) = 0;
	virtual void			BeginUpdateLightmaps(void) = 0;
	virtual void			EndUpdateLightmaps(void) = 0;
	virtual void			OLD_SetOffCenterProjectionMatrix(float fov, float zNear, float zFar, float flAspectRatio, float flBottom, float flTop, float flLeft, float flRight) = 0;
	virtual void			OLD_SetProjectionMatrixOrtho(float left, float top, float right, float bottom, float zNear, float zFar) = 0;
	virtual void			Push3DView(const void* view, int nFlags, void* pRenderTarget, void* voidPlanes, void* pDepthTexture) = 0;
	virtual void			GetMatricesForView(const void* view, void* pWorldToView, void* pViewToProjection, void* pWorldToProjection, void* pWorldToPixels) = 0;
	virtual void			DrawBrushModelEx(CPed* baseentity, modelInfo* model, const CVector& origin, const void* angles, int mode) = 0;
};
