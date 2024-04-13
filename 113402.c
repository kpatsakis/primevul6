static void S_AL_SrcLoop( alSrcPriority_t priority, sfxHandle_t sfx,
		const vec3_t origin, const vec3_t velocity, int entityNum )
{
	int				src;
	sentity_t	*sent = &entityList[ entityNum ];
	src_t		*curSource;
	vec3_t		sorigin, svelocity;

	if(S_AL_CheckInput(entityNum, sfx))
		return;

	// Do we need to allocate a new source for this entity
	if( !sent->srcAllocated )
	{
		// Try to get a channel
		src = S_AL_SrcAlloc( priority, entityNum, -1 );
		if( src == -1 )
		{
			Com_DPrintf( S_COLOR_YELLOW "WARNING: Failed to allocate source "
					"for loop sfx %d on entity %d\n", sfx, entityNum );
			return;
		}

		curSource = &srcList[src];

		sent->startLoopingSound = qtrue;

		curSource->lastTimePos = -1.0;
		curSource->lastSampleTime = Sys_Milliseconds();
	}
	else
	{
		src = sent->srcIndex;
		curSource = &srcList[src];
	}

	sent->srcAllocated = qtrue;
	sent->srcIndex = src;

	sent->loopPriority = priority;
	sent->loopSfx = sfx;

	// If this is not set then the looping sound is stopped.
	sent->loopAddedThisFrame = qtrue;

	// UGH
	// These lines should be called via S_AL_SrcSetup, but we
	// can't call that yet as it buffers sfxes that may change
	// with subsequent calls to S_AL_SrcLoop
	curSource->entity = entityNum;
	curSource->isLooping = qtrue;

	if( S_AL_HearingThroughEntity( entityNum ) )
	{
		curSource->local = qtrue;

		VectorClear(sorigin);

		qalSourcefv(curSource->alSource, AL_POSITION, sorigin);
		qalSourcefv(curSource->alSource, AL_VELOCITY, vec3_origin);
	}
	else
	{
		curSource->local = qfalse;

		if(origin)
			VectorCopy(origin, sorigin);
		else
			VectorCopy(sent->origin, sorigin);

		S_AL_SanitiseVector(sorigin);
		
		VectorCopy(sorigin, curSource->loopSpeakerPos);
		
		if(velocity)
		{
			VectorCopy(velocity, svelocity);
			S_AL_SanitiseVector(svelocity);
		}
		else
			VectorClear(svelocity);

		qalSourcefv(curSource->alSource, AL_POSITION, (ALfloat *) sorigin);
		qalSourcefv(curSource->alSource, AL_VELOCITY, (ALfloat *) svelocity);
	}
}