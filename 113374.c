void S_AL_StreamDie( int stream )
{
	if ((stream < 0) || (stream >= MAX_RAW_STREAMS))
		return;

	if(streamSourceHandles[stream] == -1)
		return;

	streamPlaying[stream] = qfalse;
	qalSourceStop(streamSources[stream]);

	S_AL_FreeStreamChannel(stream);
}