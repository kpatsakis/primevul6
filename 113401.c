qboolean S_AL_BufferInit( void )
{
	if(alBuffersInitialised)
		return qtrue;

	// Clear the hash table, and SFX table
	memset(knownSfx, 0, sizeof(knownSfx));
	numSfx = 0;

	// Load the default sound, and lock it
	default_sfx = S_AL_BufferFind("sound/feedback/hit.wav");
	S_AL_BufferUse(default_sfx);
	knownSfx[default_sfx].isLocked = qtrue;

	// All done
	alBuffersInitialised = qtrue;
	return qtrue;
}