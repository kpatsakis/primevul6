void S_AL_StartLocalSound(sfxHandle_t sfx, int channel)
{
	srcHandle_t src;
	
	if(S_AL_CheckInput(0, sfx))
		return;

	// Try to grab a source
	src = S_AL_SrcAlloc(SRCPRI_LOCAL, -1, channel);
	
	if(src == -1)
		return;

	// Set up the effect
	S_AL_SrcSetup(src, sfx, SRCPRI_LOCAL, -1, channel, qtrue);

	// Start it playing
	srcList[src].isPlaying = qtrue;
	qalSourcePlay(srcList[src].alSource);
}