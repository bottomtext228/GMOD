class Quaternion {
public:
	float x, y, z, w;
};

struct matrix3x4_t {
	float m[3][4];
};

struct mstudiobone_t
{
	int					sznameindex;
	inline char* pszName(void) 
	{
		return ((char*)this) + sznameindex;
	}
	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

	// default values
	CVector				pos;
	Quaternion			quat;
	CVector			rot;
	// compression scale
	CVector				posscale;
	CVector				rotscale;

	matrix3x4_t			poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void* pProcedure() const { if (procindex == 0) return NULL; else return  (void*)(((BYTE*)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char* const pszSurfaceProp(void) const { return ((char*)this) + surfacepropidx; }
	int					contents;		// See BSPFlags.h for the contents flags

	int					unused[8];		// remove as appropriate

	mstudiobone_t() {}
private:
	// No copy constructors allowed
	mstudiobone_t(const mstudiobone_t& vOther);
};
struct studiohdr_t {
	int id;
	int version;
	int checksum;        // this has to be the same in the phy and vtx files to load!
	char name[64];
	int length;

	CVector eyeposition;    // ideal eye position
	CVector illumposition;    // illumination center
	CVector hull_min;        // ideal movement hull size
	CVector hull_max;
	CVector view_bbmin;        // clipping bounding box
	CVector view_bbmax;

	int flags;
	int numbones;            // bones
	int boneindex;
	inline mstudiobone_t* pBone(int i) const { return (mstudiobone_t*)(((BYTE*)this) + boneindex) + i; };



	int RemapSeqBone(int iSequence, int iLocalBone) const;    // maps local sequence bone to global bone
	int RemapAnimBone(int iAnim, int iLocalBone) const;        // maps local animations bone to global bone
	int numbonecontrollers;        // bone controllers
	int bonecontrollerindex;
	int numhitboxsets;
	int hitboxsetindex;

	// Look up hitbox set by index
	void* pHitboxSet(int i) const
	{
		return (void*)((((BYTE*)this) + hitboxsetindex) + i);
	};


	// file local animations? and sequences
//private:
	int					numlocalanim;			// animations/poses
	int					localanimindex;		// animation descriptions
	inline BYTE* pLocalAnimdesc(int i) const { if (i < 0 || i >= numlocalanim) i = 0; return (BYTE*)(((BYTE*)this) + localanimindex) + i; };

	int					numlocalseq;				// sequences
	int					localseqindex;
	inline BYTE* pLocalSeqdesc(int i) const { if (i < 0 || i >= numlocalseq) i = 0; return (BYTE*)(((BYTE*)this) + localseqindex) + i; };

	//public:
	bool				SequencesAvailable() const;
	int					GetNumSeq() const;
	void* pAnimdesc(int i) const;
	void* pSeqdesc(int i) const;
	int					iRelativeAnim(int baseseq, int relanim) const;	// maps seq local anim reference to global anim index
	int					iRelativeSeq(int baseseq, int relseq) const;		// maps seq local seq reference to global seq index

//private:
	mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
	mutable int			eventsindexed;
	//public:
	int					GetSequenceActivity(int iSequence);
	void				SetSequenceActivity(int iSequence, int iActivity);
	int					GetActivityListVersion(void);
	void				SetActivityListVersion(int version) const;
	int					GetEventListVersion(void);
	void				SetEventListVersion(int version);

	// raw textures
	int					numtextures;
	int					textureindex;
	inline BYTE* pTexture(int i) const { return (BYTE*)(((BYTE*)this) + textureindex) + i; };


	// raw textures search paths
	int					numcdtextures;
	int					cdtextureindex;
	inline char* pCdtexture(int i) const { return (((char*)this) + *((int*)(((BYTE*)this) + cdtextureindex) + i)); };

	// replaceable textures tables
	int					numskinref;
	int					numskinfamilies;
	int					skinindex;
	inline short* pSkinref(int i) const { return (short*)(((BYTE*)this) + skinindex) + i; };

	int					numbodyparts;
	int					bodypartindex;
	inline BYTE* pBodypart(int i) const { return (BYTE*)(((BYTE*)this) + bodypartindex) + i; };

	// queryable attachable points
//private:
	int					numlocalattachments;
	int					localattachmentindex;
	inline BYTE* pLocalAttachment(int i) const { return (BYTE*)(((BYTE*)this) + localattachmentindex) + i; };
	//public:
	int					GetNumAttachments(void) const;
	const void* pAttachment(int i) const;
	int					GetAttachmentBone(int i);
	// used on my tools in hlmv, not persistant
	void				SetAttachmentBone(int iAttachment, int iBone);

	// animation node to animation node transition graph
//private:
	int					numlocalnodes;
	int					localnodeindex;
	int					localnodenameindex;
	inline char* pszLocalNodeName(int iNode) const { return (((char*)this) + *((int*)(((BYTE*)this) + localnodenameindex) + iNode)); }
	inline BYTE* pLocalTransition(int i) const { return (BYTE*)(((BYTE*)this) + localnodeindex) + i; };

	//public:
	int					EntryNode(int iSequence);
	int					ExitNode(int iSequence);
	char* pszNodeName(int iNode);
	int					GetTransition(int iFrom, int iTo) const;

	int					numflexdesc;
	int					flexdescindex;
	inline BYTE* pFlexdesc(int i) const { return (BYTE*)(((BYTE*)this) + flexdescindex) + i; };

	int					numflexcontrollers;
	int					flexcontrollerindex;
	inline BYTE* pFlexcontroller(int i) const { return (BYTE*)(((BYTE*)this) + flexcontrollerindex) + i; };

	int					numflexrules;
	int					flexruleindex;
	inline BYTE* pFlexRule(int i) const { return (BYTE*)(((BYTE*)this) + flexruleindex) + i; };

	int					numikchains;
	int					ikchainindex;
	inline BYTE* pIKChain(int i) const { return (BYTE*)(((BYTE*)this) + ikchainindex) + i; };

	int					nummouths;
	int					mouthindex;
	inline BYTE* pMouth(int i) const { return (BYTE*)(((BYTE*)this) + mouthindex) + i; };

	//private:
	int					numlocalposeparameters;
	int					localposeparamindex;
	inline BYTE* pLocalPoseParameter(int i) const { return (BYTE*)(((BYTE*)this) + localposeparamindex) + i; };
	//public:
	int					GetNumPoseParameters(void) const;
	const void* pPoseParameter(int i);
	int					GetSharedPoseParameter(int iSequence, int iLocalPose) const;

	int					surfacepropindex;
	inline char* const pszSurfaceProp(void) const { return ((char*)this) + surfacepropindex; }

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char* KeyValueText(void) const { return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : NULL; }

	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;
	inline BYTE* pLocalIKAutoplayLock(int i) const { return (BYTE*)(((BYTE*)this) + localikautoplaylockindex) + i; };

	int					GetNumIKAutoplayLocks(void) const;
	const void* pIKAutoplayLock(int i);
	int					CountAutoplaySequences() const;
	int					CopyAutoplaySequences(unsigned short* pOut, int outCount) const;
	int					GetAutoplayList(unsigned short** pOut) const;

	// The collision model mass that jay wanted
	float				mass;
	int					contents;

	// external animations, models, etc.
	int					numincludemodels;
	int					includemodelindex;
	inline BYTE* pModelGroup(int i) const { return (BYTE*)(((BYTE*)this) + includemodelindex) + i; };
	// implementation specific call to get a named model
	const studiohdr_t* FindModel(void** cache, char const* modelname) const;

	// implementation specific back pointer to virtual data
	mutable void* virtualModel;
	void* GetVirtualModel(void) const;

	// for demand loaded animation blocks
	int					szanimblocknameindex;
	inline char* const pszAnimBlockName(void) const { return ((char*)this) + szanimblocknameindex; }
	int					numanimblocks;
	int					animblockindex;
	/*inline BYTE* pAnimBlock(int i) const { return (BYTE*)(((BYTE*)this) + animblockindex) + i; };
	mutable void* animblockModel;
	BYTE* GetAnimBlock(int i) const;
	*/
	int					bonetablebynameindex;
	inline const BYTE* GetBoneTableSortedByName() const {
		 return(BYTE*)this + bonetablebynameindex;
	}
};
