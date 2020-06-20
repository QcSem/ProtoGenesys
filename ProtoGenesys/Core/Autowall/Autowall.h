//=====================================================================================

#pragma once

#include "../Engine/Engine.hpp"

//=====================================================================================

namespace ProtoGenesys
{
	class cAutowall
	{
	public:

		float Autowall(sCEntity* entity, ImVec3 start, ImVec3 end);
		float TraceBullet(sCEntity* entity, ImVec3 start, ImVec3 end);
		bool TraceLine(sCEntity* entity, ImVec3 start, ImVec3 end);
		bool BulletTrace(sBulletTraceResults* traceresults, sBulletFireParams* fireparams, sCEntity* attacker, int surfacetype);
		float GetRemainingDamage(sBulletFireParams* fireparams, sBulletTraceResults* traceresults, int weapon);
		bool HitRiotshield(sBulletTraceResults* traceresults);
		bool HitTeammate(sTrace* trace);
		bool HitIgnored(sTrace* trace);
	} extern _autoWall;
}

//=====================================================================================
