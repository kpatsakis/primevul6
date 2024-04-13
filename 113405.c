void S_AL_StartBackgroundTrack( const char *intro, const char *loop )
{
	int i;
	qboolean issame;

	// Stop any existing music that might be playing
	S_AL_StopBackgroundTrack();

	if((!intro || !*intro) && (!loop || !*loop))
		return;

	// Allocate a musicSource
	S_AL_MusicSourceGet();
	if(musicSourceHandle == -1)
		return;

	if (!loop || !*loop)
	{
		loop = intro;
		issame = qtrue;
	}
	else if(intro && *intro && !strcmp(intro, loop))
		issame = qtrue;
	else
		issame = qfalse;

	// Copy the loop over
	Q_strncpyz( s_backgroundLoop, loop, sizeof( s_backgroundLoop ) );

	if(!issame)
	{
		// Open the intro and don't mind whether it succeeds.
		// The important part is the loop.
		intro_stream = S_CodecOpenStream(intro);
	}
	else
		intro_stream = NULL;

	mus_stream = S_CodecOpenStream(s_backgroundLoop);
	if(!mus_stream)
	{
		S_AL_CloseMusicFiles();
		S_AL_MusicSourceFree();
		return;
	}

	// Generate the musicBuffers
	if (!S_AL_GenBuffers(NUM_MUSIC_BUFFERS, musicBuffers, "music"))
		return;
	
	// Queue the musicBuffers up
	for(i = 0; i < NUM_MUSIC_BUFFERS; i++)
	{
		S_AL_MusicProcess(musicBuffers[i]);
	}

	qalSourceQueueBuffers(musicSource, NUM_MUSIC_BUFFERS, musicBuffers);

	// Set the initial gain property
	S_AL_Gain(musicSource, s_alGain->value * s_musicVolume->value);
	
	// Start playing
	qalSourcePlay(musicSource);

	musicPlaying = qtrue;
}