//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cAutowall
	{
	public:

		float Autowall(Vector3 start, Vector3 end);
		bool TraceBullet(Vector3 start, Vector3 end, int entitynum);
		bool TraceLine(Vector3 start, Vector3 end, int entitynum);
		bool BulletTrace(sBulletTraceResults* traceresults, sBulletFireParams* fireparams, sEntity* attacker, int surfacetype);
		float GetRemainingDamage(sBulletFireParams* fireparams, sBulletTraceResults* traceresults, short partgroup, int weapon);
		bool HitRiotshield(sBulletTraceResults* traceresults);
		bool HitTeammate(sBulletTraceResults* traceresults);
	} extern _autoWall;
}

//=====================================================================================
