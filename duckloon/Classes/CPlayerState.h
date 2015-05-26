#ifndef __CPlayerState_H__
#define __CPlayerState_H__


class CPlayerState
{
public:
	enum enumPlayerMode  //deals with what the player character is currently doing
	{
		STANDING,
		CROUCHING,
		SHOOTING,
		JUMPING,
		FALLING,
		INJURED,
		DEAD,
		UNKNOWN
	};

	enum enumHeldSubstance //determines what substance the duckloon is carrying which is used for it's physics and ability to fire
	{
		WATER = (UNKNOWN + 1),
		AIR,
		HELIUM,
		EMPTY
	};

	enumPlayerMode currentPlayerMode;
	enumHeldSubstance currentHeldSubstance;
	bool bHasDoubleJump;


	CPlayerState() :
		currentPlayerMode(UNKNOWN),
		currentHeldSubstance(EMPTY),
		bHasDoubleJump(false)
	{};


};

#endif