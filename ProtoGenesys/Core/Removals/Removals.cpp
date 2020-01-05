//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cRemovals _removals;

	void cRemovals::RecoilCompensation()
	{
		RecoilAngles->vAngles[0] *= _profiler.gRecoilFactor->Custom.flValue;
		RecoilAngles->vAngles[1] *= _profiler.gRecoilFactor->Custom.flValue;
		RecoilAngles->vAngles[2] *= _profiler.gRecoilFactor->Custom.flValue;
	}
	/*
	//=====================================================================================
	*/
	void cRemovals::SpreadCompensation(sUserCmd* usercmd, int servertime)
	{
		int iSeed = servertime;

		float flSpreadMultiplier = ByteToFloat(CG->flSpread),
			flMinSpread, flMaxSpread,
			flSpreadX, flSpreadY, flSpread,
			flRandom1, flRandom2;

		sWeaponDef* WeaponDef = GetWeaponDef(CG->Entity[CG->iClientNum].iWeaponID1);
		GetSpreadForWeapon(CG->Entity[CG->iClientNum].iWeaponID1, &flMinSpread, &flMaxSpread);

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

		usercmd->iViewAngles[0] += AngleToShort(flSpreadY * _profiler.gSpreadFactor->Custom.flValue);
		usercmd->iViewAngles[1] += AngleToShort(flSpreadX * _profiler.gSpreadFactor->Custom.flValue);
	}
}

//=====================================================================================