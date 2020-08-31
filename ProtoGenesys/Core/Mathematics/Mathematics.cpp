//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cMathematics _mathematics;

	float cMathematics::CalculateFOV(ImVec3 start, ImVec3 end, ImVec3 angles)
	{
		ImVec3 vDirection = start - end, vAngles, vAimAngles;

		VectorNormalize(vDirection);
		VectorAngles(vDirection, vAimAngles);

		MakeVector(angles, vAngles);
		MakeVector(vAimAngles, vAimAngles);

		ImVec2 vLengthDot
		(
			VectorLength3D(vAngles, vAngles),
			DotProduct3D(vAngles, vAimAngles)
		);
			
		float flReturn = RadiansToDegrees(acosf(vLengthDot.y / powf(vLengthDot.x, 2.0f)));

		if (isnan(flReturn))
			flReturn = 0.0f;

		return flReturn;
	}
	/*
	//=====================================================================================
	*/
	float cMathematics::DotProduct3D(ImVec3 left, ImVec3 right)
	{
		return (left.x * right.x + left.y * right.y + left.z * right.z);
	}
	/*
	//=====================================================================================
	*/
	float cMathematics::DotProduct2D(ImVec2 left, ImVec2 right)
	{
		return (left.x * right.x + left.y * right.y);
	}
	/*
	//=====================================================================================
	*/
	float cMathematics::VectorLength3D(ImVec3 left, ImVec3 right)
	{
		return sqrtf(DotProduct3D(left, right));
	}
	/*
	//=====================================================================================
	*/
	float cMathematics::VectorLength2D(ImVec2 left, ImVec2 right)
	{
		return sqrtf(DotProduct2D(left, right));
	}
	/*
	//=====================================================================================
	*/
	float cMathematics::CalculateDistance3D(ImVec3 start, ImVec3 end)
	{
		ImVec3 vDirection;

		vDirection = start - end;

		return VectorLength3D(vDirection, vDirection);
	}
	/*
	//=====================================================================================
	*/
	float cMathematics::CalculateDistance2D(ImVec2 start, ImVec2 end)
	{
		ImVec2 vDirection;

		vDirection = start - end;

		return VectorLength2D(vDirection, vDirection);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::VectorAngles(ImVec3 direction, ImVec3& angles)
	{
		float flTemp, flYaw, flPitch;

		if (direction.x == 0.0f && direction.y == 0.0f)
		{
			flYaw = 0.0f;

			if (direction.z > 0.0f)
				flPitch = 90.0f;

			else
				flPitch = 270.0f;
		}

		else
		{
			flYaw = RadiansToDegrees(atan2f(direction.y, direction.x));

			if (flYaw < 0.0f)
				flYaw += 360.0f;

			flTemp = sqrtf(direction.x * direction.x + direction.y * direction.y);
			flPitch = RadiansToDegrees(atan2f(direction.z, flTemp));

			if (flPitch < 0.0f)
				flPitch += 360.0f;
		}

		angles.x = -flPitch;
		angles.y = flYaw;
		angles.z = 0.0f;
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::AngleVectors(ImVec3 angles, ImVec3& forward, ImVec3& right, ImVec3& up)
	{
		float flAngle, flSinRoll, flSinPitch, flSinYaw, flCosRoll, flCosPitch, flCosYaw;

		flAngle = DegreesToRadians(angles.y);
		flSinYaw = sinf(flAngle);
		flCosYaw = cosf(flAngle);
		
		flAngle = DegreesToRadians(angles.x);
		flSinPitch = sinf(flAngle);
		flCosPitch = cosf(flAngle);
		
		flAngle = DegreesToRadians(angles.z);
		flSinRoll = sinf(flAngle);
		flCosRoll = cosf(flAngle);

		if (forward)
		{
			forward.x = flCosPitch * flCosYaw;
			forward.y = flCosPitch * flSinYaw;
			forward.z = -flSinPitch;
		}

		if (right)
		{
			right.x = (-1.0f * flSinRoll * flSinPitch * flCosYaw + -1.0f * flCosRoll * -flSinYaw);
			right.y = (-1.0f * flSinRoll * flSinPitch * flSinYaw + -1.0f * flCosRoll * flCosYaw);
			right.z = -1.0f * flSinRoll * flCosPitch;
		}

		if (up)
		{
			up.x = (flCosRoll * flSinPitch * flCosYaw + -flSinRoll * -flSinYaw);
			up.y = (flCosRoll * flSinPitch * flSinYaw + -flSinRoll * flCosYaw);
			up.z = flCosRoll * flCosPitch;
		}
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::VectorNormalize(ImVec3& direction)
	{
		float flLen = VectorLength3D(direction, direction);

		if (flLen == 0.0f)
		{
			direction.x = 0.0f;
			direction.y = 0.0f;
			direction.z = 1.0f;
		}

		else
		{
			flLen = 1.0f / flLen;

			direction *= flLen;
		}
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::ClampAngles(ImVec3& angles)
	{
		while (angles.x > 180.0f)
			angles.x -= 360.0f;

		while (angles.x < -180.0f)
			angles.x += 360.0f;

		while (angles.y > 180.0f)
			angles.y -= 360.0f;

		while (angles.y < -180.0f)
			angles.y += 360.0f;

		if (angles.z != 0.0f)
			angles.z = 0.0f;
	}
	/*
	//=====================================================================================
	*/
	char cMathematics::ClampMove(char value)
	{
		if (value < -128)
			return -128;

		if (value > 127)
			return 127;

		return value;
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::CalculateAimAngles(ImVec3 start, ImVec3 end, ImVec3& angles)
	{
		ImVec3 vDirection;
		vDirection = start - end;

		VectorNormalize(vDirection);
		VectorAngles(vDirection, angles);

		ClampAngles(angles);

		angles -= CG->vRefDefViewAngles;

		ClampAngles(angles);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::CalculateAntiAimAngles(ImVec3 start, ImVec3 end, ImVec3& angles)
	{
		ImVec3 vDirection;
		vDirection = start - end;

		VectorNormalize(vDirection);
		VectorAngles(vDirection, angles);

		ClampAngles(angles);

		angles.x = _profiler.gShieldPitchShift->Current.flValue - angles.x;
		angles.y = angles.y - 170.0f;

		angles -= CG->PlayerState.vDeltaAngles;

		ClampAngles(angles);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::MakeVector(ImVec3 angles, ImVec3& out)
	{
		ImVec2 vAngles
		(
			DegreesToRadians(angles.x),
			DegreesToRadians(angles.y)
		);

		out.x = -cosf(vAngles.x) * -cosf(vAngles.y);
		out.y = sinf(vAngles.y) * cosf(vAngles.x);
		out.z = -sinf(vAngles.x);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::MovementFix(sUserCmd* usercmd, float yaw, float oldyaw, float forward, float right)
	{
		float flDelta = DegreesToRadians(yaw - oldyaw);

		usercmd->szForwardMove = ClampMove((char)(cosf(flDelta) * forward - sinf(flDelta) * right));
		usercmd->szRightMove = ClampMove((char)(sinf(flDelta) * forward + cosf(flDelta) * right));
	}
	/*
	//=====================================================================================
	*/
	bool cMathematics::WorldToScreen(ImVec3 world, ImVec2& screen)
	{
		sScreenPlacement* ScreenPlacement = GetScreenPlacement();

		ImVec3 vDirection(world - CG->RefDef.vViewOrigin);
		ImVec3 vProjection
		(
			DotProduct3D(vDirection, CG->RefDef.vViewAxis[0]),
			DotProduct3D(vDirection, CG->RefDef.vViewAxis[1]),
			DotProduct3D(vDirection, CG->RefDef.vViewAxis[2])
		);

		if (vProjection.x >= 0.0f)
		{
			screen = ImVec2
			(
				((1.0f - ((vProjection.y / CG->RefDef.flFovX) * (1.0f / vProjection.x))) * (ScreenPlacement->vRealViewportSize.x / 2.0f)) + ScreenPlacement->vRealViewportBase.x,
				((1.0f - ((vProjection.z / CG->RefDef.flFovY) * (1.0f / vProjection.x))) * (ScreenPlacement->vRealViewportSize.y / 2.0f)) + ScreenPlacement->vRealViewportBase.y
			);

			return true;
		}

		else
		{
			screen = ImVec2(-vProjection.y, -vProjection.z);

			if (abs(screen.x) < 0.001f)
			{
				if (abs(screen.y) < 0.001f)
				{
					screen.y = ScreenPlacement->vRealViewportSize.y * 2.0f;

					return false;
				}

				screen.x = 0.001f;
			}

			if (abs(screen.y) < 0.001f)
				screen.y = 0.001f;

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
		vDirection = vViewOrigin - world;

		VectorNormalize(vDirection);
		VectorAngles(vDirection, vAngles);

		vAngles = CG->vRefDefViewAngles - vAngles;

		ClampAngles(vAngles);

		flAngle = ((vAngles.y + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE;

		screen.x = compasspos.x + (cosf(flAngle) * (compasssize / 2.0f));
		screen.y = compasspos.y + (sinf(flAngle) * (compasssize / 2.0f));
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::WorldToRadar(ImVec3 world, ImVec2 radarpos, float scale, float radarsize, float blipsize, ImVec2& screen)
	{
		ImVec3 vViewOrigin;
		ImVec2 vDelta, vLocation;
		ImVec2 vCosSin
		(
			cosf(DegreesToRadians(CG->vRefDefViewAngles.y)),
			sinf(DegreesToRadians(CG->vRefDefViewAngles.y))
		);

		GetPlayerViewOrigin(&vViewOrigin);

		vDelta.x = world.x - vViewOrigin.x;
		vDelta.y = world.y - vViewOrigin.y;

		vLocation.x = (vDelta.y * vCosSin.x - vDelta.x * vCosSin.y) / scale;
		vLocation.y = (vDelta.x * vCosSin.x + vDelta.y * vCosSin.y) / scale;

		if (vLocation.x < -(radarsize / 2.0f - blipsize / 2.0f - 1.0f))
			vLocation.x = -(radarsize / 2.0f - blipsize / 2.0f - 1.0f);

		else if (vLocation.x > (radarsize / 2.0f - blipsize / 2.0f - 1.0f))
			vLocation.x = (radarsize / 2.0f - blipsize / 2.0f - 1.0f);

		if (vLocation.y < -(radarsize / 2.0f - blipsize / 2.0f - 1.0f))
			vLocation.y = -(radarsize / 2.0f - blipsize / 2.0f - 1.0f);

		else if (vLocation.y > (radarsize / 2.0f - blipsize / 2.0f))
			vLocation.y = (radarsize / 2.0f - blipsize / 2.0f);

		screen = (vLocation * -1.0f) + radarpos;
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::RotatePoint(ImVec3 point, ImVec3 center, float yaw, ImVec3& out)
	{
		ImVec2 vDelta;
		ImVec2 vCosSin
		(
			cosf(((-yaw + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE),
			sinf(((-yaw + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE)
		);

		vDelta.x = point.x - center.x;
		vDelta.y = point.y - center.y;

		out.x = center.x + vCosSin.y * vDelta.x - vCosSin.x * vDelta.y;
		out.y = center.y + vCosSin.x * vDelta.x + vCosSin.y * vDelta.y;
		out.z = point.z;
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

		vDeltaPosition = vNewPosition - vOldPosition;

		vDeltaPosition.x = (float)GetSign(vDeltaPosition.x);
		vDeltaPosition.y = (float)GetSign(vDeltaPosition.y);
		vDeltaPosition.z = (float)GetSign(vDeltaPosition.z);

		entity->vOrigin += (vDeltaPosition * (CG->iFrameTime / 1000.0f));
		entity->vOrigin += (vDeltaPosition * (ClientActive->iPing / 1000.0f));
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

		vDeltaAngles.x = AngleNormalize180(vNewAngles.x - vOldAngles.x);
		vDeltaAngles.y = AngleNormalize180(vNewAngles.y - vOldAngles.y);
		vDeltaAngles.z = AngleNormalize180(vNewAngles.z - vOldAngles.z);

		vDeltaAngles.x = (float)GetSign(vDeltaAngles.x);
		vDeltaAngles.y = (float)GetSign(vDeltaAngles.y);
		vDeltaAngles.z = (float)GetSign(vDeltaAngles.z);

		entity->vViewAngles += (vDeltaAngles * (CG->iFrameTime / 1000.0f));
		entity->vViewAngles += (vDeltaAngles * (ClientActive->iPing / 1000.0f));
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
				fld scale
				push result
				push time
				push trajectory
				call dwEvaluateTrajectory
				add esp, 0xC
				fstp flResult
			}
		}

		else
		{
			result = trajectory->vBase;
		}

		return flResult;
	}
}

//=====================================================================================