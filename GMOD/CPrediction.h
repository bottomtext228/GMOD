#pragma once
class CPed;

class IMoveHelper;

//-----------------------------------------------------------------------------
// Purpose: Engine interface into client side prediction system
//-----------------------------------------------------------------------------
class IPrediction
{
public:
	virtual			~IPrediction(void) {};

	virtual void	Init(void) = 0;
	virtual void	Shutdown(void) = 0;

	// Run prediction
	virtual void	Update
	(
		int startframe,				// World update ( un-modded ) most recently received
		bool validframe,			// Is frame data valid
		int incoming_acknowledged,	// Last command acknowledged to have been run by server (un-modded)
		int outgoing_command		// Last command (most recent) sent to server (un-modded)
	) = 0;

	// We are about to get a network update from the server.  We know the update #, so we can pull any
	//  data purely predicted on the client side and transfer it to the new from data state.
	virtual void	PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet) = 0;
	virtual void	PostEntityPacketReceived(void) = 0;
	virtual void	PostNetworkDataReceived(int commands_acknowledged) = 0;

	virtual void	OnReceivedUncompressedPacket(void) = 0;

	// The engine needs to be able to access a few predicted values
	virtual void	GetViewOrigin(CVector& org) = 0;
	virtual void	SetViewOrigin(CVector& org) = 0;
	virtual void	GetViewAngles(CVector& ang) = 0;
	virtual void	SetViewAngles(CVector& ang) = 0;
	virtual void	GetLocalViewAngles(CVector& ang) = 0;
	virtual void	SetLocalViewAngles(CVector& ang) = 0;
};



class CMoveData;
class CUserCmd;

//-----------------------------------------------------------------------------
// Purpose: Implements prediction in the client .dll
//-----------------------------------------------------------------------------
class CPrediction : public IPrediction
{
	// Construction
public:

	CPrediction(void);
	virtual			~CPrediction(void);

	virtual void	Init(void);
	virtual void	Shutdown(void);

	// Implement IPrediction
public:

	virtual void	Update
	(
		int startframe,		// World update ( un-modded ) most recently received
		bool validframe,		// Is frame data valid
		int incoming_acknowledged, // Last command acknowledged to have been run by server (un-modded)
		int outgoing_command	// Last command (most recent) sent to server (un-modded)
	);

	virtual void	OnReceivedUncompressedPacket(void);

	virtual void	PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet);
	virtual void	PostEntityPacketReceived(void);
	virtual void	PostNetworkDataReceived(int commands_acknowledged);

	virtual bool	InPrediction(void) const;
	virtual bool	IsFirstTimePredicted(void) const;




	// The engine needs to be able to access a few predicted values
	virtual void	GetViewOrigin(CVector& org);
	virtual void	SetViewOrigin(CVector& org);
	virtual void	GetViewAngles(CVector& ang);
	virtual void	SetViewAngles(CVector ang);

	virtual void	GetLocalViewAngles(CVector& ang);
	virtual void	SetLocalViewAngles(CVector& ang);

	virtual void	RunCommand(CPed* player, CUserCmd* ucmd, IMoveHelper* moveHelper);

	// Internal

	virtual void	SetupMove(CPed* player, CUserCmd* ucmd, IMoveHelper* pHelper, CMoveData* move);
	virtual void	FinishMove(CPed* player, CUserCmd* ucmd, CMoveData* move);
	virtual void	SetIdealPitch(CPed* player, const CVector& origin, const CVector& angles, const CVector& viewheight);

	void			CheckError(int commands_acknowledged);

	// Called before and after any movement processing
	void			StartCommand(CPed* player, CUserCmd* cmd);
	void			FinishCommand(CPed* player);

	// Helpers to call pre and post think for player, and to call think if a think function is set
	void			RunPreThink(CPed* player);
	void			RunThink(CPed* ent, double frametime);
	void			RunPostThink(CPed* player);


	virtual void	_Update
	(
		bool received_new_world_update,
		bool validframe,		// Is frame data valid
		int incoming_acknowledged, // Last command acknowledged to have been run by server (un-modded)
		int outgoing_command	// Last command (most recent) sent to server (un-modded)
	);

	// Actually does the prediction work, returns false if an error occurred
	bool			PerformPrediction(bool received_new_world_update, CPed* localPlayer, int incoming_acknowledged, int outgoing_command);

	void			ShiftIntermediateDataForward(int slots_to_remove, int previous_last_slot);
	void			RestoreEntityToPredictedFrame(int predicted_frame);
	int				ComputeFirstCommandToExecute(bool received_new_world_update, int incoming_acknowledged, int outgoing_command);

	void			DumpEntity(CPed* ent, int commands_acknowledged);

	void			ShutdownPredictables(void);
	void			ReinitPredictables(void);

	void			RemoveStalePredictedEntities(int last_command_packet);
	void			RestoreOriginalEntityState(void);
	void			RunSimulation(int current_command, float curtime, CUserCmd* cmd, CPed* localPlayer);
	void			Untouch(void);
	void			StorePredictionResults(int predicted_frame);
	bool			ShouldDumpEntity(CPed* ent);

	void			SmoothViewOnMovingPlatform(CPed* pPlayer, CVector& offset);
	bool			IsEnginePaused() {
		return *(bool*)((uintptr_t)this + 0xB);
	}

};




class CMoveData
{
public:
	bool			m_bFirstRunOfFunctions : 1;
	bool			m_bGameCodeMovedPlayer : 1;

	void* m_nPlayerHandle;	// edict index on server, client entity handle on client

	int				m_nImpulseCommand;	// Impulse command issued.
	CVector			m_vecViewAngles;	// Command view angles (local space)
	CVector			m_vecAbsViewAngles;	// Command view angles (world space)
	int				m_nButtons;			// Attack buttons.
	int				m_nOldButtons;		// From host_client->oldbuttons;
	float			m_flForwardMove;
	float			m_flSideMove;
	float			m_flUpMove;

	float			m_flMaxSpeed;
	float			m_flClientMaxSpeed;

	// Variables from the player edict (sv_player) or entvars on the client.
	// These are copied in here before calling and copied out after calling.
	CVector			m_vecVelocity;		// edict::velocity		// Current movement direction.
	CVector			m_vecAngles;		// edict::angles
	CVector			m_vecOldAngles;

	// Output only
	float			m_outStepHeight;	// how much you climbed this move
	CVector			m_outWishVel;		// This is where you tried 
	CVector			m_outJumpVel;		// This is your jump velocity

	// Movement constraints	(radius 0 means no constraint)
	CVector			m_vecConstraintCenter;
	float			m_flConstraintRadius;
	float			m_flConstraintWidth;
	float			m_flConstraintSpeedFactor;

	CVector			m_vecAbsOrigin;		// edict::origin
};




class IGameMovement
{
public:
	virtual			~IGameMovement(void) {}

	// Process the current movement command
	virtual void	ProcessMovement(CPed* pPlayer, CMoveData* pMove) = 0;
	virtual void	StartTrackPredictionErrors(CPed* pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(CPed* pPlayer) = 0;
	virtual void	DiffPrint(char const* fmt, ...) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual CVector	GetPlayerMins(bool ducked) const = 0;
	virtual CVector	GetPlayerMaxs(bool ducked) const = 0;
	virtual CVector  GetPlayerViewOffset(bool ducked) const = 0;

};


class CGameMovement : public IGameMovement
{
public:

	CGameMovement(void);
	virtual			~CGameMovement(void);

	virtual void	ProcessMovement(CPed* pPlayer, CMoveData* pMove);

	virtual void	StartTrackPredictionErrors(CPed* pPlayer);
	virtual void	FinishTrackPredictionErrors(CPed* pPlayer);
	virtual void	DiffPrint(char const* fmt, ...);
	virtual CVector	GetPlayerMins(bool ducked) const;
	virtual CVector	GetPlayerMaxs(bool ducked) const;
	virtual CVector	GetPlayerViewOffset(bool ducked) const;

	// For sanity checking getting stuck on CMoveData::SetAbsOrigin
	virtual void	TracePlayerBBox(const CVector& start, const CVector& end, unsigned int fMask, int collisionGroup, trace_t& pm);

	// allows derived classes to exclude entities from trace
	virtual void	TryTouchGround(const CVector& start, const CVector& end, const CVector& mins, const CVector& maxs, unsigned int fMask, int collisionGroup, trace_t& pm);


#define BRUSH_ONLY true
	virtual unsigned int PlayerSolidMask(bool brushOnly = false);	///< returns the solid mask for the given player, so bots can have a more-restrictive set
	CPed* player;
	CMoveData* GetMoveData() { return mv; }
public:
	// Input/Output for this movement
	CMoveData* mv;

	int				m_nOldWaterLevel;
	float			m_flWaterEntryTime;
	int				m_nOnLadder;

	CVector			m_vecForward;
	CVector			m_vecRight;
	CVector			m_vecUp;


	// Does most of the player movement logic.
	// Returns with origin, angles, and velocity modified in place.
	// were contacted during the move.
	virtual void	PlayerMove(void);

	// Set ground data, etc.
	void			FinishMove(void);

	virtual float	CalcRoll(const CVector& angles, const CVector& velocity, float rollangle, float rollspeed);

	virtual	void	DecayPunchAngle(void);

	virtual void	CheckWaterJump(void);

	virtual void	WaterMove(void);

	void			WaterJump(void);

	// Handles both ground friction and water friction
	void			Friction(void);

	virtual void	AirAccelerate(CVector& wishdir, float wishspeed, float accel);

	virtual void	AirMove(void);
	virtual float	GetAirSpeedCap(void) { return 30.f; }

	virtual bool	CanAccelerate();
	virtual void	Accelerate(CVector& wishdir, float wishspeed, float accel);

	// Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
	virtual void	WalkMove(void);

	// Try to keep a walking player on the ground when running down slopes etc
	void			StayOnGround(void);

	// Handle MOVETYPE_WALK.
	virtual void	FullWalkMove();

	// allow overridden versions to respond to jumping
	virtual void	OnJump(float fImpulse) {}
	virtual void	OnLand(float fVelocity) {}

	// Implement this if you want to know when the player collides during OnPlayerMove
	virtual void	OnTryPlayerMoveCollision(trace_t& tr) {}

	virtual CVector	GetPlayerMins(void) const; // uses local player
	virtual CVector	GetPlayerMaxs(void) const; // uses local player

	typedef enum
	{
		GROUND = 0,
		STUCK,
		LADDER
	} IntervalType_t;

	virtual int		GetCheckInterval(IntervalType_t type);

	// Useful for things that happen periodically. This lets things happen on the specified interval, but
	// spaces the events onto different frames for different players so they don't all hit their spikes
	// simultaneously.
	bool			CheckInterval(IntervalType_t type);


	// Decompoosed gravity
	void			StartGravity(void);
	void			FinishGravity(void);

	// Apply normal ( undecomposed ) gravity
	void			AddGravity(void);

	// Handle movement in noclip mode.
	void			FullNoClipMove(float factor, float maxacceleration);

	// Returns true if he started a jump (ie: should he play the jump animation)?
	virtual bool	CheckJumpButton(void);	// Overridden by each game.

	// Dead player flying through air., e.g.
	virtual void    FullTossMove(void);

	// Player is a Observer chasing another player
	void			FullObserverMove(void);

	// Handle movement when in MOVETYPE_LADDER mode.
	virtual void	FullLadderMove();

	// The basic solid body movement clip that slides along multiple planes
	virtual int		TryPlayerMove(CVector* pFirstDest = NULL, trace_t* pFirstTrace = NULL);

	virtual bool	LadderMove(void);
	virtual bool	OnLadder(trace_t& trace);
	virtual float	LadderDistance(void);	///< Returns the distance a player can be from a ladder and still attach to it
	virtual unsigned int LadderMask(void);
	virtual float	ClimbSpeed(void);
	virtual float	LadderLateralMultiplier(void);

	// See if the player has a bogus velocity value.
	void			CheckVelocity(void);

	// Does not change the entities velocity at all
	void			PushEntity(CVector& push, trace_t* pTrace);

	// Slide off of the impacting object
	// returns the blocked flags:
	// 0x01 == floor
	// 0x02 == step / wall
	int				ClipVelocity(CVector& in, CVector& normal, CVector& out, float overbounce);

	// If pmove.origin is in a solid position,
	// try nudging slightly on all axis to
	// allow for the cut precision of the net coordinates
	virtual int		CheckStuck(void);

	// Check if the point is in water.
	// Sets refWaterLevel and refWaterType appropriately.
	// If in water, applies current to baseVelocity, and returns true.
	virtual bool			CheckWater(void);

	// Determine if player is in water, on ground, etc.
	virtual void CategorizePosition(void);

	virtual void	CheckParameters(void);

	virtual	void	ReduceTimers(void);

	virtual void	CheckFalling(void);

	virtual void	PlayerRoughLandingEffects(float fvol);

	void			PlayerWaterSounds(void);

	void ResetGetPointContentsCache();
	int GetPointContentsCached(const CVector& point, int slot);

	// Ducking
	virtual void	Duck(void);
	virtual void	HandleDuckingSpeedCrop();
	virtual void	FinishUnDuck(void);
	virtual void	FinishDuck(void);
	virtual bool	CanUnduck();
	void			UpdateDuckJumpEyeOffset(void);
	bool			CanUnDuckJump(trace_t& trace);
	void			StartUnDuckJump(void);
	void			FinishUnDuckJump(trace_t& trace);
	void			SetDuckedEyeOffset(float duckFraction);
	void			FixPlayerCrouchStuck(bool moveup);

	float			SplineFraction(float value, float scale);

	void			CategorizeGroundSurface(trace_t& pm);

	bool			InWater(void);

	// Commander view movement
	void			IsometricMove(void);

	// Traces the player bbox as it is swept from start to end
	virtual void* TestPlayerPosition(const CVector& pos, int collisionGroup, trace_t& pm);

	// Checks to see if we should actually jump 
	void			PlaySwimSound();

	bool			IsDead(void) const;

	// Figures out how the constraint should slow us down
	float			ComputeConstraintSpeedFactor(void);

	virtual void	SetGroundEntity(trace_t* pm);

	virtual void	StepMove(CVector& vecDestination, trace_t& trace);

	// when we step on ground that's too steep, search to see if there's any ground nearby that isn't too steep
	void			TryTouchGroundInQuadrants(const CVector& start, const CVector& end, unsigned int fMask, int collisionGroup, trace_t& pm);
};

class IMoveHelper {
private:
	virtual void UnknownVirtual() = 0;
public:
	virtual void SetHost(CPed* host) = 0;
};