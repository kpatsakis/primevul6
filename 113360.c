static void S_AL_BufferLoad(sfxHandle_t sfx, qboolean cache)
{
	ALenum error;
	ALuint format;

	void *data;
	snd_info_t info;
	alSfx_t *curSfx = &knownSfx[sfx];

	// Nothing?
	if(curSfx->filename[0] == '\0')
		return;

	// Already done?
	if((curSfx->inMemory) || (curSfx->isDefault) || (!cache && curSfx->isDefaultChecked))
		return;

	// Try to load
	data = S_CodecLoad(curSfx->filename, &info);
	if(!data)
	{
		S_AL_BufferUseDefault(sfx);
		return;
	}

	curSfx->isDefaultChecked = qtrue;

	if (!cache)
	{
		// Don't create AL cache
		Hunk_FreeTempMemory(data);
		return;
	}

	format = S_AL_Format(info.width, info.channels);

	// Create a buffer
	if (!S_AL_GenBuffers(1, &curSfx->buffer, curSfx->filename))
	{
		S_AL_BufferUseDefault(sfx);
		Hunk_FreeTempMemory(data);
		return;
	}

	// Fill the buffer
	if( info.size == 0 )
	{
		// We have no data to buffer, so buffer silence
		byte dummyData[ 2 ] = { 0 };

		qalBufferData(curSfx->buffer, AL_FORMAT_MONO16, (void *)dummyData, 2, 22050);
	}
	else
		qalBufferData(curSfx->buffer, format, data, info.size, info.rate);

	error = qalGetError();

	// If we ran out of memory, start evicting the least recently used sounds
	while(error == AL_OUT_OF_MEMORY)
	{
		if( !S_AL_BufferEvict( ) )
		{
			qalDeleteBuffers(1, &curSfx->buffer);
			S_AL_BufferUseDefault(sfx);
			Hunk_FreeTempMemory(data);
			Com_Printf( S_COLOR_RED "ERROR: Out of memory loading %s\n", curSfx->filename);
			return;
		}

		// Try load it again
		qalBufferData(curSfx->buffer, format, data, info.size, info.rate);
		error = qalGetError();
	}

	// Some other error condition
	if(error != AL_NO_ERROR)
	{
		qalDeleteBuffers(1, &curSfx->buffer);
		S_AL_BufferUseDefault(sfx);
		Hunk_FreeTempMemory(data);
		Com_Printf( S_COLOR_RED "ERROR: Can't fill sound buffer for %s - %s\n",
				curSfx->filename, S_AL_ErrorMsg(error));
		return;
	}

	curSfx->info = info;
	
	// Free the memory
	Hunk_FreeTempMemory(data);

	// Woo!
	curSfx->inMemory = qtrue;
}