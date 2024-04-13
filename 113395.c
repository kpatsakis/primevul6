static void S_AL_FreeStreamChannel( int stream )
{
	if ((stream < 0) || (stream >= MAX_RAW_STREAMS))
		return;

	// Detach any buffers
	qalSourcei(streamSources[stream], AL_BUFFER, 0);

	// Delete the buffers
	if (streamNumBuffers[stream] > 0) {
		qalDeleteBuffers(streamNumBuffers[stream], streamBuffers[stream]);
		streamNumBuffers[stream] = 0;
	}

	// Release the output streamSource
	S_AL_SrcUnlock(streamSourceHandles[stream]);
	S_AL_SrcKill(streamSourceHandles[stream]);
	streamSources[stream] = 0;
	streamSourceHandles[stream] = -1;
}