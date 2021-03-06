#include "CXBoxController.h"
#include <math.h>	// for sqrt() and fabs() in joystick thingy
#include <algorithm>

CXBoxController::CXBoxController( int playerNumber ) 
{
	this->m_playerNumber = playerNumber;
	ZeroMemory( &this->m_State, sizeof( this->m_State ) );
	this->m_deadZoneCalculation = IGameController::SCALED_RADIAL_DEAD_ZONE;
	this->m_deadZone = CXBoxController::DEAD_ZONE_DEFAULT;
	return;
}
	
CXBoxController::~CXBoxController() 
{
	// Stop any vibration, just in case
	this->Vibrate( static_cast<unsigned int>(0), static_cast<unsigned int>(0) );
	return;
}



IGameController::enumDeadZoneCalculation CXBoxController::getDeadZoneCalculation(void)
{
	return this->m_deadZoneCalculation;
}

void CXBoxController::setDeadZoneCalculation( IGameController::enumDeadZoneCalculation newDeadZoneSetting)
{
	this->m_deadZoneCalculation = newDeadZoneSetting;
	return;
}


std::string CXBoxController::getDeadZoneCalculationString(void)
{
	switch ( this->m_deadZoneCalculation )
	{
	case IGameController::NO_DEAD_ZONE:
		return "No Dead Zone";
		break;
	case IGameController::AXIAL_DEAD_ZONE:
		return "Axial Dead Zone";
		break;
	case IGameController::RADIAL_DEAD_ZONE:
		return "Radial Dead Zone";
		break;
	case IGameController::SCALED_RADIAL_DEAD_ZONE:
		return "Scaled Radial Dead Zone";
		break;
	}
	return "Unknown";
}

/*static*/ 
const float CXBoxController::DEAD_ZONE_DEFAULT = 0.1f;

void CXBoxController::setDeadZone(float newDeadZone)
{
	this->m_deadZone = newDeadZone;
	return;
}

float CXBoxController::getDeadZone(void)
{
	return this->m_deadZone;
}

// from: http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
void CXBoxController::m_adjustForDeadZone(void)
{
	this->m_UpdateLeftJoystickValues();
	this->m_UpdateRightJoystickValues();

	switch ( this->m_deadZoneCalculation )
	{
	case IGameController::AXIAL_DEAD_ZONE:
		if ( fabsf( this->m_LeftJoyX ) < this->m_deadZone )	{ this->m_LeftJoyX = 0.0f; }
		if ( fabsf( this->m_LeftJoyY ) < this->m_deadZone ) { this->m_LeftJoyY = 0.0f; }
		if ( fabsf( this->m_RightJoyX ) < this->m_deadZone ) { this->m_RightJoyX = 0.0f; }
		if ( fabsf( this->m_RightJoyY ) < this->m_deadZone ) { this->m_RightJoyY = 0.0f; }
		break;
	case IGameController::RADIAL_DEAD_ZONE:
		if ( this->m_LeftJoyMagnitude < this->m_deadZone ) { this->m_LeftJoyX = this->m_LeftJoyY = 0.0f; }
		if ( this->m_RightJoyMagnitude < this->m_deadZone ) { this->m_RightJoyX = this->m_RightJoyY = 0.0f; }
		break;
	case IGameController::SCALED_RADIAL_DEAD_ZONE:
		if ( this->m_LeftJoyMagnitude < this->m_deadZone ) { this->m_LeftJoyX = this->m_LeftJoyY = 0.0f; }
		else
		{
			float adjustLeft = (this->m_LeftJoyMagnitude - this->m_deadZone) / ( 1.0f - this->m_deadZone );
			this->m_LeftJoyX *= adjustLeft;
			this->m_LeftJoyY *= adjustLeft;
			float adjustRight = (this->m_RightJoyMagnitude - this->m_deadZone) / ( 1.0f - this->m_deadZone );
			this->m_RightJoyX *= adjustRight;
			this->m_RightJoyY *= adjustRight;
		}

		break;
	// else...
		// No dead zone
	}

	return;
}

XINPUT_STATE CXBoxController::GetState(void)
{
	ZeroMemory( &this->m_State, sizeof( this->m_State ) );
	
	::XInputGetState( this->m_playerNumber, &this->m_State );

	return this->m_State;
}

int CXBoxController::GetPlayerNumber(void)
{
	return this->m_playerNumber;
}


bool CXBoxController::bIsConnected(void)
{
	ZeroMemory( &this->m_State, sizeof( this->m_State ) );

	DWORD hr = ::XInputGetState( this->m_playerNumber, &this->m_State );

	if ( hr == ERROR_SUCCESS )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CXBoxController::Vibrate( unsigned int leftVal, unsigned int rightVal )
{
	XINPUT_VIBRATION vibration;
	ZeroMemory( &vibration, sizeof( vibration ) );
	vibration.wLeftMotorSpeed = static_cast<WORD>(leftVal);
	vibration.wRightMotorSpeed = static_cast<WORD>(rightVal);
	DWORD returnVal = XInputSetState( this->m_playerNumber, &vibration );
}


void CXBoxController::Vibrate( float fLeftValNormalized, float fRightValNormalized )
{
	if ( fLeftValNormalized > 1.0f ) { fLeftValNormalized = 1.0f; }
	if ( fLeftValNormalized < 0.0f ) { fLeftValNormalized = 0.0f; }
	if ( fRightValNormalized > 1.0f )	{ fRightValNormalized = 1.0f; }
	if ( fLeftValNormalized < 0.0f )	{ fLeftValNormalized = 0.0f; }

	unsigned int iLeftVal = static_cast<unsigned int>( fLeftValNormalized * ( CXBoxController::MAXVIBRATEMOTORSPEED - CXBoxController::MINVIBRATEMOTORSPEED ) );
	unsigned int iRightVal = static_cast<unsigned int>( fRightValNormalized * ( CXBoxController::MAXVIBRATEMOTORSPEED - CXBoxController::MINVIBRATEMOTORSPEED ) );

	this->Vibrate( iLeftVal, iRightVal );
	return;
}

// This is really only used for vibration effects
void CXBoxController::AddVibrationSequence( const std::vector<CVibStep> &vecVibSequence )
{
	// Append the next sequence to current one...
	for ( std::vector<CVibStep>::const_iterator itVib = vecVibSequence.begin(); 
		  itVib != vecVibSequence.end(); itVib++ )
	{
		// Add the appropriate steps to the appropriate motors
		switch ( itVib->motor )
		{
		case CVibStep::LEFT:
			this->m_VibSequenceLeft.insert( this->m_VibSequenceLeft.end(), *itVib );
			break;
		case CVibStep::RIGHT:
			this->m_VibSequenceRight.insert( this->m_VibSequenceRight.end(), *itVib );
			break;
		}
	}
	return;
}

void CXBoxController::AddVibrationSequence( const CVibStep &VibStep )
{
	// Add the appropriate steps to the appropriate motors
	switch ( VibStep.motor )
	{
	case CVibStep::LEFT:
		this->m_VibSequenceLeft.insert( this->m_VibSequenceLeft.end(), VibStep );
		break;
	case CVibStep::RIGHT:
		this->m_VibSequenceRight.insert( this->m_VibSequenceRight.end(), VibStep );
		break;
	}
	return;
}

void CXBoxController::AddVibrationSequence( CVibStep::enumMotor motor_, float speed_, float duration_ )
{
	IGameController::CVibStep vibStep( motor_, speed_, duration_ );
	this->AddVibrationSequence( vibStep );
	return;
}


void CXBoxController::ClearVibrationSequence(void)
{
	this->m_VibSequenceLeft.clear();
	this->m_VibSequenceRight.clear();
	this->Vibrate( static_cast<unsigned int>(0), static_cast<unsigned int>(0) );
	return;
}

static float lg_lastLeftVibSpeed;
static float lg_lastRightVibSpeed;

// This is used for the vibration sequence
bool CXBoxController::Update( float deltaSeconds )
{
	float lVib = 0.0f;
	float rVib = 0.0f;
	// Go through each list, seeing if it's time for a change...
	if ( !this->m_VibSequenceLeft.empty() )
	{	
		lVib = this->m_VibSequenceLeft.begin()->fSpeed;
		// Decrement the time
		this->m_VibSequenceLeft.begin()->durationSeconds -= deltaSeconds;
		// Still time? 
		if ( this->m_VibSequenceLeft.begin()->durationSeconds <= 0.0f )
		{	// nope, so delete this node
			this->m_VibSequenceLeft.erase( this->m_VibSequenceLeft.begin() );
		}
	}

	if ( !this->m_VibSequenceRight.empty() )
	{	
		rVib = this->m_VibSequenceRight.begin()->fSpeed;
		// Decrement the time
		this->m_VibSequenceRight.begin()->durationSeconds -= deltaSeconds;
		// Still time? 
		if ( this->m_VibSequenceRight.begin()->durationSeconds <= 0.0f )
		{	// nope, so delete this node
			this->m_VibSequenceRight.erase( this->m_VibSequenceRight.begin() );
		}
	}

	// Have the vibration speeds changed since the last call?
	if ( ( lg_lastLeftVibSpeed != lVib ) || ( lg_lastRightVibSpeed != rVib ) )
	{
		// Save the current speed (replacing the old speed)
		lg_lastLeftVibSpeed = lVib;
		lg_lastRightVibSpeed = rVib;
		// Yes, so update the vibration
		this->Vibrate( lVib, rVib );
	}
	return true;
}


bool CXBoxController::bIsAButtonDown(void)	// Button 1
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsBButtonDown(void)	// Button 2
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsXButtonDown(void)	// Button 3
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsYButtonDown(void)	// Button 4
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsBackButtonDown(void)	// Button 7
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsStartButtonDown(void)	// Button 8
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START ) 
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsLeftButtonDown(void)	// Button 5
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsRightButtonDown(void)	// Button 6
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsLeftJoystickButtonDown(void)	// Button 9
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsRightJoystickButtonDown(void)	// Button 10
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsGamePadLeft(void)
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsGamePadRight(void)
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsGamePadUp(void)
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsGamePadDown(void)
{
	if ( this->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN )
	{
		return true;
	}
	return false;
}

float CXBoxController::getLeftJoystickXAxis(void)
{
	this->m_UpdateLeftJoystickValues();
	return this->m_LeftJoyX;
}

float CXBoxController::getLeftJoystickYAxis(void)
{
	this->m_adjustForDeadZone();
	return this->m_LeftJoyY;
}

float CXBoxController::getRightJoystickXAxis(void)
{
	this->m_adjustForDeadZone();
	return this->m_RightJoyX;
}

float CXBoxController::getRightJoystickYAxis(void)
{
	this->m_adjustForDeadZone();
	return this->m_RightJoyY;
}

float CXBoxController::getLeftTrigger(void)
{
	int trigVal = this->GetState().Gamepad.bLeftTrigger;
	if ( trigVal > XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
	{
		return static_cast<float>(trigVal) / static_cast<float>(255);
	}
	return 0.0f;
}

float CXBoxController::getRightTrigger(void)
{
	int trigVal = this->GetState().Gamepad.bRightTrigger;
	if ( trigVal > XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
	{
		return static_cast<float>(trigVal) / static_cast<float>(255);
	}
	return 0.0f;
}

bool CXBoxController::bIsLeftTriggerDown(void)
{
	if ( this->getLeftTrigger() > XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
	{
		return true;
	}
	return false;
}

bool CXBoxController::bIsRightTriggerDown(void)
{
	if ( this->getRightTrigger() > XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
	{
		return true;
	}
	return false;
}

float CXBoxController::getLeftJoystickMagnitudeFromCentre(void)
{
	this->m_UpdateLeftJoystickValues();
	return this->m_LeftJoyMagnitude;
}

float CXBoxController::getRightJoystickMagnitudeFromCentre(void)
{
	this->m_UpdateRightJoystickValues();
	return this->m_RightJoyMagnitude;
}

float getLeftJoystickMagnitudeFromCentre(void);

void CXBoxController::m_UpdateLeftJoystickValues(void)
{	// From: http://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx
	XINPUT_STATE state = this->GetState();

	float LX = state.Gamepad.sThumbLX;
	float LY = state.Gamepad.sThumbLY;

	//determine how far the controller is pushed
	float magnitude = sqrt(LX*LX + LY*LY);

	//determine the direction the controller is pushed
	this->m_LeftJoyX = LX / magnitude;
	this->m_LeftJoyY = LY / magnitude;

	this->m_LeftJoyMagnitude = 0.0f;

	//check if the controller is outside a circular dead zone
	if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
	{
	  //clip the magnitude at its expected maximum value
	  if (magnitude > 32767) magnitude = 32767;
  
	  //adjust magnitude relative to the end of the dead zone
	  magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

	  //optionally normalize the magnitude with respect to its expected range
	  //giving a magnitude value of 0.0 to 1.0
	  this->m_LeftJoyMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE );
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		this->m_LeftJoyMagnitude = 0.0;
		//
		this->m_LeftJoyX = 0.0f;
		this->m_LeftJoyY = 0.0f;
	}

	return;
}

void CXBoxController::m_UpdateRightJoystickValues(void)
{	// From: http://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx
	XINPUT_STATE state = this->GetState();

	float RX = state.Gamepad.sThumbRX;
	float RY = state.Gamepad.sThumbRY;

	//determine how far the controller is pushed
	float magnitude = sqrt(RX*RX + RY*RY);

	//determine the direction the controller is pushed
	this->m_RightJoyX = RX / magnitude;
	this->m_RightJoyY = RY / magnitude;

	this->m_RightJoyMagnitude = 0.0f;

	//check if the controller is outside a circular dead zone
	if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE )
	{
	  //clip the magnitude at its expected maximum value
	  if (magnitude > 32767) magnitude = 32767;
  
	  //adjust magnitude relative to the end of the dead zone
	  magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

	  //optionally normalize the magnitude with respect to its expected range
	  //giving a magnitude value of 0.0 to 1.0
	  this->m_RightJoyMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE );
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		this->m_RightJoyMagnitude = 0.0;
		//
		this->m_RightJoyX = 0.0f;
		this->m_RightJoyY = 0.0f;
	}

	return;
}
