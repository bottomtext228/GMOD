#pragma once


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
