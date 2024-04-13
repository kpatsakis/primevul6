static void S_AL_BufferUnload(sfxHandle_t sfx)
{
	if(knownSfx[sfx].filename[0] == '\0')
		return;

	if(!knownSfx[sfx].inMemory)
		return;

	// Delete it 
	S_AL_ClearError( qfalse );
	qalDeleteBuffers(1, &knownSfx[sfx].buffer);
	if(qalGetError() != AL_NO_ERROR)
		Com_Printf( S_COLOR_RED "ERROR: Can't delete sound buffer for %s\n",
				knownSfx[sfx].filename);

	knownSfx[sfx].inMemory = qfalse;
}