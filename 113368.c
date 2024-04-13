static void S_AL_StartSound( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx )
{
	vec3_t sorigin;
	srcHandle_t src;
	src_t *curSource;

	if(origin)
	{
		if(S_AL_CheckInput(0, sfx))
			return;
		
		VectorCopy(origin, sorigin);
	}
	else
	{
		if(S_AL_CheckInput(entnum, sfx))
			return;

		if(S_AL_HearingThroughEntity(entnum))
		{
			S_AL_StartLocalSound(sfx, entchannel);
			return;
		}
		
		VectorCopy(entityList[entnum].origin, sorigin);
	}
	
	S_AL_SanitiseVector(sorigin);
	
	if((srcActiveCnt > 5 * srcCount / 3) &&
		(DistanceSquared(sorigin, lastListenerOrigin) >=
		(s_alMaxDistance->value + s_alGraceDistance->value) * (s_alMaxDistance->value + s_alGraceDistance->value)))
	{
		// We're getting tight on sources and source is not within hearing distance so don't add it
		return;
	}

	// Try to grab a source
	src = S_AL_SrcAlloc(SRCPRI_ONESHOT, entnum, entchannel);
	if(src == -1)
		return;

	S_AL_SrcSetup(src, sfx, SRCPRI_ONESHOT, entnum, entchannel, qfalse);
	
	curSource = &srcList[src];

	if(!origin)
		curSource->isTracking = qtrue;
		
	qalSourcefv(curSource->alSource, AL_POSITION, sorigin );
	S_AL_ScaleGain(curSource, sorigin);

	// Start it playing
	curSource->isPlaying = qtrue;
	qalSourcePlay(curSource->alSource);
}