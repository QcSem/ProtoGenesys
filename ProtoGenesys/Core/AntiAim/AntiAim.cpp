//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cAntiAim _antiAim;

	void cAntiAim::AntiAimPitch(sUserCmd* usercmd)
	{
		if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_UP)
		{
			vAntiAimAngles[0] = -85.0f - CG->PlayerState.vDeltaAngles[0];
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_DOWN)
		{
			vAntiAimAngles[0] = 85.0f - CG->PlayerState.vDeltaAngles[0];
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_JITTER)
		{
			static bool bMode = false;

			switch (bMode)
			{
			case false:
				vAntiAimAngles[0] = -85.0f - CG->PlayerState.vDeltaAngles[0];
				bMode = true;
				break;

			case true:
				vAntiAimAngles[0] = 85.0f - CG->PlayerState.vDeltaAngles[0];
				bMode = false;
				break;
			}
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_RANDOM)
		{
			std::random_device Device;
			std::uniform_real_distribution<float> RandomPitch(-85.0f, 85.0f);

			vAntiAimAngles[0] = RandomPitch(Device) - CG->PlayerState.vDeltaAngles[0];
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_REVERSE)
		{
			if (_aimBot.AimState.bAntiAimTargetAcquired || _aimBot.AimState.bIsAutoAiming)
			{
				vAntiAimAngles[0] = _aimBot.AimState.vAntiAimAngles[0];
			}

			else
			{
				vAntiAimAngles[0] = -40.0f - CG->PlayerState.vDeltaAngles[0];
			}
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_CUSTOM)
		{
			vAntiAimAngles[0] = _profiler.gAntiAimCustomPitch->Current.flValue - CG->PlayerState.vDeltaAngles[0];
		}

		if (ReadyForAntiAim() && _profiler.gAntiAimPitch->Current.iValue > cProfiler::ANTIAIM_PITCH_OFF)
		{
			usercmd->iViewAngles[0] = AngleToShort(vAntiAimAngles[0]);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAntiAim::AntiAimYaw(sUserCmd* usercmd)
	{
		if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_SPIN)
		{
			static float flAngle = 0.0f;

			if (flAngle > 360.0f)
				flAngle -= 360.0f;

			vAntiAimAngles[1] = flAngle - CG->PlayerState.vDeltaAngles[1];

			flAngle += 45.0f;
		}

		else if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_JITTER)
		{
			static int iMode = 1;

			switch (iMode)
			{
			case 1:
				vAntiAimAngles[1] = 0.0f - CG->PlayerState.vDeltaAngles[1];
				iMode = 2;
				break;

			case 2:
				vAntiAimAngles[1] = 90.0f - CG->PlayerState.vDeltaAngles[1];
				iMode = 3;
				break;

			case 3:
				vAntiAimAngles[1] = 180.0f - CG->PlayerState.vDeltaAngles[1];
				iMode = 4;
				break;

			case 4:
				vAntiAimAngles[1] = 270.0f - CG->PlayerState.vDeltaAngles[1];
				iMode = 1;
				break;
			}
		}

		else if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_RANDOM)
		{
			std::random_device Device;
			std::uniform_real_distribution<float> RandomYaw(0.0f, 360.0f);

			vAntiAimAngles[1] = RandomYaw(Device) - CG->PlayerState.vDeltaAngles[1];
		}

		else if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_REVERSE)
		{
			if (_aimBot.AimState.bAntiAimTargetAcquired || _aimBot.AimState.bIsAutoAiming)
			{
				vAntiAimAngles[1] = _aimBot.AimState.vAntiAimAngles[1];
			}

			else
			{
				vAntiAimAngles[1] = -170.0f - CG->PlayerState.vDeltaAngles[1];
			}
		}

		else if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_CUSTOM)
		{
			vAntiAimAngles[1] = _profiler.gAntiAimCustomYaw->Current.flValue - CG->PlayerState.vDeltaAngles[1];
		}

		if (ReadyForAntiAim() && _profiler.gAntiAimYaw->Current.iValue > cProfiler::ANTIAIM_YAW_OFF)
		{
			usercmd->iViewAngles[1] = AngleToShort(vAntiAimAngles[1]);
		}
	}
	/*
	//=====================================================================================
	*/
	bool cAntiAim::ReadyForAntiAim()
	{
		return (GetWeaponDef(CG->Entity[CG->iClientNum].NextEntityState.iWeaponID) && !WeaponIsVehicle() &&
			!(CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_PRONE) &&
			!(CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags2 & EF2_MANTLE));
	}
}

//=====================================================================================