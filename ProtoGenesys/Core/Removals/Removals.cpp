//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cRemovals _removals;

	void cRemovals::RecoilCompensation()
	{
		RecoilAngles->vAngles[0] *= _profiler.gRecoilFactor->Current.flValue;
		RecoilAngles->vAngles[1] *= _profiler.gRecoilFactor->Current.flValue;
		RecoilAngles->vAngles[2] *= _profiler.gRecoilFactor->Current.flValue;
	}
	/*
	//=====================================================================================
	*/
	void cRemovals::SpreadCompensation(sUserCmd* usercmd, int servertime)
	{
		FirstBulletFix();

		int iSeed = servertime;

		float flSpreadMultiplier = ByteToFloat(CG->flSpread),
			flMinSpread, flMaxSpread,
			flSpreadX, flSpreadY, flSpread,
			flRandom1, flRandom2;

		sWeaponDef* WeaponDef = GetWeaponDef(CG->CEntity[CG->iClientNum].NextEntityState.iWeaponID);
		GetSpreadForWeapon(CG->CEntity[CG->iClientNum].NextEntityState.iWeaponID, &flMinSpread, &flMaxSpread);

		if (CG->PlayerState.flZoomProgress == 1.0f)
			flSpread = ((flMaxSpread - WeaponDef->flADSSpread) * flSpreadMultiplier) + WeaponDef->flADSSpread;

		else
			flSpread = ((flMaxSpread - flMinSpread) * flSpreadMultiplier) + flMinSpread;

		HashSeed(&iSeed);
		flRandom1 = RandomFloat(&iSeed);
		SeedRandom(&iSeed);
		flRandom2 = RandomFloat(&iSeed);
		flRandom1 *= M_PI_DOUBLE;

		flSpreadX = cosf(flRandom1) * flRandom2 * flSpread;
		flSpreadY = sinf(flRandom1) * flRandom2 * flSpread;

		usercmd->iViewAngles[0] += AngleToShort(flSpreadY * (1.0f - _profiler.gSpreadFactor->Current.flValue));
		usercmd->iViewAngles[1] += AngleToShort(flSpreadX * (1.0f - _profiler.gSpreadFactor->Current.flValue));
	}
	/*
	//=====================================================================================
	*/
	void cRemovals::FirstBulletFix()
	{
		ImVec3 vAngles;

		if (CG->flShockSensitivity != 0.0)
			CG->flZoomSensitivity *= CG->flShockSensitivity;

		SetFovSensitivityScale(CG->flZoomSensitivity);

		if (WeaponHasPerk(GetViewmodelWeaponIndex(), 7) &&
			UsingSniperScope() && CG->PlayerState.flZoomProgress == 1.0f)
		{
			vAngles[0] = CG->vOffsetAngles[0] + CG->vKickAngles[0] * 0.25f;
			vAngles[1] = CG->vOffsetAngles[1] + CG->vKickAngles[1] * 0.25f;
			vAngles[2] = CG->vOffsetAngles[2] + CG->vKickAngles[2] * 0.25f;
		}

		else
		{
			vAngles[0] = CG->vOffsetAngles[0] + CG->vKickAngles[0];
			vAngles[1] = CG->vOffsetAngles[1] + CG->vKickAngles[1];
			vAngles[2] = CG->vOffsetAngles[2] + CG->vKickAngles[2];
		}

		SetUserCmdAimValues(&vAngles);
		SetUserCmdWeapons(GetLastWeaponForAlt());
		SetExtraButtons();

		CG->iExtraButtonBits[0] = 0;
		CG->iExtraButtonBits[1] = 0;

		CG->RefDef.iSplitScreenBlurEdges = 0;
		CG->RefDef.iPlayerTeleported = CG->iPlayerTeleported;
	}
}

//=====================================================================================