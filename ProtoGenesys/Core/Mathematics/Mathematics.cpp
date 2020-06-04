//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cMathematics _mathematics;

	float cMathematics::CalculateFOV(ImVec3 position)
	{
		ImVec3 vViewOrigin, vDirection, vAngles, vAimAngles;

		GetPlayerViewOrigin(&vViewOrigin);
		VectorSubtract(position, vViewOrigin, vDirection);

		VectorNormalize(vDirection);
		VectorAngles(vDirection, vAngles);

		MakeVector(CG->vRefDefViewAngles, vAimAngles);
		MakeVector(vAngles, vAngles);

		float flMag = VectorLength(vAimAngles),
			flDot = DotProduct(vAimAngles, vAngles),
			flReturn = RadiansToDegrees(acosf(flDot / powf(flMag, 2.0f)));

		if (isnan(flReturn))
			flReturn = 0.0f;

		return flReturn;
	}
	/*
	//=====================================================================================
	*/
	float cMathematics::CalculateDistance(ImVec3 start, ImVec3 end)
	{
		ImVec3 vDirection;

		VectorSubtract(start, end, vDirection);

		return VectorLength(vDirection);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::VectorAngles(ImVec3 direction, ImVec3& angles)
	{
		float flTemp, flYaw, flPitch;

		if (direction[0] == 0.0f && direction[1] == 0.0f)
		{
			flYaw = 0.0f;

			if (direction[2] > 0.0f)
				flPitch = 90.0f;

			else
				flPitch = 270.0f;
		}

		else
		{
			flYaw = RadiansToDegrees(atan2f(direction[1], direction[0]));

			if (flYaw < 0.0f)
				flYaw += 360.0f;

			flTemp = sqrtf(direction[0] * direction[0] + direction[1] * direction[1]);
			flPitch = RadiansToDegrees(atan2f(direction[2], flTemp));

			if (flPitch < 0.0f)
				flPitch += 360.0f;
		}

		angles[0] = -flPitch;
		angles[1] = flYaw;
		angles[2] = 0.0f;
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::AngleVectors(ImVec3 angles, ImVec3& forward, ImVec3& right, ImVec3& up)
	{
		float flAngle, flSinRoll, flSinPitch, flSinYaw, flCosRoll, flCosPitch, flCosYaw;

		flAngle = DegreesToRadians(angles[1]);
		flSinYaw = sinf(flAngle);
		flCosYaw = cosf(flAngle);
		
		flAngle = DegreesToRadians(angles[0]);
		flSinPitch = sinf(flAngle);
		flCosPitch = cosf(flAngle);
		
		flAngle = DegreesToRadians(angles[2]);
		flSinRoll = sinf(flAngle);
		flCosRoll = cosf(flAngle);

		if (forward)
		{
			forward[0] = flCosPitch * flCosYaw;
			forward[1] = flCosPitch * flSinYaw;
			forward[2] = -flSinPitch;
		}

		if (right)
		{
			right[0] = (-1.0f * flSinRoll * flSinPitch * flCosYaw + -1.0f * flCosRoll * -flSinYaw);
			right[1] = (-1.0f * flSinRoll * flSinPitch * flSinYaw + -1.0f * flCosRoll * flCosYaw);
			right[2] = -1.0f * flSinRoll * flCosPitch;
		}

		if (up)
		{
			up[0] = (flCosRoll * flSinPitch * flCosYaw + -flSinRoll * -flSinYaw);
			up[1] = (flCosRoll * flSinPitch * flSinYaw + -flSinRoll * flCosYaw);
			up[2] = flCosRoll * flCosPitch;
		}
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::VectorNormalize(ImVec3& direction)
	{
		float flLen = VectorLength(direction);

		if (flLen == 0.0f)
		{
			direction[0] = 0.0f;
			direction[1] = 0.0f;
			direction[2] = 1.0f;
		}

		else
		{
			flLen = 1.0f / flLen;

			direction[0] *= flLen;
			direction[1] *= flLen;
			direction[2] *= flLen;
		}
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::ClampAngles(ImVec3& angles)
	{
		while (angles[0] > 180.0f)
			angles[0] -= 360.0f;

		while (angles[0] < -180.0f)
			angles[0] += 360.0f;

		while (angles[1] > 180.0f)
			angles[1] -= 360.0f;

		while (angles[1] < -180.0f)
			angles[1] += 360.0f;

		if (angles[2] != 0.0f)
			angles[2] = 0.0f;
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::ClampMove(char value[])
	{
		while (value[0] > 127)
			value[0] = 127;

		while (value[0] < -128)
			value[0] = -128;

		while (value[1] > 127)
			value[1] = 127;

		while (value[1] < -128)
			value[1] = -128;

		if (value[2] != 0)
			value[2] = 0;
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::CalculateAimAngles(ImVec3 start, ImVec3 end, ImVec3& angles)
	{
		ImVec3 vDirection;
		VectorSubtract(start, end, vDirection);

		VectorNormalize(vDirection);
		VectorAngles(vDirection, angles);

		ClampAngles(angles);

		angles[0] -= CG->vRefDefViewAngles[0];
		angles[1] -= CG->vRefDefViewAngles[1];

		ClampAngles(angles);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::CalculateAntiAimAngles(ImVec3 start, ImVec3 end, ImVec3& angles)
	{
		ImVec3 vDirection;
		VectorSubtract(start, end, vDirection);

		VectorNormalize(vDirection);
		VectorAngles(vDirection, angles);

		ClampAngles(angles);

		angles[0] = -40.0f - angles[0];
		angles[1] = angles[1] - 170.0f;

		angles[0] -= CG->PlayerState.vDeltaAngles[0];
		angles[1] -= CG->PlayerState.vDeltaAngles[1];

		ClampAngles(angles);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::MakeVector(ImVec3 angles, ImVec3& out)
	{
		float flPitch = DegreesToRadians(angles[0]), 
			flYaw = DegreesToRadians(angles[1]);

		out[0] = -cosf(flPitch) * -cosf(flYaw);
		out[1] = sinf(flYaw) * cosf(flPitch);
		out[2] = -sinf(flPitch);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::MovementFix(sUserCmd* usercmd, float yaw, float oldyaw, float forward, float right)
	{
		float flDelta = DegreesToRadians(yaw - oldyaw);

		usercmd->szMove[0] = (char)(cosf(flDelta) * forward - sinf(flDelta) * right);
		usercmd->szMove[1] = (char)(sinf(flDelta) * forward + cosf(flDelta) * right);

		ClampMove(usercmd->szMove);
	}
	/*
	//=====================================================================================
	*/
	bool cMathematics::WorldToScreen(ImVec3 world, ImVec2& screen)
	{
		sScreenPlacement* ScreenPlacement = GetScreenPlacement();

		ImVec3 vDirection(world - CG->RefDef.vViewOrigin);

		ImVec3 vProjection(DotProduct(vDirection, CG->RefDef.vViewAxis[0]), 
			DotProduct(vDirection, CG->RefDef.vViewAxis[1]), 
			DotProduct(vDirection, CG->RefDef.vViewAxis[2]));

		if (vProjection.x >= 0.0f)
		{
			screen = ImVec2((1.0f - vProjection.y / CG->RefDef.flFovX / vProjection.x) * ScreenPlacement->vRealViewportSize.x / 2.0f,
				(1.0f - vProjection.z / CG->RefDef.flFovY / vProjection.x) * ScreenPlacement->vRealViewportSize.y / 2.0f);

			return true;
		}

		else
		{
			vProjection *= -1.0f;

			screen = ImVec2(vProjection.y, vProjection.z);
			
			if (abs(vProjection.y < 0.001f))
			{
				if (abs(vProjection.z) < 0.001f)
				{
					screen.y = ScreenPlacement->vRealViewportSize.y * 2.0f;

					return false;
				}

				screen.x = 0.001f;
			}

			while (ScreenPlacement->vRealViewportSize.x > abs(screen.x))
			{
				screen.x *= ScreenPlacement->vRealViewportSize.x;
				screen.y *= ScreenPlacement->vRealViewportSize.x;
			}

			while (ScreenPlacement->vRealViewportSize.y > abs(screen.y))
			{
				screen.x *= ScreenPlacement->vRealViewportSize.y;
				screen.y *= ScreenPlacement->vRealViewportSize.y;
			}

			return false;
		}
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::WorldToCompass(ImVec3 world, ImVec2 compasspos, float compasssize, ImVec2& screen)
	{
		float flAngle;

		ImVec3 vViewOrigin, vDirection, vAngles;

		GetPlayerViewOrigin(&vViewOrigin);
		VectorSubtract(vViewOrigin, world, vDirection);

		VectorNormalize(vDirection);
		VectorAngles(vDirection, vAngles);

		VectorSubtract(CG->vRefDefViewAngles, vAngles, vAngles);
		ClampAngles(vAngles);

		flAngle = ((vAngles[1] + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE;

		screen.x = compasspos.x + (cosf(flAngle) * (compasssize / 2.0f));
		screen.y = compasspos.y + (sinf(flAngle) * (compasssize / 2.0f));
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::WorldToRadar(ImVec3 world, ImVec2 radarpos, float scale, float radarsize, float blipsize, ImVec2& screen)
	{
		ImVec3 vViewOrigin;

		GetPlayerViewOrigin(&vViewOrigin);

		float flCosYaw = cosf(DegreesToRadians(CG->vRefDefViewAngles[1])),
			flSinYaw = sinf(DegreesToRadians(CG->vRefDefViewAngles[1])),
			flDeltaX = world[0] - vViewOrigin[0],
			flDeltaY = world[1] - vViewOrigin[1],
			flLocationX = (flDeltaY * flCosYaw - flDeltaX * flSinYaw) / scale,
			flLocationY = (flDeltaX * flCosYaw + flDeltaY * flSinYaw) / scale;

		if (flLocationX < -(radarsize / 2.0f - blipsize / 2.0f - 1.0f))
			flLocationX = -(radarsize / 2.0f - blipsize / 2.0f - 1.0f);

		else if (flLocationX > (radarsize / 2.0f - blipsize / 2.0f - 1.0f))
			flLocationX = (radarsize / 2.0f - blipsize / 2.0f - 1.0f);

		if (flLocationY < -(radarsize / 2.0f - blipsize / 2.0f - 1.0f))
			flLocationY = -(radarsize / 2.0f - blipsize / 2.0f - 1.0f);

		else if (flLocationY > (radarsize / 2.0f - blipsize / 2.0f))
			flLocationY = (radarsize / 2.0f - blipsize / 2.0f);

		screen.x = -flLocationX + radarpos.x;
		screen.y = -flLocationY + radarpos.y;
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::RotatePoint(ImVec3 point, ImVec3 center, float yaw, ImVec3& out)
	{
		float flAngleCos = cosf(((-yaw + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE),
			flAngleSin = sinf(((-yaw + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE),
			flDifferenceX = point[0] - center[0],
			flDifferenceY = point[1] - center[1];

		out[0] = center[0] + flAngleSin * flDifferenceX - flAngleCos * flDifferenceY;
		out[1] = center[1] + flAngleCos * flDifferenceX + flAngleSin * flDifferenceY;
		out[2] = point[2];
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::ApplyPositionPrediction(sCEntity* entity)
	{
		float flResult;
		ImVec3 vOldPosition, vNewPosition, vDeltaPosition;

		flResult = EntityInterpolation(&entity->CurrentEntityState.PositionTrajectory, CG->OldSnapShot->iServerTime, vOldPosition, CG->flFrameInterpolation);
		EntityInterpolation(&entity->NextEntityState.LerpEntityState.PositionTrajectory, CG->NewSnapShot->iServerTime, vNewPosition, flResult);

		vDeltaPosition[0] = vNewPosition[0] - vOldPosition[0];
		vDeltaPosition[1] = vNewPosition[1] - vOldPosition[1];
		vDeltaPosition[2] = vNewPosition[2] - vOldPosition[2];

		VectorGetSign(vDeltaPosition);

		VectorMA(entity->vOrigin, CG->iFrameTime / 1000.0f, vDeltaPosition, entity->vOrigin);
		VectorMA(entity->vOrigin, ClientActive->iPing / 1000.0f, vDeltaPosition, entity->vOrigin);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::ApplyAnglePrediction(sCEntity* entity)
	{
		float flResult;
		ImVec3 vOldAngles, vNewAngles, vDeltaAngles;

		flResult = EntityInterpolation(&entity->CurrentEntityState.AngleTrajectory, CG->OldSnapShot->iServerTime, vOldAngles, CG->flFrameInterpolation);
		EntityInterpolation(&entity->NextEntityState.LerpEntityState.AngleTrajectory, CG->NewSnapShot->iServerTime, vNewAngles, flResult);

		vDeltaAngles[0] = AngleNormalize180(vNewAngles[0] - vOldAngles[0]);
		vDeltaAngles[1] = AngleNormalize180(vNewAngles[1] - vOldAngles[1]);
		vDeltaAngles[2] = AngleNormalize180(vNewAngles[2] - vOldAngles[2]);

		VectorGetSign(vDeltaAngles);

		VectorMA(entity->vViewAngles, CG->iFrameTime / 1000.0f, vDeltaAngles, entity->vViewAngles);
		VectorMA(entity->vViewAngles, ClientActive->iPing / 1000.0f, vDeltaAngles, entity->vViewAngles);
	}
	/*
	//=====================================================================================
	*/
	float cMathematics::EntityInterpolation(sTrajectory* trajectory, int time, ImVec3& result, float scale)
	{
		float flResult = 0.0f;

		if (trajectory->iType && trajectory->iType != 1 && trajectory->iType != 14 && trajectory->iType != 10)
		{
			__asm
			{
				fld scale;
				push result;
				push time;
				push trajectory;
				call dwEvaluateTrajectory;
				add esp, 0xC;
				fstp flResult;
			}
		}

		else
		{
			VectorCopy(trajectory->vBase, result);
		}

		return flResult;
	}
}

//=====================================================================================