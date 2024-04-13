static sfxHandle_t S_AL_BufferFindFree( void )
{
	int i;

	for(i = 0; i < MAX_SFX; i++)
	{
		// Got one
		if(knownSfx[i].filename[0] == '\0')
		{
			if(i >= numSfx)
				numSfx = i + 1;
			return i;
		}
	}

	// Shit...
	Com_Error(ERR_FATAL, "S_AL_BufferFindFree: No free sound handles");
	return -1;
}