//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cAutowall _autoWall;

	float cAutowall::Autowall(sCEntity* entity, ImVec3 start, ImVec3 end)
	{
		int iClientNum = CG->iClientNum;
		sCEntity* pCEntity = &CG->CEntity[iClientNum];
		int iWeaponID = pCEntity->NextEntityState.iWeaponID;
		sWeaponDef* pWeaponDef = GetWeaponDef(iWeaponID);
		ePenetrateType iPenetrateType = pWeaponDef->iPenetrateType;
		eWeaponType iWeaponType = pWeaponDef->iWeaponType;

		sBulletFireParams FP_Enter;
		ZeroMemory(&FP_Enter, sizeof(sBulletFireParams));

		sBulletTraceResults TR_Enter;
		ZeroMemory(&TR_Enter, sizeof(sBulletTraceResults));

		sBulletFireParams FP_Exit;
		ZeroMemory(&FP_Exit, sizeof(sBulletFireParams));

		sBulletTraceResults TR_Exit;
		ZeroMemory(&TR_Exit, sizeof(sBulletTraceResults));

		FP_Enter.iMaxEntNum = 1022;
		FP_Enter.iEntityNum = iClientNum;
		FP_Enter.flPower = 1.0f;
		FP_Enter.iBulletType = (pWeaponDef->bRifleBullet != 0) + 1;

		FP_Enter.vViewOrigin = start;
		FP_Enter.vStart = start;
		FP_Enter.vEnd = end;

		FP_Enter.vDir = end - start;
		float flLength = _mathematics.VectorLength(FP_Enter.vDir, FP_Enter.vDir);
		_mathematics.VectorNormalize(FP_Enter.vDir);

		bool bEnterHit = BulletTrace(&TR_Enter, &FP_Enter, pCEntity, TRACE_HITTYPE_NONE);

		if (bEnterHit)
		{
			if (iPenetrateType <= PENETRATE_TYPE_NONE)
				return -FLT_MAX;

			if (HitRiotshield(&TR_Enter))
				return -FLT_MAX;

			if (_profiler.gAntiKillTeamMates->Current.iValue)
				if (HitTeammate(&TR_Enter.Trace))
					return -FLT_MAX;

			if (_profiler.gAntiKillIgnored->Current.iValue)
				if (HitIgnored(&TR_Enter.Trace))
					return -FLT_MAX;

			if (GetTraceHitType(&TR_Enter.Trace) == entity->NextEntityState.iEntityNum)
				return GetRemainingDamage(&FP_Enter, &TR_Enter, iWeaponID);

			float flEnterDepth = 0.0f;
			float flExitDepth = 0.0f;
			float flSurfaceDepth = 0.0f;

			ImVec3 vHitPos;
			ImVec3 vTemp;

			for (int iSurfaceCount = 0; bEnterHit && iSurfaceCount < FindVar("penetrationCount")->Current.iValue; ++iSurfaceCount)
			{
				flEnterDepth = GetSurfacePenetrationDepth(iPenetrateType, TR_Enter.iSurfaceType);

				if (HasPerk(6))
					flEnterDepth *= FindVar("perk_bulletPenetrationMultiplier")->Current.flValue;

				if (flEnterDepth <= 0.0)
					return -FLT_MAX;

				vHitPos = TR_Enter.vHitPos;
				vTemp = vHitPos - FP_Enter.vStart;

				if (_mathematics.VectorLength(vTemp, vTemp) >= flLength)
					return GetRemainingDamage(&FP_Enter, &TR_Enter, iWeaponID);
				
				if (!AdvanceTrace(&FP_Enter, &TR_Enter, 0.13500001f))
					return -FLT_MAX;

				bEnterHit = BulletTrace(&TR_Enter, &FP_Enter, pCEntity, TR_Enter.iSurfaceType);

				if (!PenetrationCheck(&FP_Enter))
					return -FLT_MAX;

				if (HitRiotshield(&TR_Enter))
					return -FLT_MAX;

				if (_profiler.gAntiKillTeamMates->Current.iValue)
					if (HitTeammate(&TR_Enter.Trace))
						return -FLT_MAX;

				if (_profiler.gAntiKillIgnored->Current.iValue)
					if (HitIgnored(&TR_Enter.Trace))
						return -FLT_MAX;

				CopyMemory(&FP_Exit, &FP_Enter, sizeof(sBulletFireParams));
				CopyMemory(&TR_Exit, &TR_Enter, sizeof(sBulletTraceResults));

				FP_Exit.vDir = FP_Enter.vDir * -1.0f;
				FP_Exit.vStart = FP_Enter.vEnd;
				FP_Exit.vEnd = (FP_Exit.vDir * 0.0099999998f) + vHitPos;
				TR_Exit.Trace.vNormal *= -1.0f;

				if (bEnterHit)
					AdvanceTrace(&FP_Exit, &TR_Exit, 0.0099999998f);

				bool bExitHit = BulletTrace(&TR_Exit, &FP_Exit, pCEntity, TR_Exit.iSurfaceType);
				bool bStaticModel = bExitHit && TR_Exit.Trace.bAllSolid || TR_Enter.Trace.bStartSolid && TR_Exit.Trace.bStartSolid;

				if (HitRiotshield(&TR_Exit))
					return -FLT_MAX;

				if (_profiler.gAntiKillTeamMates->Current.iValue)
					if (HitTeammate(&TR_Exit.Trace))
						return -FLT_MAX;

				if (_profiler.gAntiKillIgnored->Current.iValue)
					if (HitIgnored(&TR_Exit.Trace))
						return -FLT_MAX;

				if (bExitHit || bStaticModel)
				{
					if (bStaticModel)
						flSurfaceDepth = _mathematics.CalculateDistance(FP_Exit.vEnd, FP_Exit.vStart);
					else
						flSurfaceDepth = _mathematics.CalculateDistance(vHitPos, TR_Exit.vHitPos);

					flSurfaceDepth = max(flSurfaceDepth, 1.0f);

					if (bExitHit)
					{
						flExitDepth = GetSurfacePenetrationDepth(iPenetrateType, TR_Exit.iSurfaceType);

						if (HasPerk(6))
							flExitDepth *= FindVar("perk_bulletPenetrationMultiplier")->Current.flValue;

						flEnterDepth = min(flEnterDepth, flExitDepth);

						if (flEnterDepth <= 0.0f)
							return -FLT_MAX;
					}

					FP_Enter.flPower -= flSurfaceDepth / flEnterDepth;

					if (FP_Enter.flPower <= 0.0f)
						return -FLT_MAX;

					if (!bStaticModel && iWeaponType == WEAPTYPE_BULLET)
					{
						ImVec3 vLength;

						vLength = TR_Exit.vHitPos - TR_Enter.vHitPos;

						if (_mathematics.DotProduct(vLength, vLength) > FindVar("bullet_penetrationMinFxDist")->Current.flValue * FindVar("bullet_penetrationMinFxDist")->Current.flValue)
						{
							if (!bEnterHit)
								return GetRemainingDamage(&FP_Enter, &TR_Enter, iWeaponID);
						}

						if (GetTraceHitType(&TR_Exit.Trace) == entity->NextEntityState.iEntityNum)
							return GetRemainingDamage(&FP_Enter, &TR_Enter, iWeaponID);
					}
				}

				else if (!bEnterHit)
					return GetRemainingDamage(&FP_Enter, &TR_Enter, iWeaponID);

				if (GetTraceHitType(&TR_Enter.Trace) == entity->NextEntityState.iEntityNum)
					return GetRemainingDamage(&FP_Enter, &TR_Enter, iWeaponID);
			}

			return -FLT_MAX;
		}

		return GetRemainingDamage(&FP_Enter, &TR_Enter, iWeaponID);
	}
	/*
	//=====================================================================================
	*/
	float cAutowall::TraceBullet(sCEntity* entity, ImVec3 start, ImVec3 end)
	{
		int iClientNum = CG->iClientNum;
		sCEntity* pCEntity = &CG->CEntity[iClientNum];
		int iWeaponID = pCEntity->NextEntityState.iWeaponID;
		sWeaponDef* pWeaponDef = GetWeaponDef(iWeaponID);

		sBulletFireParams FP_Enter;
		ZeroMemory(&FP_Enter, sizeof(sBulletFireParams));

		sBulletTraceResults TR_Enter;
		ZeroMemory(&TR_Enter, sizeof(sBulletTraceResults));

		FP_Enter.iMaxEntNum = 1022;
		FP_Enter.iEntityNum = iClientNum;
		FP_Enter.flPower = 1.0f;
		FP_Enter.iBulletType = (pWeaponDef->bRifleBullet != 0) + 1;

		FP_Enter.vViewOrigin = start;
		FP_Enter.vStart = start;
		FP_Enter.vEnd = end;

		FP_Enter.vDir = end - start;
		_mathematics.VectorNormalize(FP_Enter.vDir);

		BulletTrace(&TR_Enter, &FP_Enter, pCEntity, TRACE_HITTYPE_NONE);

		if (HitRiotshield(&TR_Enter))
			return -FLT_MAX;

		if (GetTraceHitType(&TR_Enter.Trace) == entity->NextEntityState.iEntityNum || TR_Enter.Trace.flFraction == 1.0f)
			return GetRemainingDamage(&FP_Enter, &TR_Enter, iWeaponID);

		return -FLT_MAX;
	}
	/*
	//=====================================================================================
	*/
	bool cAutowall::TraceLine(sCEntity* entity, ImVec3 start, ImVec3 end)
	{
		ImVec3 vStart, vEnd;

		vStart = start;
		vEnd = end;

		sTrace Trace;
		ZeroMemory(&Trace, sizeof(sTrace));

		LocationalTrace(&Trace, &vStart, &vEnd, CG->iClientNum, 0x803003);

		return (Trace.wHitID == entity->NextEntityState.iEntityNum || Trace.flFraction == 1.0f);
	}
	/*
	//=====================================================================================
	*/
	bool cAutowall::BulletTrace(sBulletTraceResults* traceresults, sBulletFireParams* fireparams, sCEntity* attacker, int surfacetype)
	{
		bool bResult = false;

		__declspec(align(16)) char szSave[512];
		_fxsave(szSave);
		__asm
		{
			push 0
			push surfacetype
			push attacker
			push fireparams
			push 0
			mov esi, traceresults
			call dwBulletTrace
			mov bResult, al
			add esp, 14h
		}
		_fxrstor(szSave);

		return bResult;
	}
	/*
	//=====================================================================================
	*/
	float cAutowall::GetRemainingDamage(sBulletFireParams* fireparams, sBulletTraceResults* traceresults, int weapon)
	{
		float flDamage = 0.0f;
		ImVec3 vHitPos, vStart;

		if (fireparams->flPower > 0.0f)
		{
			vHitPos = traceresults->vHitPos;
			vStart = fireparams->vStart;

			flDamage = (float)GetWeaponDamageForRange(weapon, &vStart, &vHitPos) * fireparams->flPower;
			flDamage = GetWeaponHitLocationMultiplier(traceresults->Trace.wPartGroup, weapon) * flDamage;

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

		WORD wHitID = GetTraceHitType(&traceresults->Trace);

		if (wHitID != 1022)
		{
			if (EntityIsDeployedRiotshield(&CG->CEntity[wHitID]))
			{
				return true;
			}
		}

		return false;
	}
	/*
	//=====================================================================================
	*/
	bool cAutowall::HitTeammate(sTrace* trace)
	{
		WORD wHitID = GetTraceHitType(trace);

		if (wHitID < MAX_CLIENTS)
		{
			if (EntityIsTeammate(&CG->CEntity[wHitID]))
			{
				return true;
			}
		}

		return false;
	}
	/*
	//=====================================================================================
	*/
	bool cAutowall::HitIgnored(sTrace* trace)
	{
		WORD wHitID = GetTraceHitType(trace);

		if (wHitID < MAX_CLIENTS)
		{
			if (_targetList.Priorities[wHitID].bIsIgnored)
			{
				return true;
			}
		}

		return false;
	}
}

//=====================================================================================