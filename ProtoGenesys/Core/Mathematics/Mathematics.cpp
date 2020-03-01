//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	cMathematics _mathematics;

	float cMathematics::CalculateFOV(Vector3 position)
	{
		Vector3 vViewOrigin, vDirection, vAngles, vAimAngles;

		GetPlayerViewOrigin(vViewOrigin);
		VectorSubtract(position, vViewOrigin, vDirection);

		VectorNormalize(vDirection);
		VectorAngles(vDirection, vAngles);

		MakeVector(CG->vRefDefViewAngles, vAimAngles);
		MakeVector(vAngles, vAngles);

		float flMag = sqrtf(DotProduct(vAimAngles, vAimAngles)),
			flDot = DotProduct(vAimAngles, vAngles),
			flReturn = RadiansToDegrees(acosf(flDot / powf(flMag, 2.0f)));

		if (isnan(flReturn))
			flReturn = 0.0f;

		return flReturn;
	}
	/*
	//=====================================================================================
	*/
	float cMathematics::CalculateDistance(Vector3 start, Vector3 end)
	{
		Vector3 vDirection;

		VectorSubtract(start, end, vDirection);

		return sqrtf(DotProduct(vDirection, vDirection));
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::VectorAngles(Vector3 direction, Vector3 angles)
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
	void cMathematics::AngleVectors(Vector3 angles, Vector3 forward, Vector3 right, Vector3 up)
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
	void cMathematics::VectorNormalize(Vector3 direction)
	{
		float flLen = sqrtf(DotProduct(direction, direction));

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
	void cMathematics::NormalizeAngles(Vector3 angles)
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
	void cMathematics::CalculateAngles(Vector3 start, Vector3 end, Vector3 angles)
	{
		Vector3 vDirection;
		VectorSubtract(end, start, vDirection);

		VectorNormalize(vDirection);
		VectorAngles(vDirection, angles);

		NormalizeAngles(angles);

		angles[0] -= CG->vRefDefViewAngles[0];
		angles[1] -= CG->vRefDefViewAngles[1];

		NormalizeAngles(angles);
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::MakeVector(Vector3 angles, Vector3 out)
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
	void cMathematics::MovementFix(sUserCmd* usercmd, float yaw)
	{
		if (usercmd->szForwardMove || usercmd->szRightMove)
		{
			float flMove = AngleNormalize(RadiansToDegrees(atan2(-usercmd->szRightMove / 127.0f, usercmd->szForwardMove / 127.0f))),
				flDelta = AngleNormalize(yaw),
				flDestination = AngleNormalize(flMove - flDelta),
				flForwardRatio = cos(DegreesToRadians(flDestination)),
				flRightRatio = -sin(DegreesToRadians(flDestination));

			if (abs(flForwardRatio) < abs(flRightRatio))
			{
				flForwardRatio *= 1.0f / abs(flRightRatio);
				flRightRatio = flRightRatio > 0.0f ? 1.0f : -1.0f;
			}

			else if (abs(flForwardRatio) > abs(flRightRatio))
			{
				flRightRatio *= 1.0f / abs(flForwardRatio);
				flForwardRatio = flForwardRatio > 0.0f ? 1.0f : -1.0f;
			}

			else
			{
				flForwardRatio = 1.0f;
				flRightRatio = 1.0f;
			}

			usercmd->szForwardMove = (char)(flForwardRatio * 127.0f);
			usercmd->szRightMove = (char)(flRightRatio * 127.0f);
		}
	}
	/*
	//=====================================================================================
	*/
	bool cMathematics::WorldToScreen(Vector3 world, ImVec2& screen)
	{
		float flCenterX = CG->RefDef.iWidth / 2.0f,
			flCenterY = CG->RefDef.iHeight / 2.0f;

		Vector3 vLocal, vTransForm;

		VectorSubtract(world, CG->RefDef.vViewOrg, vLocal);

		vTransForm[0] = DotProduct(vLocal, CG->RefDef.vViewAxis[1]);
		vTransForm[1] = DotProduct(vLocal, CG->RefDef.vViewAxis[2]);
		vTransForm[2] = DotProduct(vLocal, CG->RefDef.vViewAxis[0]);

		if (vTransForm[2] < 0.01f)
			return false;

		screen.x = flCenterX * (1.0f - (vTransForm[0] / CG->RefDef.flFovX / vTransForm[2]));
		screen.y = flCenterY * (1.0f - (vTransForm[1] / CG->RefDef.flFovY / vTransForm[2]));

		return true;
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::WorldToCompass(Vector3 world, ImVec2 compasspos, float compasssize, ImVec2& screen)
	{
		float flAngle;

		Vector3 vViewOrigin, vDirection, vAngles;

		GetPlayerViewOrigin(vViewOrigin);
		VectorSubtract(vViewOrigin, world, vDirection);

		VectorNormalize(vDirection);
		VectorAngles(vDirection, vAngles);

		VectorSubtract(CG->vRefDefViewAngles, vAngles, vAngles);
		_mathematics.NormalizeAngles(vAngles);

		flAngle = ((vAngles[1] + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE;

		screen.x = compasspos.x + (cosf(flAngle) * (compasssize / 2.0f));
		screen.y = compasspos.y + (sinf(flAngle) * (compasssize / 2.0f));
	}
	/*
	//=====================================================================================
	*/
	void cMathematics::WorldToRadar(Vector3 world, ImVec2 radarpos, float scale, float radarsize, float blipsize, ImVec2& screen)
	{
		Vector3 vViewOrigin;

		GetPlayerViewOrigin(vViewOrigin);

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
	void cMathematics::RotatePoint(Vector3 point, Vector3 center, float yaw, Vector3 out)
	{
		float flAngleCos = cosf(((-yaw + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE),
			flAngleSin = sinf(((-yaw + 180.0f) / 360.0f - 0.25f) * M_PI_DOUBLE),
			flDifferenceX = point[0] - center[0],
			flDifferenceY = point[1] - center[1];

		out[0] = center[0] + flAngleSin * flDifferenceX - flAngleCos * flDifferenceY;
		out[1] = center[1] + flAngleCos * flDifferenceX + flAngleSin * flDifferenceY;
		out[2] = point[2];
	}
}