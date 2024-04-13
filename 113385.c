static void S_AL_SaveLoopPos(src_t *dest, ALuint alSource)
{
	int error;
	
	S_AL_ClearError(qfalse);
	
	qalGetSourcef(alSource, AL_SEC_OFFSET, &dest->lastTimePos);
	if((error = qalGetError()) != AL_NO_ERROR)
	{
		// Old OpenAL implementations don't support AL_SEC_OFFSET

		if(error != AL_INVALID_ENUM)
		{
			Com_Printf(S_COLOR_YELLOW "WARNING: Could not get time offset for alSource %d: %s\n",
				   alSource, S_AL_ErrorMsg(error));
		}
		
		dest->lastTimePos = -1;
	}
	else
		dest->lastSampleTime = Sys_Milliseconds();
}