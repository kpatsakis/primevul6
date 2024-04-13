static void S_AL_SrcSetup(srcHandle_t src, sfxHandle_t sfx, alSrcPriority_t priority,
		int entity, int channel, qboolean local)
{
	src_t *curSource;

	// Set up src struct
	curSource = &srcList[src];
	
	curSource->lastUsedTime = Sys_Milliseconds();
	curSource->sfx = sfx;
	curSource->priority = priority;
	curSource->entity = entity;
	curSource->channel = channel;
	curSource->isPlaying = qfalse;
	curSource->isLocked = qfalse;
	curSource->isLooping = qfalse;
	curSource->isTracking = qfalse;
	curSource->isStream = qfalse;
	curSource->curGain = s_alGain->value * s_volume->value;
	curSource->scaleGain = curSource->curGain;
	curSource->local = local;

	// Set up OpenAL source
	if(sfx >= 0)
	{
        	// Mark the SFX as used, and grab the raw AL buffer
        	S_AL_BufferUse(sfx);
        	qalSourcei(curSource->alSource, AL_BUFFER, S_AL_BufferGet(sfx));
	}

	qalSourcef(curSource->alSource, AL_PITCH, 1.0f);
	S_AL_Gain(curSource->alSource, curSource->curGain);
	qalSourcefv(curSource->alSource, AL_POSITION, vec3_origin);
	qalSourcefv(curSource->alSource, AL_VELOCITY, vec3_origin);
	qalSourcei(curSource->alSource, AL_LOOPING, AL_FALSE);
	qalSourcef(curSource->alSource, AL_REFERENCE_DISTANCE, s_alMinDistance->value);

	if(local)
	{
		qalSourcei(curSource->alSource, AL_SOURCE_RELATIVE, AL_TRUE);
		qalSourcef(curSource->alSource, AL_ROLLOFF_FACTOR, 0.0f);
	}
	else
	{
		qalSourcei(curSource->alSource, AL_SOURCE_RELATIVE, AL_FALSE);
		qalSourcef(curSource->alSource, AL_ROLLOFF_FACTOR, s_alRolloff->value);
	}
}