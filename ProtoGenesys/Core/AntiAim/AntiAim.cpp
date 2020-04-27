//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cAntiAim _antiAim;

	void cAntiAim::AntiAim(sUserCmd* usercmd)
	{
		if (_profiler.gAntiAim->Current.iValue == cProfiler::ANTIAIM_SPINBOT)
		{
			static float flAngle = 0.0f;

			if (flAngle > 360.0f)
				flAngle -= 360.0f;

			vAntiAimAngles[0] = 85.0f - CG->PlayerState.vDeltaAngles[0];
			vAntiAimAngles[1] = flAngle - CG->PlayerState.vDeltaAngles[1];

			flAngle += 45.0f;
		}

		else if (_profiler.gAntiAim->Current.iValue == cProfiler::ANTIAIM_JITTERBOT)
		{
			static int iMode = 1;

			switch (iMode)
			{
			case 1:
				vAntiAimAngles[0] = -85.0f - CG->PlayerState.vDeltaAngles[0];
				vAntiAimAngles[1] = 0.0f - CG->PlayerState.vDeltaAngles[1];
				iMode = 2;
				break;

			case 2:
				vAntiAimAngles[0] = 85.0f - CG->PlayerState.vDeltaAngles[0];
				vAntiAimAngles[1] = 90.0f - CG->PlayerState.vDeltaAngles[1];
				iMode = 3;
				break;

			case 3:
				vAntiAimAngles[0] = -85.0f - CG->PlayerState.vDeltaAngles[0];
				vAntiAimAngles[1] = 180.0f - CG->PlayerState.vDeltaAngles[1];
				iMode = 4;
				break;

			case 4:
				vAntiAimAngles[0] = 85.0f - CG->PlayerState.vDeltaAngles[0];
				vAntiAimAngles[1] = 270.0f - CG->PlayerState.vDeltaAngles[1];
				iMode = 1;
				break;
			}
		}

		else if (_profiler.gAntiAim->Current.iValue == cProfiler::ANTIAIM_RANDOMIZED)
		{
			std::random_device Device;
			std::uniform_real_distribution<float> RandomPitch(-85.0f, 85.0f), RandomYaw(0.0f, 360.0f);

			vAntiAimAngles[0] = RandomPitch(Device) - CG->PlayerState.vDeltaAngles[0];
			vAntiAimAngles[1] = RandomYaw(Device) - CG->PlayerState.vDeltaAngles[1];
		}

		else if (_profiler.gAntiAim->Current.iValue == cProfiler::ANTIAIM_REVERSED)
		{
			if (_aimBot.AimState.bAntiAimTargetAcquired || _aimBot.AimState.bIsAutoAiming)
			{
				vAntiAimAngles[0] = _aimBot.AimState.vAntiAimAngles[0];
				vAntiAimAngles[1] = _aimBot.AimState.vAntiAimAngles[1];
			}

			else
			{
				vAntiAimAngles[0] = -40.0f - CG->PlayerState.vDeltaAngles[0];
				vAntiAimAngles[1] = -170.0f - CG->PlayerState.vDeltaAngles[1];
			}
		}

		if (IsAntiAiming())
		{
			usercmd->iViewAngles[0] = AngleToShort(vAntiAimAngles[0]);
			usercmd->iViewAngles[1] = AngleToShort(vAntiAimAngles[1]);
		}
	}
	/*
	//=====================================================================================
	*/
	bool cAntiAim::IsAntiAiming()
	{
		return (GetWeaponDef(CG->Entity[CG->iClientNum].NextEntityState.iWeaponID) && !WeaponIsVehicle() &&
			!(CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_PRONE) &&
			!(CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags2 & EF2_MANTLE) &&
			(_profiler.gAntiAim->Current.iValue > cProfiler::ANTIAIM_OFF));
	}
}

//=====================================================================================