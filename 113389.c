void S_AL_StreamUpdate( int stream )
{
	int		numBuffers;
	ALint	state;

	if ((stream < 0) || (stream >= MAX_RAW_STREAMS))
		return;

	if(streamSourceHandles[stream] == -1)
		return;

	// Un-queue any buffers
	qalGetSourcei( streamSources[stream], AL_BUFFERS_PROCESSED, &numBuffers );
	while( numBuffers-- )
	{
		ALuint buffer;
		qalSourceUnqueueBuffers(streamSources[stream], 1, &buffer);
	}

	// Start the streamSource playing if necessary
	qalGetSourcei( streamSources[stream], AL_BUFFERS_QUEUED, &numBuffers );

	qalGetSourcei(streamSources[stream], AL_SOURCE_STATE, &state);
	if(state == AL_STOPPED)
	{
		streamPlaying[stream] = qfalse;

		// If there are no buffers queued up, release the streamSource
		if( !numBuffers )
			S_AL_FreeStreamChannel( stream );
	}

	if( !streamPlaying[stream] && numBuffers )
	{
		qalSourcePlay( streamSources[stream] );
		streamPlaying[stream] = qtrue;
	}
}