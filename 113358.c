static sfxHandle_t S_AL_BufferFind(const char *filename)
{
	// Look it up in the table
	sfxHandle_t sfx = -1;
	int i;

	if ( !filename ) {
		Com_Error( ERR_FATAL, "Sound name is NULL" );
	}

	if ( !filename[0] ) {
		Com_Printf( S_COLOR_YELLOW "WARNING: Sound name is empty\n" );
		return 0;
	}

	if ( strlen( filename ) >= MAX_QPATH ) {
		Com_Printf( S_COLOR_YELLOW "WARNING: Sound name is too long: %s\n", filename );
		return 0;
	}

	if ( filename[0] == '*' ) {
		Com_Printf( S_COLOR_YELLOW "WARNING: Tried to load player sound directly: %s\n", filename );
		return 0;
	}

	for(i = 0; i < numSfx; i++)
	{
		if(!Q_stricmp(knownSfx[i].filename, filename))
		{
			sfx = i;
			break;
		}
	}

	// Not found in table?
	if(sfx == -1)
	{
		alSfx_t *ptr;

		sfx = S_AL_BufferFindFree();

		// Clear and copy the filename over
		ptr = &knownSfx[sfx];
		memset(ptr, 0, sizeof(*ptr));
		ptr->masterLoopSrc = -1;
		strcpy(ptr->filename, filename);
	}

	// Return the handle
	return sfx;
}