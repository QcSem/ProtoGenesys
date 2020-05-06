//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cAutowall
	{
	public:

		float Autowall(sEntity* entity, Vector3 start, Vector3 end, short hitloc);
		float TraceBullet(sEntity* entity, Vector3 start, Vector3 end, short hitloc);
		bool TraceLine(sEntity* entity, Vector3 start, Vector3 end);
		bool BulletTrace(sBulletTraceResults* traceresults, sBulletFireParams* fireparams, sEntity* attacker, int surfacetype);
		float GetRemainingDamage(sBulletFireParams* fireparams, sBulletTraceResults* traceresults, short partgroup, int weapon);
		bool HitRiotshield(sBulletTraceResults* traceresults);
		bool HitTeammate(sBulletTraceResults* traceresults);
	} extern _autoWall;
}

//=====================================================================================
