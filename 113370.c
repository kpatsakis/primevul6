static qboolean S_AL_GenBuffers(ALsizei numBuffers, ALuint *buffers, const char *name)
{
	ALenum error;

	S_AL_ClearError( qfalse );
	qalGenBuffers( numBuffers, buffers );
	error = qalGetError();

	// If we ran out of buffers, start evicting the least recently used sounds
	while( error == AL_INVALID_VALUE )
	{
		if( !S_AL_BufferEvict( ) )
		{
			Com_Printf( S_COLOR_RED "ERROR: Out of audio buffers\n");
			return qfalse;
		}

		// Try again
		S_AL_ClearError( qfalse );
		qalGenBuffers( numBuffers, buffers );
		error = qalGetError();
	}

	if( error != AL_NO_ERROR )
	{
		Com_Printf( S_COLOR_RED "ERROR: Can't create a sound buffer for %s - %s\n",
				    name, S_AL_ErrorMsg(error));
		return qfalse;
	}

	return qtrue;
}