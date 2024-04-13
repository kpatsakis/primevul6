void S_AL_BufferShutdown( void )
{
	int i;

	if(!alBuffersInitialised)
		return;

	// Unlock the default sound effect
	knownSfx[default_sfx].isLocked = qfalse;

	// Free all used effects
	for(i = 0; i < numSfx; i++)
		S_AL_BufferUnload(i);

	// Clear the tables
	numSfx = 0;

	// All undone
	alBuffersInitialised = qfalse;
}