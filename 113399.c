void S_AL_MusicUpdate( void )
{
	int		numBuffers;
	ALint	state;

	if(!musicPlaying)
		return;

	qalGetSourcei( musicSource, AL_BUFFERS_PROCESSED, &numBuffers );
	while( numBuffers-- )
	{
		ALuint b;
		qalSourceUnqueueBuffers(musicSource, 1, &b);
		S_AL_MusicProcess(b);
		qalSourceQueueBuffers(musicSource, 1, &b);
	}

	// Hitches can cause OpenAL to be starved of buffers when streaming.
	// If this happens, it will stop playback. This restarts the source if
	// it is no longer playing, and if there are buffers available
	qalGetSourcei( musicSource, AL_SOURCE_STATE, &state );
	qalGetSourcei( musicSource, AL_BUFFERS_QUEUED, &numBuffers );
	if( state == AL_STOPPED && numBuffers )
	{
		Com_DPrintf( S_COLOR_YELLOW "Restarted OpenAL music\n" );
		qalSourcePlay(musicSource);
	}

	// Set the gain property
	S_AL_Gain(musicSource, s_alGain->value * s_musicVolume->value);
}