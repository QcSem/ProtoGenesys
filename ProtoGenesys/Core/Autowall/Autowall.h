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
		float TraceLine(Vector3 start, Vector3 end, int entitynum);
		bool BulletTrace(sBulletTraceResults* traceresults, int localnum, sBulletFireParams* fireparams, sEntity* attacker, int surfacetype, int simulated);
		float GetRemainingDamage(sBulletFireParams* fireparams, sBulletTraceResults* traceresults, short partgroup, int weapon);
		bool HitRiotshield(sBulletTraceResults* traceresults);
	} extern _autoWall;
}

//=====================================================================================
