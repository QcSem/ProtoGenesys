//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cAntiAim _antiAim;

	void cAntiAim::AntiAimPitch(sUserCmd* usercmd)
	{
		if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_ZERO)
		{
			vAntiAimAngles.x = 0.0f - CG->PlayerState.vDeltaAngles.x;
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_UP)
		{
			vAntiAimAngles.x = -85.0f - CG->PlayerState.vDeltaAngles.x;
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_DOWN)
		{
			vAntiAimAngles.x = 85.0f - CG->PlayerState.vDeltaAngles.x;
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_JITTER)
		{
			static bool bMode = false;

			switch (bMode)
			{
			case false:
				vAntiAimAngles.x = -85.0f - CG->PlayerState.vDeltaAngles.x;
				bMode = true;
				break;

			case true:
				vAntiAimAngles.x = 85.0f - CG->PlayerState.vDeltaAngles.x;
				bMode = false;
				break;
			}
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_RANDOM)
		{
			std::random_device Device;
			std::uniform_real_distribution<float> RandomPitch(-85.0f, 85.0f);

			vAntiAimAngles.x = RandomPitch(Device) - CG->PlayerState.vDeltaAngles.x;
		}

		else if (_profiler.gAntiAimPitch->Current.iValue == cProfiler::ANTIAIM_PITCH_REVERSE)
		{
			if (_aimBot.AimState.bTargetAcquired || _aimBot.AimState.bAntiAimTargetAcquired)
			{
				vAntiAimAngles.x = _aimBot.AimState.vAntiAimAngles.x;
			}

			else
			{
				vAntiAimAngles.x = -40.0f - CG->vRefDefViewAngles.x;

				if (vAntiAimAngles.x > 85.0f) vAntiAimAngles.x = 85.0f;
				if (vAntiAimAngles.x < -85.0f) vAntiAimAngles.x = -85.0f;

				vAntiAimAngles.x -= CG->PlayerState.vDeltaAngles.x;
			}
		}

		if (ReadyForAntiAim() && _profiler.gAntiAimPitch->Current.iValue > cProfiler::ANTIAIM_PITCH_OFF)
		{
			usercmd->iViewAngles[0] = AngleToShort(vAntiAimAngles.x);
		}
	}
	/*
	//=====================================================================================
	*/
	void cAntiAim::AntiAimYaw(sUserCmd* usercmd)
	{
		if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_ZERO)
		{
			vAntiAimAngles.y = 0.0f - CG->PlayerState.vDeltaAngles.y;
		}

		else if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_SPIN)
		{
			static float flAngle = 0.0f;

			if (flAngle > 180.0f)
				flAngle -= 360.0f;

			vAntiAimAngles.y = flAngle - CG->PlayerState.vDeltaAngles.y;

			flAngle += 45.0f;
		}

		else if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_JITTER)
		{
			static int iMode = 1;

			switch (iMode)
			{
			case 1:
				vAntiAimAngles.y = 0.0f - CG->PlayerState.vDeltaAngles.y;
				iMode = 2;
				break;

			case 2:
				vAntiAimAngles.y = 90.0f - CG->PlayerState.vDeltaAngles.y;
				iMode = 3;
				break;

			case 3:
				vAntiAimAngles.y = 180.0f - CG->PlayerState.vDeltaAngles.y;
				iMode = 4;
				break;

			case 4:
				vAntiAimAngles.y = -90.0f - CG->PlayerState.vDeltaAngles.y;
				iMode = 1;
				break;
			}
		}

		else if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_RANDOM)
		{
			std::random_device Device;
			std::uniform_real_distribution<float> RandomYaw(-180.0f, 180.0f);

			vAntiAimAngles.y = RandomYaw(Device) - CG->PlayerState.vDeltaAngles.y;
		}

		else if (_profiler.gAntiAimYaw->Current.iValue == cProfiler::ANTIAIM_YAW_REVERSE)
		{
			if (_aimBot.AimState.bTargetAcquired || _aimBot.AimState.bAntiAimTargetAcquired)
			{
				vAntiAimAngles.y = _aimBot.AimState.vAntiAimAngles.y;
			}

			else
			{
				vAntiAimAngles.y = CG->vRefDefViewAngles.y - 165.0f;

				while (vAntiAimAngles.y > 180.0f) vAntiAimAngles.y -= 360.0f;
				while (vAntiAimAngles.y < -180.0f) vAntiAimAngles.y += 360.0f;

				vAntiAimAngles.y -= CG->PlayerState.vDeltaAngles.y;
			}
		}

		if (ReadyForAntiAim() && _profiler.gAntiAimYaw->Current.iValue > cProfiler::ANTIAIM_YAW_OFF)
		{
			usercmd->iViewAngles[1] = AngleToShort(vAntiAimAngles.y);
		}
	}
	/*
	//=====================================================================================
	*/
	bool cAntiAim::ReadyForAntiAim()
	{
		return (GetWeaponDef(CG->CEntity[CG->iClientNum].NextEntityState.iWeaponID) && !WeaponIsVehicle() &&
			!(CG->CEntity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_PRONE) &&
			!(CG->CEntity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags2 & EF2_MANTLE));
	}
}

//=====================================================================================