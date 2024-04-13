static qboolean S_AL_HearingThroughEntity( int entityNum )
{
	float	distanceSq;

	if( lastListenerNumber == entityNum )
	{
		// This is an outrageous hack to detect
		// whether or not the player is rendering in third person or not. We can't
		// ask the renderer because the renderer has no notion of entities and we
		// can't ask cgame since that would involve changing the API and hence mod
		// compatibility. I don't think there is any way around this, but I'll leave
		// the FIXME just in case anyone has a bright idea.
		distanceSq = DistanceSquared(
				entityList[ entityNum ].origin,
				lastListenerOrigin );

		if( distanceSq > THIRD_PERSON_THRESHOLD_SQ )
			return qfalse; //we're the player, but third person
		else
			return qtrue;  //we're the player
	}
	else
		return qfalse; //not the player
}