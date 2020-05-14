//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cAutowall
	{
	public:

		float Autowall(sCEntity* entity, Vector3 start, Vector3 end);
		float TraceBullet(sCEntity* entity, Vector3 start, Vector3 end);
		bool TraceLine(sCEntity* entity, Vector3 start, Vector3 end);
		bool BulletTrace(sBulletTraceResults* traceresults, sBulletFireParams* fireparams, sCEntity* attacker, int surfacetype);
		float GetRemainingDamage(sBulletFireParams* fireparams, sBulletTraceResults* traceresults, int weapon);
		bool HitRiotshield(sBulletTraceResults* traceresults);
		bool HitTeammate(sBulletTraceResults* traceresults);
	} extern _autoWall;
}

//=====================================================================================
