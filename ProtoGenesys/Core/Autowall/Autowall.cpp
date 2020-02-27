//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cAutowall _autoWall;

	float cAutowall::Autowall(Vector3 start, Vector3 end)
	{
		int iIndex = CG->iClientNum;
		sEntity* pCEntity = &CG->Entity[iIndex];
		int iWeaponID = pCEntity->iWeaponID1;
		sWeaponDef* WeaponDef = GetWeaponDef(iWeaponID);

		sBulletFireParams FP_Enter;
		ZeroMemory(&FP_Enter, sizeof(sBulletFireParams));

		sBulletTraceResults TR_Enter;
		ZeroMemory(&TR_Enter, sizeof(sBulletTraceResults));

		sBulletFireParams FP_Exit;
		ZeroMemory(&FP_Exit, sizeof(sBulletFireParams));

		sBulletTraceResults TR_Exit;
		ZeroMemory(&TR_Exit, sizeof(sBulletTraceResults));

		FP_Enter.iMaxEntNum = 1022;
		FP_Enter.iEntityNum = iIndex;
		FP_Enter.flPower = 1.0f;
		FP_Enter.iBulletType = (WeaponDef->bRifleBullet != 0) + 1;

		VectorCopy(start, FP_Enter.vViewOrigin);
		VectorCopy(start, FP_Enter.vStart);
		VectorCopy(end, FP_Enter.vEnd);

		VectorSubtract(end, start, FP_Enter.vDir);
		_mathematics.VectorNormalize(FP_Enter.vDir);

		bool bEnterHit = BulletTrace(&TR_Enter, &FP_Enter, pCEntity, TRACE_HITTYPE_NONE);

		if (HitRiotshield(&TR_Enter))
			return 0.0f;

		if (_profiler.gAntiTeamKill->Custom.bValue)
			if (HitTeammate(&TR_Enter))
				return 0.0f;

		if (bEnterHit)
		{
			ePenetrateType iPenetrateType = GetPenetrateType(iWeaponID);

			bool bHasFMJ = HasPerk(6);
			int iSurfaceCount = 0;
			float flEnterDepth = 0.0f;
			float flExitDepth = 0.0f;
			float flSurfaceDepth = 0.0f;

			Vector3 vHitPos = { 0.0f };

			while (TRUE)
			{
				flEnterDepth = GetSurfacePenetrationDepth(iPenetrateType, TR_Enter.iSurfaceType);

				if (TR_Enter.Trace.iSurfaceFlags & 0x4)
					flEnterDepth = 100.0f;

				if (bHasFMJ)
					flEnterDepth *= ((sDvar*)dwPenetrationMultiplier)->Current.flValue;

				if (flEnterDepth <= 0.0)
					return 0.0f;

				VectorCopy(TR_Enter.vHitPos, vHitPos);

				if (!AdvanceTrace(&FP_Enter, &TR_Enter, 0.13500001f))
					return 0.0f;

				bEnterHit = BulletTrace(&TR_Enter, &FP_Enter, pCEntity, TR_Enter.iSurfaceType);

				if (HitRiotshield(&TR_Enter))
					return 0.0f;

				if (_profiler.gAntiTeamKill->Custom.bValue)
					if (HitTeammate(&TR_Enter))
						return 0.0f;

				CopyMemory(&FP_Exit, &FP_Enter, sizeof(sBulletFireParams));
				VectorScale(FP_Enter.vDir, -1.0f, FP_Exit.vDir);

				VectorCopy(FP_Enter.vEnd, FP_Exit.vStart);
				VectorMA(vHitPos, 0.0099999998f, FP_Exit.vDir, FP_Exit.vEnd);

				CopyMemory(&TR_Exit, &TR_Enter, sizeof(sBulletTraceResults));
				VectorScale(TR_Exit.Trace.vNormal, -1.0f, TR_Exit.Trace.vNormal);

				if (bEnterHit)
					AdvanceTrace(&FP_Exit, &TR_Exit, 0.0099999998f);

				bool bExitHit = BulletTrace(&TR_Exit, &FP_Exit, pCEntity, TR_Exit.iSurfaceType);
				bool bSolid = bExitHit && TR_Exit.Trace.bAllSolid || TR_Enter.Trace.bStartSolid && TR_Exit.Trace.bStartSolid;

				if (HitRiotshield(&TR_Exit))
					return 0.0f;

				if (_profiler.gAntiTeamKill->Custom.bValue)
					if (HitTeammate(&TR_Exit))
						return 0.0f;

				if (bExitHit || bSolid)
				{
					if (bSolid)
						flSurfaceDepth = _mathematics.CalculateDistance(FP_Exit.vEnd, FP_Exit.vStart);
					else
						flSurfaceDepth = _mathematics.CalculateDistance(vHitPos, TR_Exit.vHitPos);

					flSurfaceDepth = max(flSurfaceDepth, 1.0f);

					if (bExitHit)
					{
						flExitDepth = GetSurfacePenetrationDepth(iPenetrateType, TR_Exit.iSurfaceType);

						if (TR_Exit.Trace.iSurfaceFlags & 0x4)
							flExitDepth = 100.0f;

						if (bHasFMJ)
							flExitDepth *= ((sDvar*)dwPenetrationMultiplier)->Current.flValue;

						flEnterDepth = min(flEnterDepth, flExitDepth);

						if (flEnterDepth <= 0.0f)
							return 0.0f;
					}

					FP_Enter.flPower -= flSurfaceDepth / flEnterDepth;

					if (FP_Enter.flPower <= 0.0f)
						return 0.0f;
				}

				else if (!bEnterHit)
					return GetRemainingDamage(&FP_Enter, &TR_Enter, TR_Enter.Trace.wPartGroup, iWeaponID);

				if (bEnterHit)
				{
					if (++iSurfaceCount < ((sDvar*)dwPenetrationCount)->Current.iValue)
						continue;
				}

				return 0.0f;
			}
		}

		else
		{
			FP_Enter.flPower = 100.0f;
			return GetRemainingDamage(&FP_Enter, &TR_Enter, TR_Enter.Trace.wPartGroup, iWeaponID);
		}
	}
	/*
	//=====================================================================================
	*/
	bool cAutowall::TraceBullet(Vector3 start, Vector3 end, int entitynum)
	{
		int iIndex = CG->iClientNum;
		sEntity* pCEntity = &CG->Entity[iIndex];
		int iWeaponID = pCEntity->iWeaponID1;
		sWeaponDef* WeaponDef = GetWeaponDef(iWeaponID);

		sBulletFireParams FP_Enter;
		ZeroMemory(&FP_Enter, sizeof(sBulletFireParams));

		sBulletTraceResults TR_Enter;
		ZeroMemory(&TR_Enter, sizeof(sBulletTraceResults));

		FP_Enter.iMaxEntNum = 1022;
		FP_Enter.iEntityNum = iIndex;
		FP_Enter.flPower = 1.0f;
		FP_Enter.iBulletType = (WeaponDef->bRifleBullet != 0) + 1;

		VectorCopy(start, FP_Enter.vViewOrigin);
		VectorCopy(start, FP_Enter.vStart);
		VectorCopy(end, FP_Enter.vEnd);

		VectorSubtract(end, start, FP_Enter.vDir);
		_mathematics.VectorNormalize(FP_Enter.vDir);

		BulletTrace(&TR_Enter, &FP_Enter, pCEntity, TRACE_HITTYPE_NONE);

		return ((TR_Enter.Trace.wHitID == entitynum || TR_Enter.Trace.flFraction == 1.0f) && !HitRiotshield(&TR_Enter));
	}
	/*
	//=====================================================================================
	*/
	bool cAutowall::TraceLine(Vector3 start, Vector3 end, int entitynum)
	{
		Vector3 vStart, vEnd;

		VectorCopy(start, vStart);
		VectorCopy(end, vEnd);

		sTrace Trace;
		ZeroMemory(&Trace, sizeof(sTrace));

		LocationalTrace(&Trace, vStart, vEnd, CG->iClientNum, 0x803003);

		return (Trace.wHitID == entitynum || Trace.flFraction == 1.0f);
	}
	/*
	//=====================================================================================
	*/
	bool cAutowall::BulletTrace(sBulletTraceResults* traceresults, sBulletFireParams* fireparams, sEntity* attacker, int surfacetype)
	{
		bool bResult = false;
		DWORD_PTR dwAddress = dwBulletTrace;
		_declspec(align(16)) char szSave[512];
		_fxsave(szSave);
		_asm
		{
			push 0
			push surfacetype
			push attacker
			push fireparams
			push 0
			mov  esi, traceresults
			call dwAddress
			mov	 bResult, al
			add  esp, 14h
		}
		_fxrstor(szSave);
		return bResult;
	}
	/*
	//=====================================================================================
	*/
	float cAutowall::GetRemainingDamage(sBulletFireParams* fireparams, sBulletTraceResults* traceresults, short partgroup, int weapon)
	{
		float flDamage = 0.0f;
		Vector3 vHitPos, vOrigin;

		if (fireparams->flPower > 0.0f)
		{
			VectorCopy(traceresults->vHitPos, vHitPos);
			VectorCopy(fireparams->vViewOrigin, vOrigin);

			flDamage = (float)GetWeaponDamageForRange(weapon, vOrigin, vHitPos) * fireparams->flPower;
			flDamage = GetWeaponHitLocationMultiplier(partgroup, weapon) * flDamage;

			if (flDamage <= 0.0f)
				flDamage = 1.0f;
		}

		return flDamage;
	}
	/*
	//=====================================================================================
	*/
	bool cAutowall::HitRiotshield(sBulletTraceResults* traceresults)
	{
		if (traceresults->iIgnoreHitEnt)
			return false;

		if (traceresults->Trace.wPartGroup == 20)
			return true;

		WORD wHitID = GetTraceHitType(traceresults);

		if (wHitID != 1022)
		{
			if (EntityIsDeployedRiotshield(&CG->Entity[wHitID]))
			{
				return true;
			}
		}

		return false;
	}
	/*
	//=====================================================================================
	*/
	bool cAutowall::HitTeammate(sBulletTraceResults* traceresults)
	{
		WORD wHitID = GetTraceHitType(traceresults);

		if (wHitID < MAX_CLIENTS)
		{
			if (!_targetList.EntityIsEnemy(wHitID))
			{
				return true;
			}
		}

		return false;
	}
}

//=====================================================================================