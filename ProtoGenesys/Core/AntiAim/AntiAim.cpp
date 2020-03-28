//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cAntiAim _antiAim;

	void cAntiAim::AntiAim(sUserCmd* usercmd)
	{
		if (IsAntiAiming())
		{
			if (_profiler.gAntiAim->Custom.iValue == cProfiler::ANTIAIM_SPINBOT)
			{
				static float flAngle = 0.0f;

				if (flAngle > 360.0f)
					flAngle -= 360.0f;

				vAntiAimAngles[0] = 70.0f - CG->vRefDefViewAngles[0];
				vAntiAimAngles[1] = flAngle - CG->vRefDefViewAngles[1];

				usercmd->iViewAngles[0] += AngleToShort(vAntiAimAngles[0]);
				usercmd->iViewAngles[1] += AngleToShort(vAntiAimAngles[1]);

				flAngle += 40.0f;
			}

			else if (_profiler.gAntiAim->Custom.iValue == cProfiler::ANTIAIM_JITTERBOT)
			{
				static int iMode = 1;

				switch (iMode)
				{
				case 1:
					vAntiAimAngles[0] = -80.0f - CG->vRefDefViewAngles[0];
					vAntiAimAngles[1] = 0.0f - CG->vRefDefViewAngles[1];

					usercmd->iViewAngles[0] += AngleToShort(vAntiAimAngles[0]);
					usercmd->iViewAngles[1] += AngleToShort(vAntiAimAngles[1]);
					iMode = 2;
					break;

				case 2:
					vAntiAimAngles[0] = 80.0f - CG->vRefDefViewAngles[0];
					vAntiAimAngles[1] = 90.0f - CG->vRefDefViewAngles[1];

					usercmd->iViewAngles[0] += AngleToShort(vAntiAimAngles[0]);
					usercmd->iViewAngles[1] += AngleToShort(vAntiAimAngles[1]);
					iMode = 3;
					break;

				case 3:
					vAntiAimAngles[0] = -80.0f - CG->vRefDefViewAngles[0];
					vAntiAimAngles[1] = 180.0f - CG->vRefDefViewAngles[1];

					usercmd->iViewAngles[0] += AngleToShort(vAntiAimAngles[0]);
					usercmd->iViewAngles[1] += AngleToShort(vAntiAimAngles[1]);
					iMode = 4;
					break;

				case 4:
					vAntiAimAngles[0] = 80.0f - CG->vRefDefViewAngles[0];
					vAntiAimAngles[1] = 270.0f - CG->vRefDefViewAngles[1];

					usercmd->iViewAngles[0] += AngleToShort(vAntiAimAngles[0]);
					usercmd->iViewAngles[1] += AngleToShort(vAntiAimAngles[1]);
					iMode = 1;
					break;
				}
			}

			else if (_profiler.gAntiAim->Custom.iValue == cProfiler::ANTIAIM_RANDOMIZED)
			{
				std::random_device Device;
				std::uniform_real_distribution<float> RandomPitch(-80.0f, 80.0f), RandomYaw(0.0f, 360.0f);

				vAntiAimAngles[0] = RandomPitch(Device) - CG->vRefDefViewAngles[0];
				vAntiAimAngles[1] = RandomYaw(Device) - CG->vRefDefViewAngles[1];

				usercmd->iViewAngles[0] += AngleToShort(vAntiAimAngles[0]);
				usercmd->iViewAngles[1] += AngleToShort(vAntiAimAngles[1]);
			}

			else if (_profiler.gAntiAim->Custom.iValue == cProfiler::ANTIAIM_REVERSED)
			{
				if (_targetList.vIsTarget[_targetList.iRiotShieldTarget] && _targetList.iRiotShieldTarget != CG->iClientNum)
				{
					_mathematics.CalculateAngles(CG->vOrigin, CG->Entity[_targetList.iRiotShieldTarget].vOrigin, _targetList.vRiotShieldTarget);

					vAntiAimAngles[0] = _targetList.vRiotShieldTarget[0] - 40.0f;
					vAntiAimAngles[1] = _targetList.vRiotShieldTarget[1] - 180.0f;

					usercmd->iViewAngles[0] += AngleToShort(vAntiAimAngles[0]);
					usercmd->iViewAngles[1] += AngleToShort(vAntiAimAngles[1]);
				}
				
				else
				{
					if (_aimBot.AimState.bIsAutoAiming)
					{
						vAntiAimAngles[0] = _aimBot.AimState.vAimbotAngles[0] - 40.0f;
						vAntiAimAngles[1] = _aimBot.AimState.vAimbotAngles[1] - 180.0f;

						usercmd->iViewAngles[0] += AngleToShort(vAntiAimAngles[0]);
						usercmd->iViewAngles[1] += AngleToShort(vAntiAimAngles[1]);
					}

					else
					{
						vAntiAimAngles[0] = -40.0f;
						vAntiAimAngles[1] = -180.0f;

						usercmd->iViewAngles[0] += AngleToShort(vAntiAimAngles[0]);
						usercmd->iViewAngles[1] += AngleToShort(vAntiAimAngles[1]);
					}
				}
			}
		}
	}
	/*
	//=====================================================================================
	*/
	bool cAntiAim::IsAntiAiming()
	{
		return (GetWeaponDef(CG->Entity[CG->iClientNum].NextEntityState.iWeaponID) && !WeaponIsVehicle() &&
			!(CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags1 & EF1_PRONE) &&
			!(CG->Entity[CG->iClientNum].NextEntityState.LerpEntityState.eFlags2 & EF2_MANTLE));
	}
}

//=====================================================================================